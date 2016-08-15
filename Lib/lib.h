#pragma once

#include <sstream>
#include <string>

namespace ACJudge 
{
    std::string i2s(int i)
    {
        std::stringstream ss;
        std::string s;
        ss << i;
        ss >> s;
        return s;
    }
}