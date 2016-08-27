#pragma once

#include <string>
#include "../Definations/types.h"
#include "../Definations/enums.h"
#include "../Sandbox/sandbox.h"
#include "../Support/filesys.h"

namespace ACJudge
{
    class Judge
    {
    protected:
        Tstring name;
        
    public:
        // [Interface] judge
        // Judge <submission> of <task>
        // Return grades 
        Grades judge(Task task, Submission submission);

        // [Constructor] Judge
        // Init the judge and set its name
        Judge(Tstring s);
    };
}