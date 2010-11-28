/*
 * timeGetTime.cpp
 *
 * Andrew Armenia
 * 11-28-2010
 *
 * Emulate timeGetTime( ) on Unix-like systems.
 */

#include "timegettime.h"
#include <sys/time.h>
#include <stdexcept>
#include <assert.h>

struct timeval tv_initial = {0, 0};

uint32_t timeGetTime(void) {
    struct timeval tv;
    int64_t useconds;

    if (tv_initial.tv_sec == 0) {
        if (gettimeofday(&tv_initial, NULL) != 0) {
            throw std::runtime_error("gettimeofday( ) failed");
        }
    }

    if (gettimeofday(&tv, NULL) != 0) {
        throw std::runtime_error("gettimeofday( ) failed");
    }

    /* subtract tv_initial from tv */
    useconds = tv.seconds - tv_initial.seconds;
    useconds *= 1000000;
    useconds += tv.tv_usec;
    useconds -= tv_initial.tv_usec;

    assert(useconds >= 0);

    return useconds / 1000;
}
