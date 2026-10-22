/*
 * Window transparency tool added for the ShengZiMao distribution in 2026.
 * This file is part of the GPL-3.0-or-later AltSnap derivative.
 */

#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER 0x00010000
#endif

#define WM_WT_PICKED (WM_APP + 37)
#define WT_MIN_OPACITY 10
#define WT_SNAPSHOT_COUNT 128
#define WT_TARGET_COUNT 512
#define WT_MARKER_COUNT 1024

typedef struct wt_snapshot {
    HWND hwnd;
    DWORD pid;
    DWORD tid;
    ATOM class_atom;
    HANDLE generation;
    UCHAR had_layered;
    UCHAR attributes_valid;
    UCHAR alpha_owned;
    UCHAR force_reinitialized;
    COLORREF color_key;
    BYTE alpha;
    DWORD flags;
} wt_snapshot_t;

typedef struct wt_target {
    HWND hwnd;
    DWORD pid;
    DWORD tid;
    ATOM class_atom;
    HANDLE generation;
} wt_target_t;

typedef struct wt_marker {
    HWND hwnd;
    HANDLE generation;
} wt_marker_t;

typedef struct wt_secure_mirror {
    HWND hwnd;
    HTHUMBNAIL thumbnail;
    HWND target;
    DWORD pid;
    DWORD tid;
    ATOM class_atom;
    HANDLE generation;
    RECT visible_rect;
    BOOL target_topmost;
    int guarded;
} wt_secure_mirror_t;

typedef struct wt_strings {
    const TCHAR *title;
    const TCHAR *select_window;
    const TCHAR *untitled_window;
    const TCHAR *unknown_program;
    const TCHAR *unsupported;
    const TCHAR *yes;
    const TCHAR *no;
    const TCHAR *columns[4];
    const TCHAR *refresh;
    const TCHAR *activate;
    const TCHAR *topmost;
    const TCHAR *settings;
    const TCHAR *exit;
    const TCHAR *opacity;
    const TCHAR *restore;
    const TCHAR *force_transparency;
    const TCHAR *capture_protection;
    const TCHAR *secure_mirror_start;
    const TCHAR *secure_mirror_stop;
    const TCHAR *global_enabled;
    const TCHAR *global_paused;
    const TCHAR *special_layered;
    const TCHAR *no_windows;
    const TCHAR *no_top_windows;
    const TCHAR *picker_tip;
    const TCHAR *picker_cancelled;
    const TCHAR *picker_start_failed;
    const TCHAR *picker_capture_failed;
    const TCHAR *picker_active;
    const TCHAR *picker_selected;
    const TCHAR *list_refreshed;
    const TCHAR *opacity_updated;
    const TCHAR *change_failed;
    const TCHAR *restored;
    const TCHAR *restore_failed;
    const TCHAR *topmost_failed;
    const TCHAR *topmost_on;
    const TCHAR *topmost_off;
    const TCHAR *capture_protection_on;
    const TCHAR *capture_protection_off;
    const TCHAR *capture_protection_failed;
    const TCHAR *secure_mirror_started;
    const TCHAR *secure_mirror_stopped;
    const TCHAR *secure_mirror_unsupported;
    const TCHAR *secure_mirror_failed;
    const TCHAR *secure_mirror_lost;
    const TCHAR *secure_mirror_foreign;
    const TCHAR *subtitle;
    const TCHAR *card_windows;
    const TCHAR *picker;
    const TCHAR *settings_title;
    const TCHAR *settings_subtitle;
    const TCHAR *settings_hint;
    const TCHAR *settings_applied;
    const TCHAR *secure_mirror_placeholder;
    const TCHAR *settings_failed;
    const TCHAR *apply;
    const TCHAR *done;
} wt_strings_t;

static const wt_strings_t g_wt_zh = {
    TEXT("圣子猫透明窗体"), TEXT("请选择一个窗口"), TEXT("（无标题窗口）"),
    TEXT("未知程序"), TEXT("不支持"), TEXT("是"), TEXT("否"),
    { TEXT("窗口"), TEXT("程序"), TEXT("不透明度"), TEXT("置顶") },
    TEXT("刷新"), TEXT("激活窗口"), TEXT("窗口置顶"), TEXT("设置"), TEXT("退出"),
    TEXT("不透明度"), TEXT("恢复原状态"), TEXT("强制透明（特殊窗口）"),
    TEXT("防录屏（本程序窗口）"),
    TEXT("防录屏（保护此窗口）"), TEXT("停止防录屏"),
    TEXT("全局控制已启用"), TEXT("全局控制已暂停"),
    TEXT("特殊窗口默认受保护；启用强制透明可能导致显示异常"),
    TEXT("未找到可调整的窗口"), TEXT("刷新后仍没有可调整的顶层窗口"),
    TEXT("准星选窗"), TEXT("已取消准星选窗"), TEXT("无法启动准星选窗"),
    TEXT("无法捕获鼠标，准星选窗未启动"),
    TEXT("准星选窗已启动，右键或按 Esc 可取消"), TEXT("已通过准星选择窗口"),
    TEXT("窗口列表已刷新"), TEXT("透明度已更新"),
    TEXT("修改失败，目标窗口可能需要更高权限"), TEXT("已恢复到修改前状态"),
    TEXT("没有可恢复的状态，或目标窗口需要更高权限"),
    TEXT("置顶修改失败，目标窗口可能需要更高权限"),
    TEXT("窗口已置顶"), TEXT("窗口已取消置顶"),
    TEXT("防录屏已开启"), TEXT("防录屏已关闭"),
    TEXT("无法开启防录屏：当前 Windows 或录屏方式不支持"),
    TEXT("防录屏已开启：录制与截图中不会出现该窗口的画面"),
    TEXT("防录屏已关闭"),
    TEXT("防录屏需要 Windows 10 2004 或更高版本"),
    TEXT("无法开启防录屏：当前 Windows 不支持该录屏排除方式"),
    TEXT("防录屏已停止：目标窗口已关闭或不再可用"),
    TEXT("无法开启防录屏：该窗口拒绝更改录屏属性，或需要更高权限"),
    TEXT("选择目标窗口后拖动滑块即可调整"),
    TEXT("目标窗口"),
    TEXT("准星选窗"),
    TEXT("设置"),
    TEXT("调整窗口移动、缩放、不透明度与全局快捷操作"),
    TEXT("更改会写入配置文件，点击“应用”或“完成”后生效"),
    TEXT("设置已保存"),
    TEXT("受保护的窗口（录屏中不显示内容）"),
    TEXT("无法保存设置，请检查配置文件是否可写"),
    TEXT("应用"),
    TEXT("完成")
};

static HWND g_toolwnd = NULL;
static HWND g_pickerwnd = NULL;
static HWND g_wt_tooltip = NULL;
static UCHAR g_picker_finishing = 0;
static wt_snapshot_t g_wt_snapshots[WT_SNAPSHOT_COUNT];
static wt_target_t g_wt_targets[WT_TARGET_COUNT];
static wt_marker_t g_wt_markers[WT_MARKER_COUNT];
static wt_secure_mirror_t g_wt_secure_mirror;
static size_t g_wt_target_count = 0;
static UINT_PTR g_wt_generation = 0;
static TCHAR g_wt_generation_property[64];
static const wt_strings_t *g_wt_text = &g_wt_zh;
static HWND g_wt_force_target = NULL;
static BOOL g_wt_force_enabled = FALSE;
static BOOL g_wt_capture_protection = TRUE;
static HICON g_wt_icon_big = NULL;
static HICON g_wt_icon_small = NULL;

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

static void WTSecureMirrorStop(BOOL report_status);
static void WTSecureMirrorFollowTarget(void);
static void WTSetStatus(HWND hwnd, const TCHAR *text);
static HANDLE WTEnsureGeneration(HWND hwnd);
static BOOL WTGetIdentity(HWND hwnd, DWORD *pid, DWORD *tid, ATOM *class_atom);
static BOOL WTIdentityMatches(HWND hwnd, DWORD pid, DWORD tid, ATOM class_atom,
    HANDLE generation);
static void WTSnapListHeight(HWND hwnd);
static void WTRefreshWindows(HWND hwnd, HWND preferred);

/* ------------------------------------------------------------------------
 * Flat card painting for the transparency panel.  Everything is plain GDI so
 * the executable keeps importing only the libraries it already links against.
 */
#define WT_CLR_BG      RGB(245, 247, 250)
#define WT_CLR_CARD    RGB(255, 255, 255)
#define WT_CLR_BORDER  RGB(224, 229, 236)
#define WT_CLR_TEXT    RGB(26, 31, 39)
#define WT_CLR_DIM     RGB(112, 121, 133)
#define WT_CLR_ACCENT  RGB(45, 108, 240)
#define WT_CLR_OK      RGB(32, 164, 105)
#define WT_CLR_WARN    RGB(206, 145, 24)
#define WT_CLR_IDLE    RGB(158, 166, 177)

typedef struct wt_theme {
    UINT dpi;
    HBRUSH bg_brush;
    HBRUSH card_brush;
    HPEN border_pen;
    HFONT body_font;
    HFONT title_font;
    HFONT value_font;
    HFONT percent_font;
    HFONT small_font;
    HFONT section_font;
    HICON logo;
    HIMAGELIST row_images;
    RECT header_rect;
    RECT card1_rect;
    RECT card2_rect;
    RECT footer_rect;
    int picker_w;
    int refresh_w;
    int activate_w;
    int settings_w;
    int exit_w;
    int reset_w;
    int mirror_w;
    int topmost_w;
    int force_w;
    int capture_w;
    int row_height;
} wt_theme_t;

static wt_theme_t g_wt_theme;

static int WTS(int value)
{
    UINT dpi = g_wt_theme.dpi ? g_wt_theme.dpi : 96;
    return MulDiv(value, (int)dpi, 96);
}

static HFONT WTCreateFont(int points, int weight)
{
    LOGFONTW lf;
    mem00(&lf, sizeof(lf));
    lf.lfHeight = -MulDiv(points, (int)(g_wt_theme.dpi ? g_wt_theme.dpi : 96), 72);
    lf.lfWeight = weight;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    lstrcpyW(lf.lfFaceName, L"Microsoft YaHei UI");
    return CreateFontIndirectW(&lf);
}

static void WTReleaseTheme(void)
{
    if (g_wt_theme.bg_brush)
        DeleteObject(g_wt_theme.bg_brush);
    if (g_wt_theme.card_brush)
        DeleteObject(g_wt_theme.card_brush);
    if (g_wt_theme.border_pen)
        DeleteObject(g_wt_theme.border_pen);
    if (g_wt_theme.body_font)
        DeleteObject(g_wt_theme.body_font);
    if (g_wt_theme.title_font)
        DeleteObject(g_wt_theme.title_font);
    if (g_wt_theme.value_font)
        DeleteObject(g_wt_theme.value_font);
    if (g_wt_theme.percent_font)
        DeleteObject(g_wt_theme.percent_font);
    if (g_wt_theme.small_font)
        DeleteObject(g_wt_theme.small_font);
    if (g_wt_theme.section_font)
        DeleteObject(g_wt_theme.section_font);
    if (g_wt_theme.logo)
        DestroyIcon(g_wt_theme.logo);
    if (g_wt_theme.row_images)
        ImageList_Destroy(g_wt_theme.row_images);
    mem00(&g_wt_theme, sizeof(g_wt_theme));
}

static void WTInitializeTheme(HWND hwnd)
{
    WTReleaseTheme();

    g_wt_theme.dpi = ReallyGetDpiForWindow(hwnd);
    if (!g_wt_theme.dpi)
        g_wt_theme.dpi = 96;

    g_wt_theme.bg_brush = CreateSolidBrush(WT_CLR_BG);
    g_wt_theme.card_brush = CreateSolidBrush(WT_CLR_CARD);
    g_wt_theme.border_pen = CreatePen(PS_SOLID, max(1, WTS(1)), WT_CLR_BORDER);
    g_wt_theme.body_font = WTCreateFont(10, FW_NORMAL);
    g_wt_theme.title_font = WTCreateFont(13, FW_SEMIBOLD);
    g_wt_theme.value_font = WTCreateFont(11, FW_SEMIBOLD);
    g_wt_theme.percent_font = WTCreateFont(15, FW_BOLD);
    g_wt_theme.small_font = WTCreateFont(9, FW_NORMAL);
    g_wt_theme.section_font = WTCreateFont(10, FW_SEMIBOLD);
    g_wt_theme.logo = (HICON)LoadImage(g_hinst, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON,
        WTS(32), WTS(32), LR_DEFAULTCOLOR);
}

static int WTTextWidth(HWND hwnd, HFONT font, const TCHAR *text)
{
    HDC dc = GetDC(hwnd);
    if (!dc)
        return WTS(48);

    HGDIOBJ old = SelectObject(dc, font ? font : g_wt_theme.body_font);
    SIZE size;
    mem00(&size, sizeof(size));
    GetTextExtentPoint32(dc, text, lstrlen(text), &size);
    SelectObject(dc, old);
    ReleaseDC(hwnd, dc);
    return size.cx;
}

static int WTControlWidth(HWND hwnd, const TCHAR *text, int minimum, int padding)
{
    return max(WTS(minimum), WTTextWidth(hwnd, g_wt_theme.body_font, text) + WTS(padding));
}

static void WTSetControlFonts(HWND hwnd)
{
    static const int body_ids[] = {
        IDC_WT_LIST, IDC_WT_REFRESH, IDC_WT_PICKER, IDC_WT_ACTIVATE, IDC_WT_TOPMOST,
        IDC_WT_SETTINGS, IDC_WT_EXIT, IDC_WT_RESET, IDC_WT_SECURE_MIRROR,
        IDC_WT_FORCE, IDC_WT_CAPTURE_PROTECTION, IDC_WT_OPACITY_LABEL, IDC_WT_STATUS
    };
    for (size_t i = 0; i < ARR_SZ(body_ids); i++) {
        HWND control = GetDlgItem(hwnd, body_ids[i]);
        if (control)
            SendMessage(control, WM_SETFONT, (WPARAM)g_wt_theme.body_font, TRUE);
    }

    SendMessage(GetDlgItem(hwnd, IDC_WT_SELECTED), WM_SETFONT,
        (WPARAM)g_wt_theme.value_font, TRUE);
    SendMessage(GetDlgItem(hwnd, IDC_WT_PERCENT), WM_SETFONT,
        (WPARAM)g_wt_theme.percent_font, TRUE);
}

/* A one pixel wide, row-high image list is the supported way to raise the
 * height of list view rows without owner drawing the whole control. */
