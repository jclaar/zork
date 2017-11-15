// mdlfun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <memory>
#include <time.h>
#include "zstring.h"
#include "object.h"
#include "globals.h"
#include "funcs.h"
#include "util.h"
#include "objfns.h"
#include "parser.h"
#include "act1.h"
#include "act2.h"
#include "act4.h"
#include "makstr.h"
#include "dung.h"
#include "rooms.h"

std::string pw(Iterator<std::string> unm, Iterator<std::string> key);
std::string username();

int main()
{
    std::cerr << "Hello : " << pw(username(), std::string("HELLO")) << std::endl;
	init_dung();

    save_it(true);

    rdcom();

    return 0;
}

