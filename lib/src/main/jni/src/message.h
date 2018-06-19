/*
 * Message.h
 *
 *  Created on: 2013年9月13日
 *      Author: Yichou
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_


struct Message {
	int what;
	int arg0;
	int arg1;
	int arg2;
	void *expand;
};

typedef struct Message T_MSG;
typedef struct Message *PT_MSG;


#endif /* MESSAGE_H_ */
