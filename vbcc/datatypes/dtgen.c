#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#ifndef __STDC_VERSION__
#ifdef __GNUC__
#define __STDC_VERSION__ 199901L
#endif
#endif

struct dtlist {char *spec,*descr;} dts[]={
#include "datatypes.h"
};

struct dtconvlist {char *from,*to,*filef,*filet;int size;} cnvs[]={
#include "dtconv.h"
};

int dtcnt=sizeof(dts)/sizeof(dts[0]);
int cnvcnt=sizeof(cnvs)/sizeof(cnvs[0]);

signed char *have;

#define CHAR 1
#define UCHAR 2
#define SHORT 3 
#define USHORT 4
#define INT 5
#define UINT 6
#define LONG 7
#define ULONG 8
#define LLONG 9
#define ULLONG 10
#define FLOAT 11
#define DOUBLE 12
#define LDOUBLE 13
#define POINTER 14

#define TYPECNT POINTER

char *typen[TYPECNT+1]={"error","char","uchar","short","ushort","int","uint",
		        "long","ulong","llong","ullong","float","double","ldouble",
			"pointer"};
char *ftypen[TYPECNT+1]={"error","char","unsigned char","short","unsigned short",
                                 "int","unsigned int","long","unsigned long",
			         "long long","unsigned long long",
                                 "float","double","long double",
			         "char *"};

int dt[TYPECNT+1],cnv[TYPECNT+1];
char *nt[TYPECNT+1];
FILE *fin,*cout,*hout;
int crosscompiler;

void *mymalloc(size_t size)
{
  void *p=malloc(size);
  if(!p){
    printf("Out of memory!\n");
    exit(EXIT_FAILURE);
  }
  return p;
}

#define TESTTYPE(type,VALA,VALB) \
    if(sizeof(type)*CHAR_BIT==bits){\
      type tst=VALA;\
      if(islittle&&*((unsigned char *)&tst)==VALB)\
        return yn?"y":#type;\
      if(isbig&&*(((unsigned char *)(&tst+1))-1)==VALB)\
	return yn?"y":#type;\
    }\


char *yndefault(char *spec,int yn)
{
  int isieee=0,isunsigned=0,issigned,islittle=0,isbig=0,bits=0;
  char *none;
  if(yn) none="n"; else none="";
  if(*spec++!='S') return none;
  while(isdigit(*(unsigned char *)spec))
    bits=bits*10+*spec++-'0';
  if(*spec++!='B') return none;
  if(!strncmp(spec,"IEEE",4))
    {isieee=1;spec+=4;}
  else if(*spec=='U')
    {isunsigned=1;spec++;}
  else if(*spec=='S')
    {issigned=1;spec++;}
  else
    return none;
  if(spec[0]=='B'&&spec[1]=='E')
    {isbig=1;spec++;}
  else if(spec[0]=='L'&&spec[1]=='E')
    {islittle=1;spec++;}
  else
    islittle=isbig=1;
  if(isieee){
    TESTTYPE(float,1.0,0);
    TESTTYPE(double,1.0,0);
    TESTTYPE(long double,1.0,0);
    return none;
  }
  if(isunsigned){
    TESTTYPE(unsigned char,123,123);
    TESTTYPE(unsigned short,123,123);
    TESTTYPE(unsigned int,123,123);
    TESTTYPE(unsigned long,123,123);
#if __STDC_VERSION__==199901L
    TESTTYPE(unsigned long long,123,123);
#endif
    return none;
  }
  if(issigned){
    TESTTYPE(signed char,123,123);
    TESTTYPE(signed short,123,123);
    TESTTYPE(signed int,123,123);
    TESTTYPE(signed long,123,123);
#if __STDC_VERSION__==199901L
    TESTTYPE(signed long long,123,123);
#endif
    return none;
  }
  return none;
}

    
int askyn(char *def)
{
  char in[8];
  do{
    printf("Type y or n [%s]: ",def);
    fflush(stdout);
    fgets(in,sizeof(in),stdin);
    if(*in=='\n') strcpy(in,def);
  }while(*in!='y'&&*in!='n');
  return *in=='y';
}

