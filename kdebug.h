#ifndef K_DEBUG_H
#define K_DEBUG_H

int kprintf(const char* fmt, ...)
__attribute__ ((__format__ (__printf__, 1, 2)));

void dump_scb();
void dump_ufsr();

#endif
