#include "util.h"

unsigned long long get_epoch_milis()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;

    return millisecondsSinceEpoch;
}

bool in_str_array(const char *needle, const char haystack[6][12], size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (strcmp(needle, haystack[i]) == 0) return true;
    }
    return false;
}

char *int_to_string(const int i)
{
    const int queueSize_str_length = i > 0 ? (int)(ceil(log10(i)) + 1) : 2;
    char *queueSize_str = (char *) mem_alloc(queueSize_str_length + 1);
    sprintf(queueSize_str, "%d", i);
    return queueSize_str;
}

inline int is_hexadecimal(int c)
{
	return	(c >= '0' && c <= '9')	||
		    (c >= 'a' && c <= 'f')	||
		    (c >= 'A' && c <= 'F');
}

static int sort_cmp_func (const void *a, const void *b) {
   return (*(int*)a - *(int*)b);
}

int *sort_int_array_asc(int *arr, size_t size)
{
	qsort(arr, size, sizeof(int), sort_cmp_func);
	return arr;
}

int url_decode(const char *str, char *decoded)
{
	char *output;
	const char *end = str + strlen(str);
	int c;
    
	for (output = decoded; str <= end; output++) {
		c = *str++;
		if (c == '+') c = ' ';
		else if (c == '%' && 
				(!is_hexadecimal(*str++)	||
				!is_hexadecimal(*str++)	||
				!sscanf(str - 2, "%2x", &c)))
			return -1;
 
		if (decoded) *output = c;
	}
 
	return output - decoded;
}