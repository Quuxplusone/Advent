#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vcpr.h"


const char tg_copyright[]="6502 code compressor V0.1 (c) 2020 by Volker Barthelmann";

char *inst[]={
  "lda","ldx","sta","stx","ldy","sty",
  "adc","sbc","and","ora","eor",
  "bne","beq","bcc","bcs","bvs","bvc","bpl","bmi",
  "clc","sec",
  /*"pla","pha",*/
  "txa","tax","tya","tay",
  "inc","dec","dex","inx","iny","dey",
  /*"rts","rti",*/"jsr","jmp",
  "cmp","cpx","cpy",
  "asl","lsr","ror","rol"
};

int minsave=5;

void parse_line(char *s,line *p)
{
  int l,i;
  static int secok=0;
  char buf[16],buf2[16],*e;

  if(sscanf(s," %8s %8s",buf,buf2)==2&&!strcmp(buf,"section")){
    if(!strcmp(buf2,"text"))
      secok=1;
    else
      secok=0;
    p->flags=BARRIER;
    return;
  }

  if(sscanf(s,"l%d:",&l)==1){
    p->flags|=LABDEF;
    p->l1=l;
    p->size=0;
    if(!secok) p->flags|=BARRIER;
    return;
  }

  if(!secok){
    p->flags|=BARRIER;
    return;
  }

  if(!isspace((unsigned char)*s)){
    p->flags=BARRIER;
    return;
  }
  while(isspace((unsigned char)*s))
    s++;
  for(e=s;isalpha((unsigned char)*e);e++);

  for(i=0;i<sizeof(inst)/sizeof(inst[0]);i++){
    if(!strncmp(s,inst[i],e-s)){
      while(isspace((unsigned char)*e)) e++;
      if(*e==0){
	p->size=1;
	return;
      }
      p->size=2;
      if(sscanf(e,"l%d",&l)==1){
	p->flags|=LABUSE;
	p->l1=l;
      }
      if(*s=='b'||!strncmp(s,"jmp",3)){
	p->flags|=BRANCH;
      }
      return;
    }
  }
  p->flags=BARRIER;
}

static int nlab;

#define SECTION "\tsection\ttext\n"

void add_header(line *after)
{
  nlab=new_label();
  line *new;
  new=new_line();
  new->flags=LABDEF;
  new->l1=nlab;
  new->code=mymalloc(16); /* TODO */
  sprintf(new->code,"x%d:\n",nlab);
  insert_line(after,new);
  new=new_line();
  new->flags=BARRIER;
  new->code=mymalloc(strlen(SECTION)+1);
  strcpy(new->code,SECTION);
  insert_line(after,new);
}

void add_ret(line *after)
{
  line *new=new_line();
  new->size=1;
  new->flags=BARRIER;
  new->code=mymalloc(8); /*TODO*/
  strcpy(new->code,"\trts\n");
  insert_line(after,new);
}

void add_jsr(line *after)
{
  line *new=new_line();
  new->flags=LABUSE;
  new->size=3;
  new->l1=nlab;
  new->code=mymalloc(24); /* TODO */
  sprintf(new->code,"\tjsr\tx%d\n",nlab);
  insert_line(after,new);
}
