#include "HotkeyListener.h"

#include <sstream>
#include <algorithm>


// Registers hotkeys defined in config strings in Windows OS
bool HotkeyListener::Register(HWND hwnd, const std::wstring& fullHotkeyStr, const std::wstring& regionHotkeyStr) {
    
    Hotkey hk; // parsed modifier and virtual-key codes

    // Register hotkey Full-Screen Capture
    if (ParseHotkey(fullHotkeyStr, hk)) {
        if (!RegisterHotKey(hwnd, 1, hk.mods, hk.vk)) {
            OutputDebugStringW(L"[HotkeyListener] Failed to register full-screen hotkey\n");
        }
    }

    // Register hotkey Region Capture
    if (ParseHotkey(regionHotkeyStr, hk)) {
        if (!RegisterHotKey(hwnd, 2, hk.mods, hk.vk)) {
            OutputDebugStringW(L"[HotkeyListener] Failed to register region hotkey\n");
        }
    }

    return true;
}


// Parses user-friendly string (e.g., "Win+Shift+Q") into Win32 API-compatible modifier and virtual-key codes
bool HotkeyListener::ParseHotkey(const std::wstring& str, Hotkey& out) {
    std::wistringstream ss(str);
    std::wstring token;

    UINT mods = 0; // modifier flags (e.g., MOD_WIN, MOD_SHIFT)
    UINT vk = 0; // virtual-key code for the primary key (e.g., 'Q')

    // Loop through the string, splitting it by the '+' character
    while (std::getline(ss, token, L'+')) {
        std::transform(token.begin(), token.end(), token.begin(), ::towlower);

        // Check known modifier tokens and accumulate flags
        if (token == L"win") mods |= MOD_WIN;
        else if (token == L"ctrl") mods |= MOD_CONTROL;
        else if (token == L"shift") mods |= MOD_SHIFT;
        else if (token == L"alt") mods |= MOD_ALT;
        else if (token.length() == 1) {
            WCHAR c = towupper(token[0]);
            vk = VkKeyScanW(c) & 0xFF; // '& 0xFF' mask isolates the key code
        }
        else {
            return false; // Unknown token (e.g., "Win+Foo+Q")
        }
    }

    if (vk == 0 || mods == 0) return false;

    // Populate parsed values
    out.mods = mods;
    out.vk = vk;

    return true;
}

