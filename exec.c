#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

void lame()
{

}

int main(void)
{
  printf("o world");
  pid_t child_pid=fork();
  int status;

/*  if(child_pid==0)
  {
    exec("top");
    printf("henlo der");
    exit(0);
  } */
  if (child_pid == 0)
 {
   fork();
  fork();



 }
  waitpid(child_pid,&status,0);

  return 0;
}
