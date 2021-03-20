#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <jack/jack.h>

#include "sclip.h"

void initialize_buffer(unsigned int buffer_length_seconds)
{
  unsigned int i;

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

  for (i = 0; i < NUM_PORTS; i++) {
    audio_buffer[i] = calloc(buffer_length_floats, sizeof(float));    
  }
}

void write_buffer() {
  FILE *file;
  unsigned int i, j;
  void *buffer_location;

  writing = true;

  file = fopen("output.pcm", "w");

  /* poor locality? */
  for (i = 0; i < buffer_length_floats; i++) {
    for (j = 0; j < NUM_PORTS; j++) {
      buffer_location = audio_buffer[j] + audio_buffer_index;
      fwrite(buffer_location, sizeof(float), 1, file);
    }

    audio_buffer_index += 1;
    if (audio_buffer_index >= buffer_length_floats) {
      audio_buffer_index = 0;
    }
  }

  fclose(file);

  writing = false;
}
