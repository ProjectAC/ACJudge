#include "../Judge/oijudge.h"
#include "../Judge/acmjudge.h"
#include "../Definations/types.h"
#include "../Definations/enums.h"
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
using namespace ACJudge;

wstring result[10] = 
{
    L"Accepted",
    L"System Error",
    L"Time Limit Exceed",
    L"Memory Limit Exceed",
    L"Runtime Error",
    L"Compilation Error",
    L"Wrong Answer"
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
int wmain(int argc, wchar_t *args[])
{
    if(argc == 1)
    {
        cout << L"\
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
    
    wstring name(args[1]);
#if defined WINDOWS
	swscanf(args[2], L"%I64u", &tid);
	swscanf(args[3], L"%I64u", &sid);
#elif defined _NIX
    swscanf(args[2], L"%llu", &tid);
    swscanf(args[3], L"%llu", &sid);
#endif
    swscanf(args[4], L"%d", &type);
    swscanf(args[5], L"%hu", &cnt);
    swscanf(args[6], L"%u", &time);
    swscanf(args[7], L"%u", &space);
    swscanf(args[8], L"%d", &usrlan);
    swscanf(args[9], L"%d", &spjlan);

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
    Result totres = {0, 0, Return::OK, 0, L""};
    Score score = 0;

    wcout << L"------------------------------------------------" << endl;
    for(auto grade: grades)
    {
        score += grade.score;
        totres.ret = grade.res.ret;
        totres.time += grade.res.time;
        totres.space = max(totres.space, grade.res.space);
        totres.msg = grade.res.msg;

        wcout << L"[Data #" << i << L" ] L";
        wcout << result[-grade.res.ret];
        if(type == TaskType::OI) wcout << L"(" << grade.score << L")";
        wcout << L", L" << grade.res.time << L"ms, L";
        wcout << grade.res.space << L"KB." << endl;
        //cout << L"Message: \n" << grade.res.msg << endl;

        i++;
    }
    wcout << L"------------------------------------------------" << endl;

    i--;
    if(type == TaskType::OI)
    {
        wcout << L"Total score: L" << score << endl;
    }
    else if(type == TaskType::ACM)
    {
        if(i == cnt)
            wcout << L"Accepted" << endl;
        else
            wcout << result[-totres.ret] << L" on data #" << i << endl;
    }
    wcout << L"Total Time: L" << totres.time << endl;
    wcout << L"Total Memory: L" << totres.space << endl;
    wcout << L"------------------------------------------------" << endl;
    wcout << L"Message:\n" << totres.msg << endl;
    wcout << L"------------------------------------------------" << endl;

    return 0;
}