static void WTApplyListMetrics(HWND hwnd)
{
    HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
    if (!list)
        return;

    if (g_wt_theme.row_images) {
        ListView_SetImageList(list, NULL, LVSIL_SMALL);
        ImageList_Destroy(g_wt_theme.row_images);
        g_wt_theme.row_images = NULL;
    }

    int row_height = max(WTS(24), WTS(20));
    g_wt_theme.row_height = row_height;
    HIMAGELIST images = ImageList_Create(1, row_height, ILC_COLOR24, 1, 1);
    if (!images)
        return;

    HDC dc = GetDC(hwnd);
    HDC memory = dc ? CreateCompatibleDC(dc) : NULL;
    HBITMAP bitmap = dc ? CreateCompatibleBitmap(dc, 1, row_height) : NULL;
    if (dc)
        ReleaseDC(hwnd, dc);
    if (memory && bitmap) {
        HGDIOBJ old = SelectObject(memory, bitmap);
        RECT fill = { 0, 0, 1, row_height };
        HBRUSH brush = CreateSolidBrush(WT_CLR_CARD);
        FillRect(memory, &fill, brush);
        DeleteObject(brush);
        SelectObject(memory, old);
        ImageList_Add(images, bitmap, NULL);
    }
    if (bitmap)
        DeleteObject(bitmap);
    if (memory)
        DeleteDC(memory);

    ListView_SetImageList(list, images, LVSIL_SMALL);
    g_wt_theme.row_images = images;
}

static void WTDrawCard(HDC dc, const RECT *rect)
{
    HGDIOBJ old_brush = SelectObject(dc, g_wt_theme.card_brush);
    HGDIOBJ old_pen = SelectObject(dc, g_wt_theme.border_pen);
    int radius = max(4, WTS(9));
    RoundRect(dc, rect->left, rect->top, rect->right, rect->bottom, radius * 2, radius * 2);
    SelectObject(dc, old_pen);
    SelectObject(dc, old_brush);
}

static void WTDrawText(HDC dc, HFONT font, COLORREF color, int x, int y, int width,
    int height, const TCHAR *text, UINT flags)
{
    RECT rect = { x, y, x + width, y + height };
    HGDIOBJ old = SelectObject(dc, font);
    SetTextColor(dc, color);
    DrawText(dc, text, -1, &rect, flags | DT_NOPREFIX);
    SelectObject(dc, old);
}

static void WTDrawStatusDot(HDC dc, int x, int y, int diameter, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ old_brush = SelectObject(dc, brush);
    HGDIOBJ old_pen = SelectObject(dc, GetStockObject(NULL_PEN));
    Ellipse(dc, x, y, x + diameter, y + diameter);
    SelectObject(dc, old_pen);
    SelectObject(dc, old_brush);
    DeleteObject(brush);
}

/* ------------------------------------------------------------------------
 * In-window settings.  The six AltSnap preference pages are created as child
 * dialogs of this panel instead of a property sheet, so the whole settings
 * experience stays inside the one window.
 */
enum { WT_PAGE_COUNT = 6 };

/* Layout constants, in 96 dpi pixels; every use goes through WTS(). */
#define WT_PAD      14
#define WT_GAP      10
#define WT_HEADER   44
#define WT_FOOTER   20
#define WT_ACTION   52
#define WT_CARD_IN  12
#define WT_NAV_ITEM 32

typedef struct wt_page_def {
    int template_id;
    DLGPROC proc;
    int nav_id;
} wt_page_def_t;

static const wt_page_def_t g_wt_page_defs[WT_PAGE_COUNT] = {
    { IDD_GENERALPAGE,   (DLGPROC)GeneralPageDialogProc,   IDC_WT_NAV_GENERAL },
    { IDD_MOUSEPAGE,     (DLGPROC)MousePageDialogProc,     IDC_WT_NAV_MOUSE },
    { IDD_KBPAGE,        (DLGPROC)KeyboardPageDialogProc,  IDC_WT_NAV_KEYBOARD },
    { IDD_BLACKLISTPAGE, (DLGPROC)BlacklistPageDialogProc, IDC_WT_NAV_BLACKLIST },
    { IDD_ADVANCEDPAGE,  (DLGPROC)AdvancedPageDialogProc,  IDC_WT_NAV_ADVANCED },
    { IDD_ABOUTPAGE,     (DLGPROC)AboutPageDialogProc,     IDC_WT_NAV_ABOUT },
};

typedef struct wt_settings_state {
    BOOL open;
    int page;
    HWND nav[WT_PAGE_COUNT];
    HWND pages[WT_PAGE_COUNT];
    HWND apply;
    HWND done;
    int page_width;
    int page_height;
    HWND host;
    int offset;
    int restore_width;
    int restore_height;
    RECT host_rect;
    RECT nav_rect;
    RECT content_rect;
    RECT action_rect;
} wt_settings_state_t;

static wt_settings_state_t g_wt_set;
static WNDPROC g_wt_page_orig[WT_PAGE_COUNT];

static void WTLayout(HWND hwnd);
static int WTMinimumWidth(void);
static int WTMinimumHeight(void);
static void WTApplySectionFonts(HWND page);
static LRESULT CALLBACK WTPageHostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void WTPositionSettingsPage(HWND hwnd);

static const TCHAR *WTNavLabel(int index)
{
    if (!l10n)
        return TEXT("");
    switch (index) {
    case 0:  return l10n->ConfigTabGeneral;
    case 1:  return l10n->ConfigTabMouse;
    case 2:  return l10n->ConfigTabKeyboard;
    case 3:  return l10n->ConfigTabBlacklist;
    case 4:  return l10n->ConfigTabAdvanced;
    default: return l10n->ConfigTabAbout;
    }
}

static int WTSettingsNavWidth(HWND hwnd)
{
    int width = WTS(96);
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        width = max(width, WTTextWidth(hwnd, g_wt_theme.body_font, WTNavLabel(i)) + WTS(50));
    }
    return width;
}

/* The preference pages were written for a property sheet, so the sheet turns
 * out to be the only thing that colours them.  Paint them on the card instead. */
static LRESULT CALLBACK WTPageFrameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_ERASEBKGND) {
        RECT client;
        GetClientRect(hwnd, &client);
        FillRect((HDC)wParam, &client, g_wt_theme.card_brush);
        return TRUE;
    }
    if (msg == WM_CTLCOLORSTATIC || msg == WM_CTLCOLORBTN || msg == WM_CTLCOLORDLG) {
        SetBkMode((HDC)wParam, TRANSPARENT);
        SetTextColor((HDC)wParam, WT_CLR_TEXT);
        return (LRESULT)g_wt_theme.card_brush;
    }
    if (msg == WM_MOUSEWHEEL && g_wt_set.host) {
        SendMessage(g_wt_set.host, WM_MOUSEWHEEL, wParam, lParam);
        return 0;
    }

    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.pages[i] == hwnd && g_wt_page_orig[i])
            return CallWindowProc(g_wt_page_orig[i], hwnd, msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void WTSettingsShutdown(void);

/* The section captions are plain LTEXT controls now, so give them the bold
 * heading font; every other control keeps the dialog font from the template. */
static void WTApplySectionFonts(HWND page)
{
    static const struct { int id; } headings[] = {
        { IDC_GENERAL_BOX }, { IDC_RESIZECENTER }, { IDC_AUTOSTART_BOX }, { IDC_AUTOSNAP_HEADER },
        { IDC_MOUSE_BOX }, { IDC_TTBACTIONS_BOX }, { IDC_HOTCLICKS_BOX },
        { IDC_KEYBOARD_BOX }, { IDC_HOTKEYS_BOX },
        { IDC_BLACKLIST_BOX }, { IDC_FINDWINDOW_BOX },
        { IDC_METRICS_BOX }, { IDC_BEHAVIOR_BOX },
        { IDC_ABOUT_BOX }, { IDC_TRANSLATIONS_BOX },
    };

    for (size_t i = 0; i < ARR_SZ(headings); i++) {
        HWND control = GetDlgItem(page, headings[i].id);
        if (control && g_wt_theme.section_font)
            SendMessage(control, WM_SETFONT, (WPARAM)g_wt_theme.section_font, TRUE);
    }
}

static BOOL WTSettingsCreate(HWND hwnd)
{
    if (g_wt_set.nav[0] && IsWindow(g_wt_set.nav[0])
    &&  GetParent(g_wt_set.nav[0]) == hwnd)
        return TRUE;

    mem00(&g_wt_set, sizeof(g_wt_set));
    mem00(g_wt_page_orig, sizeof(g_wt_page_orig));

    /* From here on the pages must not talk to a property sheet. */
    g_cfgwnd = NULL;
    g_wt_inline_page = NULL;

    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        HWND page = CreateDialog(g_hinst, MAKEINTRESOURCE(g_wt_page_defs[i].template_id),
            hwnd, g_wt_page_defs[i].proc);
        g_wt_set.pages[i] = page;
        if (!page)
            continue;

        LONG_PTR style = GetWindowLongPtr(page, GWL_STYLE);
        SetWindowLongPtr(page, GWL_STYLE, style & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME));
        SetWindowPos(page, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        RECT rect;
        if (GetWindowRect(page, &rect)) {
            g_wt_set.page_width = max(g_wt_set.page_width, rect.right - rect.left);
            g_wt_set.page_height = max(g_wt_set.page_height, rect.bottom - rect.top);
        }
        WTApplySectionFonts(page);
        g_wt_page_orig[i] = (WNDPROC)SetWindowLongPtr(page, GWLP_WNDPROC, (LONG_PTR)WTPageFrameProc);
        ShowWindow(page, SW_HIDE);
    }

    WNDCLASS host_class;
    mem00(&host_class, sizeof(host_class));
    host_class.lpfnWndProc = WTPageHostProc;
    host_class.hInstance = g_hinst;
    host_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    host_class.lpszClassName = TEXT(APP_NAMEA) TEXT("-pagehost");
    RegisterClass(&host_class);
    g_wt_set.host = CreateWindowEx(0, host_class.lpszClassName, NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL,
        0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.pages[i] && g_wt_set.host)
            SetParent(g_wt_set.pages[i], g_wt_set.host);
    }

    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        g_wt_set.nav[i] = CreateWindowEx(0, TEXT("Button"), WTNavLabel(i),
            WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | BS_OWNERDRAW,
            0, 0, 0, 0, hwnd, (HMENU)(UINT_PTR)g_wt_page_defs[i].nav_id, g_hinst, NULL);
        if (g_wt_set.nav[i])
            SendMessage(g_wt_set.nav[i], WM_SETFONT, (WPARAM)g_wt_theme.body_font, TRUE);
    }

    g_wt_set.apply = CreateWindowEx(0, TEXT("Button"), g_wt_text->apply,
        WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)(UINT_PTR)IDC_WT_APPLY, g_hinst, NULL);
    g_wt_set.done = CreateWindowEx(0, TEXT("Button"), g_wt_text->done,
        WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
        0, 0, 0, 0, hwnd, (HMENU)(UINT_PTR)IDC_WT_DONE, g_hinst, NULL);
    if (g_wt_set.apply)
        SendMessage(g_wt_set.apply, WM_SETFONT, (WPARAM)g_wt_theme.body_font, TRUE);
    if (g_wt_set.done) {
        SendMessage(g_wt_set.done, WM_SETFONT, (WPARAM)g_wt_theme.body_font, TRUE);
        SendMessage(g_wt_set.done, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
    }

    return g_wt_set.pages[0] != NULL;
}

/* The preference pages are taller than a comfortable window, so they live in a
 * small scrolling host instead of forcing the window to full screen height. */
static void WTPositionSettingsPage(HWND hwnd)
{
    HWND page = g_wt_set.pages[g_wt_set.page];
    if (!page || !g_wt_set.host || !IsWindow(g_wt_set.host))
        return;

    RECT client;
    GetClientRect(g_wt_set.host, &client);
    int view_width = client.right - client.left;
    int view_height = client.bottom - client.top;
    int width = g_wt_set.page_width;
    int height = g_wt_set.page_height;

    int max_offset = max(0, height - view_height);
    if (g_wt_set.offset > max_offset)
        g_wt_set.offset = max_offset;
    if (g_wt_set.offset < 0)
        g_wt_set.offset = 0;

    SCROLLINFO info;
    mem00(&info, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    info.nMin = 0;
    info.nMax = max(0, height - 1);
    info.nPage = (UINT)view_height;
    info.nPos = g_wt_set.offset;
    SetScrollInfo(g_wt_set.host, SB_VERT, &info, TRUE);

    int x = max(0, (view_width - width) / 2);
    MoveWindow(page, x, -g_wt_set.offset, width, height, TRUE);
}

static void WTSettingsScroll(HWND hwnd, int delta)
{
    g_wt_set.offset += delta;
    WTPositionSettingsPage(hwnd);
}

static LRESULT CALLBACK WTPageHostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND: {
        RECT client;
        GetClientRect(hwnd, &client);
        FillRect((HDC)wParam, &client, g_wt_theme.card_brush);
        return TRUE;
    }
    case WM_SIZE:
        WTPositionSettingsPage(g_toolwnd);
        return 0;
    case WM_VSCROLL: {
        SCROLLINFO info;
        mem00(&info, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &info);
        int line = WTS(24);
        int pos = info.nPos;
        switch (LOWORD(wParam)) {
        case SB_LINEUP:   pos -= line; break;
        case SB_LINEDOWN: pos += line; break;
        case SB_PAGEUP:   pos -= (int)info.nPage; break;
        case SB_PAGEDOWN: pos += (int)info.nPage; break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION: pos = info.nTrackPos; break;
        case SB_TOP:      pos = info.nMin; break;
        case SB_BOTTOM:   pos = info.nMax; break;
        default: return 0;
        }
        if (pos == g_wt_set.offset)
            return 0;
        g_wt_set.offset = pos;
        WTPositionSettingsPage(g_toolwnd);
        return 0;
    }
    case WM_MOUSEWHEEL: {
        int lines = 3;
        SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lines, 0);
        if (lines <= 0)
            lines = 3;
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        WTSettingsScroll(g_toolwnd, -(delta * WTS(24) * lines) / WHEEL_DELTA);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void WTSettingsShowPage(HWND hwnd, int index)
{
    if (index < 0)
        index = 0;
    if (index >= WT_PAGE_COUNT)
        index = WT_PAGE_COUNT - 1;

    g_wt_set.page = index;
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.pages[i])
            ShowWindow(g_wt_set.pages[i], i == index ? SW_SHOWNA : SW_HIDE);
    }

    HWND page = g_wt_set.pages[index];
    g_wt_inline_page = page;
    if (page) {
        NMHDR header;
        mem00(&header, sizeof(header));
        header.hwndFrom = page;
        header.idFrom = (UINT_PTR)GetDlgCtrlID(page);
        header.code = PSN_SETACTIVE;
        SendMessage(page, WM_NOTIFY, 0, (LPARAM)&header);
    }

    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.nav[i])
            InvalidateRect(g_wt_set.nav[i], NULL, TRUE);
    }
    WTLayout(hwnd);
    WTPositionSettingsPage(hwnd);
}

