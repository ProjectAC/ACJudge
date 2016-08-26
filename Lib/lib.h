#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace ACJudge 
{
    template<class Integer>
    std::wstring i2s(Integer i)
    {
        std::wstringstream ss;
        std::wstring s;
        ss << i;
        ss >> s;
        return s;
    }
}