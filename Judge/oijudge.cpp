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
        grades.push_back({{0, 0, Return::CE, 0, "Compilation Error. \n" + res.msg}, 0});
        return grades;
    }

    if(task.language != Language::NONE)
    {
        res = fs.compile(CodeType::SPJ, task.language);  // compile SPJ code
        if(res.ret != Return::OK)
        {
            grades.push_back({{0, 0, Return::ERR, 0, "System Error while compiling Special Judge code. \n" + res.msg}, 0});
            return grades;
        }
    }

    Sandbox box(name);
    for(auto &data: task.data)
    {
        string sin = fs.get_std_input(data.did),
               uout = fs.get_user_output(),
               sout = fs.get_std_output(data.did),
               uerr = fs.get_user_err_output(),
               args[] = {""};
        
        res = box.run("./user", args, data.time, data.space, true, "../" + sin, "user.out", "user.err");
        if(res.ret != Return::OK)
        {
            grades.push_back({res, 0});
            continue;
        }
        
        string args2[] = {"", sin, sout, uout, i2s(data.score), ""};
        if(task.language != Language::NONE)
            args2[0] = "./spj";
        else
            args2[0] = "../default_judge";
        res2 = box.run("../default_judge", args2, 5000, LIMIT_INFINITE, true, "", "", "errlog2");

        if((res2.ret != Return::RTE && res2.ret != Return::OK) || res2.val < 0)
            grades.push_back({{res.time, res.space, Return::ERR, 0, "System Error while judging. \n" + res.msg}, 0});
        else
        {
            if(res2.val != data.score)
                res.ret = Return::WA;
            grades.push_back({res, (Score)res2.val});
        }
    }

    return grades;
}