/*
 * Timer.cpp
 *
 *  Created on: Feb 8, 2009
 *      Author: kupcu
 */
#include "Timer.h"

#ifdef TIMER
#include "CashException.h"
#include <vector>
#include <sys/time.h>
#include <boost/thread/tss.hpp>

struct BrownieTimer {
    vector<timeval> timers;
};
// use thread-local storage
boost::thread_specific_ptr<BrownieTimer> localTimer;

vector<timeval>& getTimer() {
    if (localTimer.get() == 0)
        localTimer.reset(new BrownieTimer);
    return localTimer->timers;
}

void startTimer() {
	timeval t;
	gettimeofday(&t, NULL);
	getTimer().push_back(t);
}

/* Determine the difference, in milliseconds, between two struct timevals. */
#define TV_DIFF_MS(a, b) \
    (((b).tv_sec - (a).tv_sec) * 1000 + ((b).tv_usec - (a).tv_usec) / 1000)
/* Determine the difference, in microseconds, between two struct timevals. */
#define TV_DIFF_US(a, b) \
    (((b).tv_sec - (a).tv_sec) * 1000000 + ((b).tv_usec - (a).tv_usec))

double printTimer(const string &msg) {
	long long elapsed = elapsedTime();
	cout << msg << " [" << elapsed/1000.0 << " ms]" << endl;
	return elapsed/1000.0;
}

double printTimer(const int index, const string &msg) {
	long long elapsed = elapsedTime();
	cout << index << ":" << msg << " [" << elapsed/1000.0 << " ms]" << endl;
	return elapsed/1000.0;
}

long long elapsedTime() {
	timeval now;
	gettimeofday(&now, NULL);
	if (getTimer().size() == 0)
		throw CashException(CashException::CE_TIMER_ERROR, 
							"[Timer::printTimer] No running timer found. "
							"Maybe you forgot to start one?");
	long long elapsed = TV_DIFF_US(getTimer().back(), now);
	getTimer().pop_back();
	return elapsed;
}

#endif
