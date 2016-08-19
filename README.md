# ACJudge
A Complex Judge, designed for ACOJ.  
Currently it's Linux only. But it will soon be available on Windows.  
Mac? Huh? What are you talking about?
  
**Notice: This project will support C language no more!**

### [Judge]
Scoring users' submissions.  
With ACPL script, problem designers can set their own, unique judging procedure. (Comming soon!)

### [Sandbox]
The warden that protects the system from being damaged by user codes.  
The executable file should (better) be in folder "Contianer/XXX", while XXX is the name of the Sandbox object.

### [Supports]
Including database, file and management interfaces.  
Database interface will get queueing submissions from database, send them to the judge, collect the results and restore to databse.  
File interface will prepare data, source code and things like that for the judge.  
Management interface is the connection between web server (script) and judge.  
When ACOJ Server is started, this module will be automatically started, but without it, the judge can also be used offline.
  
  
-----------------------

## License

**GPL v2.**
To avoid disputes, the license of this program was switched to GPL.
Notice that WTFPL is not in use anymore!!!!!

And thanks [QingDaoU](https://github.com/QingdaoU/Judger) for offering such a sample of sandbox on Linux!
