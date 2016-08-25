#include <iostream>
#include <cstring>
#include <cstdio>
#include <Windows.h>

using namespace std;

enum Result
{
	AC = 0,
	PE = -1,
	WA = -2,
	OLE = -3,
	TLE = -4,
	MLE = -5,
	RTE = -6,
	CE = -7,
	CRTE = -8
};

Result start()
{
	int space = 1000, time = 1000;

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION ex_lim;
	JOBOBJECT_BASIC_UI_RESTRICTIONS bs_ui;
	wchar_t str[] = L"TLE.exe";

	//清零标记
	ZeroMemory(&ex_lim, sizeof(ex_lim));

	//设置最大进程数
	ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
	ex_lim.BasicLimitInformation.ActiveProcessLimit = 1;

	//设置内存限制
	ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_MEMORY;
	ex_lim.JobMemoryLimit = SIZE_T(space * 1000 + 10000000);

	//设置时间限制
	ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_TIME;
	ex_lim.BasicLimitInformation.PerJobUserTimeLimit.QuadPart = time * 10000;

	//设置系统调用限制
	bs_ui.UIRestrictionsClass =
		JOB_OBJECT_UILIMIT_EXITWINDOWS |
		JOB_OBJECT_UILIMIT_READCLIPBOARD |
		JOB_OBJECT_UILIMIT_WRITECLIPBOARD |
		JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |
		JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
		JOB_OBJECT_UILIMIT_GLOBALATOMS |
		JOB_OBJECT_UILIMIT_DESKTOP |
		JOB_OBJECT_UILIMIT_HANDLES;

	//创建文件
	HANDLE fin = CreateFile(L"",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		0,
		CREATE_ALWAYS,
		FILE_FLAG_OVERLAPPED,
		0);
	HANDLE fout = CreateFile(L"a.out",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		0,
		CREATE_ALWAYS,
		FILE_FLAG_OVERLAPPED,
		0);
	HANDLE ferr = CreateFile(L"",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		0,
		CREATE_ALWAYS,
		FILE_FLAG_OVERLAPPED,
		0);
	//创建IO接口
	STARTUPINFO s = { sizeof(s) };
	ZeroMemory(&s, sizeof(s));
	s.cb = sizeof(STARTUPINFO);
	//s.hStdOutput = fin;
	//s.hStdError = ferr;
	s.hStdInput = fout;
	s.dwFlags = STARTF_USESTDHANDLES;

	//创建作业
	HANDLE job = CreateJobObject(NULL, NULL);
	SetInformationJobObject(job, JobObjectExtendedLimitInformation, &ex_lim, sizeof(ex_lim));
	SetInformationJobObject(job, JobObjectBasicUIRestrictions, &bs_ui, sizeof(bs_ui));

	//创建进程
	_PROCESS_INFORMATION p;
	DWORD ret =
		CreateProcess(NULL, str,
			NULL,
			NULL,
			FALSE,
			CREATE_SUSPENDED,
			NULL,
			NULL,
			&s,
			&p);
	if (!ret)
		return Result::RTE;

	//将进程绑定到作业，然后搞起！
	AssignProcessToJobObject(job, p.hProcess);
	ResumeThread(p.hThread);

	//开始计时
	HANDLE handles[2];
	handles[0] = p.hProcess;
	handles[1] = job;
	ret = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		//进程退出了
		GetExitCodeProcess(p.hProcess, &ret);
		cout << ret << endl;
		if (ret == 1816)
			return Result::TLE;
		else if (ret == 3221225495u)
			return Result::MLE;
		else if (ret)
			return Result::RTE;
		else
			return Result::AC;

	case WAIT_TIMEOUT:
		//进程超时了
		return Result::TLE;

	case WAIT_FAILED:
		//进程爆炸了
		return Result::RTE;
	}

	//未知错误
	return Result::RTE;
}

int main()
{
	cout << start() << endl;

	return 0;
}