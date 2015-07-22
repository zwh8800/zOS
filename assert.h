#ifndef Z_ASSERT_H
#define Z_ASSERT_H

#include "kdebug.h"
#include "assert.h"

inline static void _z_assert(int bool_val, const char* file, int line, const char* func);
#define z_assert(bool_val) _z_assert(bool_val, __FILE__, __LINE__, __func__)

inline static void _z_failed(const char* file, int line, const char* func);
#define z_failed() _z_failed(__FILE__, __LINE__, __func__)


inline static
void _z_assert(int bool_val, const char* file, int line, const char* func)
{
	if (!bool_val)
	{
		_z_failed(file, line, func);
	}
}

inline static 
void _z_failed(const char* file, int line, const char* func)
{
	/* kernel panic */
	kprintf("%s:%d[%s]: kernel panic\r\n", file, line, func);
	while (1);
}


#endif 
