#include "CaptureEngine.h"


// Capture a full bitmap
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


// Crop a region from a full bitmap
HBITMAP CaptureEngine::CaptureRegion(const RECT& rect, HBITMAP sourceBmp) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return nullptr;
    }

    HDC screenDC = GetDC(nullptr);
    HDC srcDC = CreateCompatibleDC(screenDC);
    HDC destDC = CreateCompatibleDC(screenDC);

    HBITMAP targetBmp = CreateCompatibleBitmap(screenDC, width, height);

    HBITMAP oldSrc = (HBITMAP)SelectObject(srcDC, sourceBmp);
    HBITMAP oldDest = (HBITMAP)SelectObject(destDC, targetBmp);

    // Copy the selected area
    BitBlt(destDC, 0, 0, width, height,
        srcDC, rect.left, rect.top, SRCCOPY);

    // Cleanup
    SelectObject(srcDC, oldSrc);
    SelectObject(destDC, oldDest);
    DeleteDC(srcDC);
    DeleteDC(destDC);
    ReleaseDC(nullptr, screenDC);

    return targetBmp;
}
