#ifndef THREAD_H
#define THREAD_H

#include <stdio.h>
#include <stdlib.h>

#include <jack/jack.h>

#define MAX_TIME 3600

int process(jack_nframes_t nframes, void *arg);

#endif /* THREAD_H */
