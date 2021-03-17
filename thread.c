#include "sclip.h"
#include "thread.h"

float * audio_buffer = NULL;
unsigned int audio_buffer_index = 0;

void initialize_buffer(unsigned int buffer_length_seconds) {
  unsigned int buffer_length_floats;

  if (buffer_length_seconds < 1) {
	printf("buffer time must be greater than 1 second\n");
	exit(EXIT_FAILURE);
  }

  if (buffer_length_seconds > MAX_TIME) {
	printf("buffer time must shorter than %d seconds\n", MAX_TIME);
	exit(EXIT_FAILURE);
  }

  /* Get sample rate from JACK client in global variable. */
  buffer_length_floats = buffer_length_seconds * jack_get_sample_rate(client);

  audio_buffer = calloc(buffer_length_floats, sizeof(float));
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int process(jack_nframes_t nframes, void *arg) {
  /*
  jack_default_audio_sample_t *in, *out;
	
  in = jack_port_get_buffer(input_port, nframes);
  out = jack_port_get_buffer(output_port, nframes);

  memcpy(out, in, sizeof (jack_default_audio_sample_t) * nframes);
  */
  return 0;      
}
