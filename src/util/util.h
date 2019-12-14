#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>
#include "mem.h"

unsigned long long getEpochMilis();

bool in_str_array(const char *needle, const char haystack[6][12], size_t size);

char *intToString(const int i);

#endif