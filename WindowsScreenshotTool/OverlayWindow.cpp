#include "OverlayWindow.h"

#include <windows.h>
#include <windowsx.h>
#include <corecrt_wstdio.h>

OverlayWindow* OverlayWindow::s_instance = nullptr;

bool OverlayWindow::Show(HBITMAP frozenScreen, RECT& outSelection, bool& aborted) {
    s_instance = this;
    hFrozen = frozenScreen;
    abortedFlag = false;

    // DEBUG
    //MessageBoxW(NULL, L"Entered Show()", L"DEBUG", MB_OK);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"ScreenshotOverlay";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    RegisterClass(&wc);

    int sx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // orig WS_EX_TRANSPARENT to WS_EX_TOOLWINDOW
        wc.lpszClassName, nullptr,
        WS_POPUP,
        sx, sy, w, h,
        nullptr, nullptr, wc.hInstance, nullptr
    );
    // before showing the window
    //SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    // Display frozen bitmap via UpdateLayeredWindow
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);
    SelectObject(memDC, hFrozen);
    SIZE size = { w, h };
    POINT zero = { 0, 0 }, dst = { sx, sy };

    //UpdateLayeredWindow(hwnd, screenDC, &dst, &size, memDC, &zero, 0, nullptr, ULW_OPAQUE);
    
    BOOL ulw = UpdateLayeredWindow(hwnd, screenDC, &dst, &size, memDC, &zero, 0, nullptr, ULW_OPAQUE);

    if (!ulw) {
        DWORD err = GetLastError();
        wchar_t buf[100];
        swprintf_s(buf, L"UpdateLayeredWindow failed: %u", err);
        MessageBoxW(NULL, buf, L"DEBUG", MB_OK);
    }

    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);

    ShowWindow(hwnd, SW_SHOW);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(hwnd);
    s_instance = nullptr;

    if (!abortedFlag) {
        RECT r = { ptStart.x, ptStart.y, ptEnd.x, ptEnd.y };
        outSelection.left = min(r.left, r.right);
        outSelection.top = min(r.top, r.bottom);
        outSelection.right = max(r.left, r.right);
        outSelection.bottom = max(r.top, r.bottom);
    }
    aborted = abortedFlag;
    return !abortedFlag;
}

LRESULT CALLBACK OverlayWindow::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto* self = s_instance;
    switch (msg) {
    case WM_LBUTTONDOWN:
        self->dragging = true;
        self->ptStart = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        SetCapture(hwnd);
        return 0;
    case WM_MOUSEMOVE:
        if (self->dragging) {
            self->ptEnd = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            HDC dc = GetDC(hwnd);
            SetROP2(dc, R2_NOT); // XOR mode for rubber-band
            Rectangle(dc,
                self->ptStart.x, self->ptStart.y,
                self->ptEnd.x, self->ptEnd.y);
            ReleaseDC(hwnd, dc);
        }
        return 0;
    case WM_LBUTTONUP:
        if (self->dragging) {
            self->ptEnd = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            self->dragging = false;
            ReleaseCapture();
            PostQuitMessage(0);
        }
        return 0;
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
        if (msg == WM_RBUTTONDOWN || (msg == WM_KEYDOWN && wp == VK_ESCAPE)) {
            self->abortedFlag = true;
            PostQuitMessage(0);
        }
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

