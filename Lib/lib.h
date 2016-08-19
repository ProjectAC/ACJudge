#pragma once

#include <sstream>
#include <string>

namespace ACJudge 
{
    template<class Integer>
    std::string i2s(Integer i)
    {
        std::stringstream ss;
        std::string s;
        ss << i;
        ss >> s;
        return s;
    }
}