char *asktype(char *def)
{
  char *in=mymalloc(128);
  printf("Enter that type[%s]: ",def);
  fflush(stdout);
  fgets(in,127,stdin);
  if(in[strlen(in)-1]=='\n') in[strlen(in)-1]=0;
  if(!*in) strcpy(in,def);
  return in;
}

int tst(int type,char *spec)
{
  int i,j;
  for(i=0;i<dtcnt;i++){
    if(strstr(spec,dts[i].spec)){ 
      if(have[i]==-2) continue;
      if(have[i]>=0){
/* 	printf("auto: %s == %s\n",dts[i].spec,nt[have[i]]); */
	dt[type]=i;
	nt[type]=nt[have[i]];
	cnv[type]=-1;
	return 1;
      }else{
	printf("Does your system/compiler support a type implemented as\n%s?\n",dts[i].descr);
	if(askyn(yndefault(dts[i].spec,1))){
	  dt[type]=i;
	  nt[type]=asktype(yndefault(dts[i].spec,0));
	  have[i]=type;
	  cnv[type]=-1;
	  return 1;
	}else{
	  have[i]=-2;
	}
      }
    }
  }
  for(j=0;j<cnvcnt;j++){
    char *s=0;
    if(strstr(spec,cnvs[j].from)) s=cnvs[j].to;
/*     if(strstr(spec,cnvs[j].to)) s=cnvs[j].from; */
    if(s){
      for(i=0;i<dtcnt;i++){
	if(!strcmp(s,dts[i].spec)){
	  if(have[i]==-2) continue;
	  if(have[i]>=0){
	    dt[type]=i;
	    nt[type]=nt[have[i]];
	    cnv[type]=j;
	    return 2;
	  }else{
	    printf("Does your system/compiler support a type implemented as\n%s?\n",dts[i].descr);
	    if(askyn(yndefault(dts[i].spec,1))){
	      dt[type]=i;
	      nt[type]=asktype(yndefault(dts[i].spec,0));
	      have[i]=type;
	      cnv[type]=j;
	      return 2;
	    }else{
	      have[i]=-2;
	    }
	  }
	}
      }
    }
  }
  return 0;
}
    
