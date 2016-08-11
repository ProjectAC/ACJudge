#include "../Sandbox/sandbox.h"
#include <stdio.h>

using namespace ACJudge;

int main(int argc, char *args[])
{
    //int i;
    Result res;
    Sandbox box("SOL");

    res = box.run(args[1], args, LIMIT_INFINITE, LIMIT_INFINITE, 0, "", "", "");
    printf("\
----------------------------------------\n\
%s (%d)\nTime: %d\nSpace: %d\n",
            res.msg, res.ret, res.time, res.space);

    return 0;
}

//set follow-fork-mode child