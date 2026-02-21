// mdlfun.cpp : Defines the entry point for the console application.
//

#include "precomp.h"
#include <boost/process.hpp>
#include <boost/process/v2.hpp>
#include <boost/dll.hpp>
#include <string>
#include <iostream>
#include "funcs.h"
#include "dung.h"
#include "rooms.h"
#include "version.h"
#include "chafa_wrapper.h"
#include "ascii_art.h"

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

    tell("ZORK++ version "sv + sVersion);
    // Determine ascii-art status
    std::string ascii_status;
    if (!flags[FlagId::ascii_art])
    {
        tell("ASCII art mode: none");
    }
    else if (chafa_available())
    {
        tell("ASCII art mode: chafa");
    }
    else
    {
        tell("ASCII art mode: builtin, install chafa for better look");
    }


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

void print_usage(const char* program_name)
{
    std::cerr << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -go           Run the game (internal use)\n"
              << "  --ascii-art   Enable ASCII art images for rooms\n"
              << "  -h, --help    Show this help message\n";
}

int main(int argc, char *argv[])
{
    int rv = 0;
    bool ascii_art_enabled = false;
    bool go_flag = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--ascii-art")
        {
            ascii_art_enabled = true;
        }
        else if (arg == "-h" || arg == "--help")
        {
            print_usage(argv[0]);
            return 0;
        }
        else if (arg == "-go")
        {
            // Internal use flag - indicates we're the child process
            go_flag = true;
        }
    }

    // If no arguments, spawn child process with -go flag
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
            auto path = boost::dll::program_location().string();
            boost::asio::io_context ctx;
            std::vector<std::string> args = { "-go" };
            if (ascii_art_enabled)
                args.push_back("--ascii-art");
            boost::process::v2::process proc(ctx, path, args);
            status = proc.wait();
        }
    }
    else if (go_flag)
    {
        // We're the child process (spawned with -go), run the game
        flags[FlagId::ascii_art] = ascii_art_enabled;
        rv = run_zork();
    }
    else
    {
        // Run with user-provided arguments
        flags[FlagId::ascii_art] = ascii_art_enabled;
        rv = run_zork();
    }

    return rv;
}
