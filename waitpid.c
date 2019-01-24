#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
  pid_t child_pid = fork();
  int status;

  if(child_pid==0)
  {
    sleep(1);
    int *p=NULL;
    *p=1;
    exit(0);
  }
  waitpid(child_pid,&status,0);

  if(WIFSIGNALED(status))
  {
    printf("Child returned with status %d\n",WTERMSIG(status));
  }
  return 0;
}
