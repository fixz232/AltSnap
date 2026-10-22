/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2015    Stefan Sundin                                   *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 or later.              *
 * Modified By Raymond Gillibert in 2021                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "hooks.h"
#include "resource.h"

#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR 0x00000100
#endif
#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#define CONFIG_PATH_CHARS 32768

// Boring stuff
static HINSTANCE g_hinst = NULL;
static HWND g_hwnd = NULL;
static UINT WM_TASKBARCREATED = 0;
static TCHAR inipath[CONFIG_PATH_CHARS];
static TCHAR hookspath[CONFIG_PATH_CHARS];

static WNDPROC G_HotKeyProc = NULL;
typedef WNDPROC (WINAPI *hooks_load_proc_t)(HWND, const TCHAR *);
typedef void (WINAPI *hooks_unload_proc_t)(void);
static hooks_load_proc_t HooksLoad = NULL;
static hooks_unload_proc_t HooksUnload = NULL;
static HOOKPROC HooksKeyboardProc = NULL;
static BOOL hooks_initialized = FALSE;
static BOOL hook_requested = TRUE;
static int showerror = 1;
static DWORD config_write_error = ERROR_SUCCESS;

// Cool stuff
HINSTANCE hinstDLL = NULL;
HHOOK keyhook = NULL;
static DWORD ACMenuItems=-1;
static unsigned char elevated = 0;
static unsigned char ScrollLockState = 0;
static char SnapGap = 0;
static BYTE WinVer = 0;

#define WIN2K 5
#define VISTA 6
#define WIN10 10

#define ENABLED() (!!keyhook)
#define GetWindowRectL(hwnd, rect) GetWindowRectLL(hwnd, rect, SnapGap)
static void UpdateSettings();
static void SetWindowToolEnabled(BOOL enabled);
static BOOL WTApplyCaptureProtection(HWND hwnd);

static BOOL AppendPathPart(TCHAR *path, size_t pathlen, const TCHAR *part)
{
    size_t len = lstrlen(path);
    size_t partlen = lstrlen(part);
    if (!len || len + partlen + 2 > pathlen) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    if (path[len-1] != TEXT('\\')) {
        path[len++] = TEXT('\\');
        path[len] = TEXT('\0');
    }
    lstrcpy_s(path + len, pathlen - len, part);
    return TRUE;
}

static BOOL EnsureDirectory(const TCHAR *path)
{
    DWORD attributes = GetFileAttributes(path);
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        if (attributes & FILE_ATTRIBUTE_DIRECTORY)
            return TRUE;
        SetLastError(ERROR_DIRECTORY);
        return FALSE;
    }
    if (CreateDirectory(path, NULL))
        return TRUE;
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

static BOOL GetEmbeddedResource(WORD id, const BYTE **data, DWORD *size)
{
    HRSRC resource = FindResource(g_hinst, MAKEINTRESOURCE(id), RT_RCDATA);
    if (!resource)
        return FALSE;
    HGLOBAL loaded = LoadResource(g_hinst, resource);
    if (!loaded)
        return FALSE;
    *size = SizeofResource(g_hinst, resource);
    *data = (const BYTE *)LockResource(loaded);
    if (!*data || !*size) {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }
    return TRUE;
}

