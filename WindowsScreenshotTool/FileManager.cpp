#include "FileManager.h"

#include <gdiplus.h>

#include <chrono>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "gdiplus.lib") // Link against GDI+ library


// Retrieve the CLSID for the specified encoder MIME type ("image/png")
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }
    free(pImageCodecInfo);
    return -1;  // Failure
}


// Convert HBITMAP into GDI+ Bitmap and save it using the PNG encoder
bool FileManager::SavePng(HBITMAP bmp, const std::wstring& filename) {
	Gdiplus::Bitmap image(bmp, nullptr);
	CLSID clsid;
	if (GetEncoderClsid(L"image/png", &clsid) < 0) return false;
	return image.Save(filename.c_str(), &clsid, nullptr) == Gdiplus::Ok;
}


// Generate timestamped filename in the specified folder
std::wstring GenerateTimestampedFilename(const std::wstring& folder) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t filename[256];
    swprintf_s(filename, 256, L"%s\\Screenshot_%04d%02d%02d_%02d%02d%02d_%03d.png",
        folder.c_str(),
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    return filename;
}

