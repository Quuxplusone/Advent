#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSIZE 4196
char buf[BSIZE];

int main()
{
  int nest=0;
  while(fgets(buf,BSIZE,stdin)){
    if(nest==0){
      fputs(buf,stdout);
      if(!strncmp(buf,"#if HAVE_OSEK",12)||!strncmp(buf,"#ifdef HAVE_MISRA",17)||!strncmp(buf,"#ifdef HAVE_ECPP",16))
	nest++;
    }else{
      if(!strncmp(buf,"#endif",6))
	nest--;
      if(!strncmp(buf,"#if",3))
	nest++;
      if(nest==1&&!strncmp(buf,"#else",5))
	fputs("illegal #else found",stderr);
      if(nest==1&&!strncmp(buf,"#elif",5))
	fputs("illegal #elif found",stderr);
      if(nest==0)
	fputs(buf,stdout);
      else
	puts("/* removed */");
    }
  }
  return 0;
}

