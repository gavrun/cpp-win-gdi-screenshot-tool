#include "OverlayWindow.h"

#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <corecrt_wstdio.h>

// Static pointer to the current instance
OverlayWindow* OverlayWindow::s_instance = nullptr;


// Helper function to exit modal message loop as GetMessage function "sleeping" and waiting for a message
void OverlayWindow::ExitLoop() {
    // Only try to exit the loop once
    if (m_isLooping) {
        m_isLooping = false;
        // Post a null message to unblock GetMessage 
        PostMessage(hwnd, WM_NULL, 0, 0);
    }
}


// Method that launches the overlay
bool OverlayWindow::Show(HBITMAP frozenScreen, RECT& outSelection, bool& aborted) {
    // WndProc to call member functions 
    s_instance = this;
    hFrozen = frozenScreen;
    abortedFlag = false;
    isDragging = false;

    // DEBUG
    //MessageBoxW(NULL, L"Entered Show()", L"DEBUG", MB_OK);

    // Register window class for overlay
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"ScreenshotOverlay";
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS); // IDC_ARROW, IDC_CROSS
    wc.hbrBackground = nullptr; // No background drawing
    RegisterClass(&wc);

    // Get full virtual screen size to cover all monitors
    int sx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Create borderless topmost window
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // WS_EX_LAYERED, WS_EX_TOPMOST, WS_EX_TRANSPARENT, WS_EX_TOOLWINDOW prevents from appearing in taskbar
        wc.lpszClassName, nullptr,
        WS_POPUP,
        sx, sy, w, h,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    // before showing the window
    //SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    // Display frozen bitmap via UpdateLayeredWindow
    //HDC screenDC = GetDC(nullptr);
    //HDC memDC = CreateCompatibleDC(screenDC);
    //SelectObject(memDC, hFrozen);
    //SIZE size = { w, h };
    //POINT zero = { 0, 0 }, dst = { sx, sy };

    //UpdateLayeredWindow(hwnd, screenDC, &dst, &size, memDC, &zero, 0, nullptr, ULW_OPAQUE);
    
    //BOOL ulw = UpdateLayeredWindow(hwnd, screenDC, &dst, &size, memDC, &zero, 0, nullptr, ULW_OPAQUE);

    //if (!ulw) {
    //    DWORD err = GetLastError();
    //    wchar_t buf[100];
    //    swprintf_s(buf, L"UpdateLayeredWindow failed: %u", err);
    //    MessageBoxW(NULL, buf, L"DEBUG", MB_OK);
    //}

    //DeleteDC(memDC);
    //ReleaseDC(nullptr, screenDC);

    //HDC hdcWindow = GetDC(hwnd);
    //HDC memDC = CreateCompatibleDC(hdcWindow);
    //SelectObject(memDC, hFrozen);
    //BitBlt(hdcWindow, 0, 0, w, h, memDC, 0, 0, SRCCOPY);

    //DeleteDC(memDC);
    //ReleaseDC(hwnd, hdcWindow);

    // Draw frozen screen onto the window as background (force initial WM_PAINT)
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    // Modal message loop (hijacks message flow to process input only for overlay)
    m_isLooping = true;
    MSG msg;
    while (m_isLooping && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    DestroyWindow(hwnd);
    s_instance = nullptr;

    if (!abortedFlag) {
        // Normalize the rectangle before returning if user didn't abort
        outSelection.left = min(ptStart.x, ptEnd.x);
        outSelection.top = min(ptStart.y, ptEnd.y);
        outSelection.right = max(ptStart.x, ptEnd.x);
        outSelection.bottom = max(ptStart.y, ptEnd.y);
    }
    aborted = abortedFlag;
    return !abortedFlag;
}

