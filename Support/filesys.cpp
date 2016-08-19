#include "filesys.h"
#include "../Lib/lib.h"

using namespace std;
using namespace ACJudge;

string FileSys::get_std_input(ID did)
{
    return tpath + i2s(did) + ".in";
}

string FileSys::get_std_output(ID did)
{
    return tpath + i2s(did) + ".out";
}

string FileSys::get_user_output(ID did)
{
    return spath + i2s(did) + ".out";
}

string FileSys::get_user_err_output()
{
    return bpath + "user.err";
}

string FileSys::get_user_output()
{
    return bpath + "user.out";
}

Result FileSys::compile(CodeType id, Language l)
{
    Sandbox box(sbn);
    string path = (id == CodeType::USER ? spath : tpath);
    string name = (id == CodeType::USER ? "user" : "spj");

    if(l == Language::CPP)  // GNU C++ 99
    {
        string args[] = {"g++", path + name + ".cpp", "-o", bpath + name, "-O2", "-Wall", "-lm", ""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, "", "", "errlog");
    }else if(l == Language::CPP11)  // GNU C++ 11 and later
    {
        string args[] = {"g++", path + name + ".cpp", "-o", bpath + name, "-O2", "-Wall", "-lm", "-DONLINE_JUDGE", "-std=c++11", ""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, "", "", "errlog");
    }else if(l == Language::C)  // GNU C 99
    {
        string args[] = {"gcc", path + name + ".c", "-o", bpath + name, "-O2", "-Wall", "-lm", "-DONLINE_JUDGE", "-std=c99", ""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, "", "", "errlog");
    }else /*if(l == Language::PYTHON)  // Python 2*/
    {
        string args[] = {"cp", path + name + ".py", bpath, ""};
        return box.run(args[0], args, 5000, LIMIT_INFINITE, 0, "", "", "errlog");
    }
}

FileSys::FileSys(ID tid, ID sid, string sbn)
{
    spath = "../Data/Submissions/" + i2s(sid) + "/";
    tpath = "../Data/Tasks/" + i2s(tid) + "/";
    bpath = "../Container/" + sbn + "/";
    FileSys::sbn = sbn;
}