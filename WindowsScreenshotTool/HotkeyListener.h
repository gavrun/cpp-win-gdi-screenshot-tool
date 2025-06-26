#pragma once

#include <windows.h>
#include <string>

// HotkeyListener registers global hotkeys based on strings like "Win+Shift+Q" via Win32 RegisterHotKey
//


class HotkeyListener
{
public:
    bool Register(HWND hwnd, const std::wstring& fullHotkeyStr, const std::wstring& regionHotkeyStr);

private:
    struct Hotkey {
        UINT mods = 0;
        UINT vk = 0;
    };

    bool ParseHotkey(const std::wstring& str, Hotkey& out);
};

