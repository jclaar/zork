// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H
#define _WIN32_WINNT 0x0601
#include <iostream>

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
#include <map>
#endif