/* Every page keeps its own dirty flag, so asking all of them to apply is safe:
 * only the pages the user touched actually write to the ini file. */
static BOOL WTSettingsApply(HWND hwnd)
{
    BOOL ok = TRUE;
    (void)hwnd;
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        HWND page = g_wt_set.pages[i];
        if (!page)
            continue;

        NMHDR header;
        mem00(&header, sizeof(header));
        header.hwndFrom = page;
        header.idFrom = (UINT_PTR)GetDlgCtrlID(page);
        header.code = PSN_APPLY;
        SetWindowLongPtr(page, DWLP_MSGRESULT, 0);
        SendMessage(page, WM_NOTIFY, 0, (LPARAM)&header);
        if ((INT_PTR)GetWindowLongPtr(page, DWLP_MSGRESULT) == PSNRET_INVALID_NOCHANGEPAGE)
            ok = FALSE;
    }
    return ok;
}

static int WTSettingsNeededWidth(HWND hwnd)
{
    return WTS(WT_PAD) * 2 + WTSettingsNavWidth(hwnd) + WTS(WT_GAP)
        + g_wt_set.page_width + WTS(WT_CARD_IN) * 2;
}

static int WTSettingsNeededHeight(void)
{
    int chrome = WTS(WT_PAD) + WTS(WT_HEADER) + WTS(WT_GAP) + WTS(WT_GAP)
        + WTS(WT_ACTION) + WTS(WT_GAP) + WTS(WT_FOOTER) + WTS(WT_PAD);
    /* Only grow enough to show a useful part of the page; the rest scrolls. */
    return chrome + min(g_wt_set.page_height, WTS(300)) + WTS(WT_CARD_IN) * 2;
}

static void WTSettingsEnsureSize(HWND hwnd)
{
    int needed_width = WTSettingsNeededWidth(hwnd);
    int needed_height = WTSettingsNeededHeight();

    RECT client;
    GetClientRect(hwnd, &client);
    int width = max(client.right, needed_width);
    int height = max(client.bottom, needed_height);
    if (width == client.right && height == client.bottom)
        return;

    RECT frame = { 0, 0, width, height };
    AdjustWindowRectEx(&frame, (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE), FALSE,
        (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE));

    RECT current;
    GetWindowRect(hwnd, &current);
    int frame_width = frame.right - frame.left;
    int frame_height = frame.bottom - frame.top;
    int x = current.left + ((current.right - current.left) - frame_width) / 2;
    int y = current.top + ((current.bottom - current.top) - frame_height) / 2;
    SetWindowPos(hwnd, NULL, x, y, frame_width, frame_height, SWP_NOZORDER | SWP_NOACTIVATE);
}

static void WTSetPanelControlsVisible(HWND hwnd, BOOL visible)
{
    static const int ids[] = {
        IDC_WT_SELECTED, IDC_WT_LIST, IDC_WT_PICKER, IDC_WT_REFRESH,
        IDC_WT_OPACITY_LABEL, IDC_WT_OPACITY, IDC_WT_PERCENT, IDC_WT_RESET,
        IDC_WT_TOPMOST, IDC_WT_FORCE, IDC_WT_CAPTURE_PROTECTION,
        IDC_WT_ACTIVATE, IDC_WT_SECURE_MIRROR, IDC_WT_SETTINGS, IDC_WT_EXIT
    };
    for (size_t i = 0; i < ARR_SZ(ids); i++) {
        HWND control = GetDlgItem(hwnd, ids[i]);
        if (control)
            ShowWindow(control, visible ? SW_SHOWNA : SW_HIDE);
    }
}

static void WTSetSettingsControlsVisible(BOOL visible)
{
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.nav[i])
            ShowWindow(g_wt_set.nav[i], visible ? SW_SHOWNA : SW_HIDE);
    }
    if (g_wt_set.apply)
        ShowWindow(g_wt_set.apply, visible ? SW_SHOWNA : SW_HIDE);
    if (g_wt_set.done)
        ShowWindow(g_wt_set.done, visible ? SW_SHOWNA : SW_HIDE);
}

static void WTSettingsEnter(HWND hwnd, int page)
{
    if (!IsWindow(hwnd))
        return;
    if (!WTSettingsCreate(hwnd))
        return;

    if (!g_wt_set.open) {
        RECT client;
        GetClientRect(hwnd, &client);
        g_wt_set.restore_width = client.right;
        g_wt_set.restore_height = client.bottom;
    }
    g_wt_set.open = TRUE;
    SendMessage(hwnd, DM_SETDEFID, (WPARAM)IDC_WT_DONE, 0);
    WTSetPanelControlsVisible(hwnd, FALSE);
    WTSetSettingsControlsVisible(TRUE);
    WTSettingsEnsureSize(hwnd);
    WTSettingsShowPage(hwnd, page);
    InvalidateRect(hwnd, NULL, TRUE);
    SetFocus(g_wt_set.pages[g_wt_set.page] ? g_wt_set.pages[g_wt_set.page] : hwnd);
}

static void WTSettingsClose(HWND hwnd)
{
    if (!g_wt_set.open)
        return;

    g_wt_set.open = FALSE;
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.pages[i])
            ShowWindow(g_wt_set.pages[i], SW_HIDE);
    }
    WTSetSettingsControlsVisible(FALSE);
    WTSetPanelControlsVisible(hwnd, TRUE);
    SendMessage(hwnd, DM_SETDEFID, (WPARAM)IDC_WT_REFRESH, 0);

    RECT client;
    GetClientRect(hwnd, &client);
    if (g_wt_set.restore_width > 0 && g_wt_set.restore_height > 0
    && (client.right > g_wt_set.restore_width || client.bottom > g_wt_set.restore_height)) {
        RECT frame = { 0, 0,
            max(g_wt_set.restore_width, WTMinimumWidth()),
            max(g_wt_set.restore_height, WTMinimumHeight()) };
        AdjustWindowRectEx(&frame, (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE), FALSE,
            (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE));
        RECT window;
        GetWindowRect(hwnd, &window);
        SetWindowPos(hwnd, NULL, window.left, window.top,
            frame.right - frame.left, frame.bottom - frame.top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    WTLayout(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
    WTRefreshWindows(hwnd, NULL);
    SetFocus(GetDlgItem(hwnd, IDC_WT_LIST));
}

static void WTSettingsShutdown(void)
{
    for (int i = 0; i < WT_PAGE_COUNT; i++) {
        if (g_wt_set.pages[i] && IsWindow(g_wt_set.pages[i]))
            DestroyWindow(g_wt_set.pages[i]);
        g_wt_set.pages[i] = NULL;
        g_wt_page_orig[i] = NULL;
        if (g_wt_set.nav[i] && IsWindow(g_wt_set.nav[i]))
            DestroyWindow(g_wt_set.nav[i]);
        g_wt_set.nav[i] = NULL;
    }
    if (g_wt_set.apply && IsWindow(g_wt_set.apply))
        DestroyWindow(g_wt_set.apply);
    if (g_wt_set.done && IsWindow(g_wt_set.done))
        DestroyWindow(g_wt_set.done);
    if (g_wt_set.host && IsWindow(g_wt_set.host))
        DestroyWindow(g_wt_set.host);
    UnregisterClass(TEXT(APP_NAMEA) TEXT("-pagehost"), g_hinst);
    g_wt_set.apply = NULL;
    g_wt_set.done = NULL;
    g_wt_set.host = NULL;
    g_wt_inline_page = NULL;
    mem00(&g_wt_set, sizeof(g_wt_set));
}

static void WTDrawNavButton(const DRAWITEMSTRUCT *item)
{
    int index = (int)item->CtlID - (int)IDC_WT_NAV_GENERAL;
    BOOL selected = index == g_wt_set.page;
    HDC dc = item->hDC;
    RECT rect = item->rcItem;
    int radius = WTS(8);

    FillRect(dc, &rect, g_wt_theme.card_brush);

    if (selected) {
        HBRUSH accent = CreateSolidBrush(RGB(233, 240, 255));
        HGDIOBJ old_brush = SelectObject(dc, accent);
        HGDIOBJ old_pen = SelectObject(dc, g_wt_theme.border_pen);
        RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius * 2, radius * 2);
        SelectObject(dc, old_pen);
        SelectObject(dc, old_brush);
        DeleteObject(accent);

        RECT bar = { rect.left + WTS(3), rect.top + WTS(9), rect.left + WTS(6), rect.bottom - WTS(9) };
        HBRUSH bar_brush = CreateSolidBrush(WT_CLR_ACCENT);
        FillRect(dc, &bar, bar_brush);
        DeleteObject(bar_brush);
    }

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, selected ? WT_CLR_ACCENT
        : (item->itemState & ODS_DISABLED) ? WT_CLR_IDLE : WT_CLR_TEXT);

    RECT text = rect;
    text.left += WTS(14);
    text.right -= WTS(8);
    HGDIOBJ old_font = SelectObject(dc, g_wt_theme.body_font);
    DrawText(dc, WTNavLabel(index), -1, &text,
        DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS);
    SelectObject(dc, old_font);

    if (item->itemState & ODS_FOCUS)
        DrawFocusRect(dc, &rect);
}

static void WTPaintPanel(HWND hwnd)
{
    wt_theme_t *t = &g_wt_theme;
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(hwnd, &paint);
    if (!t->bg_brush) {
        EndPaint(hwnd, &paint);
        return;
    }

    RECT client;
    GetClientRect(hwnd, &client);

    HDC memory = CreateCompatibleDC(dc);
    HBITMAP buffer = memory
        ? CreateCompatibleBitmap(dc, max(1, client.right), max(1, client.bottom)) : NULL;
    if (memory && buffer) {
        HGDIOBJ old_bitmap = SelectObject(memory, buffer);
        FillRect(memory, &client, t->bg_brush);
        SetBkMode(memory, TRANSPARENT);

        /* Header: logo, product name and the one line explanation. */
        RECT *header = &t->header_rect;
        int logo = WTS(32);
        int logo_y = header->top + (header->bottom - header->top - logo) / 2;
        if (t->logo)
            DrawIconEx(memory, header->left, logo_y, t->logo, logo, logo, 0, NULL, DI_NORMAL);

        int text_x = header->left + logo + WTS(12);
        int text_w = max(WTS(60), header->right - text_x);
        WTDrawText(memory, t->title_font, WT_CLR_TEXT, text_x, header->top + WTS(4),
            text_w, WTS(20), g_wt_set.open ? g_wt_text->settings_title : g_wt_text->title,
            DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS);
        WTDrawText(memory, t->small_font, WT_CLR_DIM, text_x + WTS(1), header->top + WTS(22),
            text_w, WTS(16), g_wt_set.open ? g_wt_text->settings_subtitle : g_wt_text->subtitle,
            DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS);

        if (g_wt_set.open) {
            WTDrawCard(memory, &g_wt_set.nav_rect);
            WTDrawCard(memory, &g_wt_set.content_rect);
            WTDrawCard(memory, &g_wt_set.action_rect);

            int hint_x = g_wt_set.action_rect.left + WTS(WT_CARD_IN);
            int hint_w = max(WTS(40), g_wt_set.action_rect.right - hint_x
                - WTS(WT_CARD_IN) - WTS(200));
            WTDrawText(memory, t->small_font, WT_CLR_DIM, hint_x, g_wt_set.action_rect.top,
                hint_w, g_wt_set.action_rect.bottom - g_wt_set.action_rect.top,
                g_wt_text->settings_hint, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);
        } else {
            WTDrawCard(memory, &t->card1_rect);
            WTDrawCard(memory, &t->card2_rect);

            int inner = WTS(WT_CARD_IN);
            int caption_y = t->card1_rect.top + inner + WTS(3);
            WTDrawText(memory, t->small_font, WT_CLR_DIM, t->card1_rect.left + inner,
                caption_y, WTS(72), WTS(24), g_wt_text->card_windows,
                DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS);
        }

        /* Footer: a state dot next to the status text control. */
        RECT *footer = &t->footer_rect;
        int dot = WTS(8);
        COLORREF dot_color = ENABLED() ? WT_CLR_OK : WT_CLR_WARN;
        WTDrawStatusDot(memory, footer->left, footer->top + (footer->bottom - footer->top - dot) / 2 + WTS(1),
            dot, dot_color);

        BitBlt(dc, 0, 0, client.right, client.bottom, memory, 0, 0, SRCCOPY);
        SelectObject(memory, old_bitmap);
    }

    if (buffer)
        DeleteObject(buffer);
    if (memory)
        DeleteDC(memory);
    EndPaint(hwnd, &paint);
}

static BOOL WTReadCaptureProtection(void)

{
    return GetPrivateProfileInt(TEXT("General"), TEXT("CaptureProtection"), 1, inipath) != 0;
}

static BOOL WTSetCaptureProtection(HWND hwnd, BOOL enabled)
{
    if (!IsWindow(hwnd))
        return TRUE;
    if (SetWindowDisplayAffinityL(hwnd, enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE))
        return TRUE;
    if (enabled)
        return SetWindowDisplayAffinityL(hwnd, WDA_MONITOR);
    return FALSE;
}

static BOOL WTApplyCaptureProtection(HWND hwnd)
{
    return WTSetCaptureProtection(hwnd, WTReadCaptureProtection());
}

static BOOL WTSetMirrorCaptureProtection(HWND hwnd)
{
    return IsWindow(hwnd) && SetWindowDisplayAffinityL(hwnd, WDA_EXCLUDEFROMCAPTURE);
}

static BOOL WTSecureMirrorTargetIsValid(void)
{
    if (!g_wt_secure_mirror.target)
        return FALSE;

    /* The guard does not own a window, so validity is just "still the same
     * window in the same process".  WTIdentityMatches would also demand the
     * generation property, which foreign windows never receive, and that made
     * the watchdog stop the guard on its own within milliseconds. */
    DWORD pid = 0;
    GetWindowThreadProcessId(g_wt_secure_mirror.target, &pid);
    return pid == g_wt_secure_mirror.pid;
}
static BOOL WTSecureMirrorUpdateThumbnail(void)
{
    if (!g_wt_secure_mirror.thumbnail || !IsWindow(g_wt_secure_mirror.hwnd))
        return FALSE;

    RECT client;
    if (!GetClientRect(g_wt_secure_mirror.hwnd, &client))
        return FALSE;

    DWM_THUMBNAIL_PROPERTIES properties;
    mem00(&properties, sizeof(properties));
    properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_OPACITY | DWM_TNP_VISIBLE
        | DWM_TNP_SOURCECLIENTAREAONLY;
    properties.rcDestination = client;
    properties.opacity = 255;
    properties.fVisible = TRUE;
    properties.fSourceClientAreaOnly = FALSE;
    return DwmUpdateThumbnailPropertiesL(g_wt_secure_mirror.thumbnail, &properties) == S_OK;
}

