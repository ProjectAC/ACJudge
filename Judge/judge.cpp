#include "judge.h"

using namespace std;
using namespace ACJudge;

Judge::Judge(string s)
{
    name = s;
}

Grades Judge::judge(Task task, Submission submission)
{
    return Grades(0);
}