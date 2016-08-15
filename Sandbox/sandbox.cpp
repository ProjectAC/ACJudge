#include <cstdio>
#include <cstring>
#include <cstdlib>
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
#include <grp.h>
#include <pwd.h>

#include "sandbox.h"

using namespace ACJudge;
using namespace std;

Return Sandbox::set_time_limit(Limit time)
{
    Limit stime = time * 3;
    ITimerVal time_limit;

    time_limit.it_interval.tv_sec = time_limit.it_interval.tv_usec = 0;
    
    // User mode CPU time
    time_limit.it_value.tv_sec = time / 1000;
    time_limit.it_value.tv_usec = time % 1000 * 1000;
    if(setitimer(ITIMER_VIRTUAL, &time_limit, NULL))
        return Return::ERR;
    
    // All CPU time
    time_limit.it_value.tv_sec = stime / 1000;
    time_limit.it_value.tv_usec = stime % 1000 * 1000;
    if(setitimer(ITIMER_REAL, &time_limit, NULL))
        return Return::ERR;

    // setrlimit
    RLimit limit;
    limit.rlim_cur = limit.rlim_max = (rlim_t)(time);
    if(setrlimit(RLIMIT_CPU, &limit) == -1)
        return Return::ERR;
    
    return Return::OK;
}

Return Sandbox::set_space_limit(Limit space)
{
    // setrlimit
    RLimit limit;
    limit.rlim_cur = limit.rlim_max = (rlim_t)(space + 20000000);
    if(setrlimit(RLIMIT_AS, &limit) == -1)
        return Return::ERR;
    return Return::OK;
}

Return Sandbox::set_file(FILE *fp, string file, string mode)
{
    FILE *newfp;
    file = "../Container/" + file; 
    if((newfp = fopen(file.c_str(), mode.c_str())) == NULL)
        return Return::ERR;
    if(dup2(fileno(newfp), fileno(fp)) == -1)
        return Return::ERR;
    return Return::OK;
}

void Sandbox::start(string file, char *args[], Limit time, Limit space, bool restricted, string fin, string fout, string ferr)
{
    string s = (file[0] == '.' ? "../Container/" + file : file);
    char *arguments[1000];
    int argc;
    //char const *envp[] ={"PATH=/bin:/usr/bin", "TERM=console", NULL};
    
    arguments[0] = (char *)s.c_str();
    for(argc = 1; args[argc + 1]; argc++)
        arguments[argc] = args[argc + 1];
    arguments[argc] = NULL;

    if(set_limits(time, space) == Return::ERR)
        exit(Return::ERR);
    if(redirection(fin, fout, ferr) == Return::ERR)
        exit(Return::ERR);
    if(restricted && (set_rules(s) == Return::ERR) )  // Removed: setgid
        exit(Return::ERR);
    if(execvp(s.c_str(), arguments))
    {
        perror("Error");
        exit(Return::ERR);
    }
}

Return Sandbox::redirection(string in, string out, string err)
{
    if(in  != "" && set_file(stdin , in , "r") == Return::ERR)
        return Return::ERR;
    if(out != "" && set_file(stdout, out, "w") == Return::ERR)
        return Return::ERR;
    if(err != "" && set_file(stderr, err, "w") == Return::ERR)
        return Return::ERR;
    return Return::OK;
}

Return Sandbox::set_limits(Limit time, Limit space)
{
    if(time != LIMIT_INFINITE)
        if(set_time_limit(time) == Return::ERR)
            return Return::ERR;
    if(space != LIMIT_INFINITE)
        if(set_space_limit(space * 1024) == Return::ERR)
            return Return::ERR;
    return Return::OK;
}

Return Sandbox::set_gid()
{
    passwd *pwd = getpwnam("nobody");
    const unsigned int groups[] = {pwd->pw_gid};

    if(setgid(pwd->pw_gid) == -1)
        return Return::ERR;
    if(setgroups(1, groups) == -1)
        return Return::ERR;
    if(setuid(pwd->pw_uid) == -1)
        return Return::ERR;

    return Return::OK;
}

