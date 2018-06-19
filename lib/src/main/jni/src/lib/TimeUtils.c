/*
 * TimeUtils.cpp
 *
 *  Created on: 2013年9月13日
 *      Author: Yichou
 */
#include <time.h>


//namespace mrpoid {

long uptimems()
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (now.tv_sec) * 1000 + (now.tv_nsec) / 1000000;
}

const struct timespec *ms2timespec(long ms, struct timespec *in)
{
	in->tv_sec = ms/1000;
	in->tv_nsec = (ms%1000)*1000000;
	return in;
}

//}