static void WTSecureMirrorMapPoint(LPARAM lParam, POINT *screen, POINT *client)
{
    RECT target_rect;
    GetWindowRect(g_wt_secure_mirror.target, &target_rect);
    screen->x = target_rect.left + GET_X_LPARAM(lParam);
    screen->y = target_rect.top + GET_Y_LPARAM(lParam);
    *client = *screen;
    ScreenToClient(g_wt_secure_mirror.target, client);
}

static void WTSecureMirrorSend(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DWORD_PTR result = 0;
    SendMessageTimeout(hwnd, msg, wParam, lParam, SMTO_ABORTIFHUNG, 100, &result);
}

static void WTSecureMirrorForwardMouse(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!WTSecureMirrorTargetIsValid())
        return;

    POINT screen;
    POINT client;
    WTSecureMirrorMapPoint(lParam, &screen, &client);
    LPARAM screen_lparam = MAKELPARAM(screen.x, screen.y);
    int hit = HitTestTimeoutL(g_wt_secure_mirror.target, screen_lparam);
    if (hit != HTCLIENT && hit != HTNOWHERE) {
        UINT ncmsg = msg;
        if (msg == WM_LBUTTONDOWN) ncmsg = WM_NCLBUTTONDOWN;
        else if (msg == WM_LBUTTONUP) ncmsg = WM_NCLBUTTONUP;
        else if (msg == WM_LBUTTONDBLCLK) ncmsg = WM_NCLBUTTONDBLCLK;
        else if (msg == WM_RBUTTONDOWN) ncmsg = WM_NCRBUTTONDOWN;
        else if (msg == WM_RBUTTONUP) ncmsg = WM_NCRBUTTONUP;
        else if (msg == WM_RBUTTONDBLCLK) ncmsg = WM_NCRBUTTONDBLCLK;
        WTSecureMirrorSend(g_wt_secure_mirror.target, ncmsg, (WPARAM)hit, screen_lparam);
        return;
    }

    HWND receiver = ChildWindowFromPointEx(g_wt_secure_mirror.target, client,
        CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
    if (!receiver)
        receiver = g_wt_secure_mirror.target;
    if (receiver != g_wt_secure_mirror.target)
        MapWindowPoints(g_wt_secure_mirror.target, receiver, &client, 1);
    WTSecureMirrorSend(receiver, msg, wParam, MAKELPARAM(client.x, client.y));
}

static void WTSecureMirrorForwardWheel(WPARAM wParam, LPARAM lParam)
{
    if (!WTSecureMirrorTargetIsValid())
        return;

    POINT screen;
    POINT client;
    WTSecureMirrorMapPoint(lParam, &screen, &client);
    WTSecureMirrorSend(g_wt_secure_mirror.target, WM_MOUSEWHEEL, wParam,
        MAKELPARAM(screen.x, screen.y));
}

/* Paint what a screen recorder is allowed to see in place of the protected
 * window: a neutral placeholder, so the capture never contains the content. */
