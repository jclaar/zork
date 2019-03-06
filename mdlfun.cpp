// mdlfun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifndef __GNUC__
#include <process.h>
#else
#include <sys/wait.h>
#endif
#include <string>
#include "funcs.h"
#include "dung.h"
#include "rooms.h"

std::string pw(SIterator unm, SIterator key);
std::string username();

int run_zork()
{
	// Option to print incantation to skip directly to the endgame.
#if 1
    std::string un = username();
    std::string hello("HELLO");
	std::cerr << "Hello : " << pw(un, hello) << std::endl;
#endif

	init_dung();

	save_it(true);

	rdcom();

	return restart_flag();
}

int main(int argc, char *argv[])
{
    int rv = 0;
    if (argc == 1)
    {
        intptr_t status = 1;
#ifdef __GNUC__
		// Linux: Just fork and run Zork using the run_zork function.
		// This allows a simple method to handle restarts. If run_zork
		// returns 1, then the user wants to restart. Otherwise,
		// just quit.
	    while (status == 1)
	    {
                pid_t pid = fork();
                if (pid == 0)
                {
                    rv = run_zork();
                    break;
                }
                else
                {
                    int istatus;
                    waitpid(pid, &istatus, 0);
                    status = istatus != 0;
                }
	    }
#else
		// Win32: If no arguments are passed, spawn the same process
		// with the "-go" parameter. This allows restarts to
		// happen easily. If the child process returns 1, 
		// a restart will occur. If it returns 0 the shell will
		// exit.
		while (status == 1)
        {
            status = _spawnl(P_WAIT, argv[0], argv[0], "-go", nullptr);
        }
#endif
    }
    else
    {
	    rv = run_zork();
    }

    return rv;
}

