#include "../Definations/platform.h"
#include <iostream>
#include <fstream>

#if defined WINDOWS

#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#include "../Lib/lib.h"
#include "sandbox.h"

using namespace std;
using namespace ACJudge;

JOBOBJECT_EXTENDED_LIMIT_INFORMATION Sandbox::set_limits(Limit time, Limit space, bool restricted)
{
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION ex_lim;

	// Initialize
	ZeroMemory(&ex_lim, sizeof(ex_lim));

	// Memory limit
	if (space != LIMIT_INFINITE)
	{
		ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_MEMORY;
		ex_lim.JobMemoryLimit = SIZE_T((space + 1000) * 1000);
	}

	// Time limit
	if (time != LIMIT_INFINITE)
	{
		ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_TIME;
		ex_lim.BasicLimitInformation.PerJobUserTimeLimit.QuadPart = time * 10000;
	}

	if (restricted)
	{
		// Set maximum porcess number
		ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
		ex_lim.BasicLimitInformation.ActiveProcessLimit = 1;

		// Other limits
		ex_lim.BasicLimitInformation.LimitFlags |=
			JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION |
			JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE |
			JOB_OBJECT_LIMIT_BREAKAWAY_OK;
	}

	return ex_lim;
}

JOBOBJECT_BASIC_UI_RESTRICTIONS Sandbox::set_rules_UI(bool restricted)
{
	JOBOBJECT_BASIC_UI_RESTRICTIONS bs_ui;
	bs_ui.UIRestrictionsClass = 0;
	if (restricted)
	{
		bs_ui.UIRestrictionsClass =
			JOB_OBJECT_UILIMIT_EXITWINDOWS |
			JOB_OBJECT_UILIMIT_READCLIPBOARD |
			JOB_OBJECT_UILIMIT_WRITECLIPBOARD |
			JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |
			JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
			JOB_OBJECT_UILIMIT_GLOBALATOMS |
			JOB_OBJECT_UILIMIT_DESKTOP |
			JOB_OBJECT_UILIMIT_HANDLES;
	}
	return bs_ui;
}

STARTUPINFO Sandbox::redirection(Tstring in, Tstring out, Tstring err)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	// Create StartUpInfo
	STARTUPINFO s = { sizeof(s) };
	ZeroMemory(&s, sizeof(s));
	s.cb = sizeof(STARTUPINFO);
	s.dwFlags = STARTF_USESTDHANDLES;

	// Create file
	if (in != _T(""))
	{
		HANDLE fin = CreateFile((get_path() + in).c_str(),
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			&sa,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
		s.hStdInput = fin;
	}
	if (out != _T(""))
	{
		HANDLE fout = CreateFile((get_path() + out).c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			&sa,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0);
		s.hStdOutput = fout;
	}
	if (err != _T(""))
	{
		HANDLE ferr = CreateFile((get_path() + err).c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			&sa,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0);
		s.hStdError = ferr;
	}

	return s;
}

