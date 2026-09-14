#include "timer.h"    // TIMER_*, struct timer
#include <time.h>     // struct timespec, clock_gettime, CLOCK_MONOTONIC
#include <stdio.h>    // fprintf, stderr
#include <stdlib.h>   // calloc, free


// exact same definition as `struct timespec`, but opaque
struct timer {
    double tv_sec;
    double tv_nsec;
};


struct timer * TIMER_create (void) {
    struct timer * self = calloc(1, sizeof(struct timer));
    if (self == nullptr) {
        fprintf(stderr, "ERROR %d: encountered error allocating dynamic memory for timer\n", __LINE__);
        return nullptr;
    }
    struct timespec now = {0};
    clock_gettime(CLOCK_MONOTONIC, &now);
    self->tv_nsec = now.tv_nsec;
    self->tv_sec = now.tv_sec;
    return self;
}


void TIMER_destroy (struct timer ** self) {
    if (self == nullptr) return;
    free(*self);
    *self = nullptr;
}


double TIMER_elapsed (struct timer * self) {
    struct timespec now = {0};
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_nsec - self->tv_nsec) / 1e9 + (now.tv_sec - self->tv_sec);
}
