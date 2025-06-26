#pragma once

#include <string>
#include <windows.h>

// ConfigManager handles reading and expanding configuration from an INI file
//
// Design:
// - Parses [General] section for fullHotkey, regionHotkey, outputFolder
// - Uses ExpandEnvironmentStrings to support environment variables
// - Reads defaults if INI missing

class ConfigManager 
{
public:
    std::wstring fullHotkey;
    std::wstring regionHotkey;
    std::wstring outputFolder; // Path where screenshots are saved

    // Loads settings from config.ini TRUE (failures are silent)
    bool Load();

private:
    // Expands %ENV_VAR% in strings
    std::wstring ExpandEnv(const std::wstring& input);
};
