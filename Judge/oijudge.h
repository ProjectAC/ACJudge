#include "judge.h"

namespace ACJudge
{
    class OIJudge : Judge
    {
    public:
        // [Interface] 
        Grades judge(Task task, Submission submission);
        // [Constructor]
        OIJudge(Tstring name) : Judge(name) {}
    };
}