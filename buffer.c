#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <jack/jack.h>

#include "sclip.h"

void initialize_buffer(unsigned int buffer_length_seconds)
{
  unsigned int port;

  if (buffer_length_seconds < 1) {
	fprintf(stderr, "buffer time must be greater than 1 second\n");
	exit(EXIT_FAILURE);
  }

  if (buffer_length_seconds > MAX_TIME) {
	fprintf(stderr, "buffer time must shorter than %d seconds\n", MAX_TIME);
	exit(EXIT_FAILURE);
  }

  /* Calculate number of floats to store the desired number of seconds using the
     sample rate from the JACK client in global variable. */
  buffer_length_floats = buffer_length_seconds * jack_get_sample_rate(client);

  for (port = 0; port < NUM_PORTS; port++) {
    audio_buffers[port] = calloc(buffer_length_floats, sizeof(float));
    printf("%p\n", (void *) audio_buffers[port]);
  }
}

void write_buffer() {
  FILE *file;
  unsigned int i, port;
  void *buffer_location;

  writing = true;

  file = fopen("output.pcm", "w");

  /* poor locality? */
  for (i = 0; i < buffer_length_floats; i++) {
    for (port = 0; port < 1; port++) {
      buffer_location = audio_buffers[port] + audio_buffer_index;
      fwrite(buffer_location, sizeof(char), 4, file);
    }

    audio_buffer_index += 1;
    if (audio_buffer_index >= buffer_length_floats) {
      audio_buffer_index = 0;
    }
  }

  fclose(file);

  writing = false;
}
