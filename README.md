# ACJudge
A Complex Judge, designed for ACOJ.
Currently it is linux only. But it will soon available on Windows.
Mac? Huh? What are you talking about?

## [Judge]
Scoring users' submissions.
With ACPL script, problem designers can set their own, unique judging procedure.

## [Sandbox]
The warden that protects the system from being damaged by user codes.

## [Database]
Database interface that will get queueing submissions from database, send them to the judge, collect the results and restore to databse.
When ACOJ Server is started, this module will be automatically started.
