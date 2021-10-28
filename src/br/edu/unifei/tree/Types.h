/*
 * types.h
 *
 *  Created on: 24 de set de 2019
 *      Author: windows
 */

#ifndef INCLUDE_BR_EDU_UNIFEI_TREE_TYPES_H_
#define INCLUDE_BR_EDU_UNIFEI_TREE_TYPES_H_

typedef bool Bool;
typedef char Byte;
typedef char Char;
typedef double Double;
typedef float Float;
typedef int Int;
typedef short Short;
typedef unsigned int UInt;
typedef unsigned short UShort;

#if ULONG_MAX == 0xFFFFFFFF
//architecture 32 bits
#if defined(__GNUC__)
typedef long long Long;
typedef unsigned long long ULong;
#elif defined(_MSC_VER)
#define __int64 Long;
#define __uint64 ULong;
#else
#define __int64 Long;
#define __uint64 ULong;
#endif
#else
//architecture 64 bits
#if defined(__GNUC__)
typedef long Long;
typedef unsigned long ULong;
#elif defined(_MSC_VER)
#define __int64 Long;
#define __uint64 ULong;
#else
#define __int64 Long;
#define __uint64 ULong;
#endif
#endif

#endif /* INCLUDE_BR_EDU_UNIFEI_TREE_TYPES_H_ */
