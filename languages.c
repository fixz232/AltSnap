/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2015    Stefan Sundin                                   *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 or later.              *
 * Modified By Raymond Gillibert in 2022                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "languages.h"

static struct strings *l10n_ini = NULL;
static const struct strings *l10n = &en_US;

/////////////////////////////////////////////////////////////////////////////
// Copies and remove the accelerators & sign. and txt between ( ).
static size_t lstrcpy_noaccel(TCHAR *__restrict__ dest, size_t destlen, const TCHAR *__restrict__ source)
{
    size_t i=0, j=0;
    while(i < destlen && source[i]) {
        dest[j] = source[i];
        if (source[i] == '(') {
            // If we found a '(' go to the closing one ')'.
            while(i < destlen && source[i] && source[i] != ')') i++;
            i++;
            i += source[i] == ' '; // Remove space after ) if there is one.
        } else {
            j += source[i] != '&';
            i++;
        }
    }
    dest[j] = '\0';
    return j;
}
/////////////////////////////////////////////////////////////////////////////
static pure size_t lstrlen_resolved(const TCHAR *__restrict__ str)
{
    // Return the length of str, having resolved escape sequences
    const TCHAR *ptr;
    int num_escape_sequences = 0;
    for (ptr=str; *ptr != '\0'; ptr++) {
        if (*ptr == '\\' && *(ptr+1) != '\0') {
            ptr++;
            num_escape_sequences++;
        }
    }
    return ptr-str-num_escape_sequences;
}

static void lstrcpy_resolve(TCHAR *__restrict__ dest, size_t dstlen, const TCHAR *__restrict__ source)
{
    // Copy from source to dest, resolving \\n to \n
    for (; --dstlen && *source != '\0'; source++,dest++) {
        if (*source == '\\' && *(source+1) == 'n') {
            *dest = '\n';
            source++;
        } else {
            *dest = *source;
        }
    }
    *dest = '\0';
}

/////////////////////////////////////////////////////////////////////////////
// Load the fixed Simplified Chinese table embedded in the executable.
static BOOL IsTranslationHeader(const TCHAR *line, size_t len)
{
    static const TCHAR header[] = TEXT("[Translation]");
    if (len != ARR_SZ(header) - 1)
        return FALSE;
    for (size_t i = 0; i < len; i++) {
        if (line[i] != header[i])
            return FALSE;
    }
    return TRUE;
}

static TCHAR *ExtractTranslationSection(const BYTE *raw, DWORD rawlen)
{
    if (!raw || rawlen < 4 || (rawlen & 1) || raw[0] != 0xff || raw[1] != 0xfe)
        return NULL;

    const TCHAR *text = (const TCHAR *)(raw + 2);
    size_t textlen = (rawlen - 2) / sizeof(TCHAR);
    TCHAR *section = (TCHAR *)malloc((textlen + 2) * sizeof(TCHAR));
    if (!section)
        return NULL;

    size_t pos = 0, out = 0;
    BOOL in_translation = FALSE;
    while (pos < textlen) {
        size_t start = pos;
        while (pos < textlen && text[pos] != TEXT('\r') && text[pos] != TEXT('\n'))
            pos++;
        size_t len = pos - start;
        while (pos < textlen && (text[pos] == TEXT('\r') || text[pos] == TEXT('\n')))
            pos++;

        if (len && text[start] == TEXT('[')) {
            if (IsTranslationHeader(text + start, len)) {
                in_translation = TRUE;
            } else if (in_translation) {
                break;
            }
            continue;
        }
        if (!in_translation || !len || text[start] == TEXT(';'))
            continue;

        for (size_t i = 0; i < len; i++)
            section[out++] = text[start + i];
        section[out++] = TEXT('\0');
    }

    section[out++] = TEXT('\0');
    if (!in_translation || out == 1) {
        free(section);
        return NULL;
    }
    return section;
}

static BOOL LoadTranslationSection(const TCHAR *section)
{
    l10n_ini = (struct strings *)calloc(1, sizeof(struct strings));
    if (!l10n_ini)
        return FALSE;

    TCHAR const *ini_map[ARR_SZ(l10n_inimapping)];
    RGIniMapSection(section, ini_map, l10n_inimapping, ARR_SZ(l10n_inimapping));

    for (size_t i = 0; i < ARR_SZ(l10n_inimapping); i++) {
        const TCHAR *txt = ini_map[i] ? ini_map[i] : ((const TCHAR *const *)&en_US)[i * 2];
        if (!txt)
            continue;

        TCHAR buf[128];
        TCHAR **deststr = &((TCHAR **)l10n_ini)[i * 2];
        if (deststr == &l10n_ini->AboutVersion) {
            lstrcpy_s(buf, ARR_SZ(buf), txt);
            lstrcat_s(buf, ARR_SZ(buf), TEXT(" ") TEXT(APP_VERSION));
            txt = buf;
        }
        size_t destlen = lstrlen(txt) + 1;
        *deststr = (TCHAR *)malloc(destlen * sizeof(TCHAR));
        if (!*deststr)
            return FALSE;
        lstrcpy_resolve(*deststr, destlen, txt);
    }
    l10n = l10n_ini;
    return TRUE;
}

static BOOL LoadSimplifiedChineseResource(void)
{
    HRSRC resource = FindResource(g_hinst, MAKEINTRESOURCE(IDR_ZH_CN), RT_RCDATA);
    if (!resource)
        return FALSE;
    HGLOBAL loaded = LoadResource(g_hinst, resource);
    const BYTE *raw = loaded ? (const BYTE *)LockResource(loaded) : NULL;
    TCHAR *section = ExtractTranslationSection(raw, SizeofResource(g_hinst, resource));
    if (!section)
        return FALSE;
    BOOL result = LoadTranslationSection(section);
    free(section);
    return result;
}

void FreeAllLangRelated(void)
{
    if (l10n_ini) {
        for (size_t i=0; i < sizeof(struct strings) / sizeof(TCHAR*); i++) {
            TCHAR **deststr = &((TCHAR **)l10n_ini)[i];
            free(*deststr);
        }
        free(l10n_ini);
        l10n_ini = NULL;
    }
    l10n = &en_US;
}

BOOL UpdateLanguage(void)
{
    BOOL loaded = LoadSimplifiedChineseResource();
    if (!loaded || l10n == &en_US || lstrcmpi(l10n->Code, TEXT("zh-CN"))) {
        FreeAllLangRelated();
        return FALSE;
    }
    return TRUE;
}
