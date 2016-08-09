# ACJudge
A Complex Judge, designed for ACOJ.  
Currently it's Linux only. But it will soon be available on Windows.
Mac? Huh? What are you talking about?

### [Judge]
Scoring users' submissions.  
With ACPL script, problem designers can set their own, unique judging procedure. (Comming soon!)

### [Sandbox]
The warden that protects the system from being damaged by user codes.  

### [Supports]
Including database, file and management interfaces.  
Database interface will get queueing submissions from database, send them to the judge, collect the results and restore to databse.  
File interface will prepare data, source code and things like that for the judge.  
Management interface is the connection between web server (script) and judge.  
When ACOJ Server is started, this module will be automatically started, but without it, the judge can also be used offline.
  
  
-----------------------

## License

Have you ever heard of WTFPL?  
If not, [here](https://en.wikipedia.org/wiki/WTFPL) it goes.  
Never mind, just enjoy it!
