// mdlfun.cpp : Defines the entry point for the console application.
//

#include <boost/process.hpp>
#include <boost/dll.hpp>
#include <iostream>
#include <string>
#include "funcs.h"
#include "dung.h"
#include "rooms.h"
#include "version.h"

std::string pw(SIterator unm, SIterator key);
std::string username();

using namespace std::string_view_literals;

int run_zork()
{
	// Option to print incantation to skip directly to the endgame.
#if 1
    std::string un = username();
    std::string hello("HELLO");
	std::cerr << "Hello : " << pw(un, hello) << std::endl;
#endif

	init_dung();

    tell("ZORK++ version "sv + sVersion);

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
        while (status == 1)
        {
#if BOOST_VERSION >= 108100
            auto path = boost::dll::program_location().string();
            boost::asio::io_context ctx;
            boost::process::v2::process proc(ctx, path, { "-go" });
            status = proc.wait();
#else
			// Must use v1 of the boost::process library because v2 is not available until Boost 1.81.0.
            boost::filesystem::path exe_path = boost::filesystem::system_complete(argv[0]);
            boost::system::error_code ec;
            auto canonical_path = boost::filesystem::canonical(exe_path, ec);
            std::vector<std::string> new_args;
            new_args.push_back("-go");
			boost::process::child c(canonical_path.string(), new_args);
            c.wait();
            status = c.exit_code();
#endif
        }
    }
    else
    {
	    rv = run_zork();
    }

    return rv;
}

