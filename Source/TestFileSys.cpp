#include "../Support/filesys.h"
#include <iostream>

using namespace std;
using namespace ACJudge;

int main()
{
    FileSys fs(1, 1, "SOL");
    cout << fs.compile(CodeType::USER, Language::CPP).ret << endl;

    return 0;
}