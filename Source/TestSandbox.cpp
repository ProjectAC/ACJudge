#include "../Sandbox/sandbox.h"
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace ACJudge;

int main(int argc, char *args[])
{
    //int i;
    Result res;
    Sandbox box("WXC");

    for(int i = 0; i < argc - 1; i++)
        args[i] = args[i+1];
    args[argc - 1] = NULL;

    res = box.run(args[0], args, LIMIT_INFINITE, LIMIT_INFINITE, true, "", "tmp.out", "");
    cout<<res.ret<<" "<<res.val<<" "<<res.msg<<endl;
    return 0;
}

//set follow-fork-mode child