// Function handles all drawing for the window (invoked by WM_PAINT)
void OverlayWindow::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Double Buffering
    RECT clientRect;
    GetClientRect(hwnd, &clientRect); 
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    // Create memory Device Context compatible with the screen
    HDC memDC = CreateCompatibleDC(hdc);
    // Create off-screen "back buffer" (a memory bitmap)
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap); // drawing to memDC now goes to memBitmap

    // Draw the frozen screenshot as the background 
    HDC frozenDC = CreateCompatibleDC(hdc);
    HBITMAP oldFrozenBitmap = (HBITMAP)SelectObject(frozenDC, hFrozen);
    BitBlt(memDC, 0, 0, width, height, frozenDC, 0, 0, SRCCOPY);
    SelectObject(frozenDC, oldFrozenBitmap);
    DeleteDC(frozenDC);

    // Draw the rubber-band rectangle if user is dragging
    if (isDragging) {
        // Use a hollow brush to only draw the outline
        HBRUSH hollowBrush = (HBRUSH)GetStockObject(NULL_BRUSH); // Rectangle() only draws the border
        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, hollowBrush);

        // Create a pen for the border
        HPEN borderPen = CreatePen(PS_DOT, 2, RGB(255, 0, 0)); // 2px red dotted line
        HPEN oldPen = (HPEN)SelectObject(memDC, borderPen);

        // Draw the rectangle outline into back buffer
        Rectangle(memDC, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);

        // Clean up GDI objects
        SelectObject(memDC, oldBrush);
        SelectObject(memDC, oldPen);
        DeleteObject(borderPen);
    }

    // Copy (Blit) the entire back buffer (memDC) to the screen's DC 
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    // Cleanup 
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    EndPaint(hwnd, &ps);
}


// Static WndProc function to handle window messages
LRESULT CALLBACK OverlayWindow::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    // Use s_instance to access the class members
    if (!s_instance) {
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    
    switch (msg) {
    
    case WM_PAINT:
        s_instance->OnPaint();
        return 0;

    case WM_LBUTTONDOWN:
        s_instance->isDragging = true;
        s_instance->ptStart = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        s_instance->ptEnd = s_instance->ptStart;
        SetCapture(hwnd); // Capture mouse input even if it goes outside the window
        //InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    
    //case WM_MOUSEMOVE:
    //    if (self->dragging) {
    //        // Get current mouse position
    //        self->ptEnd = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };

    //        // Get window’s device context
    //        HDC dc = GetDC(hwnd);
    //        SetROP2(dc, R2_NOT); // raster operation XOR mode for rubber-band

    //        // Draw selection rectangle
    //        Rectangle(dc,
    //            self->ptStart.x, self->ptStart.y,
    //            self->ptEnd.x, self->ptEnd.y);

    //        // Clean
    //        ReleaseDC(hwnd, dc);
    //    }
    //    return 0;

    //case WM_MOUSEMOVE:
    //    if (self->dragging) {
    //        HDC dc = GetDC(hwnd);
    //        SetROP2(dc, R2_NOT);

    //        RECT oldRect = {
    //            min(self->ptStart.x, self->ptEnd.x),
    //            min(self->ptStart.y, self->ptEnd.y),
    //            max(self->ptStart.x, self->ptEnd.x),
    //            max(self->ptStart.y, self->ptEnd.y)
    //        };

    //        // Update end point
    //        POINT newEnd = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
    //        RECT newRect = {
    //            min(self->ptStart.x, newEnd.x),
    //            min(self->ptStart.y, newEnd.y),
    //            max(self->ptStart.x, newEnd.x),
    //            max(self->ptStart.y, newEnd.y)
    //        };

    //        // Use a solid brush for XOR border only (fill is ignored)
    //        HBRUSH brush = (HBRUSH)GetStockObject(WHITE_BRUSH);

    //        // Erase old rect
    //        FrameRect(dc, &oldRect, brush);
    //        self->ptEnd = newEnd;
    //        // Draw new rect
    //        FrameRect(dc, &newRect, brush);

    //        ReleaseDC(hwnd, dc);
    //    }
    //    return 0;

    case WM_MOUSEMOVE:
        if (s_instance->isDragging) {
            s_instance->ptEnd = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            // Declare window needs to be redrawn
            InvalidateRect(hwnd, nullptr, FALSE); // FALSE prevents erasing background, reducing flicker
        }
        return 0;

    case WM_LBUTTONUP:
        if (s_instance->isDragging) {
            //self->ptEnd = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
            s_instance->isDragging = false;
            ReleaseCapture(); // Release mouse capture
            s_instance->ExitLoop(); // Safely exit local modal loop
        }
        return 0;
    
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
        if (msg == WM_RBUTTONDOWN || (msg == WM_KEYDOWN && wp == VK_ESCAPE)) {
            s_instance->abortedFlag = true;
            s_instance->ExitLoop();
        }
        return 0;

    // DEBUG
    //case WM_DESTROY:
    //    PostQuitMessage(0); // ISSUE
    //    return 0;

    case WM_DESTROY:
        // Enforce loop exit if the window is destroyed unexpectedly
        s_instance->ExitLoop();
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

