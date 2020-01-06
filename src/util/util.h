#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>
#include "mem.h"

unsigned long long get_epoch_milis();

bool in_str_array(const char *needle, const char haystack[6][12], size_t size);

char *int_to_string(const int i);

int is_hexadecimal(int x);

int *sort_int_array_asc(int *arr, size_t size);

int url_decode(const char *str, char *decoded);

#endif