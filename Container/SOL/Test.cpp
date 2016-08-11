#include <iostream>
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

using namespace std;

int main(int argc, char *argv[])
{
    cout<<"Sandbox is finished!"<<endl;
    
    return 0;
}
