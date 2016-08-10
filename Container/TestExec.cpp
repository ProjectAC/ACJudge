#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <seccomp.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    char *args[1] = {NULL};
    
    if(fork() == 0)
    {
        execv("./Test", args);
    }else
    {
        int sta;
        wait(&sta);
    }
    
    return 0;
}
