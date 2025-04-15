#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t strlen(const char* str);
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);

#endif // STRING_H 