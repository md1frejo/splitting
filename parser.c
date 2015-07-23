#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

//#include "splitting.h"

/* parsing acqu files and writing prodecomp.txt file */

int parser(char *line,int n,int j,int header,char *path,int dim,int\
	   td1,int td2,char **nucs,int args,int d,int offs) {

  FILE *f,*fw;
  char *path4,*pathd;
  char *temp,*temp1,*temp2;
  char nuc[20],buffer[100];
  int i,k;
  int ea=0;

  typedef struct {
    char nuce[10];
    int sweeph;
  } acqus;

  acqus nuclei[dim]; 
    
  if(header) {
    
    asprintf(&pathd,"%s%s",path,"/acqus");
    fw=fopen("prodecomp.txt","w");
    fprintf(fw,"FORMAT= BRUKER\n");
    fprintf(fw,"NUCLEI= ");
    // direct dim
    for(i=1; i<=dim; i++) {
      asprintf(&temp1,"%s%d=","##$NUC",i);
      asprintf(&temp2,"%s%d=","##$BF",i);
      f=fopen(pathd,"rt");   
      while(fgets(buffer,30,f))  
	if(strstr(buffer,temp1)) 
	  sscanf(buffer,"%*s %s",nuclei[i-1].nuce);
      fclose(f);
      f=fopen(pathd,"rt");   
      while(fgets(buffer,30,f))
	if(strstr(buffer,temp2)) 
	  sscanf(buffer,"%*s %d",&nuclei[i-1].sweeph);
      fclose(f);
    }
    
    for(i=0; i<dim; i++)
      printf("acqus: %s %d %d\n",nuclei[i].nuce,nuclei[i].sweeph,dim);

    fprintf(fw,"%s ",nuclei[0].nuce);
    dim++; // note this change
    
    /* if(nucs[3]) */
    /*   fprintf(fw,"%s, ",nucs[3]); */

    /* i=4; */
    /* if(args>4)  */
    /*   while(nucs[i]) */
    /* 	fprintf(fw,"%s, ",nucs[i++]); */

    if(args>4) // has to be checcked ...
      ;

    else {
      // indirect dim
      for(i=2; i<=dim; i++)  {
	asprintf(&path4,"%s%s%ds",path,"/acqu",i);
	asprintf(&temp,"%s%d=","##$SFO",i);
	printf("acqu path: %s %d dim %d\n",path4,i,dim);
	f=fopen(path4,"rt");
	while(fgets(buffer,30,f)) {
	  if(strstr(buffer,"##$NUC1=")) {
	    sscanf(buffer,"%*s %s",nuc);
	    printf("bug: %s\n",nuc);
	    fprintf(fw,"%s, ",nuc);
	  }
	}
	fclose(f);
      }
    }
    
    fprintf(fw,"\nSW_ppm= ");
    double swd;
    
    f=fopen(pathd,"rt");
    while(fgets(buffer,30,f)) {
      if(strstr(buffer,"##$SW=")) {
	sscanf(buffer,"%*s %lf",&swd);
	fprintf(fw,"(%3.5f), ",swd);
      }
    }
    fclose(f);
    // indirect dim
    for(i=2; i<=dim; i++)  {
      asprintf(&path4,"%s%s%ds",path,"/acqu",i);
      asprintf(&temp,"%s%d=","##$SFO",i);
      f=fopen(path4,"rt");
      while(fgets(buffer,30,f)) {
	if(strstr(buffer,"##$SW=")) {
	  sscanf(buffer,"%*s %lf",&swd);
	  fprintf(fw,"%3.5f, ",swd);
	}
      }
      fclose(f);
    }

    fprintf(fw,"\nSW_hz= ");

    f=fopen(pathd,"rt");
    while(fgets(buffer,30,f)) {
      if(strstr(buffer,"##$SW_h=")) {
	sscanf(buffer,"%*s %lf",&swd);
	fprintf(fw,"(%5.3f), ",swd);
      }
    }
    fclose(f);

    double sh,sp;
    double offset,fo;    

    for(i=2; i<=dim; i++)  {
      asprintf(&path4,"%s%s%ds",path,"/acqu",i);
      f=fopen(path4,"rt");
      while(fgets(buffer,30,f)) {
	if(strstr(buffer,"##$SFO1=")) 
	  sscanf(buffer,"%*s %lf",&sh);
	if(strstr(buffer,"##$SW="))
	  sscanf(buffer,"%*s %lf",&sp);
      }
      fprintf(fw,"%5.3f, ",sh*sp);
      fclose(f);
    }

    fprintf(fw,"\nO1_ppm= ");

    f=fopen(pathd,"rt");
    while(fgets(buffer,30,f)) {

      if(strstr(buffer,"##$O1=")) 
	sscanf(buffer,"%*s %lf",&offset);
      if(strstr(buffer,"##$SFO1="))
	sscanf(buffer,"%*s %lf",&fo);
      //      fprintf(fw,"(%5.3f), ",offset/fo);

      /* if(strstr(buffer,"##$LOCKPPM=")) { */
      /* 	sscanf(buffer,"%*s %lf",&swd); */
      /* 	fprintf(fw,"(%5.3f), ",swd); */
      /* } */
    }
    fprintf(fw,"(%5.3f), ",offset/fo);
    fclose(f);

    // indirect dim
    for(i=2; i<=dim; i++)  {
      asprintf(&path4,"%s%s%ds",path,"/acqu",i);
      asprintf(&temp,"%s%d=","##$SFO",i);
      f=fopen(path4,"rt");
      while(fgets(buffer,30,f)) {
	if(strstr(buffer,"##$O1=")) 
	  sscanf(buffer,"%*s %lf",&offset);
	if(strstr(buffer,"##$SFO1="))
	  sscanf(buffer,"%*s %lf",&fo);
      }
      printf("offset %d: %lf %lf %lf\n",i,offset,fo,offset/fo);
      fprintf(fw,"%5.3f, ",offset/fo);
      fclose(f);
    }

    fprintf(fw,"\nSIZE= (%d), %d\n",td2/2,td1/2);
  
    fclose(fw);
    
    return d; //ea;
  }

  else {

    int array[dim],pos[n];
    char cwd[1024];
    int planes=pow(2,n-1);
    
    getcwd(cwd,sizeof(cwd));

    /* prodec_in gives planes in opposite orders...  */
    int c=dim-1;

    for(i=0; i<strlen(line); i++) { 
      if(line[i]=='1') 
	array[c--]=1;
      if(line[i]=='0')
	array[c--]=0;
    }

    if(array[0])
      ea=1;
    
    i=0;
    for(c=0; c<dim; c++)  
      if(array[c]) 
	pos[i++]=c;

    // make prodecomp.txt
    fw=fopen("prodecomp.txt","a");
    printf("planes %d\n",planes);

    for(k=0; k<planes; k++) {
      fprintf(fw,"%d= PATH: %s/FT2/%d%s \\\n",d,cwd,offs+d++,"s"); // hardcoded
      fprintf(fw,"   DEFINITION: 1, ");      
      if(k==0)
	for(i=0; i<dim; i++)
	  fprintf(fw,"%d, ",array[i]);
      if(k==1)
	for(i=0; i<dim; i++)
	  if(i==pos[1])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      if(k==2)
	for(i=0; i<dim; i++)
	  if(i==pos[2])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      if(k==3)
	for(i=0; i<dim; i++)
	  if(i==pos[1] || i==pos[2])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      if(k==4)
	for(i=0; i<dim; i++)
	  if(i==pos[3])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      if(k==5)
	for(i=0; i<dim; i++)
	  if(i==pos[1] || i==pos[3])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      if(k==6)
	for(i=0; i<dim; i++)
	  if(i==pos[2] || i==pos[3])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      if(k==7)
	for(i=0; i<dim; i++)
	  if(i==pos[1] || i==pos[2] || i==pos[3])
	    fprintf(fw,"%d, ",-1*array[i]);
	  else
	    fprintf(fw,"%d, ",array[i]);
      
      fprintf(fw,"\n");
    }
    fclose(fw);
    
    return d; //ea;
  }
}