static BOOL FileMatchesData(const TCHAR *path, const BYTE *data, DWORD size)
{
    HANDLE file = CreateFile(path, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return FALSE;

    LARGE_INTEGER filesize;
    BOOL matches = GetFileSizeEx(file, &filesize)
        && filesize.HighPart == 0 && filesize.LowPart == size;
    DWORD offset = 0;
    static BYTE buffer[4096];
    while (matches && offset < size) {
        DWORD wanted = min((DWORD)sizeof(buffer), size - offset);
        DWORD read = 0;
        if (!ReadFile(file, buffer, wanted, &read, NULL) || read != wanted) {
            matches = FALSE;
            break;
        }
        for (DWORD i = 0; i < read; i++) {
            if (buffer[i] != data[offset+i]) {
                matches = FALSE;
                break;
            }
        }
        offset += read;
    }
    DWORD error = matches ? ERROR_SUCCESS : GetLastError();
    if (!matches && error == ERROR_SUCCESS)
        error = ERROR_CRC;
    CloseHandle(file);
    SetLastError(error);
    return matches;
}

static BOOL WriteDataAtomically(const TCHAR *path, const BYTE *data, DWORD size, BOOL replace)
{
    static TCHAR temppath[CONFIG_PATH_CHARS];
    TCHAR suffix[64];
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD error = ERROR_SUCCESS;

    if (lstrlen(path) + ARR_SZ(suffix) >= ARR_SZ(temppath)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    for (DWORD attempt = 0; attempt < 16; attempt++) {
        lstrcpy_s(temppath, ARR_SZ(temppath), path);
        wsprintf(suffix, TEXT(".%lu.%lu.tmp"), GetCurrentProcessId(), GetTickCount()+attempt);
        lstrcat_s(temppath, ARR_SZ(temppath), suffix);
        file = CreateFile(temppath, GENERIC_WRITE, 0, NULL, CREATE_NEW,
            FILE_ATTRIBUTE_TEMPORARY, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            error = ERROR_SUCCESS;
            break;
        }
        error = GetLastError();
        if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS)
            return FALSE;
    }
    if (file == INVALID_HANDLE_VALUE) {
        SetLastError(error);
        return FALSE;
    }

    DWORD offset = 0;
    while (offset < size) {
        DWORD written = 0;
        if (!WriteFile(file, data + offset, size - offset, &written, NULL) || !written) {
            error = GetLastError();
            break;
        }
        offset += written;
    }
    if (offset == size && !FlushFileBuffers(file))
        error = GetLastError();
    CloseHandle(file);

    if (offset != size || error != ERROR_SUCCESS) {
        DeleteFile(temppath);
        SetLastError(error ? error : ERROR_WRITE_FAULT);
        return FALSE;
    }

    DWORD flags = MOVEFILE_WRITE_THROUGH | (replace ? MOVEFILE_REPLACE_EXISTING : 0);
    if (!MoveFileEx(temppath, path, flags)) {
        error = GetLastError();
        DeleteFile(temppath);
        if (!replace && (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS))
            return TRUE;
        SetLastError(error);
        return FALSE;
    }
    return TRUE;
}

static BOOL WriteEmbeddedResource(WORD id, const TCHAR *path, BOOL replace)
{
    const BYTE *data;
    DWORD size;
    if (!GetEmbeddedResource(id, &data, &size))
        return FALSE;
    if (!replace && GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES)
        return TRUE;
    return WriteDataAtomically(path, data, size, replace);
}

static BOOL IsFileWritable(const TCHAR *path)
{
    HANDLE file = CreateFile(path, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return FALSE;
    CloseHandle(file);
    return TRUE;
}

static BOOL GetSidecarIniPath(TCHAR *path, size_t pathlen)
{
    DWORD ret = GetModuleFileName(NULL, path, (DWORD)pathlen);
    if (!ret || ret >= pathlen) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    TCHAR *slash = path;
    TCHAR *dot = NULL;
    for (TCHAR *cursor = path; *cursor; cursor++) {
        if (*cursor == TEXT('\\')) {
            slash = cursor;
            dot = NULL;
        } else if (*cursor == TEXT('.')) {
            dot = cursor;
        }
    }
    if (dot && dot > slash)
        *dot = TEXT('\0');
    if (lstrlen(path) + 5 > pathlen) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    lstrcat_s(path, pathlen, TEXT(".ini"));
    return TRUE;
}

static BOOL PrepareUserConfiguration(const TCHAR *sidecar, BOOL copy_sidecar)
{
    static TCHAR userpath[CONFIG_PATH_CHARS];
    DWORD ret = GetEnvironmentVariable(TEXT("APPDATA"), userpath, ARR_SZ(userpath));
    if (!ret || ret >= ARR_SZ(userpath)) {
        ret = GetEnvironmentVariable(TEXT("LOCALAPPDATA"), userpath, ARR_SZ(userpath));
        if (!ret || ret >= ARR_SZ(userpath))
            return FALSE;
    }
    if (!AppendPathPart(userpath, ARR_SZ(userpath), APP_NAME) || !EnsureDirectory(userpath)
        || !AppendPathPart(userpath, ARR_SZ(userpath), APP_NAME TEXT(".ini")))
        return FALSE;

    if (GetFileAttributes(userpath) == INVALID_FILE_ATTRIBUTES) {
        BOOL initialized = FALSE;
        if (copy_sidecar && sidecar && CopyFile(sidecar, userpath, TRUE)) {
            DWORD attributes = GetFileAttributes(userpath);
            if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_READONLY))
                SetFileAttributes(userpath, attributes & ~FILE_ATTRIBUTE_READONLY);
            initialized = TRUE;
        }
        if (!initialized && !WriteEmbeddedResource(IDR_DEFAULT_INI, userpath, FALSE))
            return FALSE;
    }
    if (!IsFileWritable(userpath))
        return FALSE;
    lstrcpy_s(inipath, ARR_SZ(inipath), userpath);
    return TRUE;
}

static BOOL PrepareConfiguration(void)
{
    static TCHAR sidecar[CONFIG_PATH_CHARS];
    if (GetSidecarIniPath(sidecar, ARR_SZ(sidecar))) {
        DWORD attributes = GetFileAttributes(sidecar);
        BOOL exists = attributes != INVALID_FILE_ATTRIBUTES
            && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
        if (exists && IsFileWritable(sidecar)) {
            lstrcpy_s(inipath, ARR_SZ(inipath), sidecar);
            return TRUE;
        }
        if (!exists && WriteEmbeddedResource(IDR_DEFAULT_INI, sidecar, FALSE)
            && IsFileWritable(sidecar)) {
            lstrcpy_s(inipath, ARR_SZ(inipath), sidecar);
            return TRUE;
        }
        return PrepareUserConfiguration(sidecar, exists);
    }
    return PrepareUserConfiguration(NULL, FALSE);
}

static BOOL WINAPI WriteConfigValue(const TCHAR *section, const TCHAR *key,
    const TCHAR *value, const TCHAR *path)
{
    if (WritePrivateProfileStringW(section, key, value, path))
        return TRUE;
    if (config_write_error == ERROR_SUCCESS) {
        config_write_error = GetLastError();
        if (config_write_error == ERROR_SUCCESS)
            config_write_error = ERROR_WRITE_FAULT;
    }
    return FALSE;
}

