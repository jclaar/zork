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

std::string pw(Iterator<std::string> unm, Iterator<std::string> key);
std::string username();

int run_zork()
{
	// Option to print incantation to skip directly to the endgame.
	//std::cerr << "Hello : " << pw(username(), std::string("HELLO")) << std::endl;
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
        // If no arguments are passed, spawn the same process
        // with the "-go" parameter. This allows restarts to
        // happen easily. If the child process returns 1, 
        // a restart will occur. If it returns 0 the shell will
        // exit. This allows a simple, portable method to handle
        // restarts.
        intptr_t status = 1;
#ifdef __GNUC__
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

