#include "ConfigManager.h"

#include <shlobj.h> // For ExpandEnvironmentStrings

// Reads config.ini or falls back to defaults
bool ConfigManager::Load() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring iniPath = std::wstring(path);
    size_t pos = iniPath.find_last_of(L"\\/");
    iniPath = iniPath.substr(0, pos + 1) + L"config.ini";

    wchar_t buffer[512] = {};

    // Use defaults if the key is missing
    GetPrivateProfileStringW(L"General", L"FullHotkey", L"Win+Shift+Q", buffer, 512, iniPath.c_str());
    fullHotkey = buffer;

    GetPrivateProfileStringW(L"General", L"RegionHotkey", L"Win+Shift+A", buffer, 512, iniPath.c_str());
    regionHotkey = buffer;

    GetPrivateProfileStringW(L"General", L"OutputFolder", L"%USERPROFILE%\\Pictures\\Screenshots", buffer, 512, iniPath.c_str());
    outputFolder = ExpandEnv(buffer);

    // Try to create output directory
    CreateDirectoryW(outputFolder.c_str(), NULL);

    return true;
}


// Expand any %VAR% tokens over the Windows API
std::wstring ConfigManager::ExpandEnv(const std::wstring& input) {
    wchar_t out[512];
    ExpandEnvironmentStringsW(input.c_str(), out, 512);
    return out;
}

