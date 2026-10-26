#pragma once
#include "defs.h"
#include "parser.h"

extern std::vector<QuestionP> qvec;

bool eg_infested(const RoomP& r);

RAPPLIC(answer);
RAPPLIC(follow);
RAPPLIC(incant);
RAPPLIC_DEF(inquisitor, Iterator<ParseContV>, Iterator<ParseContV>());

RAPPLIC(start_end);
RAPPLIC(stats);
RAPPLIC(stay);
RAPPLIC(turnto);