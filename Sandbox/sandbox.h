#pragma once

#include <string>
#include <Windows.h>
#include "../Definations/platform.h"
#include "../Definations/types.h"
#include "../Definations/enums.h"

namespace ACJudge
{
    class Sandbox
    {
    private:
        // Name of the sandbox
        // This is also the name of the folder the certain program will run in
        std::wstring name;

#if defined WINDOWS

		// Set limits
		// Tags will be saved in 
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION set_limits(Limit time, Limit space, bool restricted);

		// Set UI restrictions
		JOBOBJECT_BASIC_UI_RESTRICTIONS set_rules_UI(bool restricted);

		// Redirect file input/output/error target
		STARTUPINFO redirection(std::wstring in, std::wstring out, std::wstring err);

#elif defined _NIX
        // Set time limit
        // Both with setitimer and setrlimit
        // Here time is CPU time, and stime is actual time
        Return set_time_limit(Limit time); 
        
        // Set space limit
        // With setrlimit
        Return set_space_limit(Limit space);

        // Redirect I/O flow <file> to file with name <name>
        Return set_file(FILE *fp, std::wstring file, std::wstring mode);

        // Starter
        // This is what the starter have to do before execve
        void start(std::wstring file, wchar_t *args[], Limit time, Limit space, bool restricted, std::wstring fin, std::wstring fout, std::wstring ferr);

        // Set time and space limits for current process
        // Always called immediately after forking
        Return set_limits(Limit time, Limit space);

        // Set rules (security) for current process
        // Called when <restricted> is true
        // [Thanks] QingDaoU Judger
        // link: https://github.com/QingdaoU/Judger
        Return set_rules(std::wstring s);

        // Set group of current process to guest for safety
        Return set_gid();

        // Redirect file input/output/error target
        Return redirection(std::wstring in, std::wstring out, std::wstring err);
#endif

    public:

        // [Interface] run
        // Run the certain program with listed arguments
        // Limit time and space
        // And (might) restrict syscalls if <restricted> set to true
        // Redirect I/O to file <fin> <fout> and <ferr>
        // If these pointers are set to NULL, then stdin/out will be remained 
        // If <file> is started with L"./", the sandbox will run the program in its own path, or system path otherwise
        // Caution: the last member of array args[] must be NULL
        Result run(std::wstring file, wchar_t *args[], Limit time, Limit space, bool restricted, std::wstring fin, std::wstring fout, std::wstring ferr);

        // [Interface] run
        // Same as the last function but arguments are wstrings
        // Caution: the last member of array args[] must be L"" (a blank wstring)
        Result run(std::wstring file, std::wstring args[], Limit time, Limit space, bool restricted, std::wstring fin, std::wstring fout, std::wstring ferr);

        // [Interface] get path of this sandbox
        std::wstring get_path();

        // [Constructor]
        Sandbox(std::wstring s);
    };
}