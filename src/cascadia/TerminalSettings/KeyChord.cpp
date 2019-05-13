﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "KeyChord.h"

static const std::wstring CTRL_KEY{ L"ctrl" };
static const std::wstring SHIFT_KEY{ L"shift" };
static const std::wstring ALT_KEY{ L"alt" };

static const std::vector<std::pair<int32_t, std::wstring>> vkeyNamePairs {
    { VK_BACK       , L"backspace"},
    { VK_TAB        , L"tab"},
    { VK_RETURN     , L"enter" },
    { VK_ESCAPE     , L"esc" },
    { VK_SPACE      , L"space" },
    { VK_PRIOR      , L"pgup" },
    { VK_NEXT       , L"pgdn" },
    { VK_END        , L"end" },
    { VK_HOME       , L"home" },
    { VK_LEFT       , L"left" },
    { VK_UP         , L"up" },
    { VK_RIGHT      , L"right" },
    { VK_DOWN       , L"down" },
    { VK_INSERT     , L"insert" },
    { VK_DELETE     , L"delete" },
    { VK_NUMPAD0    , L"numpad_0" },
    { VK_NUMPAD1    , L"numpad_1" },
    { VK_NUMPAD2    , L"numpad_2" },
    { VK_NUMPAD3    , L"numpad_3" },
    { VK_NUMPAD4    , L"numpad_4" },
    { VK_NUMPAD5    , L"numpad_5" },
    { VK_NUMPAD6    , L"numpad_6" },
    { VK_NUMPAD7    , L"numpad_7" },
    { VK_NUMPAD8    , L"numpad_8" },
    { VK_NUMPAD9    , L"numpad_9" },
    { VK_MULTIPLY   , L"*" },
    { VK_ADD        , L"+" },
    { VK_SUBTRACT   , L"-" },
    { VK_DECIMAL    , L"." },
    { VK_DIVIDE     , L"/" },
    { VK_F1         , L"f1" },
    { VK_F2         , L"f2" },
    { VK_F3         , L"f3" },
    { VK_F4         , L"f4" },
    { VK_F5         , L"f5" },
    { VK_F6         , L"f6" },
    { VK_F7         , L"f7" },
    { VK_F8         , L"f8" },
    { VK_F9         , L"f9" },
    { VK_F10        , L"f10" },
    { VK_F11        , L"f11" },
    { VK_F12        , L"f12" },
    { VK_F13        , L"f13" },
    { VK_F14        , L"f14" },
    { VK_F15        , L"f15" },
    { VK_F16        , L"f16" },
    { VK_F17        , L"f17" },
    { VK_F18        , L"f18" },
    { VK_F19        , L"f19" },
    { VK_F20        , L"f20" },
    { VK_F21        , L"f21" },
    { VK_F22        , L"f22" },
    { VK_F23        , L"f23" },
    { VK_F24        , L"f24" },
    { VK_OEM_PLUS   , L"+" },
    { VK_OEM_COMMA  , L"," },
    { VK_OEM_MINUS  , L"-" },
    { VK_OEM_PERIOD , L"." }
// TODO:
// These all look like they'd be good keybindings, but change based on keyboard
// layout. How do we deal with that?
// #define VK_OEM_NEC_EQUAL  0x92   // '=' key on numpad
// #define VK_OEM_1          0xBA   // ';:' for US
// #define VK_OEM_2          0xBF   // '/?' for US
// #define VK_OEM_3          0xC0   // '`~' for US
// #define VK_OEM_4          0xDB  //  '[{' for US
// #define VK_OEM_5          0xDC  //  '\|' for US
// #define VK_OEM_6          0xDD  //  ']}' for US
// #define VK_OEM_7          0xDE  //  ''"' for US
};

namespace winrt::Microsoft::Terminal::Settings::implementation
{
    KeyChord::KeyChord(bool ctrl, bool alt, bool shift, int32_t vkey) :
        _modifiers{ (ctrl ? Settings::KeyModifiers::Ctrl : Settings::KeyModifiers::None) |
                    (alt ? Settings::KeyModifiers::Alt : Settings::KeyModifiers::None) |
                    (shift ? Settings::KeyModifiers::Shift : Settings::KeyModifiers::None) },
        _vkey{ vkey }
    {
    }

    KeyChord::KeyChord(Settings::KeyModifiers const& modifiers, int32_t vkey) :
        _modifiers{ modifiers },
        _vkey{ vkey }
    {
    }

    Settings::KeyModifiers KeyChord::Modifiers()
    {
        return _modifiers;
    }

    void KeyChord::Modifiers(Settings::KeyModifiers const& value)
    {
        _modifiers = value;
    }

    int32_t KeyChord::Vkey()
    {
        return _vkey;
    }

    void KeyChord::Vkey(int32_t value)
    {
        _vkey = value;
    }

