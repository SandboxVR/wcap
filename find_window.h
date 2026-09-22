#pragma once

#include <winuser.h>
#include <dwmapi.h>

typedef struct WindowInfo
{
    HWND handle;
    const wchar_t* title;
} WindowInfo;

#define CLASS_NAME_LEN 256

BOOL IsCapturableWindow(HWND hwnd)
{
    wchar_t className[CLASS_NAME_LEN];

    // filter invalid windows handle
    if (hwnd == GetShellWindow() || !IsWindowVisible(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd)
    {
        return FALSE;
    }

    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if (style & WS_DISABLED)
    {
        return FALSE;
    }

    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) // No tooltips
    {
        return FALSE;
    }

    memset(&className, 0, sizeof(wchar_t) * CLASS_NAME_LEN);
    GetClassNameW(hwnd, (LPWSTR)(&className), CLASS_NAME_LEN);

    if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(className, L"ApplicationFrameWindow") == 0)
    {
        DWORD cloaked = FALSE;
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && (cloaked == DWM_CLOAKED_SHELL))
        {
            return FALSE;
        }
    }

    return TRUE;
}

int cmpTitle(HWND hwnd, int titleLen, const wchar_t *target)
{
    wchar_t* title = malloc(sizeof(wchar_t) * titleLen);
    GetWindowTextW(hwnd, title, titleLen);
    title[titleLen - 1] = L'\0';

    int res = wcscmp(target, title);
    free(title);
    return res;
}

BOOL EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    WindowInfo* winInfo = (WindowInfo*)(lParam);

    // title is empty
    int titleLen = GetWindowTextLengthW(hwnd);
    if (titleLen <= 0)
    {
        return TRUE;
    }

    if (!IsCapturableWindow(hwnd))
    {
        return TRUE;
    }

    // get the title
    if (cmpTitle(hwnd, titleLen+1, winInfo->title) == 0)
    {
        // stop enumeration.
        winInfo->handle = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND findWindow(const wchar_t* title)
{
    WindowInfo winInfo;
    winInfo.title = title;
    winInfo.handle = NULL;

    // enum all windows
    EnumWindows(EnumWindowsCallback, (LPARAM)(& winInfo));

    return winInfo.handle;
}