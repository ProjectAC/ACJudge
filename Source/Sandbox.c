#include "../Sandbox/sandbox.h"
#include <stdio.h>

int main(int argc, char *args[])
{
    char *arguments[1000];
    int i;
    Result res;

    for(i = 1; i < argc; i++)
        arguments[i - 1] = args[i];
    arguments[argc - 1] = NULL;

    res = run("../Container", args[1], arguments, LIMIT_INFINITE, 1000, 0, NULL, NULL, NULL);
    printf("%d\nTime: %d\nSpace: %d\n%s\n", res.ret, res.time, res.space, res.msg);

    return 0;
}

//set follow-fork-mode child