Result Sandbox::run(Tstring file, Tchar *args[], Limit time, Limit space, bool restricted, Tstring fin, Tstring fout, Tstring ferr)
{
	// Initialize
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION ex_lim = set_limits(time, space, restricted);
	JOBOBJECT_BASIC_UI_RESTRICTIONS bs_ui = set_rules_UI(restricted);
	STARTUPINFO s = redirection(fin, fout, ferr);
	_PROCESS_INFORMATION p;

	// Init res
	Result res;
	res.time = res.space = 0;
	res.ret = Return::ERR;
	res.val = -1;
	res.msg = _T("Unknown system error.");

	// Create JOB object
	HANDLE job = CreateJobObject(NULL, NULL);
	SetInformationJobObject(job, JobObjectExtendedLimitInformation, &ex_lim, sizeof(ex_lim));
	SetInformationJobObject(job, JobObjectBasicUIRestrictions, &bs_ui, sizeof(bs_ui));
	
	// Operate arguments
	Tchar *str = new Tchar[100000];
	memset(str, 0, sizeof(str));
	if (file[0] == _T('.'))
		Tstrcat(str, get_path().c_str());
	Tstrcat(str, args[0]);
#if defined WINDOWS
	Tstrcat(str, _T(".exe"));
#endif
	for (int i = 1; args[i]; i++)
		Tstrcat(str, _T(" ")), Tstrcat(str, args[i]);

	// Create process
	DWORD ret =
		CreateProcess(NULL, str,
			NULL,
			NULL,
			TRUE,
			CREATE_SUSPENDED,
			NULL,
			NULL,
			&s,
			&p);
	if (!ret)
	{
		res.ret = Return::ERR;
		res.msg = _T("Error while creating subprocess, errcode = ") + i2s(GetLastError());
		return res;
	}

	// Start process
	AssignProcessToJobObject(job, p.hProcess);
	ResumeThread(p.hThread);

	// Start timing
	HANDLE handles[2];
	handles[0] = p.hProcess;
	handles[1] = job;
	ret = WaitForMultipleObjects(2, handles, FALSE, time * 3);

	FILETIME _, __, ___, user;
	SYSTEMTIME suser;
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessTimes(p.hProcess, &_, &__, &___, &user);
	GetProcessMemoryInfo(p.hProcess, &pmc, sizeof(pmc));
	FileTimeToSystemTime(&user, &suser);

	res.time = suser.wMilliseconds + suser.wSecond * 1000 + suser.wMinute * 60000;
	if (res.time == 0) res.time = 1;
	res.space = (Limit)(pmc.PeakWorkingSetSize / 1000);

	switch (ret)
	{
	case WAIT_OBJECT_0:
		// Process exited normally
		GetExitCodeProcess(p.hProcess, (LPDWORD)&res.val);
		if (res.val == 1816)  // CPU time TLE
		{
			res.ret = Return::TLE;
			res.msg = _T("CPU time TLE.");
		}
		else if (res.val == 3221225495u)  // Memory limit exceed
		{
			res.ret = Return::MLE;
			res.msg = _T("MLE.");
		}
		else if (res.val)  // Return value not zero
		{
			Tchar *file = new Tchar[1000000];
			Tifstream fin(get_path() + _T("errlog"));
			fin.getline(file, 1000000, EOF);

			res.ret = Return::RTE;
			res.msg = _T("Return code is ") + i2s(res.val) + _T(". Errlog:\n") + _T("");
			delete file;
		}
		else  // It seems all right
		{
			res.ret = Return::OK;
			res.msg = _T("Process exited normally.");
		}
		break;

	case WAIT_TIMEOUT:  // Real time TLE
		res.ret = Return::TLE;
		res.msg = _T("Real time TLE.");
		break;

	case WAIT_FAILED:  // System error
		res.ret = Return::ERR;
		res.msg = _T("Error while waiting.");
		break;
	}

	return res;
}

#elif defined _NIX

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
#include "../Lib/lib.h"
#include "sandbox.h"

using namespace std;
using namespace ACJudge;

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

Return Sandbox::set_file(FILE *fp, Tstring file, Tstring mode)
{
    FILE *newfp;
    file = get_path() + file; 
    if((newfp = fopen(file.c_str(), mode.c_str())) == NULL)
        return Return::ERR;
    if(dup2(fileno(newfp), fileno(fp)) == -1)
        return Return::ERR;
    return Return::OK;
}

