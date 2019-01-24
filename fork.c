#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>

int main(void)
{
  pid_t pid=fork();

  if(pid == -1)
  {
    perror("fork failed");
    exit(EXIT_FAILURE);
  }
  else if(pid==0){
  printf("Hello from the child process!\n");
  fflush(NULL);
  exit(EXIT_SUCCESS);
}
else{
int status;
(void)waitpid(pid,&status,0);
printf("HELLO FROM THE PARENT process !");
fflush(NULL);
}
return EXIT_SUCCESS;
}
