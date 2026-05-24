// mdlfun.cpp : Defines the entry point for the console application.
//

#include "precomp.h"
#include <boost/process.hpp>
#include <boost/program_options.hpp>
//#include <boost/process/v2.hpp>
#include <boost/dll.hpp>
#include <string>
#include <iostream>
#include "funcs.h"
#include "dung.h"
#include "rooms.h"
#include "version.h"
#include "chafa_wrapper.h"
#include "ascii_art.h"
#include "raylib_wrapper.h"
#include "raylib_console.h"

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

    // Initialize GUI mode if requested
    if (flags[FlagId::gui_mode])
    {
        if (raylib_console_available())
        {
            if (!raylib_console_init(1024, 768, "Zork++"))
            {
                tell("Failed to initialize GUI mode, falling back to terminal");
                flags[FlagId::gui_mode] = false;
            }
        }
        else
        {
            tell("GUI mode: NOT AVAILABLE - raylib not installed");
            tell("To enable GUI, install raylib and rebuild with: cmake -DUSE_RAYLIB=ON ..");
            flags[FlagId::gui_mode] = false;
        }
    }
    
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
    
    // Close GUI if it was opened
    if (flags[FlagId::gui_mode])
    {
        raylib_console_close();
    }

	return restart;
}

namespace po = boost::program_options;

void print_usage(const char* program_name, po::options_description &od)
{
    std::cerr << "Usage: " << program_name << " [options]\n" << od << std::endl;
}

int main(int argc, char *argv[])
{
    int rv = 0;
    bool ascii_art_enabled;
    bool gui_enabled;
    bool help;
    bool go_flag;

    po::options_description desc("Options");
    po::variables_map vm;
    desc.add_options()
		("ascii-art", po::value(&ascii_art_enabled)->default_value(false)->implicit_value(true), "Enable ASCII art images for rooms")
		("gui", po::value(&gui_enabled)->default_value(false)->implicit_value(true), "Enable GUI window for room images (requires raylib)")
		("help,h", po::value(&help)->default_value(false)->implicit_value(true), "Show this help message")
		("go", po::value(&go_flag)->default_value(false)->implicit_value(true), "Internal use flag - indicates we're the child process")
    ;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        vm.notify();

        if (help)
		{
			print_usage(argv[0], desc);
			return 0;
		}
    }
    catch (std::exception& e)
    {
        std::cerr << "Invalid options." << std::endl;
        return 2; // Causes the main program to abort.
    }

    // If no arguments, spawn child process with -go flag
    if (!go_flag)
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
            std::vector<std::string> args = { "--go" };
            if (ascii_art_enabled)
                args.push_back("--ascii-art");
            if (gui_enabled)
                args.push_back("--gui");
            boost::process::v2::process proc(ctx, path, args);
            status = proc.wait();
        }
    }
    else
    {
        // We're the child process (spawned with -go), run the game
        flags[FlagId::ascii_art] = ascii_art_enabled;
        flags[FlagId::gui_mode] = gui_enabled;
        rv = run_zork();
    }

    return rv;
}
