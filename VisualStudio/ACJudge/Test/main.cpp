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

	//������
	ZeroMemory(&ex_lim, sizeof(ex_lim));

	//������������
	ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
	ex_lim.BasicLimitInformation.ActiveProcessLimit = 1;

	//�����ڴ�����
	ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_MEMORY;
	ex_lim.JobMemoryLimit = SIZE_T(space * 1000 + 10000000);

	//����ʱ������
	ex_lim.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_TIME;
	ex_lim.BasicLimitInformation.PerJobUserTimeLimit.QuadPart = time * 10000;

	//����ϵͳ��������
	bs_ui.UIRestrictionsClass =
		JOB_OBJECT_UILIMIT_EXITWINDOWS |
		JOB_OBJECT_UILIMIT_READCLIPBOARD |
		JOB_OBJECT_UILIMIT_WRITECLIPBOARD |
		JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |
		JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
		JOB_OBJECT_UILIMIT_GLOBALATOMS |
		JOB_OBJECT_UILIMIT_DESKTOP |
		JOB_OBJECT_UILIMIT_HANDLES;

	//�����ļ�
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
	//����IO�ӿ�
	STARTUPINFO s = { sizeof(s) };
	ZeroMemory(&s, sizeof(s));
	s.cb = sizeof(STARTUPINFO);
	//s.hStdOutput = fin;
	//s.hStdError = ferr;
	s.hStdInput = fout;
	s.dwFlags = STARTF_USESTDHANDLES;

	//������ҵ
	HANDLE job = CreateJobObject(NULL, NULL);
	SetInformationJobObject(job, JobObjectExtendedLimitInformation, &ex_lim, sizeof(ex_lim));
	SetInformationJobObject(job, JobObjectBasicUIRestrictions, &bs_ui, sizeof(bs_ui));

	//��������
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

	//�����̰󶨵���ҵ��Ȼ�����
	AssignProcessToJobObject(job, p.hProcess);
	ResumeThread(p.hThread);

	//��ʼ��ʱ
	HANDLE handles[2];
	handles[0] = p.hProcess;
	handles[1] = job;
	ret = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		//�����˳���
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
		//���̳�ʱ��
		return Result::TLE;

	case WAIT_FAILED:
		//���̱�ը��
		return Result::RTE;
	}

	//δ֪����
	return Result::RTE;
}

int main()
{
	cout << start() << endl;

	return 0;
}