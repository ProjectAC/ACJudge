#pragma once

#include "enums.h"

typedef unsigned int Limit;
typedef struct rlimit RLimit;
typedef struct itimerval ITimerVal;
typedef struct rusage RUsage;

struct Result 
{
    Limit time, space;
    Return ret;
    char msg[1000];
};

struct Grade 
{
    Result res;
    
};