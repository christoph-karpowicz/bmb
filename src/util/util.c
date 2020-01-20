#include "util.h"

/**
 * get_epoch_milis - calculates milliseconds passed since epoch time. 
 * 
 * RETURNS:
 * milliseconds since epoch
 */
unsigned long long get_epoch_milis()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;

    return millisecondsSinceEpoch;
}

/**
 * in_str_array - checks if a string array includes a given string. 
 * @needle: searched string
 * @haystack: string array
 * @size: actual string array length
 * 
 * RETURNS:
 * does the array include the searched string
 */
bool in_str_array(const char *needle, const char haystack[6][12], size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (strcmp(needle, haystack[i]) == 0) return true;
    }
    return false;
}

/**
 * int_to_string - converts an integer to a string. 
 * @i: integer being converted
 * 
 * RETURNS:
 * dynamically allocated string result
 */
char *int_to_string(const int i)
{
    const int queueSize_str_length = i > 0 ? (int)(ceil(log10(i)) + 1) : 2;
    char *queueSize_str = (char *) mem_alloc(queueSize_str_length + 1);
    sprintf(queueSize_str, "%d", i);
    return queueSize_str;
}

/**
 * is_hexadecimal 
 * @c: single character
 * 
 * RETURNS:
 * can the given character be a part of a hexadecimal number
 */
inline bool is_hexadecimal(int c)
{
	return	(c >= '0' && c <= '9')	||
		    (c >= 'a' && c <= 'f')	||
		    (c >= 'A' && c <= 'F');
}

static int sort_cmp_func(const void *a, const void *b) {
   return (*(int*)a - *(int*)b);
}

/**
 * sort_int_array_asc - sort an integer array ascending. 
 * @arr: integer array being sorted
 * @size: array size
 * 
 * RETURNS:
 * sorted array
 */
int *sort_int_array_asc(int *arr, size_t size)
{
	qsort(arr, size, sizeof(int), sort_cmp_func);
	return arr;
}

/**
 * url_decode - decodes an URL encoded string. 
 * @str: string being decoded
 * @decoded: pointer to the result char array (local variable)
 * 
 * RETURNS:
 * decoded string length
 */
int url_decode(const char *str, char *decoded)
{
	char *output;
	const char *end = str + strlen(str);
	int c;
    
	for (output = decoded; str <= end; output++) {
		c = *str++;
		if (c == '+') c = ' ';
		// if a percent character is found, check if the following characters
		// can be hexadecimal and if yes, attempt to format them into an integer
		else if (c == '%' && 
				(!is_hexadecimal(*str++) ||
				!is_hexadecimal(*str++)	||
				!sscanf(str - 2, "%2x", &c)))
			return -1;
 
		if (decoded) *output = c;
	}
 
	return output - decoded;
}