#include "util.h"
#include <stdio.h>

int print(const char *format,...)
{
#ifdef DEBUG
	return printf(format);
#else
	return 0;
#endif
}

long microSec(struct timeval *start,struct timeval *end)
{
	long secs_used=(end->tv_sec - start->tv_sec);
	return (((secs_used*1000000) + end->tv_usec) - (start->tv_usec));
}
