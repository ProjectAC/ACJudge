objs = Sandbox/sandbox.o Support/filesys.o Judge/judge.o Judge/oijudge.o Judge/acmjudge.o Main/main.o
CPPFLAGS = -g -O2 -Wall -std=c++11
Release/offline : $(objs)
	g++ -o Release/offline $(objs) -lseccomp
clean:
	rm $(objs)