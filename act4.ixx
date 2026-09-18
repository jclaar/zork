module;
#include "defs.h"

export module Zork:Act4;
import std;
import ZFuncs;
import ZDefs;
import :Speech;
import :Object;
import :Rooms;

export std::string pw(SIterator unm, SIterator key);

ERAPPLIC(answer);
ERAPPLIC(incant);
//RAPPLIC_DEF(inquisitor, Iterator<ParseContV>, Iterator<ParseContV>());

ERAPPLIC(stats);
ERAPPLIC(stay);
ERAPPLIC(turnto);