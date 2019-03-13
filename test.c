/*
  Name: Abdul Rafay Mohammed
  ID: 1001331625
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define _GNU_SOURCE
#define CMD_LOG "bash_history"
#define WHITESPACE " \t\n" // We want to split our command line up into tokens
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports 10 arguments

//This is the Signal Hangler which accepts the Standard SIGNAL's value as a parameter
//This is the Signal Hangler which accepts the Standard SIGNAL's value as a parameter
static void handle_signal(int sig) {
  pid_t pid;
  int status;
  switch (sig) {
  case SIGINT:
    break;

  case SIGTSTP:
    kill(getppid(), SIGTSTP);
    break;

  case SIGCHLD:
    waitpid(-1, & status, WNOHANG);
    break;

  }
}

int main(void) {
  int signal_check = 0;
  struct sigaction act;
  memset( & act, '\0', sizeof(act));
  act.sa_handler = & handle_signal;
  /*
    Install the handler for SIGINT and SIGTSTP and check the
    return value.
  */
  if (sigaction(SIGINT, & act, NULL) < 0) {
    signal_check = 1;
    perror("sigaction: ");
    return 1;
  }
  if (sigaction(SIGTSTP, & act, NULL) < 0) {
    signal_check = 1;
    perror("sigaction: ");
    printf("\n");
    return 1;
  }
  if (sigaction(SIGCHLD, & act, NULL) < 0) {
    signal_check = 1;
    perror("sigaction: ");
    return 1;
  }

  char * token[MAX_NUM_ARGUMENTS];
  char * history[50][50]; //2d array that will store history index in one dimension and cmd_str in the other dimension.
  int counter = 0;
  int history_counter = 0;
  int pid_store[100];
  int i, x, j, k, l;

  char * cmd_str = (char * ) malloc(MAX_COMMAND_SIZE);

  while (strcmp(cmd_str, "exit") || strcmp(cmd_str, "quit")) {

    printf("msh> ");
    signal_check = 0;
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));

    /* Parse input */

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char * arg_ptr;

    char * working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep( & working_str, WHITESPACE)) != NULL) &&
      (token_count < MAX_NUM_ARGUMENTS)) {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0) {
        token[token_count] = NULL;
      }
      token_count++;
    }

    if (token[0] == NULL) {
      continue;
    }

    if (!strncmp(token[0], "!", 1)) {
      x = 0;
      char cmd[2];
      strncpy(cmd, * token + 1, 2);
      int pos = atoi(cmd);
      while (history[pos][x] && (pos >= 0 && pos <= 15)) {
        token[x] = history[pos][x];
        x++;
      }

    }

    int bg = 0;
    //Background the last suspended process.
    if (!strcmp(token[0], "bg")) {
      kill(pid_store[counter--], SIGCONT);
    }

    //Fill the Command History array and reshuffle and maintain if it gets bigger than 50.
    for (i = 0; i < token_count - 1; i++) {
      if (history_counter > 49) {
        for (j = 0; j < history_counter; j++) {
          for (l = 0; l < 2; l++) {
            history[j][l] = history[j + 1][l];
          }
        }
        for (k = 0; k < token_count - 1; k++) {
          history[49][k] = token[k];
        }
        history_counter--;
      } else {
        history[history_counter][i] = token[i];
      }
    }

    //Check if command line input is 'cd', if so we will use chdir to change the directory as per what
    //token[1] contains.
    if (!strcmp(token[0], "cd")) {
      chdir(token[1]);
      history_counter++;
      continue;
    }
    if (!strcmp(token[0], "history")) {
      for (x = 0; x <= history_counter; x++) {

        printf("%d) %s", x, history[x][0]);
        int y = 1;
        while (history[x][y]) {
          printf(" %s", history[x][y]);
          y++;
        }
        printf("\n");
      }
      history_counter++;
      continue;
    }

    if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit")) {
      exit(0);
    }

    if (!strcmp(token[0], "listpids") || !strcmp(token[0], "showpids")) {
      int x;
      for (x = 0; x < counter; x++) {
        printf("%d) %d\n", x, pid_store[x]);
      }
      history_counter++;
      continue;

    }

    while (!strcmp(token[0], "\n") || !strcmp(token[0], "\n")) {
      continue;
    }
    //fork the process and proceed to exec as the CHILD process
    int pid = fork();
    fflush(stdout);

    if (pid == 0) {
      /*We are trying to exec() the forked (child) process, we will search in the respective
      Directories*/

      fflush(stdout);
      execvp(token[0], token);
      execvp("/usr/local/bin ", token);
      execvp("/usr/bin", token);
      execvp("/bin", token);
      printf("Command Not Found\n");
      exit(EXIT_SUCCESS);

    }
    //Check if ctrl-z or ctrl-c have been used and then proceed to
    //store the process_id's
    //If no signals have been passed by the user then we will choose to wait() instead of pause()
    //Because Child process will get suspended upon SIGTSTP and parent will still be waiting.
    //therefore we will look for a SIGNAL from the system if signal_check=1

    if (!signal_check) //If no signals have been passed by the user then we will choose to wait() instead of pause()
    {
      int status;
      waitpid(pid, & status, 0); //Suspend the execution of current process until the child changes state.
      if (counter > 14) {
        for (i = 0; i < counter; i++) {
          pid_store[i] = pid_store[i + 1];
        }
        pid_store[14] = pid;
        history_counter++;

      } else {
        pid_store[counter] = pid;
        counter++;
        history_counter++;
      }
      fflush(NULL);
    } else {
      pause(); // we are
      if (counter > 14) {
        for (i = 0; i < counter; i++) {
          pid_store[i] = pid_store[i + 1];
        }
        pid_store[14] = pid;
        history_counter++;
        counter--;
      } else {
        pid_store[counter] = pid;
        history_counter++;
        counter++;
      }

      fflush(NULL);

    }
  }

}
