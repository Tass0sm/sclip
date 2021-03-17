#include <jack/jack.h>

#define MAX_TIME 3600
#define MAX_LINE 10
#define NUM_PORTS 1
#define BUFFER_SECONDS 10

int process(jack_nframes_t nframes, void *arg);

void initialize_buffer(unsigned int buffer_length_seconds);
void interface_loop();
void jack_shutdown(void *arg);

/* globals */
jack_client_t *client;
jack_port_t *ports[NUM_PORTS];

bool recording = false;
float * audio_buffer = NULL;
unsigned int audio_buffer_index = 0;
unsigned int buffer_length_floats = 0;
