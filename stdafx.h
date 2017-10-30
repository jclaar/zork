// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <iostream>

#include <assert.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#define NOMINMAX
#include <windows.h>
#endif

#if !defined(_ASSERT)
#define _ASSERT assert
#endif

template <typename T, size_t sz>
size_t ARRSIZE(const T(&)[sz])
{
    return sz;
}

