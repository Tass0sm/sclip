#include "sclip.h"
#include "thread.h"

#define NUM_PORTS 2

jack_port_t *ports[NUM_PORTS];
jack_client_t *client;

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
    exit (1);
  }

  /* keep running until stopped by the user */

  sleep (-1);

  /* this is never reached but if the program
     had some other way to exit besides being killed,
     they would be important to call. */

  jack_client_close (client);
  exit (0);
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown(void *arg) {
  exit(EXIT_FAILURE);
}
