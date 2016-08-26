#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

class Compare
{
private:
    FILE *fstd,*fout;
    int LastCharStd, LastCharOut;
protected:
    inline int Peek(FILE* f);
    inline void Pop(FILE* f);
    inline bool IsSpace(int ch);
    bool DoCompare();
    int default_compare(const char *s1, const char *s2, int score);
public:
    int compare(const char *s1, const char *s2, int score);
};

//检查下一个字符
inline int Compare::Peek(FILE* f){
    if(f==fstd){
        if(LastCharStd == -2)
            LastCharStd=fgetc(f);
        return LastCharStd;
    }else{
        if(LastCharOut == -2)
            LastCharOut=fgetc(f);
        return LastCharOut;
    }
}

//取出下一个字符
inline void Compare::Pop(FILE* f){
    if(f==fstd){
        if(LastCharStd == -2)
            fgetc(f);
        else
            LastCharStd = -2;
    }else{
        if(LastCharOut == -2)
            fgetc(f);
        else
            LastCharOut = -2;
    }
}
 
//判断字符是否为空白
inline bool Compare::IsSpace(int ch){
    return ch>=0 && (ch<=32 || ch>=127);
}

//执行比较操作。
bool Compare::DoCompare(){
    int stdPosition=0,outPosition=0;
    bool stdInSpace=true,outInSpace=true;
    while(true){
        int stdC=Peek(fstd),outC=Peek(fout);
        if(stdC==EOF && outC==EOF){
            return true;
        }else if(stdC==EOF && IsSpace(outC)){
            outPosition++;
            Pop(fout);
        }else if(outC==EOF && IsSpace(stdC)){
            stdPosition++;
            Pop(fstd);
        }else if(IsSpace(stdC) && IsSpace(outC)){
            stdPosition++;
            outPosition++;
            stdInSpace=true;
            outInSpace=true;
            Pop(fstd);
            Pop(fout);
        }else if(IsSpace(stdC) && outInSpace){
            stdPosition++;
            Pop(fstd);
        }else if(IsSpace(outC) && stdInSpace){
            outPosition++;
            Pop(fout);
        }else if(stdC==outC){
            stdPosition++;
            outPosition++;
            stdInSpace=false;
            outInSpace=false;
            Pop(fstd);
            Pop(fout);
        }else{
            return false;
        }
    }
}

int Compare::default_compare(const char *s1, const char *s2, int score)
{
    //打开文件
    if(NULL==(fstd=fopen(s1,"r"))){
        return -1;
    }
    if(NULL==(fout=fopen(s2,"r"))){
        return -1;
    }
    if(DoCompare()){
        return score;
    }else{
        return 0;
    }
}

int Compare::compare(const char *s1, const char *s2, int score)
{
    LastCharStd = -2, LastCharOut = -2;
    return default_compare(s1, s2, score);
}

int main(int argc, char *argv[])
{
    Compare compare;
    int score;
    sscanf(argv[4], "%d", &score);
    return compare.compare(argv[2], argv[3], score);
}
