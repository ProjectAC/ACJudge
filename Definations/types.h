#pragma once

#include "enums.h"

typedef unsigned char bool;
typedef unsigned int Limit;
typedef struct rlimit RLimit;
typedef struct itimerval ITimerVal;
typedef struct rusage RUsage;

typedef struct 
{
    Limit time, space;
    Return ret;
} Result;