static void WTSecureMirrorPaint(HWND hwnd)
{
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(hwnd, &paint);
    RECT client;
    GetClientRect(hwnd, &client);

    HBRUSH background = CreateSolidBrush(WT_CLR_BG);
    FillRect(dc, &client, background);
    DeleteObject(background);

    if (dc && client.right > 0 && client.bottom > 0) {
        SetBkMode(dc, TRANSPARENT);
        SelectObject(dc, g_wt_theme.body_font ? g_wt_theme.body_font : GetStockObject(DEFAULT_GUI_FONT));
        SetTextColor(dc, WT_CLR_DIM);
        DrawText(dc, g_wt_text->secure_mirror_placeholder, -1, &client,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
    }

    EndPaint(hwnd, &paint);
}

static LRESULT CALLBACK WTSecureMirrorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_MOUSEACTIVATE)
        return MA_NOACTIVATE;
    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN) {
        SetForegroundWindow(g_wt_secure_mirror.target);
        SetCapture(hwnd);
        WTSecureMirrorForwardMouse(msg, wParam, lParam);
        return 0;
    }
    if (msg == WM_LBUTTONUP || msg == WM_LBUTTONDBLCLK
        || msg == WM_RBUTTONUP || msg == WM_RBUTTONDBLCLK || msg == WM_MOUSEMOVE) {
        WTSecureMirrorForwardMouse(msg, wParam, lParam);
        if ((msg == WM_LBUTTONUP || msg == WM_RBUTTONUP) && GetCapture() == hwnd)
            ReleaseCapture();
        return 0;
    }
    if (msg == WM_MOUSEWHEEL) {
        WTSecureMirrorForwardWheel(wParam, lParam);
        return 0;
    }
    if (msg == WM_SIZE) {
        WTSecureMirrorUpdateThumbnail();
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }
    if (msg == WM_TIMER && wParam == 1) {
        if (!WTSecureMirrorTargetIsValid()) {
            WTSecureMirrorStop(TRUE);
        } else {
            /* follow the window as the user moves or resizes it */
            WTSecureMirrorFollowTarget();
        }
        return 0;
    }
    if (msg == WM_ERASEBKGND) {
        RECT client;
        GetClientRect(hwnd, &client);
        HBRUSH background = CreateSolidBrush(WT_CLR_BG);
        FillRect((HDC)wParam, &client, background);
        DeleteObject(background);
        return TRUE;
    }
    if (msg == WM_PAINT) {
        WTSecureMirrorPaint(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* Keep the mirror glued to the window it stands in for.  Without this the
 * protected window can be moved while the recorder keeps seeing the old spot. */
static void WTSecureMirrorFollowTarget(void)
{
    if (!g_wt_secure_mirror.hwnd || !WTSecureMirrorTargetIsValid())
        return;

    HWND target = g_wt_secure_mirror.target;
    if (IsIconic(target)) {
        ShowWindow(g_wt_secure_mirror.hwnd, SW_HIDE);
        return;
    }
    ShowWindow(g_wt_secure_mirror.hwnd, SW_SHOWNA);

    RECT rect;
    if (!GetWindowRect(target, &rect))
        return;

    /* The real window is deliberately parked off-screen so no capture picks up
     * its pixels; in that case the last visible rectangle is the real one. */
    if (rect.left <= -30000 || rect.top <= -30000)
        rect = g_wt_secure_mirror.visible_rect;
    else
        g_wt_secure_mirror.visible_rect = rect;

    if (g_wt_secure_mirror.target_topmost
    &&  !(GetWindowLongPtr(target, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        SetWindowPos(target, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    RECT current;
    GetWindowRect(g_wt_secure_mirror.hwnd, &current);
    if (current.left == rect.left && current.top == rect.top
    &&  current.right - current.left == rect.right - rect.left
    &&  current.bottom - current.top == rect.bottom - rect.top)
        return;

    SetWindowPos(g_wt_secure_mirror.hwnd, HWND_TOPMOST,
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOACTIVATE | SWP_SHOWWINDOW);
    WTSecureMirrorUpdateThumbnail();
    InvalidateRect(g_wt_secure_mirror.hwnd, NULL, TRUE);
}

/* Capture protection is only accepted for windows this process owns: Windows
 * refuses SetWindowDisplayAffinity for a window belonging to another process
 * and offers no supported way around it.  A foreign window is therefore
 * reported as unsupported instead of pretending to be protected. */
/* Ask a window to exclude itself from screen capture.
 *
 * Windows only honours SetWindowDisplayAffinity when the owning process makes
 * the call, so for a foreign window a short stub is placed in that process and
 * invoked with CreateRemoteThread:
 *
 *   mov rcx, <hwnd>          first argument
 *   mov edx, <affinity>      second argument
 *   mov rax, <SetWindowDisplayAffinity>
 *   jmp rax                  tail call, leaves the stack untouched
 *
 * A tail jump (what the compiler itself emits for this shape) means the API
 * returns straight into the thread entry CreateRemoteThread installed, so there
 * is no frame to unwind and no way to misalign the stack.  The immediate-field
 * offsets must match the encodings exactly: shifting them by one byte corrupts
 * the instruction stream and takes the target process down.
 */
static BOOL WTRemoteSetCaptureGuard(HWND target, BOOL enabled)
{
#ifdef _WIN64
    void *set_affinity = (void *)LoadDLLProc("USER32.DLL", "SetWindowDisplayAffinity");
    if (!set_affinity)
        return FALSE;

    DWORD pid = 0;
    GetWindowThreadProcessId(target, &pid);
    if (!pid)
        return FALSE;

    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION
        | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    if (!process)
        return FALSE;

    BOOL wow64 = FALSE;
    BOOL ok = FALSE;
    void *remote = NULL;
    HANDLE thread = NULL;
    DWORD affinity = enabled ? (DWORD)WDA_EXCLUDEFROMCAPTURE : (DWORD)WDA_NONE;

    do {
        /* Never write a 64 bit stub into a 32 bit target. */
        if (!IsWow64Process(process, &wow64) || wow64)
            break;

        BYTE stub[] = {
            0x48, 0xB9, 0,0,0,0,0,0,0,0,   /* mov rcx, hwnd     imm at 2  */
            0xBA,        0,0,0,0,          /* mov edx, affinity imm at 11 */
            0x48, 0xB8, 0,0,0,0,0,0,0,0,   /* mov rax, fn       imm at 17 */
            0xFF, 0xE0                     /* jmp rax                     */
        };
        ULONGLONG hwnd_value = (ULONGLONG)(ULONG_PTR)target;
        ULONGLONG fn_value = (ULONGLONG)(ULONG_PTR)set_affinity;
        memcpy(stub + 2,  &hwnd_value, sizeof(hwnd_value));
        memcpy(stub + 11, &affinity,   sizeof(affinity));
        memcpy(stub + 17, &fn_value,   sizeof(fn_value));

        remote = VirtualAllocEx(process, NULL, sizeof(stub), MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE);
        if (!remote)
            break;

        SIZE_T written = 0;
        if (!WriteProcessMemory(process, remote, stub, sizeof(stub), &written)
        ||  written != sizeof(stub))
            break;

        DWORD old_protect = 0;
        if (!VirtualProtectEx(process, remote, sizeof(stub), PAGE_EXECUTE_READ, &old_protect))
            break;

        thread = CreateRemoteThread(process, NULL, 0,
            (LPTHREAD_START_ROUTINE)remote, NULL, 0, NULL);
        if (!thread)
            break;
        if (WaitForSingleObject(thread, 3000) != WAIT_OBJECT_0)
            break;

        /* The stub tail-jumps into the API, so the thread result is its result. */
        DWORD exit_code = 0;
        if (!GetExitCodeThread(thread, &exit_code))
            break;
        ok = exit_code != 0;
    } while (0);

    if (thread)
        CloseHandle(thread);
    if (remote)
        VirtualFreeEx(process, remote, 0, MEM_RELEASE);
    CloseHandle(process);
    return ok;
#else
    (void)target;
    (void)enabled;
    return FALSE;
#endif
}

static BOOL WTCaptureGuardWindow(HWND target, BOOL enabled)
{
    if (!target || !IsWindow(target))
        return FALSE;

    DWORD pid = 0;
    GetWindowThreadProcessId(target, &pid);
    if (!pid)
        return FALSE;

    if (pid == GetCurrentProcessId())
        return WTSetCaptureProtection(target, enabled);

    return WTRemoteSetCaptureGuard(target, enabled);
}

/* The button reads as capture protection for "this window": tell the user when
 * the selected window cannot be protected, rather than failing silently. */
static BOOL WTSecureMirrorStart(HWND owner, HWND target)
{
    if (!target || !IsWindow(target)) {
        WTSetStatus(owner, g_wt_text->secure_mirror_failed);
        return FALSE;
    }
    if (g_wt_secure_mirror.guarded
    &&  g_wt_secure_mirror.target && IsWindow(g_wt_secure_mirror.target))
        WTSecureMirrorStop(FALSE);

    DWORD pid = 0;
    DWORD tid = 0;
    ATOM class_atom = 0;
    if (!WTGetIdentity(target, &pid, &tid, &class_atom)) {
        WTSetStatus(owner, g_wt_text->secure_mirror_failed);
        return FALSE;
    }
    BOOL own_window = pid == GetCurrentProcessId();
    /* A foreign window keeps its identity through the remote call, so the
     * generation stamp is only taken for our own windows. */
    HANDLE generation = own_window ? WTEnsureGeneration(target) : (HANDLE)1;
    if (!WTCaptureGuardWindow(target, TRUE)) {
        WTSetStatus(owner, g_wt_text->capture_protection_failed);
        return FALSE;
    }

    mem00(&g_wt_secure_mirror, sizeof(g_wt_secure_mirror));
    g_wt_secure_mirror.target = target;
    g_wt_secure_mirror.pid = pid;
    g_wt_secure_mirror.tid = tid;
    g_wt_secure_mirror.class_atom = class_atom;
    g_wt_secure_mirror.generation = generation;
    g_wt_secure_mirror.guarded = 1;
    WTSetStatus(owner, g_wt_text->secure_mirror_started);
    return TRUE;
}

static void WTSecureMirrorStop(BOOL report_status)
{
    wt_secure_mirror_t guard = g_wt_secure_mirror;
    mem00(&g_wt_secure_mirror, sizeof(g_wt_secure_mirror));

    if (guard.thumbnail)
        DwmUnregisterThumbnailL(guard.thumbnail);
    if (IsWindow(guard.hwnd)) {
        KillTimer(guard.hwnd, 1);
        if (GetCapture() == guard.hwnd)
            ReleaseCapture();
        DestroyWindow(guard.hwnd);
    }
    UnregisterClass(TEXT(APP_NAMEA) TEXT("-secure-mirror"), g_hinst);

    if (!guard.guarded)
        return;

    /* A foreign window has no generation stamp on it, so validity is simply
     * whether the window still exists and belongs to the same process. */
    DWORD current_pid = 0;
    if (guard.target && IsWindow(guard.target))
        GetWindowThreadProcessId(guard.target, &current_pid);
    if (guard.target && current_pid == guard.pid) {
        WTCaptureGuardWindow(guard.target, FALSE);
        if (report_status && IsWindow(g_toolwnd))
            WTSetStatus(g_toolwnd, g_wt_text->secure_mirror_stopped);
    } else if (report_status && IsWindow(g_toolwnd)) {
        WTSetStatus(g_toolwnd, g_wt_text->secure_mirror_lost);
    }
}

/* Apply (or drop) capture protection on every window this process owns: the
 * panel, the page host that shows the settings, and an active mirror.  Only
 * windows created by the calling process accept a display affinity, so this
 * cannot be applied to other programs' windows. */
static BOOL WTCaptureProtectionCallback(HWND hwnd, LPARAM param)
{
    BOOL enabled = (BOOL)param;
    if (enabled)
        WTSetCaptureProtection(hwnd, TRUE);
    else
        SetWindowDisplayAffinityL(hwnd, WDA_NONE);
    return TRUE;
}

static BOOL WTApplyCaptureProtectionAll(BOOL enabled)
{
    if (!IsWindow(g_toolwnd))
        return TRUE;

    BOOL ok = TRUE;
    if (!g_wt_set.host || !IsWindow(g_wt_set.host)) {
        ok = WTSetCaptureProtection(g_toolwnd, enabled) && ok;
    } else {
        ok = WTSetCaptureProtection(g_toolwnd, enabled) && ok;
        SetWindowDisplayAffinityL(g_wt_set.host, enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
        for (int i = 0; i < WT_PAGE_COUNT; i++) {
            if (g_wt_set.pages[i] && IsWindow(g_wt_set.pages[i]))
                SetWindowDisplayAffinityL(g_wt_set.pages[i], enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
            if (g_wt_set.nav[i] && IsWindow(g_wt_set.nav[i]))
                SetWindowDisplayAffinityL(g_wt_set.nav[i], enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
        }
        if (g_wt_set.apply && IsWindow(g_wt_set.apply))
            SetWindowDisplayAffinityL(g_wt_set.apply, enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
        if (g_wt_set.done && IsWindow(g_wt_set.done))
            SetWindowDisplayAffinityL(g_wt_set.done, enabled ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    }
    if (g_wt_secure_mirror.hwnd && IsWindow(g_wt_secure_mirror.hwnd))
        ok = WTSetCaptureProtection(g_wt_secure_mirror.hwnd, enabled) && ok;
    return ok;
}

static BOOL WTSaveCaptureProtection(HWND owner, BOOL enabled)
{
    BeginConfigWrite();
    WritePrivateProfileString(TEXT("General"), TEXT("CaptureProtection"),
        enabled ? TEXT("1") : TEXT("0"), inipath);
    if (!FinishConfigWrite(owner))
        return FALSE;

    g_wt_capture_protection = enabled;
    return WTApplyCaptureProtectionAll(enabled);
}

static void SetWindowToolEnabled(BOOL enabled)
{
    if (IsWindow(g_toolwnd))
        EnableWindow(g_toolwnd, enabled);
}

static void WTRememberGeneration(HWND hwnd, HANDLE generation)
{
    for (size_t i = 0; i < ARR_SZ(g_wt_markers); i++) {
        if (g_wt_markers[i].hwnd == hwnd && g_wt_markers[i].generation == generation)
            return;
    }
    for (size_t i = 0; i < ARR_SZ(g_wt_markers); i++) {
        if (!g_wt_markers[i].hwnd || !IsWindow(g_wt_markers[i].hwnd)
            || GetProp(g_wt_markers[i].hwnd, g_wt_generation_property) != g_wt_markers[i].generation) {
            g_wt_markers[i].hwnd = hwnd;
            g_wt_markers[i].generation = generation;
            return;
        }
    }
}

static HANDLE WTEnsureGeneration(HWND hwnd)
{
    if (!g_wt_generation_property[0])
        return NULL;
    HANDLE generation = GetProp(hwnd, g_wt_generation_property);
    if (!generation) {
        do {
            g_wt_generation++;
            generation = (HANDLE)g_wt_generation;
        } while (!generation);
        if (!SetProp(hwnd, g_wt_generation_property, generation))
            return NULL;
    }
    WTRememberGeneration(hwnd, generation);
    return generation;
}

static void WTReleaseGenerations(void)
{
    for (size_t i = 0; i < ARR_SZ(g_wt_markers); i++) {
        if (g_wt_markers[i].hwnd && g_wt_markers[i].generation
            && IsWindow(g_wt_markers[i].hwnd)
            && GetProp(g_wt_markers[i].hwnd, g_wt_generation_property) == g_wt_markers[i].generation) {
            RemoveProp(g_wt_markers[i].hwnd, g_wt_generation_property);
        }
    }
    mem00(g_wt_markers, sizeof(g_wt_markers));
}

static BOOL WTGetIdentity(HWND hwnd, DWORD *pid, DWORD *tid, ATOM *class_atom)
{
    if (!hwnd || !IsWindow(hwnd))
        return FALSE;
    DWORD current_pid = 0;
    DWORD current_tid = GetWindowThreadProcessId(hwnd, &current_pid);
    if (!current_pid || !current_tid)
        return FALSE;
    if (pid)
        *pid = current_pid;
    if (tid)
        *tid = current_tid;
    if (class_atom)
        *class_atom = (ATOM)GetClassLongPtr(hwnd, GCW_ATOM);
    return TRUE;
}

static BOOL WTIdentityMatches(HWND hwnd, DWORD pid, DWORD tid, ATOM class_atom, HANDLE generation)
{
    DWORD current_pid = 0;
    DWORD current_tid = 0;
    ATOM current_atom = 0;
    return WTGetIdentity(hwnd, &current_pid, &current_tid, &current_atom)
        && generation && GetProp(hwnd, g_wt_generation_property) == generation
        && current_pid == pid && current_tid == tid && current_atom == class_atom;
}

static void WTSetStatus(HWND hwnd, const TCHAR *text)
{
    SetDlgItemText(hwnd, IDC_WT_STATUS, text);
    if (g_wt_theme.bg_brush)
        InvalidateRect(hwnd, &g_wt_theme.footer_rect, FALSE);
}

static int WTPercentFromAlpha(BYTE alpha)
{
    return (alpha * 100 + 127) / 255;
}

static BYTE WTAlphaFromPercent(int percent)
{
    percent = CLAMP(WT_MIN_OPACITY, percent, 100);
    return (BYTE)((percent * 255 + 50) / 100);
}

static BOOL WTGetOpacity(HWND hwnd, BYTE *alpha)
{
    LONG_PTR exstyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (!(exstyle & WS_EX_LAYERED)) {
        *alpha = 255;
        return TRUE;
    }

    COLORREF color_key = 0;
    DWORD flags = 0;
    BYTE current_alpha = 255;
    if (!GetLayeredWindowAttributes(hwnd, &color_key, &current_alpha, &flags))
        return FALSE;

    *alpha = (flags & LWA_ALPHA) ? current_alpha : 255;
    return TRUE;
}

static wt_snapshot_t *WTFindSnapshot(HWND hwnd)
{
    DWORD pid = 0;
    DWORD tid = 0;
    ATOM class_atom = 0;
    if (!WTGetIdentity(hwnd, &pid, &tid, &class_atom))
        return NULL;
    HANDLE generation = GetProp(hwnd, g_wt_generation_property);
    for (size_t i = 0; i < ARR_SZ(g_wt_snapshots); i++) {
        if (g_wt_snapshots[i].hwnd == hwnd && g_wt_snapshots[i].pid == pid
            && g_wt_snapshots[i].tid == tid && g_wt_snapshots[i].class_atom == class_atom
            && g_wt_snapshots[i].generation == generation && generation)
            return &g_wt_snapshots[i];
    }
    return NULL;
}

static wt_snapshot_t *WTCaptureSnapshot(HWND hwnd)
{
    wt_snapshot_t *snapshot = WTFindSnapshot(hwnd);
    if (snapshot)
        return snapshot;

    DWORD pid = 0;
    DWORD tid = 0;
    ATOM class_atom = 0;
    if (!WTGetIdentity(hwnd, &pid, &tid, &class_atom))
        return NULL;
    HANDLE generation = WTEnsureGeneration(hwnd);
    if (!generation)
        return NULL;
    for (size_t i = 0; i < ARR_SZ(g_wt_snapshots); i++) {
        if (!g_wt_snapshots[i].hwnd || !WTIdentityMatches(g_wt_snapshots[i].hwnd,
                g_wt_snapshots[i].pid, g_wt_snapshots[i].tid, g_wt_snapshots[i].class_atom,
                g_wt_snapshots[i].generation)) {
            snapshot = &g_wt_snapshots[i];
            mem00(snapshot, sizeof(*snapshot));
            snapshot->hwnd = hwnd;
            snapshot->pid = pid;
            snapshot->tid = tid;
            snapshot->class_atom = class_atom;
            snapshot->generation = generation;
            snapshot->had_layered = !!(GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED);
            snapshot->alpha_owned = !!GetProp(hwnd, APP_ALPHAOWNED);
            snapshot->alpha = 255;
            if (snapshot->had_layered) {
                snapshot->attributes_valid = GetLayeredWindowAttributes(hwnd,
                    &snapshot->color_key, &snapshot->alpha, &snapshot->flags);
            }
            return snapshot;
        }
    }
    return NULL;
}

static BOOL WTSetLayeredStyle(HWND hwnd, BOOL enabled)
{
    LONG_PTR current = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    LONG_PTR desired = enabled ? current | WS_EX_LAYERED : current & ~WS_EX_LAYERED;
    if (current == desired)
        return TRUE;

    SetLastError(ERROR_SUCCESS);
    LONG_PTR previous = SetWindowLongPtr(hwnd, GWL_EXSTYLE, desired);
    if (!previous && GetLastError() != ERROR_SUCCESS)
        return FALSE;

    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    return TRUE;
}

/* Resetting the style releases the UpdateLayeredWindow lock.  This is the
 * only supported way to switch such a window to SetLayeredWindowAttributes. */
static BOOL WTReinitializeLayeredWindow(HWND hwnd)
{
    if (!WTSetLayeredStyle(hwnd, FALSE) || !WTSetLayeredStyle(hwnd, TRUE))
        return FALSE;
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
    return TRUE;
}

static BOOL WTRestoreAlphaOwnership(HWND hwnd, BOOL owned)
{
    if (owned)
        return SetProp(hwnd, APP_ALPHAOWNED, (HANDLE)(UINT_PTR)1);
    RemoveProp(hwnd, APP_ALPHAOWNED);
    return TRUE;
}

static BOOL WTSetOpacity(HWND hwnd, int percent, BOOL force)
{
    if (!IsWindow(hwnd))
        return FALSE;

    wt_snapshot_t *snapshot = WTFindSnapshot(hwnd);
    if (percent >= 100) {
        LONG_PTR exstyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (snapshot && !snapshot->had_layered) {
            BOOL ok = WTSetLayeredStyle(hwnd, FALSE);
            if (ok)
                ok = WTRestoreAlphaOwnership(hwnd, snapshot->alpha_owned);
            if (ok)
                snapshot->hwnd = NULL;
            return ok;
        }
        if (!(exstyle & WS_EX_LAYERED)) {
            if (snapshot)
                snapshot->hwnd = NULL;
            return TRUE;
        }

        COLORREF color_key = 0;
        BYTE old_alpha = 255;
        DWORD flags = 0;
        if (!GetLayeredWindowAttributes(hwnd, &color_key, &old_alpha, &flags)) {
            if (!force || !snapshot || !snapshot->had_layered || snapshot->attributes_valid)
                return FALSE;
            if (!snapshot->force_reinitialized) {
                if (!WTReinitializeLayeredWindow(hwnd))
                    return FALSE;
                snapshot->force_reinitialized = 1;
            }
            snapshot->hwnd = NULL;
            return TRUE;
        }
        if (snapshot && snapshot->had_layered) {
            color_key = snapshot->color_key;
            flags = snapshot->flags;
        }
        BOOL ok = SetLayeredWindowAttributes(hwnd, color_key, 255, flags | LWA_ALPHA);
        if (ok && snapshot)
            ok = WTRestoreAlphaOwnership(hwnd, snapshot->alpha_owned);
        return ok;
    }

    BOOL new_snapshot = snapshot == NULL;
    snapshot = WTCaptureSnapshot(hwnd);
    if (!snapshot || (snapshot->had_layered && !snapshot->attributes_valid && !force))
        return FALSE;

    /* A window rendered through UpdateLayeredWindow rejects
     * SetLayeredWindowAttributes until WS_EX_LAYERED is dropped and re-added,
     * which is what "force" means here.  Do it every time for such a window,
     * because the style reset is the only way to release the lock; assuming a
     * single reset was enough is why forced transparency used to do nothing. */
    BOOL needs_reset = snapshot->had_layered && !snapshot->attributes_valid;
    if (needs_reset) {
        if (!WTReinitializeLayeredWindow(hwnd)) {
            if (new_snapshot)
                snapshot->hwnd = NULL;
            return FALSE;
        }
        snapshot->force_reinitialized = 1;
    } else if (!WTSetLayeredStyle(hwnd, TRUE)) {
        if (new_snapshot)
            snapshot->hwnd = NULL;
        return FALSE;
    }

    /* After a reset the window carries no attributes at all, so start from a
     * clean alpha-only blend instead of the attributes captured before. */
    COLORREF color_key = (!needs_reset && snapshot->had_layered && snapshot->attributes_valid)
        ? snapshot->color_key : 0;
    DWORD flags = (!needs_reset && snapshot->had_layered && snapshot->attributes_valid
        ? snapshot->flags : 0) | LWA_ALPHA;

    if (!SetLayeredWindowAttributes(hwnd, color_key, WTAlphaFromPercent(percent), flags)) {
        if (WTSetLayeredStyle(hwnd, FALSE))
            WTSetLayeredStyle(hwnd, TRUE);
        WTRestoreAlphaOwnership(hwnd, snapshot->alpha_owned);
        if (new_snapshot)
            snapshot->hwnd = NULL;
        return FALSE;
    }

    /* The window now really is alpha blended, so the panel can report a value
     * and later restore the original state from it. */
    snapshot->attributes_valid = 1;
    snapshot->color_key = color_key;
    snapshot->flags = flags;
    snapshot->alpha = WTAlphaFromPercent(percent);
    return WTRestoreAlphaOwnership(hwnd, snapshot->alpha_owned);
}

static BOOL WTRestoreOpacity(HWND hwnd)
{
    wt_snapshot_t *snapshot = WTFindSnapshot(hwnd);
    if (!snapshot)
        return FALSE;

    BOOL ok = TRUE;
    if (snapshot->had_layered) {
        ok = WTSetLayeredStyle(hwnd, TRUE);
        if (ok && snapshot->attributes_valid) {
            ok = SetLayeredWindowAttributes(hwnd, snapshot->color_key,
                snapshot->alpha, snapshot->flags);
        } else if (ok && snapshot->force_reinitialized) {
            ok = WTReinitializeLayeredWindow(hwnd);
        }
    } else {
        ok = WTSetLayeredStyle(hwnd, FALSE);
    }
    if (ok)
        ok = WTRestoreAlphaOwnership(hwnd, snapshot->alpha_owned);
    if (ok)
        snapshot->hwnd = NULL;
    return ok;
}

static wt_target_t *WTSelectedTarget(HWND hwnd)
{
    HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
    int index = ListView_GetNextItem(list, -1, LVNI_SELECTED);
    if (index < 0)
        return NULL;

    LVITEM item;
    mem00(&item, sizeof(item));
    item.mask = LVIF_PARAM;
    item.iItem = index;
    if (!ListView_GetItem(list, &item))
        return NULL;
    wt_target_t *target = (wt_target_t *)item.lParam;
    if (!target || !WTIdentityMatches(target->hwnd, target->pid, target->tid,
            target->class_atom, target->generation))
        return NULL;
    return target;
}

static HWND WTSelectedWindow(HWND hwnd)
{
    wt_target_t *target = WTSelectedTarget(hwnd);
    return target ? target->hwnd : NULL;
}

static int WTFindWindowItem(HWND list, HWND target)
{
    int count = ListView_GetItemCount(list);
    for (int index = 0; index < count; index++) {
        LVITEM item;
        mem00(&item, sizeof(item));
        item.mask = LVIF_PARAM;
        item.iItem = index;
        if (ListView_GetItem(list, &item)) {
            wt_target_t *entry = (wt_target_t *)item.lParam;
            if (entry && entry->hwnd == target
                && WTIdentityMatches(entry->hwnd, entry->pid, entry->tid,
                    entry->class_atom, entry->generation))
                return index;
        }
    }
    return -1;
}

static void WTUpdateListItem(HWND list, int index, HWND target)
{
    BYTE alpha = 255;
    TCHAR text[64];
    if (WTGetOpacity(target, &alpha))
        wsprintf(text, TEXT("%d%%"), WTPercentFromAlpha(alpha));
    else
        lstrcpy_s(text, ARR_SZ(text), g_wt_text->unsupported);
    ListView_SetItemText(list, index, 2, text);

    lstrcpy_s(text, ARR_SZ(text),
        (GetWindowLongPtr(target, GWL_EXSTYLE) & WS_EX_TOPMOST) ? g_wt_text->yes : g_wt_text->no);
    ListView_SetItemText(list, index, 3, text);
}

static int WTInsertWindow(HWND list, HWND target)
{
    if (!IsWindow(target) || g_wt_target_count >= ARR_SZ(g_wt_targets))
        return -1;

    wt_target_t *entry = &g_wt_targets[g_wt_target_count];
    mem00(entry, sizeof(*entry));
    if (!WTGetIdentity(target, &entry->pid, &entry->tid, &entry->class_atom))
        return -1;
    entry->hwnd = target;
    entry->generation = WTEnsureGeneration(target);

    TCHAR title[512];
    title[0] = TEXT('\0');
    GetWindowText(target, title, ARR_SZ(title));
    if (!title[0])
        lstrcpy_s(title, ARR_SZ(title), g_wt_text->untitled_window);

    TCHAR program[MAX_PATH];
    program[0] = TEXT('\0');
    if (!GetWindowProgName(target, program, ARR_SZ(program)))
        lstrcpy_s(program, ARR_SZ(program), g_wt_text->unknown_program);

    LVITEM item;
    mem00(&item, sizeof(item));
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.iItem = ListView_GetItemCount(list);
    item.pszText = title;
    item.lParam = (LPARAM)entry;
    int index = ListView_InsertItem(list, &item);
    if (index >= 0) {
        g_wt_target_count++;
        ListView_SetItemText(list, index, 1, program);
        WTUpdateListItem(list, index, target);
    }
    return index;
}

static BOOL CALLBACK WTEnumWindowsProc(HWND target, LPARAM lParam)
{
    if (!IsWindowVisible(target) || !GetWindowTextLength(target))
        return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(target, &pid);
    if (pid == GetCurrentProcessId())
        return TRUE;

    LONG_PTR exstyle = GetWindowLongPtr(target, GWL_EXSTYLE);
    if ((exstyle & WS_EX_TOOLWINDOW) && !(exstyle & WS_EX_APPWINDOW))
        return TRUE;
    if (GetWindow(target, GW_OWNER) && !(exstyle & WS_EX_APPWINDOW))
        return TRUE;

    WTInsertWindow((HWND)lParam, target);
    return TRUE;
}

static void WTUpdateControls(HWND hwnd)
{
    HWND target = WTSelectedWindow(hwnd);
    BOOL valid = target && IsWindow(target);
    if (target != g_wt_force_target) {
        g_wt_force_target = target;
        g_wt_force_enabled = FALSE;
        CheckDlgButton(hwnd, IDC_WT_FORCE, BST_UNCHECKED);
    }
    EnableWindow(GetDlgItem(hwnd, IDC_WT_ACTIVATE), valid);
    EnableWindow(GetDlgItem(hwnd, IDC_WT_TOPMOST), valid);
    EnableWindow(GetDlgItem(hwnd, IDC_WT_FORCE), valid);
    BOOL guarded = g_wt_secure_mirror.guarded
        && g_wt_secure_mirror.target && IsWindow(g_wt_secure_mirror.target);
    /* Always leave this on: its handler validates the target itself and reports
     * exactly why a request failed, so a transient selection hiccup in the list
     * must not lock the user out of turning the guard off again. */
    EnableWindow(GetDlgItem(hwnd, IDC_WT_SECURE_MIRROR), TRUE);
    SetDlgItemText(hwnd, IDC_WT_SECURE_MIRROR,
        guarded ? g_wt_text->secure_mirror_stop : g_wt_text->secure_mirror_start);

    if (!valid) {
        SetDlgItemText(hwnd, IDC_WT_SELECTED, g_wt_text->select_window);
        SetDlgItemText(hwnd, IDC_WT_PERCENT, TEXT("--"));
        SendDlgItemMessage(hwnd, IDC_WT_TOPMOST, BM_SETCHECK, BST_UNCHECKED, 0);
        EnableWindow(GetDlgItem(hwnd, IDC_WT_OPACITY), FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_WT_RESET), FALSE);
        return;
    }

    TCHAR title[512];
    GetWindowText(target, title, ARR_SZ(title));
    if (!title[0])
        lstrcpy_s(title, ARR_SZ(title), g_wt_text->untitled_window);
    SetDlgItemText(hwnd, IDC_WT_SELECTED, title);

    SendDlgItemMessage(hwnd, IDC_WT_TOPMOST, BM_SETCHECK,
        (GetWindowLongPtr(target, GWL_EXSTYLE) & WS_EX_TOPMOST) ? BST_CHECKED : BST_UNCHECKED, 0);

    BYTE alpha = 255;
    BOOL supported = WTGetOpacity(target, &alpha);
    BOOL force = g_wt_force_enabled && g_wt_force_target == target;
    EnableWindow(GetDlgItem(hwnd, IDC_WT_OPACITY), supported || force);
    EnableWindow(GetDlgItem(hwnd, IDC_WT_RESET), WTFindSnapshot(target) != NULL);
    if (supported) {
        int percent = WTPercentFromAlpha(alpha);
        TCHAR text[16];
        SendDlgItemMessage(hwnd, IDC_WT_OPACITY, TBM_SETPOS, TRUE, percent);
        wsprintf(text, TEXT("%d%%"), percent);
        SetDlgItemText(hwnd, IDC_WT_PERCENT, text);
        WTSetStatus(hwnd, ENABLED() ? g_wt_text->global_enabled : g_wt_text->global_paused);
    } else if (force) {
        /* Keep whatever the user already dialled in; forcing the slider back to
         * 100% is why "force" looked like it had no effect. */
        int percent = (int)SendDlgItemMessage(hwnd, IDC_WT_OPACITY, TBM_GETPOS, 0, 0);
        if (percent >= 100 || percent < WT_MIN_OPACITY)
            percent = 100;
        TCHAR text[16];
        wsprintf(text, TEXT("%d%%"), percent);
        SetDlgItemText(hwnd, IDC_WT_PERCENT, text);
        WTSetStatus(hwnd, g_wt_text->special_layered);
    } else {
        SetDlgItemText(hwnd, IDC_WT_PERCENT, g_wt_text->unsupported);
        WTSetStatus(hwnd, g_wt_text->special_layered);
    }
}

static void WTRefreshWindows(HWND hwnd, HWND preferred)
{
    HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
    HWND selected = preferred ? preferred : WTSelectedWindow(hwnd);
    ListView_DeleteAllItems(list);
    g_wt_target_count = 0;
    mem00(g_wt_targets, sizeof(g_wt_targets));
    EnumWindows(WTEnumWindowsProc, (LPARAM)list);

    int index = WTFindWindowItem(list, selected);
    if (index >= 0) {
        ListView_SetItemState(list, index, LVIS_SELECTED | LVIS_FOCUSED,
            LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(list, index, FALSE);
    }

    WTUpdateControls(hwnd);
    WTSnapListHeight(hwnd);
    if (!ListView_GetItemCount(list)) {
        SetDlgItemText(hwnd, IDC_WT_SELECTED, g_wt_text->no_windows);
        WTSetStatus(hwnd, g_wt_text->no_top_windows);
    }
}

static void WTSelectWindow(HWND hwnd, HWND target)
{
    if (!target || !IsWindow(target))
        return;
    target = GetAncestor(target, GA_ROOT);

    HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
    int index = WTFindWindowItem(list, target);
    if (index < 0)
        index = WTInsertWindow(list, target);
    if (index >= 0) {
        ListView_SetItemState(list, index, LVIS_SELECTED | LVIS_FOCUSED,
            LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(list, index, FALSE);
        SetFocus(list);
        WTUpdateControls(hwnd);
    }
}

static void WTAddPickerTooltip(HWND hwnd)
{
    HWND picker = GetDlgItem(hwnd, IDC_WT_PICKER);
    g_wt_tooltip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, NULL, g_hinst, NULL);
    if (!g_wt_tooltip)
        return;

    TOOLINFO tool;
    mem00(&tool, sizeof(tool));
    tool.cbSize = sizeof(tool);
    tool.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    tool.hwnd = hwnd;
    tool.uId = (UINT_PTR)picker;
    tool.lpszText = (TCHAR *)g_wt_text->picker_tip;
    SendMessage(g_wt_tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool);
}

static void WTApplyLanguage(HWND hwnd)
{
    g_wt_text = &g_wt_zh;
    SetWindowText(hwnd, g_wt_text->title);
    SetDlgItemText(hwnd, IDC_WT_REFRESH, g_wt_text->refresh);
    SetDlgItemText(hwnd, IDC_WT_PICKER, g_wt_text->picker);
    SetDlgItemText(hwnd, IDC_WT_ACTIVATE, g_wt_text->activate);
    SetDlgItemText(hwnd, IDC_WT_TOPMOST, g_wt_text->topmost);
    SetDlgItemText(hwnd, IDC_WT_SETTINGS, g_wt_text->settings);
    SetDlgItemText(hwnd, IDC_WT_EXIT, g_wt_text->exit);
    SetDlgItemText(hwnd, IDC_WT_OPACITY_LABEL, g_wt_text->opacity);
    SetDlgItemText(hwnd, IDC_WT_RESET, g_wt_text->restore);
    SetDlgItemText(hwnd, IDC_WT_FORCE, g_wt_text->force_transparency);
    SetDlgItemText(hwnd, IDC_WT_CAPTURE_PROTECTION, g_wt_text->capture_protection);
    SetDlgItemText(hwnd, IDC_WT_SECURE_MIRROR, g_wt_secure_mirror.hwnd
        ? g_wt_text->secure_mirror_stop : g_wt_text->secure_mirror_start);

    HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
    for (int index = 0; index < 4; index++) {
        LVCOLUMN column;
        mem00(&column, sizeof(column));
        column.mask = LVCF_TEXT;
        column.pszText = (TCHAR *)g_wt_text->columns[index];
        ListView_SetColumn(list, index, &column);
    }

    if (IsWindow(g_wt_tooltip)) {
        TOOLINFO tool;
        mem00(&tool, sizeof(tool));
        tool.cbSize = sizeof(tool);
        tool.uFlags = TTF_IDISHWND;
        tool.hwnd = hwnd;
        tool.uId = (UINT_PTR)GetDlgItem(hwnd, IDC_WT_PICKER);
        tool.lpszText = (TCHAR *)g_wt_text->picker_tip;
        SendMessage(g_wt_tooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&tool);
    }

    WTSetStatus(hwnd, ENABLED() ? g_wt_text->global_enabled : g_wt_text->global_paused);
    WTUpdateControls(hwnd);
}

/* Trim the list so that it shows whole rows only; a half clipped row reads as
 * a rendering bug.  Item rects give the header height and the row height of the
 * live control, which is more reliable than measuring the header window. */
static void WTSnapListHeight(HWND hwnd)
{
    HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
    if (!list || !ListView_GetItemCount(list))
        return;

    RECT current;
    if (!GetWindowRect(list, &current))
        return;

    RECT item;
    item.left = LVIR_BOUNDS;
    if (!SendMessage(list, LVM_GETITEMRECT, 0, (LPARAM)&item))
        return;
    item.left = 0;
    item.right = 1;

    int header_height = item.top;
    int row_height = item.bottom - item.top;
    if (header_height <= 0 || row_height <= 0)
        return;

    int available = current.bottom - current.top;
    int rows = (available - header_height) / row_height;
    if (rows < 1)
        rows = 1;
    int height = header_height + rows * row_height;
    if (height <= header_height || height >= available)
        return;

    SetWindowPos(list, NULL, 0, 0, current.right - current.left, height,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(list, NULL, TRUE);
}

/* The panel is designed on a 96 dpi pixel grid; every measurement below is
 * scaled with the window DPI so the layout stays proportional. */
static void WTLayout(HWND hwnd)
{
    wt_theme_t *t = &g_wt_theme;
    if (!t->bg_brush || !IsWindow(hwnd))
        return;

    RECT client;
    GetClientRect(hwnd, &client);
    int width = max(client.right, WTS(360));
    int height = max(client.bottom, WTS(320));

    int pad = WTS(WT_PAD);
    int gap = WTS(WT_GAP);
    int header_height = WTS(WT_HEADER);
    int footer_height = WTS(WT_FOOTER);
    int card2_height = WTS(118);
    int inner = WTS(WT_CARD_IN);

    t->header_rect.left = pad;
    t->header_rect.top = pad;
    t->header_rect.right = width - pad;
    t->header_rect.bottom = pad + header_height;

    t->footer_rect.left = pad + WTS(1);
    t->footer_rect.right = width - pad;
    t->footer_rect.top = height - pad - footer_height;
    t->footer_rect.bottom = t->footer_rect.top + footer_height;

    if (g_wt_set.open) {
        int top = t->header_rect.bottom + gap;
        int bottom = t->footer_rect.top - gap;
        int nav_width = WTSettingsNavWidth(hwnd);
        int action_height = WTS(WT_ACTION);

        g_wt_set.action_rect.left = pad;
        g_wt_set.action_rect.right = width - pad;
        g_wt_set.action_rect.bottom = bottom;
        g_wt_set.action_rect.top = bottom - action_height;

        g_wt_set.nav_rect.left = pad;
        g_wt_set.nav_rect.right = pad + nav_width;
        g_wt_set.nav_rect.top = top;
        g_wt_set.nav_rect.bottom = g_wt_set.action_rect.top - gap;

        g_wt_set.content_rect.left = g_wt_set.nav_rect.right + gap;
        g_wt_set.content_rect.right = width - pad;
        g_wt_set.content_rect.top = top;
        g_wt_set.content_rect.bottom = g_wt_set.action_rect.top - gap;

        int item_height = WTS(WT_NAV_ITEM);
        int item_y = g_wt_set.nav_rect.top + inner - WTS(4);
        for (int i = 0; i < WT_PAGE_COUNT; i++) {
            if (!g_wt_set.nav[i])
                continue;
            MoveWindow(g_wt_set.nav[i], g_wt_set.nav_rect.left + WTS(8), item_y,
                nav_width - WTS(16), item_height, TRUE);
            item_y += item_height + WTS(4);
        }

        g_wt_set.host_rect.left = g_wt_set.content_rect.left + inner;
        g_wt_set.host_rect.right = g_wt_set.content_rect.right - inner;
        g_wt_set.host_rect.top = g_wt_set.content_rect.top + inner;
        g_wt_set.host_rect.bottom = g_wt_set.content_rect.bottom - inner;
        if (g_wt_set.host) {
            MoveWindow(g_wt_set.host, g_wt_set.host_rect.left, g_wt_set.host_rect.top,
                max(WTS(40), g_wt_set.host_rect.right - g_wt_set.host_rect.left),
                max(WTS(40), g_wt_set.host_rect.bottom - g_wt_set.host_rect.top), TRUE);
            WTPositionSettingsPage(hwnd);
        }

        int button_height = WTS(30);
        int button_y = g_wt_set.action_rect.top + (action_height - button_height) / 2;
        int done_width = max(WTS(78), WTTextWidth(hwnd, t->body_font, g_wt_text->done) + WTS(32));
        int apply_width = max(WTS(78), WTTextWidth(hwnd, t->body_font, g_wt_text->apply) + WTS(32));
        if (g_wt_set.done)
            MoveWindow(g_wt_set.done, g_wt_set.action_rect.right - inner - done_width,
                button_y, done_width, button_height, TRUE);
        if (g_wt_set.apply) {
            int apply_x = g_wt_set.action_rect.right - inner - done_width
                - WTS(8) - apply_width;
            MoveWindow(g_wt_set.apply, apply_x, button_y, apply_width, button_height, TRUE);
        }

        int status_dot = WTS(8);
        MoveWindow(GetDlgItem(hwnd, IDC_WT_STATUS),
            t->footer_rect.left + status_dot + WTS(8), t->footer_rect.top + WTS(1),
            max(WTS(60), t->footer_rect.right - t->footer_rect.left - status_dot - WTS(8)),
            t->footer_rect.bottom - t->footer_rect.top - WTS(1), TRUE);
        return;
    }

    t->card2_rect.left = pad;
    t->card2_rect.right = width - pad;
    t->card2_rect.bottom = t->footer_rect.top - WTS(8);
    t->card2_rect.top = t->card2_rect.bottom - card2_height;

    t->card1_rect.left = pad;
    t->card1_rect.right = width - pad;
    t->card1_rect.top = t->header_rect.bottom + gap;
    t->card1_rect.bottom = max(t->card1_rect.top + WTS(140), t->card2_rect.top - gap);

    t->picker_w = WTControlWidth(hwnd, g_wt_text->picker, 84, 26);
    t->refresh_w = WTControlWidth(hwnd, g_wt_text->refresh, 58, 26);
    t->activate_w = WTControlWidth(hwnd, g_wt_text->activate, 78, 26);
    t->settings_w = WTControlWidth(hwnd, g_wt_text->settings, 58, 26);
    t->exit_w = WTControlWidth(hwnd, g_wt_text->exit, 54, 26);
    t->reset_w = WTControlWidth(hwnd, g_wt_text->restore, 84, 26);
    t->mirror_w = WTControlWidth(hwnd, g_wt_text->secure_mirror_start, 150, 26);
    t->topmost_w = WTControlWidth(hwnd, g_wt_text->topmost, 56, 20);
    t->force_w = WTControlWidth(hwnd, g_wt_text->force_transparency, 116, 20);
    t->capture_w = WTControlWidth(hwnd, g_wt_text->capture_protection, 116, 20);

    /* ---- window list card ------------------------------------------- */
    int left = t->card1_rect.left + inner;
    int right = t->card1_rect.right - inner;
    int row = t->card1_rect.top + inner;
    int caption_width = WTTextWidth(hwnd, t->small_font, g_wt_text->card_windows) + WTS(8);

    MoveWindow(GetDlgItem(hwnd, IDC_WT_PICKER), right - t->picker_w, row,
        t->picker_w, WTS(28), TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_REFRESH),
        right - t->picker_w - WTS(8) - t->refresh_w, row, t->refresh_w, WTS(28), TRUE);

    int value_left = left + caption_width + WTS(6);
    int value_right = right - t->picker_w - WTS(8) - t->refresh_w - WTS(10);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_SELECTED), value_left, row + WTS(3),
        max(WTS(48), value_right - value_left), WTS(24), TRUE);

    int list_top = row + WTS(28) + WTS(6);
    int list_width = right - left;
    int list_height = max(WTS(60), t->card1_rect.bottom - inner - list_top);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_LIST), left, list_top, list_width, list_height, TRUE);

    int column_program = WTS(112);
    int column_percent = WTS(72);
    int column_topmost = WTS(52);
    int scroll = GetSystemMetrics(SM_CXVSCROLL);
    ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_WT_LIST), 0,
        max(WTS(96), list_width - column_program - column_percent - column_topmost - scroll - WTS(4)));
    ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_WT_LIST), 1, column_program);
    ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_WT_LIST), 2, column_percent);
    ListView_SetColumnWidth(GetDlgItem(hwnd, IDC_WT_LIST), 3, column_topmost);

    /* ---- opacity and actions card ------------------------------------ */
    left = t->card2_rect.left + inner;
    right = t->card2_rect.right - inner;
    int y = t->card2_rect.top + WTS(12);

    MoveWindow(GetDlgItem(hwnd, IDC_WT_OPACITY_LABEL), left, y + WTS(4), WTS(58), WTS(20), TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_RESET), right - t->reset_w, y, t->reset_w, WTS(28), TRUE);

    int percent_width = WTS(64);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_PERCENT),
        right - t->reset_w - WTS(6) - percent_width, y, percent_width, WTS(28), TRUE);

    int slider_left = left + WTS(58) + WTS(8);
    int slider_right = right - t->reset_w - WTS(6) - percent_width - WTS(8);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_OPACITY), slider_left, y,
        max(WTS(80), slider_right - slider_left), WTS(30), TRUE);

    int checks_y = y + WTS(32);
    int check_x = left;
    MoveWindow(GetDlgItem(hwnd, IDC_WT_TOPMOST), check_x, checks_y, t->topmost_w, WTS(22), TRUE);
    check_x += t->topmost_w + WTS(16);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_FORCE), check_x, checks_y, t->force_w, WTS(22), TRUE);
    check_x += t->force_w + WTS(16);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_CAPTURE_PROTECTION), check_x, checks_y,
        t->capture_w, WTS(22), TRUE);

    int actions_y = checks_y + WTS(22) + WTS(8);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_ACTIVATE), left, actions_y,
        t->activate_w, WTS(28), TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_SECURE_MIRROR), left + t->activate_w + WTS(8), actions_y,
        t->mirror_w, WTS(28), TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_EXIT), right - t->exit_w, actions_y,
        t->exit_w, WTS(28), TRUE);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_SETTINGS),
        right - t->exit_w - WTS(8) - t->settings_w, actions_y, t->settings_w, WTS(28), TRUE);

    int dot = WTS(8);
    MoveWindow(GetDlgItem(hwnd, IDC_WT_STATUS), t->footer_rect.left + dot + WTS(8),
        t->footer_rect.top + WTS(1),
        max(WTS(60), t->footer_rect.right - t->footer_rect.left - dot - WTS(8)),
        t->footer_rect.bottom - t->footer_rect.top - WTS(1), TRUE);

    WTSnapListHeight(hwnd);
}

