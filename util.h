#pragma once

#include <assert.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#if !defined(_ASSERT)
#define _ASSERT assert
#endif

#include "defs.h"
#include "rooms.h"
const HackP& get_demon(const char* id);

