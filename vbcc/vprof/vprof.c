/*
 * vprof
 *
 * vbcc profiler. Displays the contents of "mon.out" files.
 * (C)1998,2010 by Frank Wille <frank@phoenix.owl.de>
 *
 * vprof is freeware and part of the portable and retargetable ANSI C
 * compiler vbcc, copyright (c) 1995-98 by Volker Barthelmann.
 * vprof may be freely redistributed as long as no modifications are
 * made and nothing is charged for it. Non-commercial usage is allowed
 * without any restrictions.
 * EVERY PRODUCT OR PROGRAM DERIVED DIRECTLY FROM MY SOURCE MAY NOT BE
 * SOLD COMMERCIALLY WITHOUT PERMISSION FROM THE AUTHOR.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VERSION 0
#define REVISION 2
#define PLEVEL 0
#define DEFAULTNAME "mon.out"

struct profdata {
  char *funcname;
  unsigned long ncalls;
  unsigned long tottime;
  unsigned long loctime;
};


static void show(struct profdata *pdata,int n)
/* write profdata records to stdout */
{
  struct profdata *p;
  unsigned long runtime;
  int i;

  for (i=0,p=pdata,runtime=0; i<n; i++,p++)
    runtime += p->loctime;

  printf("  %%     total   local                local      total\n"
         " time  seconds seconds     calls    ms/call    ms/call  name\n");
  for (i=0,p=pdata; i<n; i++,p++) {
    printf("%5.2f %7.2f %7.2f %9lu %10.3f %10.3f  %s\n",
           ((double)p->loctime/(double)runtime)*100.0,
           (double)p->tottime/1000000.0,
           (double)p->loctime/1000000.0,
           p->ncalls,
           ((double)p->loctime/(double)p->ncalls)/1000.0,
           ((double)p->tottime/(double)p->ncalls)/1000.0,
           p->funcname);
  }
}


static int cmp_total(const void *pd1,const void *pd2)
{
  return ((int)(((struct profdata *)pd2)->loctime -
          ((struct profdata *)pd1)->loctime));
}


static void show_local(struct profdata *pd,int n)
/* show profiling data, sorted according to the local time */
/* spent in each function */
{
  qsort(pd,n,sizeof(struct profdata),cmp_total);
  show(pd,n);
}


static char *skipname(char *p)
/* skip name and alignment bytes and return pointer to profile data */
{
  int n = strlen(p)+1;

  p += n;
  if (n &= 3)
    p += 4-n;
  return (p);
}


int main(int argc,char *argv[])
{
  char *mname;
  FILE *fp;
  long size,len;
  char *buf,*p;
  int i,nrecs = 0;
  struct profdata *pdata,*pd;

  if (argc >= 2) {
    if (*argv[1]=='-' || *argv[1]=='?') {
      printf("%s V%d.%d%c (c)1998,2010 by Frank Wille\n"
             "Usage:\n  %s [mon.out]\n",argv[0],
             VERSION,REVISION,PLEVEL?('a'+PLEVEL-1):' ',argv[0]);
      exit(1);
    }
    else
      mname = argv[1];
  }
  else
    mname = DEFAULTNAME;

  if (fp = fopen(mname,"r")) {
    /* determine file size */
    fseek(fp,0,SEEK_END);
    size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    if (size < 0) {
      fclose(fp);
      fprintf(stderr,"%s: Seek error on %s!\n",argv[0],mname);
      exit(EXIT_FAILURE);
    }

    /* allocate buffer and read file */
    if (!(buf = malloc(size))) {
      fclose(fp);
      fprintf(stderr,"%s: Not enough memory!\n",argv[0]);
      exit(EXIT_FAILURE);
    }
    if (fread(buf,1,size,fp) != size) {
      fclose(fp);
      fprintf(stderr,"%s: %s had a read error!\n",argv[0],mname);
      exit(EXIT_FAILURE);
    }
    fclose(fp);

    /* count number of entries in mon.out and allocate profdata array */
    p = buf;
    while (p < buf+size) {
      nrecs++;
      p = skipname(p) + 3*sizeof(unsigned long);
    }
    if (p!=buf+size || nrecs==0) {
      fprintf(stderr,"%s: %s: Corrupted file format.\n",argv[0],mname);
      exit(EXIT_FAILURE);
    }
    if (!(pdata = malloc(nrecs * sizeof(struct profdata)))) {
      fprintf(stderr,"%s: Not enough memory!\n",argv[0]);
      exit(EXIT_FAILURE);
    }

    /* fill profdata array */
    for (i=0,p=buf,pd=pdata; i<nrecs; i++,pd++) {
      pd->funcname = p;
      p = skipname(p);
      pd->ncalls = *(unsigned long *)p;
      pd->tottime = *(unsigned long *)(p+sizeof(unsigned long));
      pd->loctime = *(unsigned long *)(p+2*sizeof(unsigned long));
      p += 3*sizeof(unsigned long);
    }

    /* display */
    show_local(pdata,nrecs);
  }
  else {
    fprintf(stderr,"%s: Can't open %s.\n",argv[0],mname);
    exit(EXIT_FAILURE);
  }

  return 0;
}
