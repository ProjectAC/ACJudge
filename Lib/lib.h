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

	template<class Integer>
	std::string i2ss(Integer i)
	{
		std::stringstream ss;
		std::string s;
		ss << i;
		ss >> s;
		return s;
	}
}