#include "../Sandbox/sandbox.h"
#include <stdio.h>

using namespace ACJudge;

int main(int argc, char *args[])
{
    //int i;
    Result res;
    Sandbox box("SOL");

    for(int i = 0; i < argc - 1; i++)
        args[i] = args[i+1];
    args[argc - 1] = NULL;

    res = box.run(args[0], args, LIMIT_INFINITE, LIMIT_INFINITE, 0, "", "", "");
    printf("----------------------------------------\n%s (%d)\nTime: %d\nSpace: %d\n",
            res.msg, res.ret, res.time, res.space);

    return 0;
}

//set follow-fork-mode child