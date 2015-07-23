#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int split(char *path,int ea,int sort,int rd_type,int expn,int td1,int td2,int c,int offs) {

  int num_rows=pow(2,rd_type);
  int i,j,k,bytesin=0,bytesout=0,tot_rows=num_rows*2;
  FILE *f,*fids[num_rows];
  int rows[tot_rows][td2];
  int rows_out[tot_rows][td2];
  char fid2[100];
  f=fopen(path,"rb");
  printf("path: %s\n",path);
  mkdir("./FT2",0777);
  for(i=0;i<num_rows;i++) { 
    snprintf(fid2,100,"%s%d%s","FT2/",offs+c++,"s");
    fids[i]=fopen(fid2,"w");
  }
  
  for(i=0; i<td1/2; i++) {
    for(k=0; k<tot_rows; k++) 
      bytesin+=fread(rows[k],sizeof(int),td2,f);
    
    /* echo anti echo */
    if(ea==1) {
      for(k=0; k<tot_rows; k++) {
	if(!(k%2)) {
	  for(j=0; j<td2/2; j++) {
	    rows_out[k][2*j]   =  rows[k][2*j]   + rows[k+1][2*j];
	    rows_out[k][2*j+1] =  rows[k][2*j+1] + rows[k+1][2*j+1];
	  }
	}
	else {
	  for(j=0; j<td2/2; j++) {
	    rows_out[k][2*j]   = -rows[k-1][2*j+1] + rows[k][2*j+1];
	    rows_out[k][2*j+1] =  rows[k-1][2*j]   - rows[k][2*j];
	  }
	}
      }
    }
    else {
      for(k=0; k<tot_rows; k++)
	for(j=0; j<td2; j++)
	  rows_out[k][j]=rows[k][j];
    }
    
    /* combine */
    if(sort==1 || rd_type==0) 
      for(k=0; k<tot_rows; k++)
	for(j=0; j<td2; j++)
	  rows[k][j]=rows_out[k][j];
    
    else {
      if(rd_type==1) {
	for(j=0; j<td2; j++) {
	  rows[0][j]=rows_out[0][j]-rows_out[3][j];
	  rows[1][j]=rows_out[1][j]+rows_out[2][j];
	  rows[2][j]=rows_out[0][j]+rows_out[3][j];
	  rows[3][j]=rows_out[1][j]-rows_out[2][j];
	}
      }
      if(rd_type==2) {
	for(j=0; j<td2; j++) {
	  rows[0][j]=rows_out[0][j]-rows_out[3][j]-rows_out[5][j]-rows_out[6][j];
	  rows[1][j]=rows_out[1][j]+rows_out[2][j]+rows_out[4][j]-rows_out[7][j];
	  rows[2][j]=rows_out[0][j]+rows_out[3][j]-rows_out[5][j]+rows_out[6][j];
	  rows[3][j]=rows_out[1][j]-rows_out[2][j]+rows_out[4][j]+rows_out[7][j];
	  rows[4][j]=rows_out[0][j]-rows_out[3][j]+rows_out[5][j]+rows_out[6][j];
	  rows[5][j]=rows_out[1][j]+rows_out[2][j]-rows_out[4][j]+rows_out[7][j];
	  rows[6][j]=rows_out[0][j]+rows_out[3][j]+rows_out[5][j]-rows_out[6][j];
	  rows[7][j]=rows_out[1][j]-rows_out[2][j]-rows_out[4][j]-rows_out[7][j];
	}
      }
      if(rd_type==3) {
	for(j=0; j<td2; j++) {
	  rows[0][j]=rows_out[0][j]-rows_out[3][j]-rows_out[5][j]-rows_out[6][j] \
	            -rows_out[9][j]-rows_out[10][j]-rows_out[12][j]+rows_out[15][j];
	  rows[1][j]=rows_out[1][j]+rows_out[2][j]+rows_out[4][j]-rows_out[7][j]\
	            +rows_out[8][j]-rows_out[11][j]-rows_out[13][j]-rows_out[14][j];
	  
	  rows[2][j]=rows_out[0][j]+rows_out[3][j]-rows_out[5][j]+rows_out[6][j]\
	            -rows_out[9][j]+rows_out[10][j]-rows_out[12][j]-rows_out[15][j];
	  rows[3][j]=rows_out[1][j]-rows_out[2][j]+rows_out[4][j]+rows_out[7][j]\
	            +rows_out[8][j]+rows_out[11][j]-rows_out[13][j]+rows_out[14][j];
	  
	  rows[4][j]=rows_out[0][j]-rows_out[3][j]+rows_out[5][j]+rows_out[6][j]\
	            -rows_out[9][j]-rows_out[10][j]+rows_out[12][j]-rows_out[15][j];	 
	  rows[5][j]=rows_out[1][j]+rows_out[2][j]-rows_out[4][j]+rows_out[7][j]\
	            +rows_out[8][j]-rows_out[11][j]+rows_out[13][j]+rows_out[14][j];
	  
	  rows[6][j]=rows_out[0][j]+rows_out[3][j]+rows_out[5][j]-rows_out[6][j]\
	            -rows_out[9][j]+rows_out[10][j]+rows_out[12][j]+rows_out[15][j];
	  rows[7][j]=rows_out[1][j]-rows_out[2][j]-rows_out[4][j]-rows_out[7][j]\
	            +rows_out[8][j]+rows_out[11][j]+rows_out[13][j]-rows_out[14][j];
	  
	  rows[8][j]=rows_out[0][j]-rows_out[3][j]-rows_out[5][j]-rows_out[6][j]\
	            +rows_out[9][j]+rows_out[10][j]+rows_out[12][j]-rows_out[15][j];
	  rows[9][j]=rows_out[1][j]+rows_out[2][j]+rows_out[4][j]-rows_out[7][j]\
	            -rows_out[8][j]+rows_out[11][j]+rows_out[13][j]+rows_out[14][j];
	  
	  rows[10][j]=rows_out[0][j]+rows_out[3][j]-rows_out[5][j]+rows_out[6][j]\
	            +rows_out[9][j]-rows_out[10][j]+rows_out[12][j]+rows_out[15][j];
	  rows[11][j]=rows_out[1][j]-rows_out[2][j]+rows_out[4][j]+rows_out[7][j]\
	            -rows_out[8][j]-rows_out[11][j]+rows_out[13][j]-rows_out[14][j];
	   
	  rows[12][j]=rows_out[0][j]-rows_out[3][j]+rows_out[5][j]+rows_out[6][j]\
	            +rows_out[9][j]+rows_out[10][j]-rows_out[12][j]+rows_out[15][j];
	  rows[13][j]=rows_out[1][j]+rows_out[2][j]-rows_out[4][j]+rows_out[7][j]\
	            -rows_out[8][j]+rows_out[11][j]-rows_out[13][j]-rows_out[14][j];
	  
	  rows[14][j]=rows_out[0][j]+rows_out[3][j]+rows_out[5][j]-rows_out[6][j]\
	            +rows_out[9][j]-rows_out[10][j]-rows_out[12][j]-rows_out[15][j];
	  rows[15][j]=rows_out[1][j]-rows_out[2][j]-rows_out[4][j]-rows_out[7][j]\
	            -rows_out[8][j]-rows_out[11][j]-rows_out[13][j]+rows_out[14][j];
	}
      }
    }
    j=0;
    for(k=0; k<tot_rows; k++) {
      bytesout+=fwrite(rows[k],sizeof(int),td2,fids[j]);
      if(k%2)
      	j++;
    }
  }
  
  printf("experiment    : %d\n",expn);
  printf("planes        : %d\n",num_rows);
  printf("bytesin       : %lu\n",bytesin*sizeof(int));
  printf("bytesout      : %lu\n\n",bytesout*sizeof(int));
  
  for(k=0;k<num_rows;k++) 
    fclose(fids[k]);

  return c;
}
