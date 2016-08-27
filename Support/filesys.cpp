#include "filesys.h"
#include "../Lib/lib.h"
#include "../Definations/platform.h"

using namespace std;
using namespace ACJudge;

Tstring FileSys::get_std_input(ID did)
{
    return tpath + i2s(did) + _T(".in");
}

Tstring FileSys::get_std_output(ID did)
{
    return tpath + i2s(did) + _T(".out");
}

Tstring FileSys::get_user_output(ID did)
{
    return spath + i2s(did) + _T(".out");
}

Tstring FileSys::get_user_err_output()
{
    return bpath + _T("user.err");
}

Tstring FileSys::get_user_output()
{
    return bpath + _T("user.out");
}

Result FileSys::compile(CodeType id, Language l)
{
    Sandbox box(sbn);
    Tstring path = (id == CodeType::USER ? spath : tpath);
    Tstring name = (id == CodeType::USER ? _T("user") : _T("spj"));
	Tstring outname = name;
#if defined WINDOWS
	outname += _T(".exe");
#endif

    if(l == Language::CPP)  // GNU C++ 99
    {
        Tstring args[] = {_T("g++"), path + name + _T(".cpp"), _T("-o"), bpath + outname, _T("-O2"), _T("-Wall"), _T("-lm"), _T("")};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, _T(""), _T(""), _T("errlog"));
    }else if(l == Language::CPP11)  // GNU C++ 11 and later
    {
        Tstring args[] = {_T("g++"), path + name + _T(".cpp"), _T("-o"), bpath + outname, _T("-O2"), _T("-Wall"), _T("-lm"), _T("-DONLINE_JUDGE"), _T("-std=c++11"), _T("")};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, _T(""), _T(""), _T("errlog"));
    }else if(l == Language::C)  // GNU C 99
    {
        Tstring args[] = {_T("gcc"), path + name + _T(".c"), _T("-o"), bpath + outname, _T("-O2"), _T("-Wall"), _T("-lm"), _T("-DONLINE_JUDGE"), _T("-std=c99"), _T("")};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, _T(""), _T(""), _T("errlog"));
    }else /*if(l == Language::PYTHON)  // Python 2*/
    {
#if defined WINDOWS
        Tstring args[] = {_T("copy"), path + name + _T(".py"), bpath, _T("")};
#elif defined _NIX
		Tstring args[] = { _T("cp"), path + name + _T(".py"), bpath, _T("")};
#endif
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, _T(""), _T(""), _T("errlog"));
    }
}

FileSys::FileSys(ID tid, ID sid, Tstring sbn)
{
	spath = _T("../Data/Submissions/")+ i2s(sid) + _T("/");
	tpath = _T("../Data/Tasks/")+ i2s(tid) + _T("/");
	bpath = _T("../Container/")+ sbn + _T("/");
    FileSys::sbn = sbn;
}