// precomp.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _WIN32_WINNT 0x0601

#include <assert.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#if !defined(_ASSERT)
#define _ASSERT assert
#endif

#include <vector>
#include <array>
#include <optional>
#include <memory>

#include <map>
#include <string_view>

using namespace std::string_view_literals;

#include "defs.h"
