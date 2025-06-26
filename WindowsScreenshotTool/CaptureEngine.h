#pragma once

#include <windows.h>

// CaptureEngine responsible for grabbing pixel data from virtual desktop
//
// Design:
// - Uses classic Win32 GDI (GetDC, BitBlt, CreateCompatibleBitmap)
// - Supports multiple monitors and full virtual desktop
// - No DPI scaling logic: raw logical pixels at 100% DPI

class CaptureEngine
{
public:
	// Capture the entire virtual screen into an HBITMAP.
	static HBITMAP CaptureFull();

	// Caller is responsible for DeleteObject(bitmap)
};
