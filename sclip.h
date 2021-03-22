#ifndef SCLIP_H
#define SCLIP_H

#define MAX_TIME 3600
#define MAX_LINE 10
#define NUM_PORTS 2
#define PORT_NAME_LENGTH 32
#define BUFFER_SECONDS 30

/* main */
int jack_process(jack_nframes_t nframes, void *arg);
void jack_shutdown(void *arg);
void interface_loop();

/* buffer */
void initialize_buffer(unsigned int buffer_length_seconds);
void write_buffer();

/* globals */
extern jack_client_t *client;
extern jack_port_t *ports[NUM_PORTS];
extern float *audio_buffers[NUM_PORTS];
extern unsigned int audio_buffer_index;
extern unsigned int buffer_length_floats;
extern bool writing;

#endif /* SCLIP_H */