    // Method Description:
    // - Deserializes the given string into a new KeyChord instance. If this
    //   fails to translate the string into a keychord, it will throw a
    //   hresult_invalid_argument exception.
    // - The string should fit the format "[ctrl+][alt+][shift+]<keyName>",
    //   where each modifier is optional, and keyName is either one of the
    //   names listed in the vkeyNamePairs vector above, or is one of 0-9a-zA-Z.
    // Arguments:
    // - hstr: the string to parse into a keychord.
    // Return Value:
    // - a newly constructed KeyChord
    winrt::Microsoft::Terminal::Settings::KeyChord KeyChord::FromString(const hstring& hstr)
    {
        std::wstring wstr{ hstr };

        // Split the string on '+'
        std::wstring temp;
        std::vector<std::wstring> parts;
        std::wstringstream wss(wstr);

        while(std::getline(wss, temp, L'+'))
        {
            parts.push_back(temp);
        }

        // If we have > 4, something's wrong.
        if (parts.size() > 4)
        {
            throw hresult_invalid_argument();
        }

        KeyModifiers modifiers = KeyModifiers::None;
        int32_t vkey = 0;

        // Look for ctrl, shift, alt. Anything else might be a key
        for (const auto& part : parts)
        {
            if (part == CTRL_KEY)
            {
                modifiers |= KeyModifiers::Ctrl;
            }
            else if (part == ALT_KEY)
            {
                modifiers |= KeyModifiers::Alt;
            }
            else if (part == SHIFT_KEY)
            {
                modifiers |= KeyModifiers::Shift;
            }
            else
            {
                bool foundKey = false;
                // For potential keys, look through the pairs of strings and vkeys
                if (part.size() == 1)
                {
                    const wchar_t wch = part[0];
                    // Quick lookup: ranges of vkeys that correlate directly to a key.
                    if (wch >= L'0' && wch <= L'9')
                    {
                        vkey = static_cast<int32_t>(wch);
                        foundKey = true;
                    }
                    else if (wch >= L'a' && wch <= L'z')
                    {
                        // subtract 0x20 to shift to uppercase
                        vkey = static_cast<int32_t>(wch - 0x20);
                        foundKey = true;
                    }
                    else if (wch >= L'A' && wch <= L'Z')
                    {
                        vkey = static_cast<int32_t>(wch);
                        foundKey = true;
                    }
                }

                // If we didn't find the key with a quick lookup, search the
                // table to see if we have a matching name.
                if (!foundKey)
                {
                    for (const auto& pair : vkeyNamePairs)
                    {
                        if (pair.second == part)
                        {
                            vkey = pair.first;
                            foundKey = true;
                            break;
                        }
                    }
                }

                // If we weren't able to find a match, throw an exception.
                if (!foundKey)
                {
                    throw hresult_invalid_argument();
                }
            }
        }

        return winrt::Microsoft::Terminal::Settings::KeyChord{ modifiers, vkey };
    }

    // Method Description:
    // - Serialize this keychord into a string represenation.
    // - The string will fit the format "[ctrl+][alt+][shift+]<keyName>",
    //   where each modifier is optional, and keyName is either one of the
    //   names listed in the vkeyNamePairs vector above, or is one of 0-9a-z.
    // Arguments:
    // - <none>
    // Return Value:
    // - a string which is an equivalent serialization of this object.
    hstring KeyChord::ToString()
    {
        bool serializedSuccessfully = false;

        std::wstring buffer{ L"" };

        // Add modifiers
        if (WI_IsFlagSet(_modifiers, Settings::KeyModifiers::Ctrl))
        {
            buffer += CTRL_KEY;
            buffer += L"+";
        }
        if (WI_IsFlagSet(_modifiers, Settings::KeyModifiers::Alt))
        {
            buffer += ALT_KEY;
            buffer += L"+";
        }
        if (WI_IsFlagSet(_modifiers, Settings::KeyModifiers::Shift))
        {
            buffer += SHIFT_KEY;
            buffer += L"+";
        }

        // Quick lookup: ranges of vkeys that correlate directly to a key.
        if (_vkey >= L'0' && _vkey <= L'9')
        {
            buffer += std::wstring(1, static_cast<wchar_t>(_vkey));
            serializedSuccessfully = true;
        }
        else if (_vkey >= L'A' && _vkey <= L'Z')
        {
            // add 0x20 to shift to lowercase
            buffer += std::wstring(1, static_cast<wchar_t>(_vkey + 0x20));
            serializedSuccessfully = true;
        }
        else
        {
            for (const auto& pair : vkeyNamePairs)
            {
                if (pair.first == _vkey)
                {
                    buffer += pair.second;
                    serializedSuccessfully = true;
                    break;
                }
            }
        }

        if (!serializedSuccessfully)
        {
            buffer = L"";
        }

        return winrt::hstring{ buffer };
    }

}
