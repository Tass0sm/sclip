#ifndef SCLIP_H
#define SCLIP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <jack/jack.h>

#define MAX_TIME 3600
#define MAX_LINE 10
#define NUM_PORTS 2
#define PORT_NAME_LENGTH 32
#define BUFFER_SECONDS 30

struct options {
  bool interactive;
  bool help;
};

/* main */
int jack_process(jack_nframes_t nframes, void *arg);
void jack_shutdown(void *arg);
void cleanup_and_exit();

/* buffer */
void initialize_buffer(unsigned int buffer_length_seconds);
void write_buffer();

/* interface */
void parse_options(int argc, char *argv[], struct options *opt);
void interface_loop();
void print_help();
void set_signal_handler();

/* globals */
extern jack_client_t *client;
extern jack_port_t *ports[NUM_PORTS];
extern float *audio_buffers[NUM_PORTS];
extern unsigned int audio_buffer_index;
extern unsigned int buffer_length_floats;
extern bool writing;

extern bool interactive;

#endif /* SCLIP_H */