Return Sandbox::set_rules(string file)
{
    // White list
    int whitelist[] = {SCMP_SYS(read), SCMP_SYS(fstat),
                       SCMP_SYS(mmap), SCMP_SYS(mprotect), 
                       SCMP_SYS(munmap), SCMP_SYS(open), 
                       SCMP_SYS(arch_prctl), SCMP_SYS(brk), 
                       SCMP_SYS(access), SCMP_SYS(exit_group), 
                       SCMP_SYS(close)};
    int whitelist_length = sizeof(whitelist) / sizeof(int);
    int i;
    scmp_filter_ctx ctx;

    // Init
    if(!(ctx = seccomp_init(SCMP_ACT_KILL)))
        return Return::ERR;

    // Add whitelist
    for(i = 0; i < whitelist_length; i++)
        if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist[i], 0))
            return Return::ERR;
    
    // Enable execve
    if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t)file.c_str())))
        return Return::ERR;
    // Allow FD but 0 1 2 only
    if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1, SCMP_A0(SCMP_CMP_LE, 2)))
        return Return::ERR;
    
    //Load
    if(seccomp_load(ctx))
        return Return::ERR;
    
    seccomp_release(ctx);
    return Return::OK;
}

Result Sandbox::run(string file, char *args[], Limit time, Limit space, bool restricted, string fin, string fout, string ferr)
{
    int starter;
    int pid, status, signal, retval;
    Result res;
    RUsage resource_usage;

    // Init res
    res.time = res.space = 0;
    res.ret = Return::OK;

    // Fork for starter
    if((starter = fork()) < 0)
        return res.ret = Return::ERR, res;
    else if (starter == 0)  // Starter subprocess
    {
        start(file, args, time, space, restricted, fin, fout, ferr);
        // If this return is accessed, then something wrong must happened
        exit(Return::ERR);
    }
    
    // Else
    // Main process
    // Deal with the return value

    // Wait for the subprocess to quit 
    if((pid = wait3(&status, 0, &resource_usage)) == -1)
        return res.ret = Return::ERR, res;
    
    // Get CPU time
    res.time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                      resource_usage.ru_utime.tv_usec / 1000 +
                      resource_usage.ru_stime.tv_sec * 1000 +
                      resource_usage.ru_stime.tv_usec / 1000);
    if(res.time == 0)
        res.time = 1;
    
    // Get space
    res.space = resource_usage.ru_maxrss;

    // Get signal
    if(WIFSIGNALED(status))
    {
        signal = WTERMSIG(status);
        if(signal == SIGALRM)  // Real time TLE
        {
            res.ret = Return::TLE;
            sprintf(res.msg, "Real time TLE");
        }else if(signal == SIGVTALRM)  // CPU time TLE
        {
            res.ret = Return::TLE;
            sprintf(res.msg, "CPU time TLE");
        }else if(signal == SIGSEGV)  // Segment fault
            if(space != LIMIT_INFINITE && res.space > space)  // MLE
            {
                res.ret = Return::MLE;
                sprintf(res.msg, "MLE");
            }else  // Signaled RTE
            {
                res.ret = RTE;
                sprintf(res.msg, "Signaled RTE");
            }
        else
        {
            res.ret = RTE;
            sprintf(res.msg, "Illegal syscall detected RTE");
        }
    }else
    {
        if(space != LIMIT_INFINITE && res.space > space)  // Return::MLE
        {
            res.ret = Return::MLE;
            sprintf(res.msg, "MLE");
        }

        retval = WEXITSTATUS(status);
        if(retval == Return::ERR || retval == 256 + Return::ERR)  // System Return::ERRor
        {
            res.ret = Return::ERR;
            sprintf(res.msg, "System Error");
        }else if(retval)  //RTE
        {
            res.ret = RTE;
            sprintf(res.msg, "RTE, Return value is %d", retval);
        }else
        {
            res.ret = Return::OK;  // All right
            sprintf(res.msg, "OK");
        }
    }

    return res;
}

Result Sandbox::run(string file, string args[], Limit time, Limit space, bool restricted, string fin, string fout, string ferr)
{
    char arguments[1000][1000];
    
    int j = 0;
    for(auto &i : args)
        arguments[j] = (char *)args[i].c_str(), j++;
    arguments[j] = NULL;

    return run(file, arguments, time, space, restricted, fin, fout, ferr);
}