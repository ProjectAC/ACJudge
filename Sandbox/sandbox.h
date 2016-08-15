#pragma once

#include <string>
#include "../Definations/types.h"
#include "../Definations/enums.h"

namespace ACJudge
{
    class Sandbox
    {
    private:
        // Name of the sandbox
        // This is also the name of the folder the certain program will run in
        std::string name;

        // Set time limit
        // Both with setitimer and setrlimit
        // Here time is CPU time, and stime is actual time
        Return set_time_limit(Limit time); 
        
        // Set space limit
        // With setrlimit
        Return set_space_limit(Limit space);

        // Redirect I/O flow <file> to file with name <name>
        Return set_file(FILE *fp, std::string file, std::string mode);

        // Starter
        // This is what the starter have to do before execve
        void start(std::string file, char *args[], Limit time, Limit space, bool restricted, std::string fin, std::string fout, std::string ferr);

        // Set time and space limits for current process
        // Always called immediately after forking
        Return set_limits(Limit time, Limit space);

        // Set rules (security) for current process
        // Called when <restricted> is true
        // [Thanks] QingDaoU Judger
        // link: https://github.com/QingdaoU/Judger
        Return set_rules(std::string s);

        // Set group of current process to guest for safety
        Return set_gid();

        // Redirect file input/output/error target
        Return redirection(std::string in, std::string out, std::string err);

    public:

        // [Interface] run
        // Run the certain program with listed arguments
        // Limit time and space
        // And (might) restrict syscalls if <restricted> set to true
        // Redirect I/O to file <fin> <fout> and <ferr>
        // If these pointers are set to NULL, then stdin/out will be remained 
        // Caution: the last member of array args[] must be NULL
        Result run(std::string file, char *args[], Limit time, Limit space, bool restricted, std::string fin, std::string fout, std::string ferr);

        // [Interface] run
        // Same as the last function but arguments are strings
        // Caution: the last member of array args[] must be "" (a blank string)
        Result run(std::string file, std::string args[], Limit time, Limit space, bool restricted, std::string fin, std::string fout, std::string ferr);

        // [Interface] get sandbox name
        std::string get_name();

        // [Constructor]
        Sandbox(std::string s);
    };
}