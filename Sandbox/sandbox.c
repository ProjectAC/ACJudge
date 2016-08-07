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
    struct itimerval time_limit;

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
    Rlimit limit;
    limit.rlim_cur = limit.rlim_max = (rlim_t)(time);
    if(setrlimit(RLIMIT_CPU, limit) == -1)
        return ERR;
    return OK;
    
    return OK;
}

// Set space limit
// With setrlimit
Return set_space_limit(Limit space)
{
    // setrlimit
    Rlimit limit;
    limit.rlim_cur = limit.rlim_max = (rlim_t)(space) + 1000000;
    if(setrlimit(RLIMIT_AS, limit) == -1)
        return ERR;
    return OK;
}

Return set_file(const char )
{


}

// [Interface] redirection
Return redirection(const char *in, const char *out, const char *err)
{
    

}

// [Interface] set_limits
Return set_limits(Limit time, Limit space)
{
    if(time != LIMIT_INFINITE)
        set_time_limit(time);
    if(space != LIMIT_INFINITE)
        set_space_limit(space * 1000);
}

// [Interface] set_rules
Return set_rules()
{
    scmp_filter_ctx ctx;

    ctx = seccomp_init(SCMP_ACT_ERRNO(5));
    
}

// [Interface] run
Return run(const char name[], Limit time, Limit space, bool restricted)
{
    
}
