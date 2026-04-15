#include "pch.h"
#include "Utils.h"

std::string Utils::GetASCIIstring(int c)
{
    std::string txt = "";
    txt = char(c);

    if (c == VK_ESCAPE)
        txt = "Echap";
    else if (c == VK_LSHIFT || c == VK_RSHIFT || c == 16)
        txt = "Shift";
    else if (c == VK_LCONTROL || c == VK_RSHIFT || c == 17)
        txt = "Ctrl";
    else if (c == 18)
        txt = "Alt";
    else if (c == VK_SPACE)
        txt = "Space";

    return txt;
}

std::string Utils::GetFloatDisplay(float val, int decimals)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << val;

    return oss.str();
}
