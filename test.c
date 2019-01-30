#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens                                // will separate the tokens on our command line
#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int main(void)
{


  char *token[MAX_NUM_ARGUMENTS];
  //while(*token!="exit"){
  char input[100];
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
while(cmd_str!="exit"){

  printf("\nmsh>");


      while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );




      int   token_count = 0;
      char *arg_ptr;
      char *working_str  = strdup( cmd_str );
      char *working_root = working_str;
      while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
                (token_count<MAX_NUM_ARGUMENTS))
      {
        token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
        if( strlen( token[token_count] ) == 0 )
        {
          token[token_count] = NULL;
        }
          token_count++;
      }

      int token_index  = 0;
      for( token_index = 0; token_index < token_count; token_index ++ )
      {
        //printf("token[%d] = %s\n", token_index, token[token_index] );
      }
  free( working_root );



  pid_t pid=fork();
  if (pid==0)
  {
    //printf("%s",token[1]);
  exec("/bin/ls",token[0],token[1], NULL);
  pid_t pid=fork();
  printf("\nAli Abdel Aziz");

   }
}
exit(0);
}
