#pragma once

#define LIMIT_INFINITE 0xffffffffu

namespace ACJudge
{
    enum Return
    {
        OK = 0,                 // So far so good
        ERR = -1,               // System internal error
        TLE = -2,               // Time limit exceeded
        MLE = -3,               // Memory limit exceeded 
        RTE = -4,               // Other kinds of runtime error
        CE = -5,                // Compilation error
        WA = -6                 // Wrong Answer
    };

    enum Language
    {
        NONE = -1,              // For SPJ this means default comparer, for user code this means answer submission
        CPP = 0,                // 
        CPP11 = 1,              //
        C = 2,                  // 
        PYTHON = 3,             // 
        ACPL = 4                // 
    };

    enum CodeType
    {
        USER,
        SPJ
    };

    enum TaskType
    {
        OI,
        ACM
    };
}