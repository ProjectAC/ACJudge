#include "judge.h"

namespace ACJudge
{
    class ACMJudge : Judge
    {
    public:
        // [Interface] 
        Grades judge(Task task, Submission submission);
        // [Constructor]
        ACMJudge(std::wstring name) : Judge(name) {}
    };
}