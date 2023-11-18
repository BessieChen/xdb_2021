// This program does not need to include _public.h, there is no need to depend on so many header files.
#include <cstdio> // C++ style, C language style is <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
    if (argc<3)
    {
        printf("Using:./procctl timetvl program argv ...\n");
        printf("Example:/home/xdb/project/tools/bin/procctl 10 /usr/bin/tar zcvf /tmp/tmp.tgz /usr/include\n");
        printf("Example:/home/xdb/project/tools/bin/procctl 60 /home/xdb/project/idc/bin/crtsurfdata /home/xdb/project/idc/ini/stcode.ini /tmp/idc/surfdata /home/xdb/project/tools/log/idc/crtsurfdata.log csv,xml,json\n");

        printf("This program is a scheduler for service programs, periodically launching service programs or shell scripts.\n");
        printf("timetvl Running interval, unit: seconds.\n");
        printf("        After the scheduled program ends, it will be restarted by procctl after timetvl seconds.\n");
        printf("        If the scheduled program is a periodic task, set timetvl as the running interval.\n");
        printf("        If the scheduled program is a resident memory service program, set timetvl less than 5 seconds.\n");
        printf("program The name of the scheduled program, must use the full path.\n");
        printf("...     Arguments for the scheduled program.\n");
        printf("Note, this program cannot be killed with kill, but can be forcibly killed with kill -9.\n\n\n");

        return -1;
    }

    // Close signals and I/O, this program does not want to be disturbed.
    // Note: 1) To prevent the scheduler from being killed by mistake, do not handle exit signals;
    //       2) If signals are ignored and I/O is closed, it will affect the scheduled program (it will also ignore signals and close I/O). Why? Because the scheduled program replaces the child process, and the child process will inherit the parent process's signal handling and I/O.
    for (int ii=0; ii<64; ii++)
    {
        signal(ii, SIG_IGN);  close(ii); // If you want to debug the process we run periodically, and see its printf(), then comment out close(ii)
    }

    // Generate a parent process, the grandparent process exits, allowing the current scheduler to run in the background, managed by system process number 1, not controlled by the shell.
    if (fork()!=0) exit(0);

    // Restore the default behavior of the child process exit signal SIGCHLD, so that the parent process can call the wait() function to wait for the child process to exit.
    signal(SIGCHLD, SIG_DFL);

    // Define a pointer array as large as argv, to store the name and arguments of the scheduled program.
    char *pargv[argc];
    for (int ii=2; ii<argc; ii++)
        pargv[ii-2] = argv[ii];

    pargv[argc-2] = nullptr; // Null indicates the end of the arguments.

    while (true)
    {
        if (fork()==0)
        {
            // The child process runs the scheduled program.
            execv(argv[2], pargv); 
            // Situation 1: Child process is a periodic program {does not run long-term, so after the final return 0, it will wake up the parent process's wait();}
            // Situation 2: Child process is a long-term resident memory function {never exits}, only when it exits or exits abnormally, will it wake up the parent process's wait(); if it never exits, the parent process will always wait(), and will not restart the second resident memory program
            // Situation 3: Run failed, execute exit(0)
            exit(0);  // This line of code will only be executed if the scheduled program fails to run.
        }
        else
        {
            // The parent process waits for the child process to terminate (the scheduled program ends).
            //int status;
            //wait(&status);           // The wait() function will block until the scheduled program terminates.
            wait(nullptr);           //If you don't need to get the child process information, then don't use status
            sleep(atoi(argv[1]));  // Sleep for timetvl seconds, then return to the loop.
        }
    }
}
