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

#include "sandbox.h"

// Set time limit
// Both with setitimer and setrlimit
// Here time is CPU time, and stime is actual time
Return set_time_limit(Limit time) 
{
    Limit stime = time * 3;
    ITimerVal time_limit;

    time_limit.it_interval.tv_sec = time_limit.it_interval.tv_usec = 0;
    
    // User mode CPU time
    time_limit.it_value.tv_sec = time / 1000;
    time_limit.it_value.tv_usec = time % 1000 * 1000;
    if(setitimer(ITIMER_VIRTUAL, &time_limit, NULL))
        return ERR;
    
    // All CPU time
    time_limit.it_value.tv_sec = stime / 1000;
    time_limit.it_value.tv_usec = stime % 1000 * 1000;
    if(setitimer(ITIMER_REAL, &time_limit, NULL))
        return ERR;

    // setrlimit
    RLimit limit;
    limit.rlim_cur = limit.rlim_max = (rlim_t)(time);
    if(setrlimit(RLIMIT_CPU, &limit) == -1)
        return ERR;
    
    return OK;
}

// Set space limit
// With setrlimit
Return set_space_limit(Limit space)
{
    // setrlimit
    RLimit limit;
    limit.rlim_cur = limit.rlim_max = (rlim_t)(space) + 1000000;
    if(setrlimit(RLIMIT_AS, &limit) == -1)
        return ERR;
    return OK;
}

// Redirect I/O flow <file> to file with name <name>
Return set_file(FILE *fp, const char *name, const char *mode)
{
    FILE *newfp;
    if((newfp = fopen(name, mode)) == NULL)
        return ERR;
    if(dup2(fileno(newfp), fileno(fp)) == -1)
        return ERR;
    return OK;
}

// Starter
// This is what the starter have to do before execve
void start(const char *path, const char *name, char *args[], Limit time, Limit space, bool restricted, const char *fin, const char *fout, const char *ferr)
{
    char s[1000];
    //char const *envp[] = {NULL};
    //char const *envp[] ={"PATH=/bin:/usr/bin", "TERM=console", NULL};

    sprintf(s, "%s/%s", path, name);
    args[0] = s;

    if(set_limits(time, space) == ERR)
        exit(ERR);
    if(redirection(fin, fout, ferr) == ERR)
        exit(ERR);
    if(restricted && (set_rules(path) == ERR || set_gid() == ERR))
        exit(ERR);
    if(execv(s, args))
        exit(ERR);
}

// [Interface] redirection
Return redirection(const char *in, const char *out, const char *err)
{
    if(in  != NULL && set_file(stdin , in , "r") == ERR)
        return ERR;
    if(out != NULL && set_file(stdout, out, "w") == ERR)
        return ERR;
    if(err != NULL && set_file(stderr, err, "w") == ERR)
        return ERR;
    return OK;
}

// [Interface] set_limits
Return set_limits(Limit time, Limit space)
{
    if(time != LIMIT_INFINITE)
        if(set_time_limit(time) == ERR)
            return ERR;
    if(space != LIMIT_INFINITE)
        if(set_space_limit(space * 1000) == ERR)
            return ERR;
    return OK;
}

// [Interface] set_gid (set group to guest)
Return set_gid()
{
    int groups[] = {65534};

    if(setgid(65534) == -1)
        return ERR;
    if(setgroups(1, groups) == -1)
        return ERR;
    if(setuid(65534) == -1)
        return ERR;

    return OK;
}

// [Interface] set_rules
Return set_rules(const char *path)
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
        return ERR;

    // Add whitelist
    for(i = 0; i < whitelist_length; i++)
        if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist[i], 0))
            return ERR;
    
    // Enable execve
    if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, path)))
        return ERR;
    // Allow FD but 0 1 2 only
    if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1, SCMP_A0(SCMP_CMP_LE, 2)))
        return ERR;
    
    //Load
    if(seccomp_load(ctx))
        return ERR;
    
    seccomp_release(ctx);
    return OK;
}

// [Interface] run
Result run(const char *path, const char *name, char *args[], Limit time, Limit space, bool restricted, const char *fin, const char *fout, const char *ferr)
{
    int starter;
    int timer;
    int pid, status, signal, retval;
    Result res;
    RUsage resource_usage;

    // Init res
    res.time = res.space = 0;
    res.ret = OK;

    // Fork for starter
    if((starter = fork()) < 0)
        return res.ret = ERR, res;
    else if (starter == 0)  // Starter subprocess
    {
        start(path, name, args, time, space, restricted, fin, fout, ferr);
        // If this return is accessed, then something wrong must happened
        exit(ERR);
    }
    
    // Else
    // Main process
    // This is the timer in old edition
    // Now abandoned
    /*
    if(timer = fork() < 0)
    {
        //Subprocess have already been started and must be killed
        kill(starter, SIGKILL);
        return res.ret = ERR, res;
    }else if(timer == 0)  //Timer subprocess
    {
        usleep(time * 3000);
        exit(0);
    }
    */

    // Else
    // Main process
    // Deal with the return value

    // Wait for the subprocess to quit 
    if((pid = wait3(&status, 0, &resource_usage)) == -1)
        return res.ret = ERR, res;
    
    // Timer process ended but running one not: Real time TLE
    /*
    if(pid == timer)
        return res.ret = TLE, res;
    */
    
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
            res.ret = TLE;
            sprintf(res.msg, "Real time TLE");
        }else if(signal == SIGVTALRM)  // CPU time TLE
        {
            res.ret = TLE;
            sprintf(res.msg, "CPU time TLE");
        }else if(signal == SIGSEGV)  // Segment fault
            if(space != LIMIT_INFINITE && res.space > space)
            {
                res.ret = MLE;
                sprintf(res.msg, "MLE");
            }else
            {
                res.ret = RTE;
                sprintf(res.msg, "Signaled RTE");
            }
        else
        {
            res.ret = RTE;
            sprintf(res.msg, "Other RTE");
        }
    }else
    {
        if(space != LIMIT_INFINITE && res.space > space)  // MLE
        {
            res.ret = MLE;
            sprintf(res.msg, "MLE");
        }

        retval = WEXITSTATUS(status);
        if(retval == ERR)  // System error
        {
            res.ret = ERR;
            sprintf(res.msg, "System Error");
        }else if(retval)  //RTE
        {
            res.ret = RTE;
            sprintf(res.msg, "Return value is not 0");
        }else
        {
            res.ret = OK;  // All right
            sprintf(res.msg, "OK");
        }
    }

    return res;
}
