#include "oijudge.h"
#include "../Lib/lib.h"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace ACJudge;

Grades OIJudge::judge(Task task, Submission submission)
{
    FileSys fs(task.tid, submission.sid, name);

    //Prepare
    Result res, res2;
    Grades grades;

    res = fs.compile(CodeType::USER, submission.language);  // compile User code
    if(res.ret != Return::OK)
    {
        grades.push_back({{0, 0, Return::CE, 0, _T("Compilation Error. \n") + res.msg}, 0});
        return grades;
    }

    if(task.language != Language::NONE)
    {
        res = fs.compile(CodeType::SPJ, task.language);  // compile SPJ code
        if(res.ret != Return::OK)
        {
            grades.push_back({{0, 0, Return::ERR, 0, _T("System Error while compiling Special Judge code. \n") + res.msg}, 0});
            return grades;
        }
    }

    Sandbox box(name);
    for(auto &data: task.data)
    {
        res = fs.run(CodeType::USER, submission.language, data);
        if(res.ret != Return::OK)
        {
            grades.push_back({res, 0});
            continue;
        }
        
		res2 = fs.run(CodeType::SPJ, task.language, data);
        if((res2.ret != Return::RTE && res2.ret != Return::OK) || res2.val < 0)
            grades.push_back({{res.time, res.space, Return::ERR, 0, _T("System Error while judging. \n")+ res.msg}, 0});
        else
        {
            if(res2.val != data.score)
                res.ret = Return::WA;
            grades.push_back({res, (Score)res2.val});
        }
    }

    return grades;
}