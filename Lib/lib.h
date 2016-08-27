#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include "../Definations/platform.h"

namespace ACJudge 
{
    template<class Integer>
    Tstring i2s(Integer i)
    {
        Tstringstream ss;
        Tstring s;
        ss << i;
        ss >> s;
        return s;
    }
}