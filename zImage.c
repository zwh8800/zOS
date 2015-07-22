#include "zImage.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void* get_zimage_start()
{
	int diff;
	void* flash_end;
	extern unsigned __etext;
	extern unsigned __data_start__;
	extern unsigned __data_end__;

	diff = &__data_end__ - &__data_start__;
	flash_end = &__etext + diff;

	return flash_end;
}

int find_zImage(void* start, zImage* dest, size_t max)
{
	int n = 0;
	char* bp = start;	/* byte pointer */
	zImage_header* p = start;
	zImage* pImg = dest;

	while (sizeof(zImage) * n < max && (*(unsigned int*)p->magic) == ZIMAGE_MAGIC)
	{
		pImg->location = p;
		pImg->header.size = p->size;
		strncpy((char*)pImg->header.name, (char*)p->name, NAME_SIZ);
		++pImg;

		++n;
		bp += sizeof(zImage_header) + p->size;	/* 字节数 */
		p = (zImage_header*)bp;
	}
	return n;
}
