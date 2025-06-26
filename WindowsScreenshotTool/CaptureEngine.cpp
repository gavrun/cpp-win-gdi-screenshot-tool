#include "CaptureEngine.h"


HBITMAP CaptureEngine::CaptureFull() {

	// Retrieve the bounding rectangle of all monitors
	int sx = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// Create a DC for the primary desktop and a compatible memory DC
	HDC screenDC = GetDC(NULL);

	// Allocate a compatible bitmap of full size
	HDC memDC = CreateCompatibleDC(screenDC);
	HBITMAP bmp = CreateCompatibleBitmap(screenDC, w, h);

	// Select the new bitmap into memory DC
	SelectObject(memDC, bmp);

	// Copy desktop pixels via BitBlt directly from source to destination
	BitBlt(memDC, 0, 0, w, h, screenDC, sx, sy, SRCCOPY);

	// Release DCs and return the HBITMAP containing screen content
	DeleteDC(memDC);
	ReleaseDC(NULL, screenDC);

	// Note: bitmap stays alive until deleted

	return bmp;
}
