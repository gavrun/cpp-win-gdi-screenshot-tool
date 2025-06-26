#pragma once

#include <windows.h>

class OverlayWindow
{
public:
    // Displays the overlay with frozenScreen; returns true if selection made (not aborted).
    // On success, outSelection contains normalized RECT; aborted flag indicates cancellation.
    bool Show(HBITMAP frozenScreen, RECT& outSelection, bool& aborted);
private:
	// Handles WM_PAINT to draw the selection rectangle and frozen screenshot
    void OnPaint();

    void ExitLoop();

    // Static WndProc and instance pointer to bridge the C-style Win32 API with our C++ class
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static OverlayWindow* s_instance;

    // Member variables to store the state of the overlay
    HWND hwnd = nullptr;
    HBITMAP hFrozen = nullptr;
    POINT ptStart = {}, ptEnd = {};
    bool isDragging = false;
    bool abortedFlag = false;
    bool m_isLooping = false;

};

