#ifndef Z_ZIMAGE_H
#define Z_ZIMAGE_H

#include <stdint.h>
#include <stddef.h>

#define ZIMAGE_MAGIC 0x676d697a		/* zimg 小端 */

typedef struct _zImage_header
{
#define NAME_SIZ 32
	int8_t magic[4];
	uint32_t size;
	int8_t name[NAME_SIZ];
} zImage_header;

typedef struct _zImage
{
	void* location;
	struct
	{
		uint32_t size;
		int8_t name[NAME_SIZ];
	} header;
} zImage;

void* get_zimage_start();
int find_zImage(void* start, zImage* dest, size_t max);

#endif
