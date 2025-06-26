#pragma once

#include <string>
#include <windows.h>

// FileManager handles saving HBITMAP images to disk as PNG files
//
// Design:
// - Uses GDI+ Bitmap.Save() to encode as PNG
// - Static method 
// - Includes helper for timestamped filename generation

class FileManager
{
public:
    // Save a raw HBITMAP as lossless PNG via GDI+
    static bool SavePng(HBITMAP bmp, const std::wstring& filename);
};

// Utility to generate timestamped filenames matching spec: Screenshot_YYYYMMDD_HHMMSS_MMM.png
std::wstring GenerateTimestampedFilename(const std::wstring& folder);