char *castfrom(int type)
{
  if(cnv[type]>=0){
    char *s=mymalloc(16);
    sprintf(s,"dtcnv%df",type);
    return s;
  }else{
    return "";
  }
}
char *castto(int type)
{
  if(cnv[type]>=0){
    char *s=mymalloc(16);
    sprintf(s,"dtcnv%dt",type);
    return s;
  }else{
    return "";
  }
} 
void gen_cast(char *name,int from,int to)
{
  fprintf(hout,"#define %s(x) %s((%s)%s(x))\n",name,castto(to),nt[to],castfrom(from));
}
void gen_2op(char *name,char *op,int type)
{
  fprintf(hout,"#define %s(a,b) %s(%s(a)%s%s(b))\n",name,castto(type),castfrom(type),op,castfrom(type));
} 
void gen_1op(char *name,char *op,int type)
{
  fprintf(hout,"#define %s(a) %s(%s%s(a))\n",name,castto(type),op,castfrom(type));
} 
void gen_cmp(char *name,char *op,int type)
{
  fprintf(hout,"#define %s(a,b) (%s(a)%s%s(b))\n",name,castfrom(type),op,castfrom(type));
} 
int main(int argc,char **argv)
{
  char type[128],spec[128];
  int i,r;
  if(argc!=4){ printf("Usage: dtgen <config-file> <output-file.h> <output-file.c>\n");exit(EXIT_FAILURE);}
/*   printf("%d datatypes, %d conversions\n",dtcnt,cnvcnt); */
  have=mymalloc(dtcnt*sizeof(*have));
  memset(have,-1,sizeof(*have)*dtcnt);
  fin=fopen(argv[1],"r");
  if(!fin){ printf("Could not open <%s> for input!\n",argv[1]);exit(EXIT_FAILURE);}
  hout=fopen(argv[2],"w");
  if(!hout){ printf("Could not open <%s> for output!\n",argv[2]);exit(EXIT_FAILURE);}
  cout=fopen(argv[3],"w");
  if(!hout){ printf("Could not open <%s> for output!\n",argv[3]);exit(EXIT_FAILURE);}
  printf("Are you building a cross-compiler?\n");
  crosscompiler=askyn("y");
  for(i=1;i<=TYPECNT;i++){
    fgets(spec,127,fin);
/*     printf("Specs for z%s:\n%s\n",typen[i],spec); */
    if(!crosscompiler){
      dt[i]=i;
      nt[i]=ftypen[i];
      have[i]=i;
      cnv[i]=-1;
    }else{
      if(!tst(i,spec)){
	printf("Problem! Your system does not seem to provide all of the data types\n"
	       "this version of vbcc needs.\nWrite to vb@compilers.de!\n");
	exit(EXIT_FAILURE);
      }
    }
  }
  fprintf(hout,"\n\n/* Machine generated file. DON'T TOUCH ME! */\n\n\n");
  fprintf(cout,"\n\n/* Machine generated file. DON'T TOUCH ME! */\n\n\n");
  fprintf(hout,"#ifndef DT_H\n");
  fprintf(hout,"#define DT_H 1\n");
  fprintf(cout,"#include \"dt.h\"\n\n");
  for(i=1;i<=TYPECNT;i++){
    if(cnv[i]>=0){
      fprintf(hout,"typedef struct {char a[%d];} dt%df;\n",cnvs[cnv[i]].size,i);
      fprintf(hout,"typedef dt%df z%s;\n",i,typen[i]);
      fprintf(hout,"typedef %s dt%dt;\n",nt[i],i);
      fprintf(hout,"dt%dt dtcnv%df(dt%df);\n",i,i,i);
      fprintf(hout,"dt%df dtcnv%dt(dt%dt);\n",i,i,i);
      fprintf(cout,"#undef DTTTYPE\n#define DTTTYPE dt%dt\n",i);
      fprintf(cout,"#undef DTFTYPE\n#define DTFTYPE dt%df\n",i);
      fprintf(cout,"dt%dt dtcnv%df(dt%df\n",i,i,i);
      fprintf(cout,"#include \"%s\"\n",cnvs[cnv[i]].filef);
      fprintf(cout,"dt%df dtcnv%dt(dt%dt\n",i,i,i);
      fprintf(cout,"#include \"%s\"\n",cnvs[cnv[i]].filet);
    }else{
      fprintf(hout,"typedef %s z%s;\n",nt[i],typen[i]);
    }
  }

  gen_cast("zc2zm",CHAR,LLONG);
  gen_cast("zs2zm",SHORT,LLONG);
  gen_cast("zi2zm",INT,LLONG);
  gen_cast("zl2zm",LONG,LLONG);
  gen_cast("zll2zm",LLONG,LLONG);
  gen_cast("zm2zc",LLONG,CHAR);
  gen_cast("zm2zs",LLONG,SHORT);
  gen_cast("zm2zi",LLONG,INT);
  gen_cast("zm2zl",LLONG,LONG);
  gen_cast("zm2zll",LLONG,LLONG);

  gen_cast("zuc2zum",UCHAR,ULLONG);
  gen_cast("zus2zum",USHORT,ULLONG);
  gen_cast("zui2zum",UINT,ULLONG);
  gen_cast("zul2zum",ULONG,ULLONG);
  gen_cast("zull2zum",ULLONG,ULLONG);
  gen_cast("zum2zuc",ULLONG,UCHAR);
  gen_cast("zum2zus",ULLONG,USHORT);
  gen_cast("zum2zui",ULLONG,UINT);
  gen_cast("zum2zul",ULLONG,ULONG);
  gen_cast("zum2zull",ULLONG,ULLONG);

  gen_cast("zum2zm",ULLONG,LLONG);
  gen_cast("zm2zum",LLONG,ULLONG);
  gen_cast("zf2zld",FLOAT,LDOUBLE);
  gen_cast("zd2zld",DOUBLE,LDOUBLE);
  gen_cast("zld2zf",LDOUBLE,FLOAT);
  gen_cast("zld2zd",LDOUBLE,DOUBLE);
  gen_cast("zld2zm",LDOUBLE,LLONG);
  gen_cast("zm2zld",LLONG,LDOUBLE);
  gen_cast("zld2zum",LDOUBLE,ULLONG);
  gen_cast("zum2zld",ULLONG,LDOUBLE);
  gen_cast("zp2zum",POINTER,ULLONG);
  gen_cast("zum2zp",ULLONG,POINTER);
 
  fprintf(hout,"#define l2zm(x) %s((%s)(x))\n",castto(LLONG),nt[LLONG]);
  fprintf(hout,"#define ul2zum(x) %s((%s)(x))\n",castto(ULLONG),nt[ULLONG]);
  fprintf(hout,"#define d2zld(x) %s((%s)(x))\n",castto(LDOUBLE),nt[LDOUBLE]);
  fprintf(hout,"#define zm2l(x) ((long)%s(x))\n",castfrom(LLONG));
  fprintf(hout,"#define zum2ul(x) ((unsigned long)%s(x))\n",castfrom(ULLONG));
  fprintf(hout,"#define zld2d(x) ((double)%s(x))\n",castfrom(LDOUBLE));

  gen_2op("zmadd","+",LLONG);
  gen_2op("zumadd","+",ULLONG);
  gen_2op("zldadd","+",LDOUBLE);
  gen_2op("zmsub","-",LLONG);
  gen_2op("zumsub","-",ULLONG);
  gen_2op("zldsub","-",LDOUBLE);
  gen_2op("zmmult","*",LLONG);
  gen_2op("zummult","*",ULLONG);
  gen_2op("zldmult","*",LDOUBLE);
  gen_2op("zmdiv","/",LLONG);
  gen_2op("zumdiv","/",ULLONG);
  gen_2op("zlddiv","/",LDOUBLE);
  gen_2op("zmmod","%",LLONG);
  gen_2op("zummod","%",ULLONG);
  gen_2op("zmlshift","<<",LLONG);
  gen_2op("zumlshift","<<",ULLONG);
  gen_2op("zmrshift",">>",LLONG);
  gen_2op("zumrshift",">>",ULLONG);
  gen_2op("zmand","&",LLONG);
  gen_2op("zumand","&",ULLONG);
  gen_2op("zmor","|",LLONG);
  gen_2op("zumor","|",ULLONG);
  gen_2op("zmxor","^",LLONG);
  gen_2op("zumxor","^",ULLONG);
  gen_2op("zmmod","%",LLONG);
  gen_2op("zummod","%",ULLONG);

  gen_1op("zmkompl","~",LLONG);
  gen_1op("zumkompl","~",ULLONG);

  gen_cmp("zmleq","<=",LLONG);
  gen_cmp("zumleq","<=",ULLONG);
  gen_cmp("zldleq","<=",LDOUBLE);
  gen_cmp("zmeqto","==",LLONG);
  gen_cmp("zumeqto","==",ULLONG);
  gen_cmp("zldeqto","==",LDOUBLE);

  fprintf(hout,"#endif\n");

  fclose(fin);
  fclose(hout);
  fclose(cout);
  free(have);  
  return 0;
}






