#pragma once
#pragma once

#include <vector>
#include <string>

typedef std::vector<const char *> tofmsg;
typedef std::vector<tofmsg> tofmsgs;

const tofmsgs &cyclops_melee();
const tofmsgs &knife_melee();
const tofmsgs &sword_melee();
const tofmsgs &thief_melee();
const tofmsgs &troll_melee();
