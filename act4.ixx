module;
#include "defs.h"

export module Zork:Act4;
import std;
import :Speech;
import :Object;
import :Rooms;
import ZFuncs;

export std::string pw(SIterator unm, SIterator key);

ERAPPLIC(answer);
RAPPLIC(incant);
//RAPPLIC_DEF(inquisitor, Iterator<ParseContV>, Iterator<ParseContV>());

RAPPLIC(stats);
RAPPLIC(stay);
RAPPLIC(turnto);