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
#include <stdint.h>
#include <ctype.h>
#include <signal.h>
#define CMD_LOG "bash_history"
#define WHITESPACE " \t\n" // We want to split our command line up into tokens
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports 10 arguments
#define slash "/"


struct information
{
  int16_t bytes_per_sec;
  int8_t sec_per_clus;
  int16_t res_sec_count;
  int8_t num_fats;
  int32_t fat_size_32;
  uint32_t root_cluster;
  uint32_t first_data_sector;
  int32_t root_dir_sectors;
};


struct directory_entry
{
  char DIR_NAME[11];
  uint8_t DIR_Attr;
  uint8_t Unused1[8];
  uint16_t DIR_FirstClusterHigh;
  uint8_t Unused2[4];
  uint16_t DIR_FirstClusterLow;
  uint32_t DIR_FileSize;
};

int directory_filler;



int main(void) {
  struct information info1;
  struct directory_entry dir[200];
  char * token[MAX_NUM_ARGUMENTS];
  char * cmd_str = (char * ) malloc(MAX_COMMAND_SIZE);
  FILE *fp;
  while (strcmp(cmd_str, "exit") || strcmp(cmd_str, "quit")) {
    printf("mfs> ");
    int slasher=0,normal=0;
      while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
      char *token[MAX_NUM_ARGUMENTS];
      int   token_count = 0;
      char *arg_ptr;
      char *working_str  = strdup( cmd_str );
      char *working_root = working_str;
      if(strchr(working_str,'/') != NULL){
      slasher=1;
      arg_ptr=strsep(&working_str,WHITESPACE);
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;


      while ( ( (arg_ptr = strsep(&working_str, slash ) ) != NULL) &&
                (token_count<MAX_NUM_ARGUMENTS))
      {
        token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
        if( strlen( token[token_count] ) == 0 )
        {
          token[token_count] = NULL;
        }
          token_count++;
      }
    }

    else{
      normal=1;
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
    }
    fp=fopen("fat32.img","r+");
    if (token[0] == NULL) {
      continue;
    }
    if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit")) {
      exit(0);
    }
    while (!strcmp(token[0], "\n") || !strcmp(token[0], "\n")) {
      continue;
    }
    if(!strcmp(token[0],"open"))
    {

      fp=fopen(token[1],"r+");
      if(fp==NULL)
      {
        printf("mfs>Error: File system image not found.\n");
        continue;
      }
    }

    if(!strcmp(token[0],"close"))
    {
      if(fp)
      {
        fclose(fp);
        fp=NULL;
        continue;
      }
      else
      {
        printf("mfs>Error: File system not open.\n");
      }
    }
    if(!strcmp(token[0],"ls"))
    {
      int i;
      for(i=0;i<=16;i++)
      {
        if((dir[i].DIR_Attr==0x01||dir[i].DIR_Attr==0x10)||(dir[i].DIR_Attr==0x20))
        {
        printf("%s\t",dir[i].DIR_NAME);
        }
      }
    }

    if(!strcmp(token[0],"cd"))
    {
      int i,x=0;
      char expanded_name[12];
      memset( expanded_name, ' ', 12 );
      if(slasher==0)
      {
      for(i=0;i<200;i++)
      {
        char *token1 = strtok(token[1], "." );

        strncpy( expanded_name, token1, strlen( token1 ) );

        token1 = strtok( NULL, "." );

        if( token1 )
        {
          strncpy( (char*)(expanded_name+8), token1, strlen(token1 ) );
        }

        expanded_name[11] = '\0';
        int z;
        for( z = 0; z < 11; z++ )
        {
            expanded_name[z] = toupper( expanded_name[z] );
        }
        if(!strncmp( expanded_name,dir[i].DIR_NAME, 11 ))
        {

          int offset=((dir[i].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
          for(x=0;x<=16;x++)
          {
            fseek(fp,offset+(32*x),SEEK_SET);
            fread(&dir[x].DIR_NAME,11,1,fp);
            fread(&dir[x].DIR_Attr,1,1,fp);
            fseek(fp,8,SEEK_CUR);
            fread(&dir[x].DIR_FirstClusterHigh,2,1,fp);
            fseek(fp,4,SEEK_CUR);
            fread(&dir[x].DIR_FirstClusterLow,2,1,fp);
            fread(&dir[x].DIR_FileSize,4,1,fp);

          }

          printf("They matched\n");
          break;
        }

      }
    }
    else
    {
      printf("AAAAAAAAAAAA");
    }
      if(!x)
      {
        printf("\nNO SUCH FILE\n");
      }

    }
    if(!strcmp(token[0],"info"))
    {
      fseek(fp,11,SEEK_SET);
      fread(&info1.bytes_per_sec,2,1,fp);
      fseek(fp,13,SEEK_SET);
      fread(&info1.sec_per_clus,1,1,fp);
      fseek(fp,14,SEEK_SET);
      fread(&info1.res_sec_count,2,1,fp);
      fseek(fp,16,SEEK_SET);
      fread(&info1.num_fats,1,1,fp);
      fseek(fp,36,SEEK_SET);
      fread(&info1.fat_size_32,4,1,fp);
      fseek(fp,44,SEEK_SET);
      fread(&info1.root_cluster,4,1,fp);
      info1.first_data_sector=(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec)+(info1.res_sec_count*info1.bytes_per_sec);
      printf("BPB_BytesPerSec %d %x\n",info1.bytes_per_sec,info1.bytes_per_sec);
      printf("BPB_SecPerClus %d %x\n",info1.sec_per_clus,info1.sec_per_clus);
      printf("BPB_RsvdSecCnt %d %x\n",info1.res_sec_count,info1.res_sec_count);
      printf("BPB_NumFATS %d %x\n",info1.num_fats,info1.num_fats);
      printf("BPB_FATz32 %d %x\n",info1.fat_size_32,info1.fat_size_32);


    }

    if(!strcmp(token[0],"stat"))
    {
      int i,x=0;
      for(i=0;i<=16;i++)
      {
        fseek(fp,info1.first_data_sector+(32*i),SEEK_SET);
        fread(&dir[i].DIR_NAME,11,1,fp);
        fread(&dir[i].DIR_Attr,1,1,fp);
        fseek(fp,8,SEEK_CUR);
        fread(&dir[i].DIR_FirstClusterHigh,2,1,fp);
        fseek(fp,4,SEEK_CUR);
        fread(&dir[i].DIR_FirstClusterLow,2,1,fp);
        fread(&dir[i].DIR_FileSize,4,1,fp);
      }
      printf("\n%c\n",dir[2].DIR_NAME[10]);
    }
  }

  //fseek(fp,info1.first_data_sector+(32*0)+11,SEEK_SET);
}
