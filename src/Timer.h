/*
 * Timer.h
 *
 *  Created on: Feb 8, 2009
 *      Author: kupcu
 */

#include "Debug.h"

#include <iostream>
#include <stdlib.h>

#ifndef TIMER_H_
#define TIMER_H_

#ifdef TIMER
void startTimer();
double printTimer(const string &msg);
double printTimer(const int index, const string &msg);
long long elapsedTime();
#else
inline void startTimer() {}
inline double printTimer(const string &msg) { return 0; }
inline double printTimer(const int index, const string &msg) { return 0; }
inline long long elapsedTime() { return 0; }
#endif

#endif /* TIMER_H_ */
