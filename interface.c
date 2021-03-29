#include "sclip.h"

void parse_options(int argc, char *argv[], struct options *opt)
{
  for (argv++; *argv != NULL; argv++) {
    if ((*argv)[0] != '-') {
      break;
    }

    switch ((*argv)[1]) {
    case 'h':
      opt->help = true;
      break;
    case 'i':
      opt->interactive = true;
      break;
    }
  }
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
      write_buffer();
    }

    inputBuffer[0] = '\0';
  }
}

void print_help()
{
  fprintf(stderr, "usage: sclip [-i] [-h]\n");
  cleanup_and_exit();
}

void handle_sigrtmin()
{
  fprintf(stderr, "Recieved SIGUSR1\n");
  write_buffer();
}

void set_signal_handler()
{
  struct sigaction handler;
  handler.sa_flags = SA_RESTART;
  handler.sa_handler = handle_sigrtmin;
  sigaction(SIGUSR1, &handler, NULL);
  fprintf(stderr, "Set signal handler.\n");
}
