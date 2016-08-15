#pragma once

#define LIMIT_INFINITE -1u 

enum Return
{
    OK = 0,                 // So far so good
    ERR = -1,               // System internal error
    TLE = -2,               // Time limit exceeded
    MLE = -3,               // Memory limit exceeded 
    RTE = -4                // Other kinds of runtime error
};

enum class Language
{
    CPP,                    // 
    CPP11,                  //
    C,                      // 
    PYTHON,                 // 
    ACPL                    // 
};

enum class CodeType
{
    USER,
    SPJ
};