/* Smallest useful panel: the widest control row plus the outer padding. */
static int WTMinimumWidth(void)
{
    if (!g_wt_theme.dpi)
        return WTS(470);

    int pad = WTS(28);
    int inner = WTS(12) * 2;
    int checks = pad + inner + g_wt_theme.topmost_w + WTS(16)
        + g_wt_theme.force_w + WTS(16) + g_wt_theme.capture_w;
    int actions = pad + inner + g_wt_theme.activate_w + WTS(8)
        + g_wt_theme.mirror_w + WTS(12) + g_wt_theme.settings_w + WTS(8) + g_wt_theme.exit_w;
    int toolbar = pad + inner + WTS(66) + WTS(40) + g_wt_theme.refresh_w + WTS(8)
        + g_wt_theme.picker_w;
    return max(WTS(470), max(checks, max(actions, toolbar)));
}

static int WTMinimumHeight(void)
{
    return WTS(400);
}

static void WTApplyDefaultSize(HWND hwnd)
{
    RECT frame = { 0, 0, max(WTS(560), WTMinimumWidth()), max(WTS(452), WTMinimumHeight()) };
    AdjustWindowRectEx(&frame, (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE), FALSE,
        (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE));
    int width = frame.right - frame.left;
    int height = frame.bottom - frame.top;

    RECT work;
    if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &work, 0)) {
        work.left = 0;
        work.top = 0;
        work.right = GetSystemMetrics(SM_CXSCREEN);
        work.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int x = work.left + (work.right - work.left - width) / 2;
    int y = work.top + (work.bottom - work.top - height) / 2;
    SetWindowPos(hwnd, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

static void WTFinishPicker(HWND picker, BOOL choose)
{
    if (g_picker_finishing || !IsWindow(picker))
        return;
    g_picker_finishing = 1;

    POINT point;
    GetCursorPos(&point);
    HWND owner = (HWND)GetWindowLongPtr(picker, GWLP_USERDATA);
    ShowWindow(picker, SW_HIDE);
    KillTimer(picker, 1);
    if (GetCapture() == picker)
        ReleaseCapture();

    HWND target = NULL;
    if (choose) {
        target = WindowFromPoint(point);
        if (target)
            target = GetAncestor(target, GA_ROOT);
        DWORD pid = 0;
        if (target)
            GetWindowThreadProcessId(target, &pid);
        if (pid == GetCurrentProcessId())
            target = NULL;
    }

    DestroyWindow(picker);
    g_pickerwnd = NULL;
    UnregisterClass(TEXT(APP_NAMEA) TEXT("-window-picker"), g_hinst);
    if (target && IsWindow(owner)) {
        PostMessage(owner, WM_WT_PICKED, (WPARAM)target, 0);
    } else if (IsWindow(owner)) {
        SetForegroundWindow(owner);
        WTSetStatus(owner, g_wt_text->picker_cancelled);
    }
    g_picker_finishing = 0;
}

static LRESULT CALLBACK WTPickerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_LBUTTONUP) {
        WTFinishPicker(hwnd, TRUE);
        return 0;
    }
    if (msg == WM_RBUTTONUP || (msg == WM_KEYDOWN && wParam == VK_ESCAPE)) {
        WTFinishPicker(hwnd, FALSE);
        return 0;
    }
    if (msg == WM_TIMER && wParam == 1) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            WTFinishPicker(hwnd, FALSE);
        return 0;
    }
    if (msg == WM_CAPTURECHANGED || msg == WM_CANCELMODE) {
        if (!g_picker_finishing)
            WTFinishPicker(hwnd, FALSE);
        return 0;
    }
    if (msg == WM_SETCURSOR) {
        SetCursor(LoadCursor(g_hinst, MAKEINTRESOURCE(IDI_FIND)));
        return TRUE;
    }
    if (msg == WM_ERASEBKGND)
        return TRUE;
    if (msg == WM_PAINT) {
        PAINTSTRUCT paint;
        BeginPaint(hwnd, &paint);
        EndPaint(hwnd, &paint);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void WTBeginPicker(HWND owner)
{
    if (IsWindow(g_pickerwnd))
        return;

    WNDCLASS wnd;
    mem00(&wnd, sizeof(wnd));
    wnd.lpfnWndProc = WTPickerProc;
    wnd.hInstance = g_hinst;
    wnd.hCursor = LoadCursor(g_hinst, MAKEINTRESOURCE(IDI_FIND));
    wnd.lpszClassName = TEXT(APP_NAMEA) TEXT("-window-picker");
    RegisterClass(&wnd);

    int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    g_pickerwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        wnd.lpszClassName, NULL, WS_POPUP, left, top, width, height,
        NULL, NULL, g_hinst, NULL);
    if (!g_pickerwnd) {
        UnregisterClass(wnd.lpszClassName, g_hinst);
        WTSetStatus(owner, g_wt_text->picker_start_failed);
        return;
    }

    SetWindowLongPtr(g_pickerwnd, GWLP_USERDATA, (LONG_PTR)owner);
    ShowWindow(g_pickerwnd, SW_SHOWNA);
    SetCapture(g_pickerwnd);
    if (GetCapture() != g_pickerwnd) {
        WTFinishPicker(g_pickerwnd, FALSE);
        WTSetStatus(owner, g_wt_text->picker_capture_failed);
        return;
    }
    SetTimer(g_pickerwnd, 1, 50, NULL);
    SetCursor(wnd.hCursor);
    WTSetStatus(owner, g_wt_text->picker_active);
}

