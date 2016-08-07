#pragma once

#define LIMIT_INFINITE -1

enum __return
{
    OK = 0,
    ERR = -1,
    TLE = -2,
    MLE = -3,
    RTE = -4
};
typedef enum __return Return;