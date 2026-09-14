#ifndef TIMER_H
#define TIMER_H

struct timer;

struct timer * TIMER_create (void);
void TIMER_destroy (struct timer ** self);
double TIMER_elapsed (struct timer * self);

#endif // TIMER_H
