#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static void handle_signal (int sig )
{

}

int main (int argc, char *argv[])
{
  struct sigaction act;

  /*
    Zero out the sigaction struct
  */
  memset (&act, '\0', sizeof(act));

  /*
    Set the handler to use the function handle_signal()
  */
  act.sa_handler = &handle_signal;

  /*
    Install the handler and check the return value.
  */
  if (sigaction(SIGTSTP , &act,NULL) < 0) {
    perror ("sigaction: ");
    return 1;
  }

  while (1) {
    sleep (1);
  }

  return 0;
}
