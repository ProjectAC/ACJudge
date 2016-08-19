#pragma once

#include <vector>
#include "enums.h"

namespace ACJudge
{
    typedef unsigned long long ID;
    typedef unsigned int Limit;
    typedef unsigned short Score;
    typedef unsigned short Number;
    typedef struct rlimit RLimit;
    typedef struct itimerval ITimerVal;
    typedef struct rusage RUsage;

    struct Result 
    {
        Limit time, space;
        Return ret;
        int val;
        std::string msg;
    };

    struct Grade 
    {
        Result res;
        Score score;
    };
    typedef std::vector<Grade> Grades;

    struct Data
    {
        ID did;
        Limit time, space;
        Score score;
    };

    struct Task
    {
        ID tid;
        TaskType type;
        std::vector<Data> data;
        Language language;
    };

    struct Submission
    {
        ID sid;
        Language language;
    };
};