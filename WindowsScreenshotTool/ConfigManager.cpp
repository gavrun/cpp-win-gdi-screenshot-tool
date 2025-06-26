#include "ConfigManager.h"

#include <shlobj.h> // For ExpandEnvironmentStrings
#include <string>


// Reads config.ini or falls back to defaults
bool ConfigManager::Load() {
    // App and INI file paths
    
    //wchar_t path[MAX_PATH];
    wchar_t exePathRaw[MAX_PATH];

    //GetModuleFileNameW(NULL, path, MAX_PATH);
    GetModuleFileNameW(NULL, exePathRaw, MAX_PATH);

    //std::wstring iniPath = std::wstring(path);
    //size_t pos = iniPath.find_last_of(L"\\/");
    std::wstring exePath = exePathRaw;
    size_t pos = exePath.find_last_of(L"\\/");

    // Extract directory where App .exe is located
    std::wstring appDirectory = (pos != std::wstring::npos) ? exePath.substr(0, pos) : L".";

    // Construct the full path to the config.ini
    //iniPath = iniPath.substr(0, pos + 1) + L"config.ini";
    std::wstring iniPath = appDirectory + L"\\config.ini";
    
    // Default hotkeys and folder in the same directory as the .exe
    bool needToWriteDefaults = false;
    std::wstring defaultFull = L"Win+Shift+Q";
    std::wstring defaultRegion = L"Win+Shift+A";
    std::wstring defaultOutputFolder = appDirectory + L"\\Screenshots";

    // Load Configuration
    wchar_t buffer[512] = {};

    // Use defaults if the key is missing
    if (GetPrivateProfileStringW(L"General", L"FullHotkey", nullptr, buffer, 512, iniPath.c_str()) == 0) {
        fullHotkey = defaultFull;
        needToWriteDefaults = true;
    }
    else {
        fullHotkey = buffer;
    }

    if (GetPrivateProfileStringW(L"General", L"RegionHotkey", nullptr, buffer, 512, iniPath.c_str()) == 0) {
        regionHotkey = defaultRegion;
        needToWriteDefaults = true;
    }
    else {
        regionHotkey = buffer;
    }

    //GetPrivateProfileStringW(L"General", L"OutputFolder", L"%USERPROFILE%\\Pictures\\Screenshots", buffer, 512, iniPath.c_str());
    if (GetPrivateProfileStringW(L"General", L"OutputFolder", nullptr, buffer, 512, iniPath.c_str()) == 0) {
        outputFolder = defaultOutputFolder;
        needToWriteDefaults = true;
    }
    else {
        outputFolder = ExpandEnv(buffer);
    }

    // Write defaults if file was missing or any key was invalid
    if (needToWriteDefaults) {
        WritePrivateProfileStringW(L"General", L"FullHotkey", defaultFull.c_str(), iniPath.c_str());
        WritePrivateProfileStringW(L"General", L"RegionHotkey", defaultRegion.c_str(), iniPath.c_str());
        WritePrivateProfileStringW(L"General", L"OutputFolder", defaultOutputFolder.c_str(), iniPath.c_str());
    }

    // Try to create output directory
    CreateDirectoryW(outputFolder.c_str(), NULL);

    return true;
}


// Expand any %VAR% tokens over the Windows API
std::wstring ConfigManager::ExpandEnv(const std::wstring& input) {
    //wchar_t out[512];
    wchar_t out[MAX_PATH]; // MAX_PATH buffer for a file path
    ExpandEnvironmentStringsW(input.c_str(), out, MAX_PATH);

    return out;
}

