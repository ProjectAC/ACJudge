#include "filesys.h"
#include "../Lib/lib.h"

using namespace std;
using namespace ACJudge;

wstring FileSys::get_std_input(ID did)
{
    return tpath + i2s(did) + L".in";
}

wstring FileSys::get_std_output(ID did)
{
    return tpath + i2s(did) + L".out";
}

wstring FileSys::get_user_output(ID did)
{
    return spath + i2s(did) + L".out";
}

wstring FileSys::get_user_err_output()
{
    return bpath + L"user.err";
}

wstring FileSys::get_user_output()
{
    return bpath + L"user.out";
}

Result FileSys::compile(CodeType id, Language l)
{
    Sandbox box(sbn);
    wstring path = (id == CodeType::USER ? spath : tpath);
    wstring name = (id == CodeType::USER ? L"user" : L"spj");

    if(l == Language::CPP)  // GNU C++ 99
    {
        wstring args[] = {L"g++", path + name + L".cpp", L"-o", bpath + name, L"-O2", L"-Wall", L"-lm", L""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, L"", L"", L"errlog");
    }else if(l == Language::CPP11)  // GNU C++ 11 and later
    {
        wstring args[] = {L"g++", path + name + L".cpp", L"-o", bpath + name, L"-O2", L"-Wall", L"-lm", L"-DONLINE_JUDGE", L"-std=c++11", L""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, L"", L"", L"errlog");
    }else if(l == Language::C)  // GNU C 99
    {
        wstring args[] = {L"gcc", path + name + L".c", L"-o", bpath + name, L"-O2", L"-Wall", L"-lm", L"-DONLINE_JUDGE", L"-std=c99", L""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, L"", L"", L"errlog");
    }else /*if(l == Language::PYTHON)  // Python 2*/
    {
        wstring args[] = {L"cp", path + name + L".py", bpath, L""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, L"", L"", L"errlog");
    }
}

FileSys::FileSys(ID tid, ID sid, wstring sbn)
{
    spath = L"../Data/Submissions/" + i2s(sid) + L"/";
    tpath = L"../Data/Tasks/" + i2s(tid) + L"/";
    bpath = L"../Container/" + sbn + L"/";
    FileSys::sbn = sbn;
}