static void RecordConfigWriteError(DWORD error)
{
    if (config_write_error == ERROR_SUCCESS)
        config_write_error = error ? error : ERROR_WRITE_FAULT;
}

static void BeginConfigWrite(void)
{
    config_write_error = ERROR_SUCCESS;
}

static BOOL FinishConfigWrite(HWND owner)
{
    if (config_write_error == ERROR_SUCCESS)
        WritePrivateProfileStringW(NULL, NULL, NULL, inipath);
    if (config_write_error == ERROR_SUCCESS)
        return TRUE;

    TCHAR message[1024];
    int len = wsprintf(message,
        TEXT("设置无法保存，配置文件或系统启动项当前不可写。请检查磁盘空间、文件权限或安全策略。\n\n错误代码：%lu\n配置文件："),
        config_write_error);
    lstrcpyn(message + len, inipath, ARR_SZ(message) - len);
    MessageBox(owner, message, APP_NAME, MB_OK | MB_ICONERROR | MB_TOPMOST);
    return FALSE;
}

#undef WritePrivateProfileString
#define WritePrivateProfileString WriteConfigValue

// Include stuff
#include "languages.c"
#include "tray.c"
#include "config.c"
#include "windowtool.c"

static BOOL ReadHooksHash(TCHAR hash[65])
{
    const BYTE *data;
    DWORD size;
    if (!GetEmbeddedResource(IDR_HOOKS_HASH, &data, &size) || size != 64) {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }
    for (DWORD i = 0; i < size; i++) {
        if (!((data[i] >= '0' && data[i] <= '9') || (data[i] >= 'A' && data[i] <= 'F'))) {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
        hash[i] = (TCHAR)data[i];
    }
    hash[64] = TEXT('\0');
    return TRUE;
}

static BOOL PrepareHooksPath(void)
{
    TCHAR hash[65];
    DWORD ret = GetEnvironmentVariable(TEXT("LOCALAPPDATA"), hookspath, ARR_SZ(hookspath));
    if (!ret || ret >= ARR_SZ(hookspath)) {
        SetLastError(ERROR_ENVVAR_NOT_FOUND);
        return FALSE;
    }
    if (!ReadHooksHash(hash)
        || !AppendPathPart(hookspath, ARR_SZ(hookspath), APP_NAME)
        || !EnsureDirectory(hookspath)
        || !AppendPathPart(hookspath, ARR_SZ(hookspath), TEXT("runtime"))
        || !EnsureDirectory(hookspath)
        || !AppendPathPart(hookspath, ARR_SZ(hookspath), hash)
        || !EnsureDirectory(hookspath)
        || !AppendPathPart(hookspath, ARR_SZ(hookspath), TEXT("hooks.dll")))
        return FALSE;

    const BYTE *data;
    DWORD size;
    if (!GetEmbeddedResource(IDR_HOOKS_DLL, &data, &size))
        return FALSE;
    if (!FileMatchesData(hookspath, data, size)) {
        if (!WriteDataAtomically(hookspath, data, size, TRUE)
            || !FileMatchesData(hookspath, data, size))
            return FALSE;
    }
    return TRUE;
}

static HINSTANCE LoadHooksDLL(void)
{
    if (!PrepareHooksPath())
        return NULL;
    return LoadLibraryEx(hookspath, NULL,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
}

static void FreeHooksDLL(void)
{
    if (hinstDLL) {
        BOOL ret = FreeLibrary(hinstDLL);
        if (!ret) LOG("FreeLibrary() FAILED!!!");
        hinstDLL = NULL;
    }
    HooksLoad = NULL;
    HooksUnload = NULL;
    HooksKeyboardProc = NULL;
}

static void ShowHookError(const TCHAR *reason, DWORD error)
{
    if (!showerror)
        return;
    TCHAR message[1024];
    int len = wsprintf(message,
        TEXT("%s\n\n全局窗口控制当前未启用。设置仍可保存，解决问题后点击“应用”会自动重试。\n\n错误代码：%lu"),
        reason, error ? error : ERROR_GEN_FAILURE);
    if (hookspath[0] && len + 8 < ARR_SZ(message)) {
        lstrcat_s(message, ARR_SZ(message), TEXT("\n运行文件："));
        len = lstrlen(message);
        lstrcpyn(message + len, hookspath, ARR_SZ(message) - len);
    }
    MessageBox(g_cfgwnd ? g_cfgwnd : g_hwnd, message, APP_NAME,
        MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
}

static BOOL FailHookSystem(const TCHAR *reason, DWORD error)
{
    ShowHookError(reason, error);
    UpdateTray();
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
static BOOL HookSystem(void)
{
    if (keyhook)
        return TRUE;
    LOG("Going to Hook the system...");

    if (!hinstDLL) {
        hinstDLL = LoadHooksDLL();
        if (!hinstDLL) {
            DWORD error = GetLastError();
            LOG("Could not load embedded HOOKS.DLL!!!");
            return FailHookSystem(TEXT("无法释放或加载内置全局控制组件。"), error);
        }
    }

    HooksLoad = (hooks_load_proc_t)GetProcAddress(hinstDLL, LOAD_PROC);
    HooksUnload = (hooks_unload_proc_t)GetProcAddress(hinstDLL, UNLOAD_PROC);
    HooksKeyboardProc = (HOOKPROC)GetProcAddress(hinstDLL, LOW_LEVEL_KB_PROC);
    if (!HooksLoad || !HooksUnload || !HooksKeyboardProc) {
        FreeHooksDLL();
        return FailHookSystem(TEXT("内置全局控制组件不完整或版本不匹配。"), ERROR_PROC_NOT_FOUND);
    }

    G_HotKeyProc = HooksLoad(g_hwnd, inipath);
    if (!G_HotKeyProc) {
        DWORD error = GetLastError();
        HooksUnload();
        G_HotKeyProc = NULL;
        return FailHookSystem(TEXT("内置全局控制组件初始化失败。"), error);
    }
    hooks_initialized = TRUE;
    LOG("HOOKS.DLL Loaded");

    keyhook = SetWindowsHookEx(WH_KEYBOARD_LL, HooksKeyboardProc, hinstDLL, 0);
    if (!keyhook) {
        DWORD error = GetLastError();
        HooksUnload();
        hooks_initialized = FALSE;
        G_HotKeyProc = NULL;
        LOG("Keyboard HOOK could not be set");
        return FailHookSystem(TEXT("系统拒绝启动键盘全局控制。"), error);
    }
    LOG("Keyboard HOOK set");

    // Reading some config options...
    UseZones = GetPrivateProfileInt(TEXT("Zones"), TEXT("UseZones"), 0, inipath);
    SnapGap = CLAMP(-128, GetPrivateProfileInt(TEXT("Advanced"), TEXT("SnapGap"), 0, inipath), 127);
    ACMenuItems = GetPrivateProfileInt(TEXT("Advanced"), TEXT("ACMenuItems"), -1, inipath);
    UpdateTray();
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
static int UnhookSystem(void)
{
    LOG("Going to UnHook the system...");
    if (!keyhook && !hooks_initialized) {
        return 1;
    }
    if (keyhook && !UnhookWindowsHookEx(keyhook) && showerror) {
        MessageBox(NULL, l10n->MiscUnhookError, TEXT(APP_NAMEA),
                   MB_ICONINFORMATION|MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
    }
    keyhook = NULL;

    if (hooks_initialized && HooksUnload) {
        HooksUnload();
        hooks_initialized = FALSE;
    }
    G_HotKeyProc = NULL;

    // Success
    UpdateTray();
    return 0;
}
/////////////////////////////////////////////////////////////////////////////
void ToggleState(void)
{
    if (ENABLED()) {
        hook_requested = FALSE;
        UnhookSystem();
    } else {
        hook_requested = TRUE;
        HookSystem();
    }
}
/////////////////////////////////////////////////////////////////////////////
static void UpdateSettings(void)
{
    if (hook_requested) {
        UnhookSystem();
        HookSystem();
    }
}
/////////////////////////////////////////////////////////////////////////////
//
void ShowSClickMenu(HWND hwnd, LPARAM param)
{
    POINT pt;
    if (param&LP_CURSORPOS) {
        // Use cursor position to place menu.
        GetCursorPos(&pt);
    } else {
        // Use the pointed window
        RECT rc;
        HWND clickhwnd = (HWND)SendMessage(hwnd, WM_GETCLICKHWND, 0, 0);
        GetWindowRect(clickhwnd, &rc);
        pt.x = rc.left + GetSystemMetricsForWin(SM_CXSIZEFRAME, clickhwnd);
        pt.y = rc.top + GetSystemMetricsForWin(SM_CYSIZEFRAME, clickhwnd)
                      + GetSystemMetricsForWin(SM_CYCAPTION, clickhwnd);
    }
    HMENU menu = CreatePopupMenu();
    UCHAR show_oriclick = (param&LP_NOALTACTION)? AC_ORICLICK: 0xFF;

    #define CHK(LP_FLAG) MF_STRING|((param&LP_FLAG)?MF_CHECKED:MF_UNCHECKED)

    const struct {
        UCHAR action; WORD mf; TCHAR *str;
    } mnlst[] = {
       /* hide, action,      MF_FLAG/CHECKED,    menu string */
        { AC_ALWAYSONTOP, CHK(LP_TOPMOST),    l10n->InputActionAlwaysOnTop },
        { AC_BORDERLESS,  CHK(LP_BORDERLESS), l10n->InputActionBorderless },
        { AC_CENTER,      MF_STRING,          l10n->InputActionCenter},
        { AC_CENTER2,     MF_STRING,          l10n->InputActionCenter2},
        { AC_ROLL,        CHK(LP_ROLLED),     l10n->InputActionRoll},
        { AC_LOWER,       MF_STRING,          l10n->InputActionLower},
        { AC_MAXHV,       MF_STRING,          l10n->InputActionMaximizeHV},
        { AC_MINALL,      MF_STRING,          l10n->InputActionMinAllOther},
        { AC_SIDESNAP,    MF_STRING,          l10n->InputActionSideSnap},
        { 0,              MF_SEPARATOR, NULL }, /* ------------------------ */
        { AC_MAXIMIZE,    CHK(LP_MAXIMIZED),  l10n->InputActionMaximize},
        { AC_MINIMIZE,    MF_STRING,          l10n->InputActionMinimize},
        { AC_CLOSE,       MF_STRING,          l10n->InputActionClose},
        { 0,              MF_SEPARATOR, NULL }, /* ------------------------ */
        { AC_KILL,        MF_STRING,          l10n->InputActionKill},
        { 0,              MF_SEPARATOR, NULL }, /* ------------------------ */
        { AC_MOVEONOFF,   CHK(LP_MOVEONOFF),  l10n->InputActionMoveOnOff},
        { 0,              MF_SEPARATOR, NULL }, /* ------------------------ */
        { show_oriclick,  MF_STRING,          l10n->InputActionOriClick},
        { AC_NONE,        MF_STRING,          l10n->InputActionNothing},
    };
    #undef CHK
    #undef K
    unsigned i;
    for (i=0; i < ARR_SZ(mnlst); i++) {
        if ( (ACMenuItems>>i)&1 && mnlst[i].action != 0xFF) // Put the action in the HIWORD of wParam
            AppendMenu(menu, mnlst[i].mf, mnlst[i].action<<16, mnlst[i].str);
    }
    TrackPopupMenu(menu, GetSystemMetrics(SM_MENUDROPALIGNMENT), pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(menu);
    PostMessage(hwnd, WM_CLOSE, 0, 0);
}
// To get the caret position in screen coordinate.
// We first try to get the carret rect
//#include <oleacc.h>
//static const GUID  my_IID_IAccessible = { 0x618736e0, 0x3c3d, 0x11cf, {0x81, 0x0c, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71} };
static void GetKaretPos(POINT *pt)
{
    GUITHREADINFO gui;
    gui.cbSize = sizeof(gui);
    gui.hwndCaret = NULL;
    if (GetGUIThreadInfoL(0, &gui)) {
        pt->x = (gui.rcCaret.right + gui.rcCaret.left)>>1;
        pt->y = (gui.rcCaret.top + gui.rcCaret.bottom)>>1;
        if (gui.hwndCaret) {
            ClientToScreen(gui.hwndCaret, pt);
            return;
//        } else if (gui.hwndFocus) {
//            IAccessible *pacc = NULL;
//            if ( S_OK==AccessibleObjectFromWindow(gui.hwndFocus, OBJID_CARET, &my_IID_IAccessible, (void**)&pacc) ) {
//        //        MessageBox(NULL, NULL, NULL, 0);
//                LONG x=0, y=0, w=0, h=0;
//                VARIANT varCaret;
//                varCaret.vt = VT_I4;
//                varCaret.lVal = CHILDID_SELF;
//                if (S_OK == pacc->lpVtbl->accLocation(pacc, &x, &y, &w, &h, varCaret)) {
//                    pt->x = x+w/2;
//                    pt->y = y+h/2;
//                    pacc->lpVtbl->Release(pacc);
//                    return;
//                }
//            }
        }
    }

    GetCursorPos(pt);
}
static void ShowUnikeyMenu(HWND hwnd, LPARAM param)
{
    UCHAR vkey = LOBYTE(LOWORD(param));
    UCHAR capital = HIBYTE(LOWORD(param));
    TCHAR *const* const ukmap = &l10n->a; //EXTRAKEYS_MAP;
    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    const TCHAR *kl, *keylist = ukmap[(vkey - 0x41)*2];
    UCHAR i;
    for (kl = keylist, i='A'; *kl; kl++) {
        if(*kl==L'%') {
            AppendMenu(menu, MF_SEPARATOR, 0, NULL);
            continue;
        }
        TCHAR unichar = *kl;
        if (kl[1] == L'|') {
            kl+=2;
            if (capital) unichar = *kl;
        } else if (capital) {
            unichar = (TCHAR)(LONG_PTR)CharUpper((TCHAR *)(LONG_PTR)*kl);
        }
        if (i > 'Z') i = '1';
        TCHAR mwstr[6];
        mwstr[0] = L'&';
        mwstr[1] = i++;
        mwstr[2] = L'\t';
        DWORD utf16c;
        if (IS_SURROGATE_PAIR(unichar, kl[1])) {
            utf16c =*(DWORD*)kl;
            mwstr[3] = LOWORD(utf16c);
            mwstr[4] = HIWORD(utf16c);
            mwstr[5] = L'\0';
            kl++; // skip high surrogate
        } else {
            mwstr[3] = utf16c = unichar;
            mwstr[4] = L'\0';
        }
        if (!AppendMenu(menu, MF_STRING, utf16c, mwstr))
            break;
    }
    if (kl > keylist) {
        POINT pt;
        GetKaretPos(&pt);
        TrackPopupMenu(menu, GetSystemMetrics(SM_MENUDROPALIGNMENT), pt.x, pt.y, 0, hwnd, NULL);
    }
    DestroyMenu(menu);
}
/////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!msg) {
        // In case some messages are not registered.
    } else if(wParam && msg == WM_ERASEBKGND) {
        return 1;
    } else if (wParam && (msg == WM_PAINT || msg == WM_NCPAINT)) {
        return 0;
    } else if (msg == WM_TRAY) {
        if (lParam == WM_LBUTTONUP || lParam == WM_LBUTTONDBLCLK) {
            SendMessage(hwnd, WM_COMMAND, CMD_WINDOWTOOL, 0);
        } else if (lParam == WM_MBUTTONDOWN) {
            SendMessage(hwnd, WM_COMMAND, CMD_OPENINIFILE, 0);
        } else if (lParam == WM_RBUTTONUP) {
            ShowContextMenu(hwnd);
        }
    } else if (msg == WM_SCLICK && wParam) {
        ShowSClickMenu((HWND)wParam, lParam);
    } else if (msg == WM_UNIKEYMENU) {
        ShowUnikeyMenu((HWND)wParam, lParam);
    } else if (msg == WM_UPDATESETTINGS) {
        // Reload hooks
        UpdateSettings();
    } else if (msg == WM_HOOKERROR) {
        if (hook_requested) {
            DWORD error = (DWORD)wParam;
            UnhookSystem();
            ShowHookError(TEXT("系统拒绝启动鼠标全局控制。"), error);
        }
    } else if (msg == WM_ADDTRAY) {
        tray_hidden = 0;
        UpdateTray();
    } else if (msg == WM_UPDATETRAY) {
        UpdateTray();
    } else if (msg == WM_HIDETRAY) {
        tray_hidden = 1;
        RemoveTray();
    } else if (msg == WM_OPENCONFIG && (lParam || !tray_hidden)) {
        OpenConfig(wParam);
    } else if (msg == WM_CLOSECONFIG) {
        CloseConfig();
    } else if (msg == WM_TASKBARCREATED) {
        // Try to add the tray icon because explorer started.
        tray_added = 0;
        UpdateTray();
    } else if (msg == WM_COMMAND) {
        int wmId = LOWORD(wParam); // int wmEvent = HIWORD(wParam);
        if (wmId == CMD_TOGGLE) {
            ToggleState();
        } else if (wmId == CMD_HIDE) {
            tray_hidden = 1;
            RemoveTray();
        } else if (wmId == CMD_ELEVATE) {
           ElevateNow(0);
        } else if (wmId == CMD_CONFIG) {
            SendMessage(hwnd, WM_OPENCONFIG, 0, 0);
        } else if (wmId == CMD_WINDOWTOOL) {
            OpenWindowTool();
        } else if (wmId == CMD_ABOUT) {
            SendMessage(hwnd, WM_OPENCONFIG, 5, 0);
        } else if (wmId == CMD_OPENINIFILE) {
            ShellExecute(NULL, TEXT("open"), inipath, NULL, NULL, SW_SHOWNORMAL);
        } else if (wmId == CMD_EXIT) {
            DestroyWindow(hwnd);
        } else if (wmId == CMD_SAVEZONES) {
            TCHAR txt[256];
            lstrcpy_s(txt, ARR_SZ(txt), l10n->MiscZoneConfirmation);
            lstrcat_s(txt, ARR_SZ(txt), TEXT("\n\n"));
            catFullLayoutName(txt, ARR_SZ(txt), LayoutNumber);
            int ret = MessageBox(NULL, txt, TEXT(APP_NAMEA), MB_OKCANCEL);
            if (ret == IDOK) {
                UnhookSystem();
                SaveCurrentLayout();
                WriteCurrentLayoutNumber();
                if (hook_requested)
                    HookSystem();
            }
        } else if (wmId == CMD_CLOSEZONES) {
            CloseAllTestWindows();
        } else if (wmId == CMD_TESTWIN) {
            NewTestWindow();
        } else if (CMD_SNAPLAYOUT <= wmId && wmId <= CMD_SNAPLAYOUTEND) {
            // Inform hooks.dll that the snap layout changed
            LayoutNumber = wmId-CMD_SNAPLAYOUT;
            if(G_HotKeyProc)
                G_HotKeyProc(hwnd, WM_SETLAYOUTNUM, LayoutNumber, 0);
            // Save new value in the .ini file
            WriteCurrentLayoutNumber();
        } else if (wmId == CMD_EDITLAYOUT) {
            if (G_HotKeyProc) {
                RECT *zones = NULL;
                unsigned len = G_HotKeyProc(hwnd, WM_GETZONES, LayoutNumber, (LPARAM)&zones);
                if (!len || !zones) {
                    // Empty layout, Let's open a new Test Window
                    return !NewTestWindow();
                }
                // Open them from bottom to top to ensure
                // the windows are in the correct order.
                while (len--) {
                    const RECT *rc = &zones[len];
                    NewTestWindowAt(rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top);
                }
            }
        }
    } else if (msg == WM_QUERYENDSESSION) {
        showerror = 0;
        UnhookSystem();
    } else if (msg == WM_DESTROY) {
        DestroyWindowTool();
        UnhookSystem();
        RemoveTray();
        PostQuitMessage(0);
    } else if (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) {
        // Hide cursorwnd if clicked on, this might happen if
        // it wasn't hidden by hooks.c for some reason
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    } else if (msg == WM_DISPLAYCHANGE || (msg == WM_SETTINGCHANGE && wParam  == SPI_SETWORKAREA)) {
        LOG("WM_DISPLAYCHANGE %d:%d, %dbpp in WindowProc", LOWORD(lParam), HIWORD(lParam), wParam );
        if (G_HotKeyProc) {
            int bestlayout = G_HotKeyProc(hwnd, WM_GETBESTLAYOUT, 0, 0);
            if( bestlayout != LayoutNumber
            &&  0 <= bestlayout && bestlayout < MaxLayouts ) {
                LayoutNumber = bestlayout;
                G_HotKeyProc(hwnd, WM_SETLAYOUTNUM, LayoutNumber, 0);
            }
        }
    } else if (msg == WM_HOTKEY || msg == WM_STACKLIST) {
        if (G_HotKeyProc)
            return G_HotKeyProc(hwnd, msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
int WINAPI tWinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, TCHAR *params, int iCmdShow)
{
    g_hinst = hInst;
    SetThreadUILanguage(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED));

    // Select a writable portable or per-user configuration file.
    LOG("\n\n" APP_NAMEA " STARTED");
    if (!PrepareConfiguration()) {
        DWORD error = GetLastError();
        TCHAR message[512];
        wsprintf(message,
            TEXT("无法创建可写配置文件，程序不能安全启动。\n\n请检查当前目录和用户数据目录权限。\n错误代码：%lu"),
            error ? error : ERROR_WRITE_FAULT);
        MessageBox(NULL, message, APP_NAME, MB_OK | MB_ICONERROR);
        return 0;
    }
    LOG("ini file: %S", inipath);

    // Read parameters on command line
    int help    = !!lstrstr(params, TEXT("-help"))
               || !!lstrstr(params, TEXT("/?"))
               || !!lstrstr(params, TEXT("-?"));
    if (help) {
        static const TCHAR *txthelp =
            TEXT(APP_NAMEA " 命令行选项：\n\n")
            TEXT("--help\t显示此帮助\n")
            TEXT("-h\t隐藏通知区域图标\n")
            TEXT("-q\t静默模式\n")
            TEXT("-m\t允许多个实例\n")
            TEXT("-c\t打开设置窗口\n")
            TEXT("-e\t以管理员权限运行\n")
            TEXT("-r\t重新加载设置\n")
            TEXT("-lX\t选择编号为 X 的吸附布局\n")
            TEXT("-afX\t对前台窗口执行动作 X\n")
            TEXT("-apX\t对指针所在窗口执行动作 X\n");

        MessageBox(NULL, txthelp, TEXT(APP_NAMEA)TEXT(" 使用方法"), MB_OK|MB_ICONINFORMATION);
        return 0;
    }

    tray_hidden = !!lstrstr(params, TEXT("-h"));
    int quiet   = !!lstrstr(params, TEXT("-q"));
    int elevate = !!lstrstr(params, TEXT("-e"));
    int multi   = !!lstrstr(params, TEXT("-m"));
    int config  = !!lstrstr(params, TEXT("-c"));
    int rlini   = !!lstrstr(params, TEXT("-r"));

    // Check if elevated if in >= WinVer
    WinVer = LOBYTE(LOWORD(GetVersion()));
    LOG("Running with Windows version %lX", GetVersion());
    #ifndef NO_VISTA
    if (WinVer >= VISTA) {
        HANDLE token=NULL;
        TOKEN_ELEVATION elevation={0};
        DWORD len=0;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &token)
        && GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &len)) {
            elevated = elevation.TokenIsElevated;
            CloseHandle(token);
        }
        LOG("Process started %s elevated", elevated? "already": "non");
    }
    #endif // NO_VISTA
    LOG("Command line parameters read, hide=%d, quiet=%d, elevate=%d, multi=%d, config=%d"
                                     , tray_hidden, quiet, elevate, multi, config);

    // Look for previous instance
    if (!multi && !GetPrivateProfileInt(TEXT("Advanced"), TEXT("MultipleInstances"), 0, inipath)){
        if (quiet) return 0;

        HWND previnst = FindWindow(TEXT(APP_NAMEA), TEXT(""));
        if (previnst) {
            // Ask old HotKey window to perform an action.
            const TCHAR *actionstr = lstrstr(params, TEXT("-a"));
            if (actionstr && actionstr[2] && actionstr[3] && actionstr[4]) {
                action_t action = MapActionW(&actionstr[3]);
                PostMessage(previnst, WM_HOTKEY, (actionstr[2] == 'p')*0x1000+action.ac, PACK_ACTION(action));
                return 0;
            }
            // Change layout if asked...
            #define isUDigit(x) ( TEXT('0') <= (x) && (x) <= TEXT('9') )
            const TCHAR *layout = lstrstr(params, TEXT("-l"));
            if (layout && isUDigit(layout[2])) {
                TCHAR numstr[3] = { layout[2], layout[3], TEXT('\0') };
                if (!isUDigit(numstr[1]))
                    numstr[1] = TEXT('\0');
                //MessageBox(NULL, NULL, NULL, 0);
                int layoutnumber = CLAMP(0, strtoi(numstr)-1, 9);
                PostMessage(previnst, WM_COMMAND, CMD_SNAPLAYOUT+layoutnumber, 0);
                return 0;
            }
            // Update old instance if no action to be made.
            LOG("Previous instance found and no -multi mode");
            if(tray_hidden) PostMessage(previnst, WM_CLOSECONFIG, 0, 0);
            if(config) PostMessage(previnst, WM_OPENCONFIG, 0, 0);
            if(rlini)  PostMessage(previnst, WM_UPDATESETTINGS, 0, 0);
            if(!tray_hidden) PostMessage(previnst, WM_COMMAND, CMD_WINDOWTOOL, 0);
            PostMessage(previnst, tray_hidden? WM_HIDETRAY : WM_ADDTRAY, 0, 0);
            LOG("Updated old instance and NORMAL EXIT");
            return 0;
        }
        LOG("No previous instance found");
    }

    // Check AlwaysElevate (Vista+ only)
    if (WinVer >= 6 && !elevated) {
        if(!elevate) elevate = GetPrivateProfileInt(TEXT("Advanced"), TEXT("AlwaysElevate"), 0, inipath);

        // Handle request to elevate to administrator privileges
        if (elevate) {
            LOG("Elevation requested");
            TCHAR path[MAX_PATH];
            GetModuleFileName(NULL, path, ARR_SZ(path));
            HINSTANCE ret = ShellExecute(NULL, TEXT("runas"), path, (tray_hidden? TEXT("-h"): NULL), NULL, SW_SHOWNORMAL);
            if ((DorQWORD)ret > 32) {
                LOG("Elevation Faild => Not cool NORMAL EXIT");
                return 0;
            }
            LOG("Elevation sucess");
        } else {
            LOG("No Elevation requested");
        }
    }
    if (!UpdateLanguage()) {
        MessageBox(NULL, TEXT("无法加载内置简体中文资源，程序将退出。"),
            TEXT(APP_NAMEA), MB_OK | MB_ICONERROR);
        return 0;
    }
    LOG("Simplified Chinese language loaded");

    // Create window
    WNDCLASS wnd =
        { 0
        , WindowProc, 0, 0, hInst, NULL, NULL
        , NULL, NULL, TEXT(APP_NAMEA) };
    RegisterClass(&wnd);
    g_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST| WS_EX_TRANSPARENT
                            , wnd.lpszClassName , NULL , WS_POPUP
                            , 0, 0, 0, 0, NULL, NULL, hInst, NULL);
    LOG("Create main APP Window: %s", g_hwnd? "Sucess": "Failed");
    if (elevated) {
        // AltSnap was started elevated!
        // Allow some messages to be sent from non-elevated instance
        // so that user can do AltSnap.exe -c/r/h/l
        UINT i;
        for (i = WM_UPDATETRAY; i <= WM_HIDETRAY; i++)
            ChangeWindowMessageFilterExL(g_hwnd, i, /*MSGFLT_ALLOW*/1, NULL);
    }
    // Tray icon

    InitTray();
    UpdateTray();

    // Hook system
    HookSystem();

    if (!tray_hidden)
        OpenWindowTool();

    // Add tray if hook failed, even though -hide was supplied
    if (tray_hidden && !keyhook) {
        tray_hidden = 0;
        UpdateTray();
    }
    // Open config if -config was supplied
    if (config) {
        PostMessage(g_hwnd, WM_OPENCONFIG, 0, 0);
    }
    // Message loop
    LOG("Starting "APP_NAMEA" message loop...");
    BOOL ret;
    MSG msg;
    while ((ret = GetMessage( &msg, NULL, 0, 0 )) != 0) {
        if (ret == -1) {
            break;
        } else if (IsWindow(g_toolwnd) && IsDialogMessage(g_toolwnd, &msg)) {
            continue;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnhookSystem();
    FreeHooksDLL();
    DestroyWindow(g_hwnd);
    LOG("GOOD NORMAL EXIT");
    FreeAllLangRelated();

    CHECK_MEMORY_LEAK_DB();

    return (int)msg.wParam;
}
static pure const TCHAR *ParamsFromCmdline(const TCHAR *cmdl)
{
    /* in case it starts with " we need to go to the next " */
    if (cmdl[0] == TEXT('"')) {
        do {
            cmdl++;
        } while(*cmdl && *cmdl != TEXT('"'));
    } else {
        while(*cmdl && *cmdl != TEXT(' ') && *cmdl != TEXT('\t')) {
            cmdl++;
        }
    }
    cmdl += !!*cmdl; // Skip the " or the ' '
    while(*cmdl == TEXT(' ') || *cmdl == TEXT('\t')) cmdl++;
    return cmdl;
}

/////////////////////////////////////////////////////////////////////////////
// Use -nostdlib and -e_unfuckMain@0 to use this main, -eunfuckMain for x64.
#ifdef _MSC_VER
#pragma comment(linker, "/entry:\"unfuckWinMain\"")
#endif
void noreturn WINAPI unfuckWinMain(void)
{
    HINSTANCE hInst;
    HINSTANCE hPrevInstance = NULL;
    const TCHAR *szCmdLine;
    int iCmdShow = 0;

    hInst = GetModuleHandle(NULL);
    szCmdLine = ParamsFromCmdline(GetCommandLine());

    ExitProcess((UINT)tWinMain(hInst, hPrevInstance, (TCHAR *)szCmdLine, iCmdShow));
}
