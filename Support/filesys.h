#pragma once

#include <string>
#include "../Definations/types.h"
#include "../Definations/enums.h"
#include "../Sandbox/sandbox.h"

namespace ACJudge
{
    class FileSys
    {
        Tstring tpath, spath, bpath, sbn;

    public:
        // [Interface] Get std input
        // It will return the name of the certain file
        Tstring get_std_input(ID did);

        // [Interface] Get std output
        // It will return the name of the certain file
        Tstring get_std_output(ID did);

        // [Interface] Get user output
        // It will return the name of the certain file
        // This method is designed only for answer-submitting task 
        Tstring get_user_output(ID did);

        // [Interface] Get user output
        // It will return the name of the certain file
        // This method is designed for normal code task 
        Tstring get_user_output();

        // [Interface] Get user error output
        // It will return the name of the certain file
        // This method is designed for normal code task 
        Tstring get_user_err_output();

        // [Interface] Compile code
        // It will compile code to the sandbox direction
        // If the code is written in a interpreted language like python
        // It will straightly copy the code to the sandbox direction
        Result compile(CodeType id, Language l);

        // [Constructor]
        FileSys(ID tid, ID sid, Tstring sbn);
    };
}