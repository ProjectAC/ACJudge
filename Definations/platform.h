#pragma once

#include <string>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#define WINDOWS
#endif
#ifdef __unix__
#define _NIX
#endif 

#if defined WINDOWS

#define Tsscanf swscanf
#define _T(x) L##x
#define Tcout wcout
#define Tstrcat wcscat
typedef wchar_t Tchar;
typedef std::wstring Tstring;
typedef std::wifstream Tifstream;
typedef std::wofstream Tofstream;
typedef std::wstringstream Tstringstream;

#elif defined _NIX

#define Tsscanf sscanf
#define _T(x) x
#define Tcout cout
#define Tstrcat strcat
typedef char Tchar;
typedef std::string Tstring;
typedef std::ifstream Tifstream;
typedef std::ofstream Tofstream;
typedef std::stringstream Tstringstream;

#endif 
