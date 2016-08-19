#include "../Judge/oijudge.h"
#include "../Judge/acmjudge.h"
#include "../Definations/types.h"
#include "../Definations/enums.h"
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;
using namespace ACJudge;

string result[10] = 
{
    "Accepted",
    "System Error",
    "Time Limit Exceed",
    "Memory Limit Exceed",
    "Runtime Error",
    "Compilation Error",
    "Wrong Answer"
};

// The arguments are:
// 1. judge name
// 2. tid
// 3. sid
// 4. task type (see Definations/enums, currently OI and ACM only)
// 5. number of data
// 6. time limit (ms)
// 7. memory limit (KB)
// 8. user language
// 9. spj language (if this argument is -1, the judge will use default comparer)
int main(int argc, char *args[])
{
    if(argc == 1)
    {
        cout << "\
The arguments are:\n\
1. judge name\n\
2. tid\n\
3. sid\n\
4. task type (see Definations/enums.h, currently OI and ACM only)\n\
5. number of data\n\
6. time limit (ms)\n\
7. memory limit (KB)\n\
8. user language\n\
9. spj language (if this argument is -1, the judge will use default comparer)" << endl;
        return 0;
    }


    ID tid, sid;
    int type, usrlan, spjlan;
    Number cnt;
    Limit time, space;
    
    string name(args[1]);
    sscanf(args[2], "%llu", &tid);
    sscanf(args[3], "%llu", &sid);
    sscanf(args[4], "%d", &type);
    sscanf(args[5], "%hu", &cnt);
    sscanf(args[6], "%u", &time);
    sscanf(args[7], "%u", &space);
    sscanf(args[8], "%d", &usrlan);
    sscanf(args[9], "%d", &spjlan);

    vector<Data> data;
    for(Number i = 1; i <= cnt; i++)
    {
        if(type == TaskType::ACM)
            data.push_back({i, time, space, 1});
        else if(type == TaskType::OI)
            data.push_back({i, time, space, (Score)(100 / cnt)});
    }

    Grades grades;
    if(type == TaskType::OI)
        grades = (new OIJudge(name))  -> judge({tid, (TaskType)type, data, (Language)spjlan}, {sid, (Language)usrlan});
    else if(type == TaskType::ACM)
        grades = (new ACMJudge(name)) -> judge({tid, (TaskType)type, data, (Language)spjlan}, {sid, (Language)usrlan});
    else
    {
        puts("Invalid task type!");
        return 0;
    }
    
    int i = 1;
    Result totres = {0, 0, Return::OK, 0, ""};
    Score score = 0;

    cout << "-------------------------------------" << endl;
    for(auto grade: grades)
    {
        score += grade.score;
        totres.ret = grade.res.ret;
        totres.time += grade.res.time;
        totres.space = max(totres.space, grade.res.space);
        totres.msg = grade.res.msg;

        cout << "[Data #" << i << " ]" << endl;
        cout << "Score: " << grade.score << endl;
        cout << "Result: " << result[-grade.res.ret] << endl;
        cout << "Time: " << grade.res.time << endl;
        cout << "Memory: " << grade.res.space << endl;
        cout << "Message:\n" << grade.res.msg << endl;
        cout << "-------------------------------------" << endl;

        i++;
    }

    i--;
    if(type == TaskType::OI)
    {
        cout << "Total score: " << score << endl;
    }
    else if(type == TaskType::ACM)
    {
        if(i == cnt)
            cout << "Accepted." << endl;
        else
            cout << result[-totres.ret] << " on data #" << i << endl;
    }
    cout << "Total Time: " << totres.time << endl;
    cout << "Total Memory: " << totres.space << endl;
    cout << "-------------------------------------" << endl;

    return 0;
}