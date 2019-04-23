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
#include <limits.h>
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

//Return size of file, takes file name as a parameter and returns -1 if file does not exist.

long int fileSize(char name[])
{
  FILE* fp = fopen(name, "r");
  if (fp == NULL) {
    return -1;
  }
  fseek(fp, 0L, SEEK_END);
  long int result = ftell(fp);
  fclose(fp);
  return result;
}

/*Find the next Logical Block of Data takes the file pointer,sector number, bytes per sector and reserved sector count as parameters */

int16_t NextLB(FILE *fp,uint32_t sector,int16_t bytes_per_sec,int16_t res_sec_count)
{
  uint32_t FATAddress=(bytes_per_sec*res_sec_count)+(sector*4);
  int16_t val;
  fseek(fp,FATAddress,SEEK_SET);
  fread(&val,2,1,fp);
  return val;
}


int main(void) {
  struct information info1;
  struct directory_entry dir[200]; //directory struct to store information about current directory.
  /*this is for storing information about the previous dir without changing the current one, used in the ls code*/
  struct directory_entry reserve_dir[200];
  char * token[MAX_NUM_ARGUMENTS];
  char * cmd_str = (char * ) malloc(MAX_COMMAND_SIZE);
  FILE *fp=NULL;
  while (strcmp(cmd_str, "exit") || strcmp(cmd_str, "quit")) {
    printf("\nmfs> ");
    int slasher=0,normal=0;
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
    char *token[MAX_NUM_ARGUMENTS];
    int   token_count = 0;
    char *arg_ptr;
    char *working_str  = strdup( cmd_str );
    char *working_root = working_str;


    if(strchr(working_str,'/') != NULL){
      slasher=1;
    }
    else{
      normal=1;
    }



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

    while (!strcmp(token[0], "\n") || !strcmp(token[0], "\n")) {
      continue;
    }

    //fp=fopen("fat32.img","r+");
    if (token[0] == NULL) {
      continue;
    }
    else if (!strcmp(token[0], "exit") || !strcmp(token[0], "quit")) {
      exit(0);
    }


    else if(!strcmp(token[0],"open"))
    {
      if(fp)
      {
        printf("Error: File system image already open");
      }

      fp=fopen(token[1],"r+");
      if(fp==NULL)
      {
        printf("mfs>Error: File system image not found.\n");
        continue;
      }
      int i;
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
    }

    else if(!strcmp(token[0],"close"))
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
    else if(!strcmp(token[0],"put")) //
    {

      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }
      FILE *fp2=fopen(token[1],"r");
      long int file_size,size_available,next_block;
      size_available=512;
      int i,end=0;
      int32_t sector;
      file_size=fileSize(token[1]);
      if(file_size==-1)
      {
        printf("Error: File not found\n");
        continue;
      }

      for(i=0;i<16;i++)
      {
        if(dir[i].DIR_NAME[0]==0||dir[i].DIR_NAME[0]=='\xe5')
        {
          int16_t val;
          val=dir[i].DIR_FirstClusterLow;
          strcpy(dir[i].DIR_NAME,token[1]);
          fseek(fp,info1.first_data_sector+(32*i),SEEK_SET);
          fwrite(dir[i].DIR_NAME,11,1,fp);
          fseek(fp,info1.first_data_sector+(32*i)+11,SEEK_SET);
          dir[i].DIR_Attr=32;
          fwrite(&dir[i].DIR_Attr,sizeof(int),1,fp);
          dir[i].DIR_FileSize=file_size;
          fseek(fp,info1.first_data_sector+(32*i)+28,SEEK_SET);
          fwrite(&dir[i].DIR_FileSize,sizeof(uint32_t),4,fp);


          char copy[dir[i].DIR_FileSize+1];
          int x=1;
          if(file_size>512)
          {
          while(file_size > 512){
        fread(&sector, 512, 1, fp);
        fwrite(&sector, 512, 1, fp2);
        file_size-= 512;
        uint32_t FATAddress=(info1.bytes_per_sec*info1.res_sec_count)+(sector*4);
        fseek(fp,((sector-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec),SEEK_SET);
      }
    }
    else
    {
      fread(copy,512,1,fp2);
      fseek(fp,((val-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec),SEEK_SET);
      fwrite(copy,512,1,fp);
      uint32_t FATAddress=(info1.bytes_per_sec*info1.res_sec_count)+(val*4);
      fseek(fp,FATAddress,SEEK_SET);
      fread(&val,2,1,fp);
      x++;
    }
          fclose(fp2);

          break;
        }
      }

    }



    else if(!strcmp(token[0],"get"))
    {
      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
      }
      char *copy;
      copy=strdup(token[1]);
      int i,x=0;
      char expanded_name[12];
      memset( expanded_name, ' ', 12 );
      int found=0;
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
        if(!strncmp( expanded_name,dir[i].DIR_NAME, 11 )||!strcmp(copy,dir[i].DIR_NAME))
        {
          found=1;
          if(dir[i].DIR_FileSize<=512)
          {
            int offset=((dir[i].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
            uint8_t text[dir[i].DIR_FileSize+1];
            copy[10]='\0';
            FILE *fp2=fopen(copy,"w+");
            fseek(fp,offset,SEEK_SET);
            fread(text,dir[i].DIR_FileSize,1,fp);
            text[dir[i].DIR_FileSize]='\0';
            fwrite (&text,dir[i].DIR_FileSize,1,fp2);
            fclose(fp2);
            break;
          }
          int offset=((dir[i].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
          uint8_t text[512];
          copy[10]='\0';
          FILE *fp2=fopen(copy,"w+");
          fseek(fp,offset,SEEK_SET);
          fread(&text,512,1,fp);
          text[513]='\0';
          uint32_t FATAddress=(info1.bytes_per_sec*info1.res_sec_count)+(dir[i].DIR_FirstClusterLow*4);
          int16_t val;
          fseek(fp,FATAddress,SEEK_SET);
          fread(&val,2,1,fp);
          fwrite(&text,512,1,fp2);
          int x=1;
          while(val!=-1&&(512*x)<dir[i].DIR_FileSize)
          {
            int offset=((val-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
            fseek(fp,offset,SEEK_SET);
            fread(&text,512,1,fp);
            text[513]='\0';
            fwrite(&text,512,1,fp2);
            uint32_t FATAddress=(info1.bytes_per_sec*info1.res_sec_count)+(val*4);
            fseek(fp,FATAddress,SEEK_SET);
            fread(&val,2,1,fp);
            x++;
          }

          fclose(fp2);
          break;


        }

      }
      if(!found)
      {
        printf("Error: File not found\n");
      }
    }
    else if(!strcmp(token[0],"read"))
    {
      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }
      char copy[10];
      memset(copy, ' ', 10);
      strcpy(copy,token[1]);
      int i,x=0;
      char expanded_name[12];
      memset( expanded_name, ' ', 12 );
      char *token1 = strtok(token[1], "." );

      strncpy( expanded_name, token1, strlen( token1 ) );

      token1 = strtok( NULL, "." );

      if( token1 )
      {
        strncpy( (char*)(expanded_name+8), token1, strlen(token1 ) );
      }

      expanded_name[11] = '\0';
      for(i=0;i<200;i++)
      {
        int z;
        for( z = 0; z < 11; z++ )
        {
          expanded_name[z] = toupper( expanded_name[z] );
        }
        if(!strncmp( expanded_name,dir[i].DIR_NAME, 11 )||!strcmp(copy,dir[i].DIR_NAME))
        {

          int offset=((dir[i].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
          uint8_t value[dir[i].DIR_FileSize+1];
          fseek(fp,offset+atoi(token[2]),SEEK_SET);
          fread(value,atoi(token[3]),1,fp);
          value[atoi(token[3])]='\0';
          fflush(NULL);
          fflush(stdout);
          fflush(stdin);
          int p;
          for(p=0;p<atoi(token[3]);p++)
          {
            printf("%X\t",value[p]);
          }

          printf("\n");
          break;


        }

      }
    }
    else if(!strcmp(token[0],"ls"))
    {
      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }
      int i,x;
      if(token[1]){
        if(!strcmp(token[1],".."))
        {
          int offset=((dir[1].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
          if(dir[1].DIR_FirstClusterLow==0)
          {
            for(i=0;i<16;i++)
            {
              fseek(fp,info1.first_data_sector+(32*i),SEEK_SET);
              fread(&reserve_dir[i].DIR_NAME,11,1,fp);
              fread(&reserve_dir[i].DIR_Attr,1,1,fp);
              fseek(fp,8,SEEK_CUR);
              fread(&reserve_dir[i].DIR_FirstClusterHigh,2,1,fp);
              fseek(fp,4,SEEK_CUR);
              fread(&reserve_dir[i].DIR_FirstClusterLow,2,1,fp);
              fread(&reserve_dir[i].DIR_FileSize,4,1,fp);
            }
            for(i=0;i<16;i++)
            {
              if((reserve_dir[i].DIR_Attr==0x01||(reserve_dir[i].DIR_Attr==0x20))||(reserve_dir[i].DIR_Attr==0x10))
              {
                if(strncmp(reserve_dir[i].DIR_NAME,"\xe5",1)&&reserve_dir[i].DIR_NAME[0]!='.')
                {
                  if(reserve_dir[i].DIR_Attr==0x10)
                  {
                    printf("\n.) %.9s\t",reserve_dir[i].DIR_NAME);
                  }
                  else{
                    printf("\n.) %s\t",reserve_dir[i].DIR_NAME);
                  }
                }
              }
            }
            printf("\n");
            continue;

          }
          for(x=0;x<=16;x++)
          {
            fseek(fp,offset+(32*x),SEEK_SET);
            fread(&reserve_dir[x].DIR_NAME,11,1,fp);
            fread(&reserve_dir[x].DIR_Attr,1,1,fp);
            fseek(fp,8,SEEK_CUR);
            fread(&reserve_dir[x].DIR_FirstClusterHigh,2,1,fp);
            fseek(fp,4,SEEK_CUR);
            fread(&reserve_dir[x].DIR_FirstClusterLow,2,1,fp);
            fread(&reserve_dir[x].DIR_FileSize,4,1,fp);


          }
          for(i=0;i<16;i++)
          {
            if((reserve_dir[i].DIR_Attr==0x01||(reserve_dir[i].DIR_Attr==0x20))||(reserve_dir[i].DIR_Attr==0x10))
            {
              if(reserve_dir[i].DIR_Attr==0x10)
              {
                printf("\n.) %.9s\t",reserve_dir[i].DIR_NAME);
              }
              else{
                printf("\n.) %s\t",reserve_dir[i].DIR_NAME);
              }
            }

          }

        }
        else{
          for(i=0;i<16;i++)
          {
            if((dir[i].DIR_Attr==0x01||(dir[i].DIR_Attr==0x20))||(dir[i].DIR_Attr==0x10))
            {
              if(strncmp(dir[i].DIR_NAME,"\xe5",1)&&dir[i].DIR_NAME[0]!='.')
              {
                if(dir[i].DIR_Attr==0x10)
                {
                  printf("\n.) %.9s\t",dir[i].DIR_NAME);
                }
                else{
                  printf("\n.) %s\t",dir[i].DIR_NAME);
                }
              }
            }
          }
        }
        printf("\n");

      }
      else{
        for(i=0;i<16;i++)
        {
          if((dir[i].DIR_Attr==0x01||(dir[i].DIR_Attr==0x20))||(dir[i].DIR_Attr==0x10))
          {
            if(strncmp(dir[i].DIR_NAME,"\xe5",1)&&dir[i].DIR_NAME[0]!='.')
            {
              if(dir[i].DIR_Attr==0x10)
              {
                printf("\n.) %.9s\t",dir[i].DIR_NAME);
              }
              else{
                printf("\n.) %s\t",dir[i].DIR_NAME);
              }
            }
          }

        }
        printf("\n");

      }
    }

    else if(!strcmp(token[0],"cd"))
    {
      int found=0;
      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }
      int i,x=0;
      char expanded_name[12];
      memset( expanded_name, ' ', 12 );
      if(slasher==0)
      {
        // if token 1 is .. then we take the low cluster number from the second entry in the directory and seek to that point
        if(!strcmp(token[1],".."))
        {
          fflush(NULL);
          fflush(stdout);
          found=1;
          int offset=((dir[1].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
          if(dir[1].DIR_FirstClusterLow==0)
          {
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
            continue;

          }
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

        }
        else{
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
              found=1;
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

              break;
            }
          }

        }
      }
      else
      {

        char *ptr = strtok(token[1],"/");
        while(ptr!=NULL)
        {
          found=0;
          if(!strcmp(ptr,".."))
          {

            //printf("AAAAAAAA");
            fflush(NULL);
            fflush(stdout);

            int offset=((dir[1].DIR_FirstClusterLow-2)*info1.bytes_per_sec)+(info1.bytes_per_sec*info1.res_sec_count)+(info1.num_fats*info1.fat_size_32*info1.bytes_per_sec);
            if(dir[1].DIR_FirstClusterLow==0)
            {
              found=1;
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
              continue;

            }
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

          }
          else{
            for(i=0;i<200;i++)
            {
              strncpy( expanded_name,ptr, strlen(ptr) );


              expanded_name[11] = '\0';
              int z;
              for( z = 0; z < 11; z++ )
              {
                expanded_name[z] = toupper( expanded_name[z] );
              }

              if(!strncmp( expanded_name,dir[i].DIR_NAME, 11 ))
              {
                found=1;
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

              }


            }
          }
          ptr = strtok(NULL,"/");

        }
      }
      if(!found)
      {
        printf("\nNO SUCH FILE\n");
      }

    }
    else if(!strcmp(token[0],"info"))
    {
      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }
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
      printf("                 Decimal  HexaDecimal\n");
      printf("BPB_BytesPerSec %d       %x\n",info1.bytes_per_sec,info1.bytes_per_sec);
      printf("BPB_SecPerClus  %d         %x\n",info1.sec_per_clus,info1.sec_per_clus);
      printf("BPB_RsvdSecCnt  %d        %x\n",info1.res_sec_count,info1.res_sec_count);
      printf("BPB_NumFATS     %d         %x\n",info1.num_fats,info1.num_fats);
      printf("BPB_FATz32      %d     %x\n",info1.fat_size_32,info1.fat_size_32);
      printf("\n");

    }

    else if(!strcmp(token[0],"stat"))
    {
      if(fp==NULL)
      {
        printf("Error: File System Must Be Opened First");
        continue;
      }

      int i,found=0;
      char *copy;
      copy=strdup(token[1]);
      char expanded_name[12];
      memset( expanded_name, ' ', 12 );
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
      for(i=0;i<=16;i++)
      {
        if(!strncmp( expanded_name,dir[i].DIR_NAME, 11 )||!strcmp(copy,dir[i].DIR_NAME))
        {
          printf("\n ATTRIBUTE :- %d",dir[i].DIR_Attr);
          printf("\n CLUSTER LOW :- %d",dir[i].DIR_FirstClusterLow);
          printf("\n File Size:- %d",dir[i].DIR_FileSize);
          printf("\n");
          found=1;
        }

      }
      if(!found)
      {
        printf("Error File Not Found");
        printf("\n");

      }
    }

  }

}
