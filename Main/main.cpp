#include "../Judge/oijudge.h"
#include "../Judge/acmjudge.h"
#include "../Support/database.h"
#include "../Definations/platform.h"
#include "../Definations/types.h"
#include "../Definations/enums.h"
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
using namespace ACJudge;

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
#if defined WINDOWS
int wmain(int argc, Tchar *args[])
#elif defined _NIX
int main(int argc, Tchar *args[])
#endif
{
    if(argc == 1)
    {
        Tcout << _T("\
The arguments for offline are:\n\
1. judge name\n\
2. tid\n\
3. sid\n\
4. task type (see Definations/enums.h, currently OI and ACM only)\n\
5. number of data\n\
6. time limit (ms)\n\
7. memory limit (KB)\n\
8. user language\n\
9. spj language (if this argument is -1, the judge will use default comparer)\n\
The arguments for offline are:\n\
1. judge count (the maximum number of thread(s) that can run on the server)") << endl;
        return 0;
	}
	else if (argc == 2)
	{
		Tstring name[2] =
		{
			_T("WXC"),
			_T("SOL")
		};

		Database db("127.0.0.1", "access", "1234567");
		Submission s;
		Task t;
		int cnt = 0;

		while (true)
		{
			if (cnt < 1 && (s = db.get_latest_submission(), s.sid))
			{
				t = db.get_task(s.tid);
				cnt++;

				Grades grades;
				if (t.type == TaskType::OI)
					grades = (new OIJudge(name[cnt]))->judge(t, s);
				else if (t.type == TaskType::ACM)
					grades = (new ACMJudge(name[cnt]))->judge(t, s);

				db.set_grades(s.sid, grades);
			}
			else
			{
#if defined WINDOWS
				Sleep(500);
#elif define _NIX
				usleep(500000);
#endif
			}
		}
	}
	else
	{
		Tstring result[10] =
		{
			_T("Accepted"),
			_T("System Error"),
			_T("Time Limit Exceed"),
			_T("Memory Limit Exceed"),
			_T("Runtime Error"),
			_T("Compilation Error"),
			_T("Wrong Answer")
		};

		ID tid, sid;
		int type, usrlan, spjlan;
		Number cnt;
		Limit time, space;

		Tstring name(args[1]);
#if defined WINDOWS
		Tsscanf(args[2], _T("%I64u"), &tid);
		Tsscanf(args[3], _T("%I64u"), &sid);
#elif defined _NIX
		Tsscanf(args[2], _T("%llu"), &tid);
		Tsscanf(args[3], _T("%llu"), &sid);
#endif
		Tsscanf(args[4], _T("%d"), &type);
		Tsscanf(args[5], _T("%u"), &cnt);
		Tsscanf(args[6], _T("%u"), &time);
		Tsscanf(args[7], _T("%u"), &space);
		Tsscanf(args[8], _T("%d"), &usrlan);
		Tsscanf(args[9], _T("%d"), &spjlan);

		vector<Data> data;
		for (Number i = 1; i <= cnt; i++)
		{
			if (type == TaskType::ACM)
				data.push_back({ i, time, space, 1 });
			else if (type == TaskType::OI)
				data.push_back({ i, time, space, (Score)(100 / cnt) });
		}

		Grades grades;
		if (type == TaskType::OI)
			grades = (new OIJudge(name))->judge({ tid, (TaskType)type, data, (Language)spjlan }, { sid, tid, (Language)usrlan });
		else if (type == TaskType::ACM)
			grades = (new ACMJudge(name))->judge({ tid, (TaskType)type, data, (Language)spjlan }, { sid, tid, (Language)usrlan });
		else
		{
			puts("Invalid task type!");
			return 0;
		}

		int i = 1;
		Result totres = { 0, 0, Return::OK, 0, _T("") };
		Score score = 0;

		Tcout << _T("------------------------------------------------") << endl;
		for (auto grade : grades)
		{
			score += grade.score;
			totres.ret = grade.res.ret;
			totres.time += grade.res.time;
			totres.space = max(totres.space, grade.res.space);
			totres.msg = grade.res.msg;

			Tcout << _T("[ Data #") << i << _T(" ] ");
			Tcout << result[-grade.res.ret];
			if (type == TaskType::OI) Tcout << _T(" (") << grade.score << _T(") ");
			Tcout << _T(", ") << grade.res.time << _T("ms, ");
			Tcout << grade.res.space << _T("KB.") << endl;
			//Tcout << _T("Message: \n")<< grade.res.msg << endl;

			i++;
		}
		Tcout << _T("------------------------------------------------") << endl;

		i--;
		if (type == TaskType::OI)
		{
			Tcout << _T("Total score: ") << score << endl;
		}
		else if (type == TaskType::ACM)
		{
			if (score == cnt)
				Tcout << _T("Accepted") << endl;
			else
				Tcout << result[-totres.ret] << _T(" on data #") << i << endl;
		}
		Tcout << _T("Total Time: ") << totres.time << _T("ms") << endl;
		Tcout << _T("Total Memory: ") << totres.space << _T("KB") << endl;
		Tcout << _T("------------------------------------------------") << endl;
		Tcout << _T("Message:\n") << totres.msg << endl;
		Tcout << _T("------------------------------------------------") << endl;
	}

    return 0;
}