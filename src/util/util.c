#include "util.h"

unsigned long long getEpochMilis()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;

    return millisecondsSinceEpoch;
}

char *intToString(const int i)
{
    const int queueSize_str_length = i > 0 ? (int)(ceil(log10(i)) + 1) : 2;
    char *queueSize_str = (char *) malloc(queueSize_str_length + 1);
    sprintf(queueSize_str, "%d", i);
    return queueSize_str;
}