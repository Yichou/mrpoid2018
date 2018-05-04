/*
 * TimeUtils.h
 *
 *  Created on: 2013年9月13日
 *      Author: Yichou
 */

#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

//namespace mrpoid {

long uptimems();

const struct timespec *ms2timespec(long ms, struct timespec *in);

//}


#endif /* TIMEUTILS_H_ */
