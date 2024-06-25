// mdlfun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/process.hpp>
#include <boost/dll.hpp>
#include <string>
#include "funcs.h"
#include "dung.h"
#include "rooms.h"

std::string pw(SIterator unm, SIterator key);
std::string username();

using namespace std::string_view_literals;

int run_zork()
{
	// Option to print incantation to skip directly to the endgame.
#if 0
    std::string un = username();
    std::string hello("HELLO");
	std::cerr << "Hello : " << pw(un, hello) << std::endl;
#endif

	init_dung();

	save_it(true);

    bool restart = false;
    try
    {
        rdcom();
    }
    catch (ExitException &e)
    {
        restart = e.restart_flag();
    }

	return restart;
}

int main(int argc, char *argv[])
{
    int rv = 0;
    if (argc == 1)
    {
        intptr_t status = 1;
		// If no arguments are passed, spawn the same process
		// with the "-go" parameter. This allows restarts to
		// happen easily. If the child process returns 1, 
		// a restart will occur. If it returns 0 the shell will
		// exit.
        boost::asio::io_context ctx;
        while (status == 1)
        {
            auto path = boost::dll::program_location().string();
            status = boost::process::system(path + " -go");
        }
    }
    else
    {
	    rv = run_zork();
    }

    return rv;
}

