#include "../Sandbox/sandbox.h"
#include <stdio.h>

int main(int argc, char *args[])
{
    //int i;
    Result res;

    res = run("../Container", args[1], args, 1000, 1000, 1, NULL, "out", NULL);
    printf("%d\nTime: %d\nSpace: %d\n%s\n", res.ret, res.time, res.space, res.msg);

    return 0;
}

//set follow-fork-mode child