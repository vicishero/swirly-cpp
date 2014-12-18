/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <dbr/ash/util.h>

#include <limits.h>
#include <sys/time.h>

DBR_API int
dbr_int_dig(int i)
{
    if (i < 0)
        i = (i == INT_MIN) ? INT_MAX : -i;
    int d;
    if (i < 10000) { // 4
        if (i < 100) {
            if (i < 10) d = 1;
            else d = 2;
        } else {
            if (i < 1000) d = 3;
            else d = 4;
        }
    } else if (i < 100000000) { // 8
        if (i < 1000000) {
            if (i < 100000) d = 5;
            else d = 6;
        } else {
            if (i < 10000000) d = 7;
            else d = 8;
        }
    } else if (i < 1000000000) d = 9;
    else d = 10;
    return d;
}

DBR_API int
dbr_long_dig(long l)
{
    if (l < 0)
        l = (l == LONG_MIN) ? LONG_MAX : -l;
    int d;
    if (l < 10000L) { // 4
        if (l < 100L) {
            if (l < 10L) d = 1;
            else d = 2;
        } else {
            if (l < 1000L) d = 3;
            else d = 4;
        }
    } else if (l < 100000000L) { // 8
        if (l < 1000000L) {
            if (l < 100000L) d = 5;
            else d = 6;
        } else {
            if (l < 10000000L) d = 7;
            else d = 8;
        }
    } else if (l < 1000000000000L) { // 12
        if (l < 10000000000L) {
            if (l < 1000000000L) d = 9;
            else d = 10;
        } else {
            if (l < 100000000000L) d = 11;
            else d = 12;
        }
    } else if (l < 10000000000000000L) { // 16
        if (l < 100000000000000L) {
            if (l < 10000000000000L) d = 13;
            else d = 14;
        } else {
            if (l < 1000000000000000L) d = 15;
            else d = 16;
        }
    } else if (l < 100000000000000000L) d = 17;
    else if (l < 1000000000000000000L) d = 18;
    else d = 19;
    return d;
}

DBR_API long
dbr_millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long ms = tv.tv_sec * 1000L;
    ms += (tv.tv_usec + 500L) / 1000L;
    return ms;
}
