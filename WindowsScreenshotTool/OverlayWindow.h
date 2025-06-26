#pragma once

#include <windows.h>

class OverlayWindow
{
public:
    // Displays the overlay with frozenScreen; returns true if selection made (not aborted).
    // On success, outSelection contains normalized RECT; aborted flag indicates cancellation.
    bool Show(HBITMAP frozenScreen, RECT& outSelection, bool& aborted);
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static OverlayWindow* s_instance;

    HWND hwnd = nullptr;
    HBITMAP hFrozen = nullptr;
    POINT ptStart = {}, ptEnd = {};
    bool dragging = false;
    bool abortedFlag = false;
};

