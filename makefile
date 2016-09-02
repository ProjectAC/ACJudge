objs = Sandbox/sandbox.o Support/filesys.o Judge/judge.o Judge/oijudge.o Judge/acmjudge.o Main/main.o
CPPFLAGS = -s -O2 -Wall -Wextra -std=c++11 -I/usr/include/mysql
Release/acjudge : $(objs)
	g++ -o Release/offline $(objs) -lseccomp
clean:
	rm $(objs)