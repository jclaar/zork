#include <iostream>
#include "funcs.h"
#include "rooms.h"
#include <vector>
#include <random>
#include <sstream>
#include <chrono>
#include <thread>
import ZGlobals;
import ZTell;



SIterator uppercase(SIterator src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return toupper(c); });
    return src;
}

