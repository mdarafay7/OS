#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define _GNU_SOURCE
#define CMD_LOG "bash_history"
#define WHITESPACE " \t\n" // We want to split our command line up into tokens                                // will separate the tokens on our command line
#define MAX_COMMAND_SIZE 255 // The maximum command-line size
#define MAX_NUM_ARGUMENTS 5 // Mav shell only supports five arguments



static void handle_signal(int sig)
{
  pid_t pid;
  int status;
  switch(sig)
  {
    case SIGINT:
      break;

    case SIGTSTP:
      pid = waitpid(-1, &status, WNOHANG);
      break;
  }
}

int main(void) {
  int signal_check=0;
  struct sigaction act;
  memset(&act,'\0',sizeof(act));
  act.sa_handler=&handle_signal;
  if (sigaction(SIGINT , &act,NULL) < 0) {
      signal_check=1;
      perror ("sigaction: ");
      return 1;
    }
  if (sigaction(SIGTSTP, &act,NULL) < 0) {
  signal_check=1;
  perror ("sigaction: ");
  return 1;
  }

  FILE * fp;
  int cd = 1;
  char * token[MAX_NUM_ARGUMENTS];
  char *history[50][50];
  int counter = 0;
  int pid_store[100];
  char input[100];
  size_t len=0;
  ssize_t read;
  int i,x;


  char * cmd_str = (char * ) malloc(MAX_COMMAND_SIZE);

  while (cmd_str != "exit") {

    printf("msh> ");
    signal_check=0;
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while ( !fgets(cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char * token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char * arg_ptr;

    char * working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char * working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep( & working_str, WHITESPACE)) != NULL) &&
      (token_count < MAX_NUM_ARGUMENTS)) {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0) {
        token[token_count] = NULL;
      }
      token_count++;
    }


    if(token[0]==NULL)
    {
      continue;
    }
    if(!strncmp(token[0],"!",1))
      {
        x=0;
        char cmd[2];
        strncpy(cmd,*token+1,2);
        int pos=atoi(cmd);
        while(history[pos][x])
        {
          token[x]=history[pos][x];
          x++;
        }


      }


for (i = 0; i < token_count-1; i++) {
  history[counter][i]=token[i];
}



    //fprintf(fp,'\0');

    cd = strcmp(token[0], "cd");
    if (!cd) {
      chdir(token[1]);
      continue;
    }
    if (!strcmp(token[0], "history")) {
      int y=1;
      for (x = 0; x<=counter; x++) {

        printf("%d) %s",x, history[x][0]);
        while(history[x][y])
        {
          printf(" %s",history[x][y]);
          y++;
        }
        printf("\n");
      }

        continue;
      }



    if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit")) {
      exit(0);
    }
    if (!strcmp(token[0], "listpids")) {
      int x;
      for (x = 0; x<counter; x++) {

        printf("%d) %d\n",x, pid_store[x]);
      }

    }

    while (!strcmp(token[0], "\n") || !strcmp(token[0], "\n")) {
      continue;
    }

    int pid = fork();
    fflush(stdout);



    if(pid==0){

      fflush(stdout);
      execvp(token[0], token);
      execvp("/usr/local/bin ", token);
      execvp("/usr/bin", token);
      execvp("/bin", token);
      printf("Command Not Found");
      exit(EXIT_SUCCESS);


    }

    if(!signal_check)
    { int status;
      waitpid(pid, &status, 0 );
      if(counter>14)
      {
        for(i=0;i<counter;i++)
        {
          pid_store[i]=pid_store[i+1];
        }
        pid_store[14]=pid;

      }
      else{
        pid_store[counter]=pid;
        counter++;
      }
      fflush( NULL );
    }

    else
    {
      int status;
      pause();
      if(counter>14)
      {
        for(i=0;i<counter;i++)
        {
          pid_store[i]=pid_store[i+1];
        }
        pid_store[14]=pid;
        counter--;
      }
      else{
        pid_store[counter]=pid;
        counter++;
      }

      fflush( NULL );

    }

}


    }
  //fclose(fp);