static INT_PTR CALLBACK WindowToolDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INITDIALOG) {
        g_toolwnd = hwnd;
        g_wt_text = &g_wt_zh;
        g_wt_capture_protection = WTReadCaptureProtection();
        wsprintf(g_wt_generation_property, TEXT("ShengZiMao-WindowGeneration-%lu"),
            GetCurrentProcessId());
        g_wt_generation = ((UINT_PTR)GetTickCount() << 12) ^ GetCurrentProcessId();

        WTInitializeTheme(hwnd);
        WTSetCaptureProtection(hwnd, g_wt_capture_protection);

        g_wt_icon_big = (HICON)LoadImage(g_hinst, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON,
            GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
        g_wt_icon_small = (HICON)LoadImage(g_hinst, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
        if (g_wt_icon_big)
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_wt_icon_big);
        if (g_wt_icon_small || g_wt_icon_big)
            SendMessage(hwnd, WM_SETICON, ICON_SMALL,
                (LPARAM)(g_wt_icon_small ? g_wt_icon_small : g_wt_icon_big));

        if (g_wt_theme.dpi >= 96) {
            int corner = 2; /* DWMWCP_ROUND, ignored before Windows 11 */
            DwmSetWindowAttributeL(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
        }

        HWND list = GetDlgItem(hwnd, IDC_WT_LIST);
        ListView_SetExtendedListViewStyle(list,
            LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
        for (int index = 0; index < 4; index++) {
            LVCOLUMN column;
            mem00(&column, sizeof(column));
            column.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
            column.iSubItem = index;
            column.pszText = (TCHAR *)g_wt_text->columns[index];
            column.cx = 80;
            ListView_InsertColumn(list, index, &column);
        }

        SendDlgItemMessage(hwnd, IDC_WT_OPACITY, TBM_SETRANGE, TRUE,
            MAKELPARAM(WT_MIN_OPACITY, 100));
        SendDlgItemMessage(hwnd, IDC_WT_OPACITY, TBM_SETTICFREQ, 10, 0);
        SendDlgItemMessage(hwnd, IDC_WT_OPACITY, TBM_SETPAGESIZE, 0, 10);
        CheckDlgButton(hwnd, IDC_WT_CAPTURE_PROTECTION,
            g_wt_capture_protection ? BST_CHECKED : BST_UNCHECKED);
        WTSetControlFonts(hwnd);
        WTApplyListMetrics(hwnd);
        WTAddPickerTooltip(hwnd);
        WTApplyLanguage(hwnd);
        WTApplyDefaultSize(hwnd);
        WTLayout(hwnd);
        WTRefreshWindows(hwnd, NULL);
        SetFocus(GetDlgItem(hwnd, IDC_WT_LIST));
        return TRUE;
    }

    if (msg == WM_ERASEBKGND) {
        if (g_wt_theme.bg_brush) {
            RECT client;
            GetClientRect(hwnd, &client);
            FillRect((HDC)wParam, &client, g_wt_theme.bg_brush);
        }
        return TRUE;
    }

    if (msg == WM_PAINT) {
        WTPaintPanel(hwnd);
        return TRUE;
    }

    if (msg == WM_CTLCOLORSTATIC || msg == WM_CTLCOLORBTN) {
        if (!g_wt_theme.card_brush)
            return FALSE;
        int id = GetDlgCtrlID((HWND)lParam);
        SetBkMode((HDC)wParam, TRANSPARENT);
        if (id == IDC_WT_PERCENT) {
            SetTextColor((HDC)wParam, WT_CLR_ACCENT);
            return (INT_PTR)g_wt_theme.card_brush;
        }
        if (id == IDC_WT_OPACITY_LABEL) {
            SetTextColor((HDC)wParam, WT_CLR_DIM);
            return (INT_PTR)g_wt_theme.card_brush;
        }
        if (id == IDC_WT_STATUS) {
            SetTextColor((HDC)wParam, WT_CLR_DIM);
            return (INT_PTR)g_wt_theme.bg_brush;
        }
        SetTextColor((HDC)wParam, WT_CLR_TEXT);
        return (INT_PTR)g_wt_theme.card_brush;
    }

    if (msg == WM_CTLCOLORDLG)
        return (INT_PTR)g_wt_theme.bg_brush;

    if (msg == WM_DPICHANGED) {
        RECT *suggested = (RECT *)lParam;
        BOOL settings_open = g_wt_set.open;
        int settings_page = g_wt_set.page;
        WTSettingsShutdown();
        WTInitializeTheme(hwnd);
        WTSetControlFonts(hwnd);
        WTApplyListMetrics(hwnd);
        SetWindowPos(hwnd, NULL, suggested->left, suggested->top,
            suggested->right - suggested->left, suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        if (settings_open)
            WTSettingsEnter(hwnd, settings_page);
        WTLayout(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        return TRUE;
    }

    if (msg == WM_SIZE) {
        if (wParam == SIZE_MINIMIZED) {
            ShowWindow(hwnd, SW_HIDE);
            return TRUE;
        }
        WTLayout(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        return TRUE;
    }

    if (msg == WM_GETMINMAXINFO) {
        MINMAXINFO *mmi = (MINMAXINFO *)lParam;
        int min_width = WTMinimumWidth();
        int min_height = WTMinimumHeight();
        if (g_wt_set.open) {
            min_width = max(min_width, WTSettingsNeededWidth(hwnd));
            min_height = max(min_height, WTSettingsNeededHeight());
        }
        RECT minimum = { 0, 0, min_width, min_height };
        AdjustWindowRectEx(&minimum, (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE), FALSE,
            (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE));
        mmi->ptMinTrackSize.x = minimum.right - minimum.left;
        mmi->ptMinTrackSize.y = minimum.bottom - minimum.top;
        return TRUE;
    }

    if (msg == WM_DRAWITEM) {
        DRAWITEMSTRUCT *item = (DRAWITEMSTRUCT *)lParam;
        if (item->CtlID >= IDC_WT_NAV_GENERAL && item->CtlID <= IDC_WT_NAV_ABOUT) {
            WTDrawNavButton(item);
            return TRUE;
        }
        return FALSE;
    }

    if (msg == WM_NOTIFY && ((LPNMHDR)lParam)->idFrom == IDC_WT_LIST) {
        LPNMHDR header = (LPNMHDR)lParam;
        if (header->code == LVN_ITEMCHANGED) {
            WTUpdateControls(hwnd);
        } else if (header->code == NM_DBLCLK) {
            HWND target = WTSelectedWindow(hwnd);
            if (target) {
                if (IsIconic(target))
                    ShowWindow(target, SW_RESTORE);
                SetForegroundWindow(target);
            }
        }
        return TRUE;
    }

    if (msg == WM_HSCROLL && (HWND)lParam == GetDlgItem(hwnd, IDC_WT_OPACITY)) {
        HWND target = WTSelectedWindow(hwnd);
        if (target) {
            int percent = (int)SendDlgItemMessage(hwnd, IDC_WT_OPACITY, TBM_GETPOS, 0, 0);
            BOOL force = g_wt_force_enabled && g_wt_force_target == target;
            if (WTSetOpacity(target, percent, force)) {
                TCHAR text[16];
                wsprintf(text, TEXT("%d%%"), percent);
                SetDlgItemText(hwnd, IDC_WT_PERCENT, text);
                int index = WTFindWindowItem(GetDlgItem(hwnd, IDC_WT_LIST), target);
                if (index >= 0)
                    WTUpdateListItem(GetDlgItem(hwnd, IDC_WT_LIST), index, target);
                WTSetStatus(hwnd, g_wt_text->opacity_updated);
            } else {
                WTSetStatus(hwnd, g_wt_text->change_failed);
                WTUpdateControls(hwnd);
            }
        }
        return TRUE;
    }

    if (msg == WM_WT_PICKED) {
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        WTSelectWindow(hwnd, (HWND)wParam);
        WTSetStatus(hwnd, g_wt_text->picker_selected);
        return TRUE;
    }

    if (msg == WM_COMMAND) {
        int id = LOWORD(wParam);
        int event = HIWORD(wParam);
        if (id == IDC_WT_REFRESH && event == BN_CLICKED) {
            WTRefreshWindows(hwnd, NULL);
            WTSetStatus(hwnd, g_wt_text->list_refreshed);
            return TRUE;
        }
        if (id == IDC_WT_PICKER && event == BN_CLICKED) {
            WTBeginPicker(hwnd);
            return TRUE;
        }
        if (id == IDC_WT_ACTIVATE && event == BN_CLICKED) {
            HWND target = WTSelectedWindow(hwnd);
            if (target) {
                if (IsIconic(target))
                    ShowWindow(target, SW_RESTORE);
                SetForegroundWindow(target);
            }
            return TRUE;
        }
        if (id == IDC_WT_SETTINGS && event == BN_CLICKED) {
            WTSettingsEnter(hwnd, 0);
            return TRUE;
        }
        if (id >= IDC_WT_NAV_GENERAL && id <= IDC_WT_NAV_ABOUT && event == BN_CLICKED) {
            WTSettingsShowPage(hwnd, id - IDC_WT_NAV_GENERAL);
            return TRUE;
        }
        if (id == IDC_WT_APPLY && event == BN_CLICKED) {
            WTSetStatus(hwnd, WTSettingsApply(hwnd)
                ? g_wt_text->settings_applied : g_wt_text->settings_failed);
            return TRUE;
        }
        if (id == IDC_WT_DONE && event == BN_CLICKED) {
            if (WTSettingsApply(hwnd))
                WTSettingsClose(hwnd);
            return TRUE;
        }
        if (id == IDCANCEL && g_wt_set.open) {
            if (WTSettingsApply(hwnd))
                WTSettingsClose(hwnd);
            return TRUE;
        }
        if (id == IDC_WT_EXIT && event == BN_CLICKED) {
            SendMessage(g_hwnd, WM_COMMAND, CMD_EXIT, 0);
            return TRUE;
        }
        if (id == IDC_WT_RESET && event == BN_CLICKED) {
            HWND target = WTSelectedWindow(hwnd);
            if (target && WTRestoreOpacity(target)) {
                WTUpdateControls(hwnd);
                int index = WTFindWindowItem(GetDlgItem(hwnd, IDC_WT_LIST), target);
                if (index >= 0)
                    WTUpdateListItem(GetDlgItem(hwnd, IDC_WT_LIST), index, target);
                WTSetStatus(hwnd, g_wt_text->restored);
            } else {
                WTSetStatus(hwnd, g_wt_text->restore_failed);
            }
            return TRUE;
        }
        if (id == IDC_WT_FORCE && event == BN_CLICKED) {
            HWND target = WTSelectedWindow(hwnd);
            g_wt_force_target = target;
            g_wt_force_enabled = target
                && SendDlgItemMessage(hwnd, IDC_WT_FORCE, BM_GETCHECK, 0, 0) == BST_CHECKED;
            WTUpdateControls(hwnd);
            if (g_wt_force_enabled)
                WTSetStatus(hwnd, g_wt_text->special_layered);
            return TRUE;
        }
        if (id == IDC_WT_CAPTURE_PROTECTION && event == BN_CLICKED) {
            BOOL enabled = IsDlgButtonChecked(hwnd, IDC_WT_CAPTURE_PROTECTION) == BST_CHECKED;
            if (WTSaveCaptureProtection(hwnd, enabled)) {
                WTSetStatus(hwnd, enabled ? g_wt_text->capture_protection_on
                    : g_wt_text->capture_protection_off);
            } else {
                CheckDlgButton(hwnd, IDC_WT_CAPTURE_PROTECTION,
                    g_wt_capture_protection ? BST_CHECKED : BST_UNCHECKED);
                WTSetStatus(hwnd, g_wt_text->capture_protection_failed);
            }
            return TRUE;
        }
        if (id == IDC_WT_SECURE_MIRROR && event == BN_CLICKED) {
            BOOL guarded = g_wt_secure_mirror.guarded
                && g_wt_secure_mirror.target && IsWindow(g_wt_secure_mirror.target);
            if (guarded) {
                WTSecureMirrorStop(TRUE);
                WTUpdateControls(hwnd);
            } else {
                WTSecureMirrorStart(hwnd, WTSelectedWindow(hwnd));
                WTUpdateControls(hwnd);
            }
            return TRUE;
        }
        if (id == IDC_WT_TOPMOST && event == BN_CLICKED) {
            HWND target = WTSelectedWindow(hwnd);
            BOOL topmost = SendDlgItemMessage(hwnd, IDC_WT_TOPMOST, BM_GETCHECK, 0, 0) == BST_CHECKED;
            if (!target || !SetWindowPos(target, topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                    0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)) {
                WTSetStatus(hwnd, g_wt_text->topmost_failed);
                WTUpdateControls(hwnd);
            } else {
                int index = WTFindWindowItem(GetDlgItem(hwnd, IDC_WT_LIST), target);
                if (index >= 0)
                    WTUpdateListItem(GetDlgItem(hwnd, IDC_WT_LIST), index, target);
                WTSetStatus(hwnd, topmost ? g_wt_text->topmost_on : g_wt_text->topmost_off);
            }
            return TRUE;
        }
    }

    if (msg == WM_CLOSE) {
        ShowWindow(hwnd, SW_HIDE);
        return TRUE;
    }
    if (msg == WM_DESTROY) {
        WTSecureMirrorStop(FALSE);
        WTSettingsShutdown();
        if (IsWindow(g_pickerwnd)) {
            g_picker_finishing = 1;
            KillTimer(g_pickerwnd, 1);
            if (GetCapture() == g_pickerwnd)
                ReleaseCapture();
            DestroyWindow(g_pickerwnd);
            UnregisterClass(TEXT(APP_NAMEA) TEXT("-window-picker"), g_hinst);
            g_picker_finishing = 0;
        }
        g_pickerwnd = NULL;
        WTReleaseGenerations();
        WTReleaseTheme();
        if (g_wt_icon_big)
            DestroyIcon(g_wt_icon_big);
        if (g_wt_icon_small)
            DestroyIcon(g_wt_icon_small);
        g_wt_icon_big = NULL;
        g_wt_icon_small = NULL;
        g_toolwnd = NULL;
        g_wt_target_count = 0;
        g_wt_force_target = NULL;
        g_wt_force_enabled = FALSE;
        return TRUE;
    }
    return FALSE;
}

static void OpenWindowTool(void)
{
    if (IsWindow(g_toolwnd)) {
        ShowWindow(g_toolwnd, SW_RESTORE);
        SetForegroundWindow(g_toolwnd);
        WTRefreshWindows(g_toolwnd, NULL);
        return;
    }

    InitCommonControls();
    g_toolwnd = CreateDialog(g_hinst, MAKEINTRESOURCE(IDD_WINDOWTOOL),
        NULL, WindowToolDialogProc);
    if (g_toolwnd) {
        ShowWindow(g_toolwnd, SW_SHOW);
        SetForegroundWindow(g_toolwnd);
    }
}


/* Entry points used by the tray menu and the command line: open the settings
 * pages inside this window instead of a separate property sheet. */
static void OpenWindowToolSettings(int page)
{
    if (!IsWindow(g_toolwnd))
        OpenWindowTool();
    if (!IsWindow(g_toolwnd))
        return;

    ShowWindow(g_toolwnd, SW_SHOW);
    SetForegroundWindow(g_toolwnd);
    WTSettingsEnter(g_toolwnd, page);
}

static void CloseWindowToolSettings(void)
{
    if (IsWindow(g_toolwnd))
        WTSettingsClose(g_toolwnd);
}

static void DestroyWindowTool(void)
{
    if (IsWindow(g_toolwnd))
        DestroyWindow(g_toolwnd);
    g_toolwnd = NULL;
}















