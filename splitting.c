#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "splitting.h"

/* gcc -Wall -O2 -g parser.c splitter.c splitting.c -lm -o splitting */

//int parser(char*,int,int,int,char*,int,int,int,char **nuclei,int args);
//int split(char*,int,int,int,int,int,int);

// ex: 5D/100 0 200

int main(int argc, char* argv[]) {
  
  if(argc<4) {
    printf("not enough arguments\n");
    printf("start dir echo antiecho (0 or 1) offset\n");
    printf("ex: 5D/100 0 200\n");
    return 0;
  }

  int i;
    
  char *dir1=argv[1]; /* ex: 5D/100 1 1 */
  int ea=atoi(argv[2]),temp=0;
  int offs=atoi(argv[3]);
  int d=0,c=0;
  int sort=0;
  FILE *ff;
  char prodec_in[100],acqus[100],acqu2s[100];
  char str[10];
  int j=0,jj=0;
  int n,len1,len2,dim,expno,exp,td1,td2;
  char *path1,*path2,*path3,*serpath;
  len1=strlen(dir1);
  temp=asprintf(&path1,"%s%s",dir1,"/acqu2s");
  temp=asprintf(&path2,"%s%s",dir1,"/acqus");
  temp=asprintf(&path3,"%s%s",dir1,"/prodec_in.txt");
  ff=fopen(path1,"rt");
  //  printf("c1: %d\n",c);
  while(fgets(acqu2s,30,ff)) 
    if(strstr(acqu2s,"##$TD="))
      sscanf(acqu2s,"%*s %d",&td1);
  
  fclose(ff);
  ff=fopen(path2,"rt");
  
  while(fgets(acqus,30,ff)) 
    if(strstr(acqus,"##$TD="))
      sscanf(acqus,"%*s %d",&td2);
  fclose(ff);
  
  printf("\ntd1 (r+i)     : %d\n",td1);
  printf("td2 (r+i)     : %d\n",td2);
  printf("sort          : %d\n\n",sort);
  temp++;
  
  ff=fopen(path3,"rt");
  char *dir2=calloc(100,sizeof(char));
  //char dir2[100];
  //  printf("%s\n",path3);
  while(fgets(prodec_in,30,ff)) {

    if(j==0) {
      sscanf(prodec_in,"%*s %d",&expno);
      sscanf(prodec_in,"%*s %s",str);
      len2=strlen(str);
      strncpy(dir2,dir1,len1-len2);
    }
    printf("1 dir2: %s\n",dir2);
    if(j==1) {
      sscanf(prodec_in,"%*s %d",&dim);
      printf("dim split: %d\n",dim);
      d=parser("",0,0,1,dir1,dim,td1,td2,argv,argc,d,offs);
    }
    if(j==2)
      sscanf(prodec_in,"%*s %d",&exp);
    if(j>2) {
      jj++;
      n=0;
      for(i=0; i<80; i++)
    	if(prodec_in[i]=='1')
    	  n++;
      
      //if(prodec_in[3]=='1')
      //	ea=1;
      
      temp=asprintf(&serpath,"%s%d%s",dir2,expno,"/ser");
      printf("2 dir2: %s\n",dir2);
      printf("serpath: %s\n",serpath);
      d=parser(prodec_in,n,jj,0,dir1,dim,td1,td2,argv,argc,d,offs);
      n--;
      printf("ea: %d\n",ea);
      
      c=split(serpath,ea,sort,n,jj,td1,td2,c,offs);
    
      //      printf("c2: %d\n",c);
      expno++;
    }

    j++;
  }

  // generate dir list
  system("ls FT2/* > dirlist");

  free(dir2);
  fclose(ff);
  return 1;
}