void Sandbox::start(Tstring file, Tchar *args[], Limit time, Limit space, bool restricted, Tstring fin, Tstring fout, Tstring ferr)
{
    Tstring s = (file[0] == '.' ? get_path() + file : file);
    Tchar *arguments[1000];
    int argc;
    //Tchar const *envp[] ={"PATH=/bin:/usr/bin"), _T("TERM=console"), NULL};
    
    arguments[0] = (Tchar *)s.c_str();
    for(argc = 1; args[argc]; argc++)
        arguments[argc] = args[argc];
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

Return Sandbox::redirection(Tstring in, Tstring out, Tstring err)
{
    if(in  != _T("") && set_file(stdin , in , _T("r")) == Return::ERR)
        return Return::ERR;
    if(out != _T("") && set_file(stdout, out, _T("w")) == Return::ERR)
        return Return::ERR;
    if(err != _T("") && set_file(stderr, err, _T("w")) == Return::ERR)
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

Return Sandbox::set_rules(Tstring file)
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

Result Sandbox::run(Tstring file, Tchar *args[], Limit time, Limit space, bool restricted, Tstring fin, Tstring fout, Tstring ferr)
{
    int starter;
    int pid, status, signal, retval;
    Result res;
    RUsage resource_usage;

    // Init res
    res.time = res.space = 0;
    res.ret = Return::OK;
    res.val = -1;

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
            res.msg = _T("Real time TLE.");
        }else if(signal == SIGVTALRM)  // CPU time TLE
        {
            res.ret = Return::TLE;
            res.msg = _T("CPU time TLE.");
        }else if(signal == SIGSEGV)  // Segment fault
            if(space != LIMIT_INFINITE && res.space > space)  // MLE
            {
                res.ret = Return::MLE;
                res.msg = _T("MLE.");
            }else  // Signaled RTE
            {
                res.ret = Return::RTE;
                res.msg = _T("Signaled RTE.\nStack overflow, NULL pointer or something like that.");
            }
        else if(signal == SIGFPE)
        {
            res.ret = Return::RTE;
            res.msg = _T("Signaled RTE.\nFloating point error.");
        } 
        else if(signal == SIGKILL)
        {
            res.ret = Return::MLE;
            res.msg = _T("Process killed (Memory Limit Exceed).");
        }
        else
        {
            res.ret = Return::RTE;
            res.msg = _T("Syscall failed.\nThis might happen when memory limit exceeded.\nBut it also might caused by an illegal syscall, such as fork.");
        }
    }else
    {
        if(space != LIMIT_INFINITE && res.space > space)  // Return::MLE
        {
            res.ret = Return::MLE;
            res.msg = _T("MLE.");
        }

        retval = WEXITSTATUS(status);
        if(retval == Return::ERR || retval == 256 + Return::ERR)  // System Return::ERRor
        {
			Tchar *file = new Tchar[1000000];
            Tifstream fin((char *)(get_path() + _T("errlog")).c_str());
            fin.getline(file, 1000000, EOF);
            
            res.ret = Return::ERR;
            res.val = retval;
            res.msg = _T("System Error #") + i2s(retval) + _T(".\nErrlog:\n") + file;
			delete file;
        }else if(retval)  //RTE
        {
			Tchar *file = new Tchar[1000000];
            Tifstream fin((char *)(get_path() + _T("errlog")).c_str());
            fin.getline(file, 1000000, EOF);

            res.ret = Return::RTE;
            res.val = retval;
            res.msg = _T("Returned ") + i2s(retval) + _T(".\nErrlog:\n") + file;
			delete file;
        }else
        {
            res.ret = Return::OK;  // All right
            res.val = 0;
            res.msg = _T("Process exited normally.");
        }
    }

    return res;
}

#endif

Result Sandbox::run(Tstring file, Tstring args[], Limit time, Limit space, bool restricted, Tstring fin, Tstring fout, Tstring ferr)
{
    Tchar *arguments[1000];
    int i;

    for(i = 0; args[i] != _T(""); i++)
        arguments[i] = (Tchar *)(args[i].c_str());
    arguments[i] = NULL;

    return run(file, arguments, time, space, restricted, fin, fout, ferr);
}

Tstring Sandbox::get_path()
{
    return _T("../Container/")+ name + _T("/");
}

Sandbox::Sandbox(Tstring s)
{
    name = s;
}
