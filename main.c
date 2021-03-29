#include "sclip.h"

jack_client_t *client;
jack_port_t *ports[NUM_PORTS];
float *audio_buffers[NUM_PORTS];
unsigned int audio_buffer_index;
unsigned int buffer_length_floats;
bool writing = false;

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client copies data from its input port to a ring buffer. It will exit
 * when stopped by the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int jack_process(jack_nframes_t nframes, void *arg)
{
  unsigned int port, frame, previous_index;
  float *current_buffer;
  jack_default_audio_sample_t *in; /* float pointer */

  /* Unless writing... */
  if (writing) {
    return 0;
  }

  previous_index = audio_buffer_index;

  /* ...write the data to the corresponding buffer for every port. */
  for (port = 0; port < NUM_PORTS; port++) {
    /* if the port isn't registered */
    if (ports[port] == NULL) {
      break;
    }

    in = (jack_default_audio_sample_t *)
      jack_port_get_buffer(ports[port], nframes);

    if (!in) {
      fprintf(stderr, "bad buffer!\n");
      break;
	}

    current_buffer = audio_buffers[port];

    audio_buffer_index = previous_index;
    for (frame = 0; frame < nframes; frame++) {
      current_buffer[audio_buffer_index] = in[frame];
      audio_buffer_index += 1;
      if (audio_buffer_index >= buffer_length_floats) {
        audio_buffer_index = 0;
      }
    }
  }

  return 0;      
}


/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown(void *arg)
{
  cleanup_and_exit();
}

int main(int argc, char *argv[])
{
  const char *client_name = "sclip";
  const char *server_name = NULL;
  jack_options_t options = JackNullOption;
  jack_status_t status;

  unsigned int i;
  char port_name[PORT_NAME_LENGTH];

  struct options opt = {
    false, false
  };

  parse_options(argc, argv, &opt);

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

  /* call `jack_process()' when there is work to be done. */
  jack_set_process_callback(client, jack_process, NULL);

  /* call `jack_shutdown()' if the server shuts down or stops calling us. */
  jack_on_shutdown(client, jack_shutdown, NULL);

  /* create input ports */
  for (i = 0; i < NUM_PORTS; i++) {
    snprintf(port_name, PORT_NAME_LENGTH - 1, "in_%d", i + 1);
    ports[i] = jack_port_register(client, port_name,
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsInput, 0);

    if (ports[i] == NULL) {
      fprintf(stderr, "could not register JACK port\n");
      exit(EXIT_FAILURE);
    }
  }

  if (jack_activate(client)) {
    fprintf (stderr, "cannot activate client");
    exit(EXIT_FAILURE);
  }

  if (opt.help) {
    print_help();
  }

  if (opt.interactive) {
    interface_loop();
  } else {
    set_signal_handler();
    sleep(-1);
  }

  cleanup_and_exit();
}

void cleanup_and_exit()
{
  int i;

  for (i = 0; i < NUM_PORTS; i++) {
    free(audio_buffers[i]);
  }

  jack_client_close(client);
  exit(EXIT_SUCCESS);
}
