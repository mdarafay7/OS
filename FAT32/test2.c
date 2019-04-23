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
