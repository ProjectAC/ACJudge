# ACJudge
A Complex Judge, designed for ACOJ.  
Currently it's Linux only. But it will soon be available on Windows.  
Mac? Huh? What are you talking about?  
**ACOJ Web goes [here](https://github.com/DXkite/ACOJ)!**   
This project will support ANSI C no more.  
**Warning: My Ubuntu has crashed, so the new (online) version for Linux is NOT TESTED yet!!**

### [Judge]
Scoring users' submissions.  
With ACPL script, problem designers can set their own, unique judging procedure. (Comming soon!)

### [Sandbox]
The warden that protects the system from being damaged by user codes.  
The executable file should (better) be in folder "Container/XXX", while XXX is the name of the Sandbox object.

### [Supports]
Including database, file and management interfaces.  
Database interface will get queueing submissions from database, send them to the judge, collect the results and restore to databse.  
File interface will prepare data, source code and things like that for the judge.  
Management interface is the connection between web server (script) and judge.  
When ACOJ Server is started, this module will be automatically started, but without it, the judge can also be used offline.
  
  
-----------------------

## Update

**[ACJudge 1.4] Link Start**   
ACJudge Online is OK!  
You only need a mysql DBMS to make it run.  
Maybe you also want a web server (classic ones like Apache).   

## License

**GPL v3.**
To avoid disputes, the license of this program was switched to GPL.  
Notice that WTFPL is not in use anymore!!!!!  
  
Thanks [QingDaoU](https://github.com/QingdaoU/Judger) for offering such a sample of sandbox on Linux!  
Thanks [ContestHunter](https://contesthunter.org) for default comparer!
*ContestHunter is OK now! Awesome!*
  
-----------------------

## Build & Run

### For Linux User:

Before build, please make sure you have installed libseccomp, as it is the main part of the sandbox.  
Then, after switching to the direction where the project is placed, follow these steps:
  
    cd Container
    make
    make clean
    cd ..
    make
    make clean
    cd Release
    ./acjudge
  
If it shows the usage of the program...  
Voila! You've done it!  

### For Windows User:

You may straightly switch to /Release/ folder (for the code has already been built) and run ACJudge.exe!  
Solution for MS - Visual Studio 2015 is in folder Visual studio. You may need this if you wish to deal some changes.  
Before building, you should edit /Definations/platform.h to switch the platform, that is, from
    
    // #define WINDOWS
    #define _NIX

to

    #define WINDOWS
    // #define _NIX 

Tip: The pronounciation of word "Windows" is similar with "Wen Dao Si" in Chinese, which means "Plague to Death".   
