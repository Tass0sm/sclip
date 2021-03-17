#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <jack/jack.h>

#include "sclip.h"

void initialize_buffer(unsigned int buffer_length_seconds)
{
  if (buffer_length_seconds < 1) {
	printf("buffer time must be greater than 1 second\n");
	exit(EXIT_FAILURE);
  }

  if (buffer_length_seconds > MAX_TIME) {
	printf("buffer time must shorter than %d seconds\n", MAX_TIME);
	exit(EXIT_FAILURE);
  }

  /* Calculate number of floats to store the desired number of seconds using the
     sample rate from the JACK client in global variable. */
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
  unsigned int i;
  jack_default_audio_sample_t *in;
	
  /* just incase the port isn't registered yet */
  if (ports[0] == NULL) {
    return 0;
  }

  in = jack_port_get_buffer(ports[0], nframes);

  if (!recording) {
    for (i = 0; i < nframes; i++) {
      audio_buffer[audio_buffer_index] = in[i];
      audio_buffer_index = (audio_buffer_index + 1) % buffer_length_floats;
    }  
  }

  return 0;      
}

void interface_loop()
{
  char inputBuffer[MAX_LINE];
  ssize_t length = 1;

  while (length > 0) {
    printf("> ");
    fflush(stdout);
    
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

    if (inputBuffer[0] == 'w') {
      printf("Writing\n");
      fflush(stdout);
      recording = true;
    }
  }
}


int main(int argc, char *argv[]) {
  const char *client_name = "sclip";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;

  unsigned int i;
  char port_name[32];

  /* open a client connection to the JACK server */
  client = jack_client_open(client_name, options, &status, server_name);

  if (client == NULL) {
    fprintf(stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);

    if (status & JackServerFailed) {
      fprintf(stderr, "Unable to connect to JACK server\n");
    }

    exit(EXIT_FAILURE);
  }

  if (status & JackServerStarted) {
    fprintf(stderr, "JACK server started\n");
  }

  if (status & JackNameNotUnique) {
    client_name = jack_get_client_name(client);
    fprintf(stderr, "unique name `%s' assigned\n", client_name);
  }

  initialize_buffer(BUFFER_SECONDS);

  printf("audio buffer: %p\n", audio_buffer);

  /* tell the JACK server to call `process()' whenever
     there is work to be done. */
  jack_set_process_callback(client, process, 0);

  /* tell the JACK server to call `jack_shutdown()' if
     it ever shuts down, either entirely, or if it
     just decides to stop calling us. */
  jack_on_shutdown(client, jack_shutdown, 0);

  /* display the current sample rate. */
  printf("engine sample rate: %u\n", jack_get_sample_rate(client));

  /* create input ports */
  for (i = 0; i < NUM_PORTS; i++) {
    /* Create an input port */
    snprintf(port_name, 31, "in_%d", i + 1);
    printf("Registering port: %s\n", port_name);
    fflush(stdout);

    ports[i] = jack_port_register(client, port_name,
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsInput, 0);

    if (ports[i] == NULL) {
      fprintf(stderr, "no more JACK ports available\n");
      exit(1);
    }
  }


  /* Tell the JACK server that we are ready to roll.  Our
   * process() callback will start running now. */
  if (jack_activate(client)) {
    fprintf (stderr, "cannot activate client");
    exit(EXIT_FAILURE);
  }

  /* keep running until stopped by the user */
  interface_loop();
  
  /* this is never reached but if the program
     had some other way to exit besides being killed,
     they would be important to call. */

  free(audio_buffer);
  jack_client_close (client);
  exit(EXIT_SUCCESS);
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown(void *arg) {
  free(audio_buffer);
  exit(EXIT_FAILURE);
}
