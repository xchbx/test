#ifndef __STDAFX_H__
#define __STDAFX_H__
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//



#include <stdio.h>

#include <limits.h>

#include <stdlib.h>
typedef unsigned char uint8_t;

//typedef unsigned char uint8_t;
//typedef char int8_t;
//typedef unsigned short uint16_t;
//typedef short int16_t;
//typedef unsigned int uint32_t;
//typedef int int32_t;
//typedef unsigned long long uint64_t;
//typedef long long int64_t;

#define av_unused
#define av_const	const

//#define BITSTREAM_READER_LE 1

#define AV_RL32(x)                           \
	((((const uint8_t*)(x))[3] << 24) |         \
	(((const uint8_t*)(x))[2] << 16) |         \
	(((const uint8_t*)(x))[1] <<  8) |         \
	((const uint8_t*)(x))[0])

#define AV_RB32(x)                                \
	(((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
	(((const uint8_t*)(x))[1] << 16) |    \
	(((const uint8_t*)(x))[2] <<  8) |    \
	((const uint8_t*)(x))[3])

#ifndef NEG_SSR32
#   define NEG_SSR32(a,s) (((int32_t)(a))>>(32-(s)))
#endif

#ifndef NEG_USR32
#   define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif

#define AVERROR_INVALIDDATA        (-0x3ebbb1b7) ///< Invalid data found when processing input


#   define AV_WL32(p, d) do {                   \
	((uint8_t*)(p))[0] = (d);               \
	((uint8_t*)(p))[1] = (d)>>8;            \
	((uint8_t*)(p))[2] = (d)>>16;           \
	((uint8_t*)(p))[3] = (d)>>24;           \
} while(0)

#   define AV_WB32(p, d) do {                   \
	((uint8_t*)(p))[3] = (d);               \
	((uint8_t*)(p))[2] = (d)>>8;            \
	((uint8_t*)(p))[1] = (d)>>16;           \
	((uint8_t*)(p))[0] = (d)>>24;           \
} while(0)

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

#define av_always_inline	inline

static inline av_const int sign_extend(int val, unsigned bits)
{
	unsigned shift = 8 * sizeof(int) - bits;
	union { unsigned u; int s; } v = { (unsigned) val << shift };
	return v.s >> shift;
}

static inline av_const unsigned zero_extend(unsigned val, unsigned bits)
{
	return (val << ((8 * sizeof(int)) - bits)) >> ((8 * sizeof(int)) - bits);
}
// TODO: 在此处引用程序需要的其他头文件
//
#endif
