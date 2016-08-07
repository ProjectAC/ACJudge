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
    time_limit.it_value.tv_sec = time / 1000;
    time_limit.it_value.tv_usec = time % 1000 * 1000;
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

// Redirect i/o flow <file> to file with name <name>
Return set_file(FILE *fp, const char *name, const char *mode)
{
    FILE *newfp;
    if((newfp = fopen(name, mode)) == NULL)
        return ERR;
    if(dup2(fileno(newfp), fileno(fp)) == -1)
        return ERR;
    return OK;
}

// [Interface] redirection
Return redirection(const char *in, const char *out, const char *err)
{
    if(set_file(stdin , in , "r") == ERR)
        return ERR;
    if(set_file(stdout, out, "w") == ERR)
        return ERR;
    if(set_file(stderr, err, "w") == ERR)
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

    if(!(ctx = seccomp_init(SCMP_ACT_KILL)))
        return ERR;

    // Add whitelist
    for(i = 0; i < whitelist_length; i++)
        if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist[i], 0))
            return ERR;
    
    // Enable execve
    if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_EQ, path)))
        return ERR;
    // Allow fd 0 1 2
    if(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 1, SCMP_A0(SCMP_CMP_LE, 2)))
        return ERR;
    
    //Load
    if(seccomp_load(ctx))
        return ERR;
    
    seccomp_release(ctx);
    return OK;
}

// [Interface] run
Return run(const char *path, const char *name, Limit time, Limit space, bool restricted)
{
    
}
