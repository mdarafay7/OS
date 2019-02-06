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


int main(void) {

  FILE * fp;
  int cd = 1;
  char * token[MAX_NUM_ARGUMENTS];
  int counter = 0;
  int pid_store[100];
  char input[100];




  char * cmd_str = (char * ) malloc(MAX_COMMAND_SIZE);
  fp = fopen("bash_history.txt", "w+");
  while (cmd_str != "exit") {

    printf("msh> ");

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
    int i;
    for (i = 0; i < token_count; i++) {
    //printf("%s",token[i]);
    fprintf(fp, token[i]);
    fprintf(fp, " ");
    }
    fprintf(fp,"\n");
    fprintf(fp,'\0');

    cd = strcmp(token[0], "cd");
    if (!cd) {
      chdir(token[1]);
      continue;
    }
    if (!strcmp(token[0], "history")) {
      printf("Alsi Abdel Aziz");
      char str[999];
      if (fp) {
        int n = 0;
        while (fscanf(fp, "%s", str) != EOF) {
          printf("%d: %s", n, str);
          n++;
        }
        continue;
      }
    }


    if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit")) {
      exit(0);
    }
    if (!strcmp(token[0], "listpids")) {
      int x;
      for (x = 0; x<counter; x++) {

        printf("%d) %d\n",x, pid_store[x]);
      }
      continue;
    }

    while (!strcmp(token[0], "\n") || !strcmp(token[0], "\n")) {
      continue;
    }

    int pid = fork();
    fflush(stdout);



    if(pid==0){
      execvp(token[0], token);
      execvp("/usr/local/bin ", token);
      execvp("/usr/bin", token);
      execvp("/bin", token);
      printf("Command Not Found");
      exit(EXIT_SUCCESS);


    }

    int status;
    waitpid(pid, &status, 0 );
    counter++;
    printf("%d",counter);
    pid_store[counter-1]=pid;



    }
  fclose(fp);




}
