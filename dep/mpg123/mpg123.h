/*
	mpg123_msvc: MPEG Audio Decoder library wrapper header for MS VC++ 2005

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	initially written by Patrick Dehne and Thomas Orgis.
*/
#ifndef MPG123_MSVC_H
#define MPG123_MSVC_H

#ifdef _MSC_VER
#include <tchar.h>
#include <stdlib.h>
#include <sys/types.h>
typedef long ssize_t;
#endif

// Needed for Visual Studio versions before VS 2010.
#if (_MSC_VER < 1600)
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif

#ifdef _MSC_VER
#define PRIiMAX "I64i"
typedef __int64 intmax_t;
// ftell returns long, _ftelli64 returns __int64
// off_t is long, not __int64, use ftell
#define ftello ftell
#endif

#define MPG123_NO_CONFIGURE
#include "mpg123.h.in" /* Yes, .h.in; we include the configure template! */


#endif