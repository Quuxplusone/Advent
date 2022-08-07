#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "vcpr.h"

line *first_line,*last_line;

int debug=0;



int labcnt,maxlab;
labinfo *labinfos;

const char vs_copyright[]="vcpr code compressor V0.1 (c) 2020 by Volker Barthelmann";



void *mymalloc(size_t sz)
{
  void *p=malloc(sz);
  if(!p){
    fprintf(stderr,"Out of memory!\n");
    exit(EXIT_FAILURE);
  }
  return p;
}

match *new_match(void)
{
  match *new;
  new=mymalloc(sizeof(*new));
  new->next=0;
  new->start=0;
  new->max=0;
  return new;
}

void extend_labinfos(int n)
{
  if(n>maxlab)
    maxlab=n;
  /* TODO */
}

int new_label(void)
{
  maxlab++;
  extend_labinfos(maxlab);
  return maxlab;
}

line *new_line(void)
{
  line *new;
  new=mymalloc(sizeof(*new));
  new->next=new->prev=0;
  new->code=0;
  new->flags=0;
  new->size=0;
  new->l1=new->l2=0;
  return new;
}

void add_line(line *new)
{
  if(last_line){
    last_line->next=new;
    new->prev=last_line;
    last_line=new;
  }else{
    first_line=last_line=new;
  }
}

void insert_line(line *after, line *new)
{
  if(!after){
    new->next=first_line;
    first_line=new;
  }else{
    new->next=after->next;
    after->next=new;
  }
  new->prev=after;
  if(new->next)
    new->next->prev=new;
  else
    last_line=new;
}

void free_line(line *p)
{
  if(p->code) free(p->code);
  free(p);
}

void remove_line(line *p)
{
  if(p->prev) p->prev->next=p->next; else first_line=p->next;
  if(p->next) p->next->prev=p->prev; else last_line=p->prev;
}






void read_lines(FILE *f)
{
  char buf[MAXLEN],*p;
  line *new;
  int idx=0;

  while(1){
    p=fgets(buf,MAXLEN,f);
    if(feof(f))
      return;
    new=new_line();
    parse_line(buf,new);
    new->code=mymalloc(strlen(buf)+1);
    strcpy(new->code,buf);
    new->idx=++idx;
    add_line(new);
    if(new->flags&LABUSE){
      extend_labinfos(new->l1);
      extend_labinfos(new->l2);
      labinfos[new->l1].usecnt++;
      labinfos[new->l2].usecnt++;
    }
    if(new->flags&LABDEF)
      extend_labinfos(new->l1);
    /* skip rest of long line, TODO */
    while(p==&buf[MAXLEN-1])
      p=fgets(buf,MAXLEN,f);
  }
}

void print_line(line *p)
{
  printf("[idx=%d flags=%d sz=%d l1=%d l2=%d]: %s",p->idx,p->flags,p->size,p->l1,p->l2,p->code);
}

int slab,dlab;

int match_line(line *a, line *b)
{
  if((a->flags&(BARRIER|LBARRIER))||(b->flags&(BARRIER|LBARRIER)))
    return 0;

  /* we currently handle only one label */
  if(a->l2!=b->l2)
    return 0;

  if(a->l1&&!b->l1)
    return 0;
  if(b->l1&&!a->l1)
    return 0;

  if(a->l1&&a->l1==slab)
    if(b->l1!=dlab)
      return 0;

  if(!strcmp(a->code, b->code)&&!(a->flags&BRANCH))
    return 1;

  if(slab&&a->l1!=slab&&a->l1!=b->l1)
    return 0;

  if(a->l1&&b->l1){
    char *pa=a->code;
    char *pb=b->code;
    char al[32],bl[32];
    int la,lb;
    sprintf(al,"l%d",a->l1);
    sprintf(bl,"l%d",b->l1);
    la=strlen(al);
    lb=strlen(bl);
    while(*pa&&*pb){
      if(!strncmp(pa,al,la)){
	if(!strncmp(pb,bl,lb)){
	  pa+=la;
	  pb+=lb;
	}else
	  return 0;
      }else{
	if(*pa!=*pb)
	  return 0;
	pa++;
	pb++;
      }
    }
    if(*pa||*pb)
      return 0;
    if(!slab){
      slab=a->l1;
      dlab=b->l1;
    }else{
      if(a->l1!=slab||b->l1!=dlab)
	return 0;
    }
    return 1;
  }

  return 0;
}

match *check_matches(line *s,line *p, int min)
{
  int sz=0,n=0;
  match *new;
  line *first;

  slab=dlab=0;

  if(!match_line(s, p))
    return 0;

  first=p;

  new=new_match();
  new->start=p;

  do{
    sz+=p->size;
    new->size[n]=sz;
    n++;
    new->max=n;
    p=p->next;
    s=s->next;
  }while(n<MAXMLEN&&p&&s&&match_line(s ,p)&&s!=first);

  if(sz<min){
    free(new);
    return 0;
  }

  if(slab){
    int lstart=0,luse=0,ldef=0;
    if(debug) printf("checking labvalid\n");
    for(n=0,p=new->start;n<new->max;n++,p=p->next){
      if(p->l1==dlab){
	lstart=1;
	if(p->flags&LABDEF)
	  ldef=1;
	if(p->flags&LABUSE)
	  luse++;
      }
      if(lstart&&(!ldef||luse<labinfos[dlab].usecnt))
	new->size[n]=0;
    }
  }

  return new;
}   

void free_matches(match *p)
{
  match *n;
  while(p){
    n=p->next;
    free(p);
    p=n;
  }
}

void print_matches(match *p)
{
  int i;
  line *l;
  if(p->next)
    print_matches(p->next);
  printf("match:\n");
  for(i=0,l=p->start;i<p->max;i++){
    printf("sv=%d ",p->size[i]);
    print_line(l);
    l=l->next;
  }
}
  

int main(int argc,char *argv[])
{
  int i,quiet=0,changed,pass=0;
  FILE *in,*out; char *inname=0,*outname=0;
  line *p;

  for(i=1;i<argc;i++){
    if(*argv[i]!='-'){
      if(!inname){
        inname=argv[i];
      }else{
        if(outname){
          printf("Only one input file allowed\n");
	  exit(EXIT_FAILURE);
        }else{
          outname=argv[i];
        }
      }
    }else{
      if(!strcmp("-quiet",argv[i])) {quiet=1;continue;}
#ifndef DEBUG
      if(!strncmp("-debug=",argv[i],7)){debug=atoi(argv[i]+7);continue;}
#endif
      printf("Unknown option \"%s\"\n",argv[i]);
      exit(EXIT_FAILURE);
    }
  }
  if(!quiet){
#ifdef SPECIAL_COPYRIGHT
    printf("%s\n",SPECIAL_COPYRIGHT);
#else
    printf("%s\n%s\n",vs_copyright,tg_copyright);
#endif
  }
  if(!inname){printf("No input file\n");exit(EXIT_FAILURE);}
  if(!outname){printf("No output file\n");exit(EXIT_FAILURE);}
  in=fopen(inname,"r");
  if(!in){printf("Could not open input file \"%s\"\n",inname);exit(EXIT_FAILURE);}
  out=fopen(outname,"w");
  if(!out){printf("Could not open output file \"%s\"\n",outname);exit(EXIT_FAILURE);}

  labinfos=mymalloc(sizeof(labinfo)*LABNUM);
  labcnt=LABNUM;

  for(i=0;i<labcnt;i++)
    labinfos[i].usecnt=0;

  read_lines(in);

  if(debug)
    for(p=first_line;p;p=p->next)
      print_line(p);

  if(debug)
    for(i=1;i<labcnt;i++)
      if(labinfos[i].usecnt)
	printf("lab %d used %d times\n",i,labinfos[i].usecnt);

  do{
    pass++;
    changed=0;
    if(debug) printf("pass %d\n",pass);
    for(p=first_line;p;p=p->next){
      line *n;
      match *m,*mlist=0;int max=0,tmp;

      if(sscanf(p->code,";vcprmin=%d",&tmp)==1)
	minsave=tmp;
      if(sscanf(p->code,"#vcprmin=%d",&tmp)==1)
	minsave=tmp;
      
      for(n=p->next;n;n=n->next){
	m=check_matches(p,n,minsave);
	if(m){
	  m->next=mlist;
	  mlist=m;
	  if(m->max>max) max=m->max;
	  /* TODO: is this necessary? */
	  for(i=0;i<m->max;i++)
	    n=n->next;
	}
      }
      if(mlist){
	int maxsav=0,maxn=0,csav,cnt;
	
	if(debug){
	  printf("mlist: \n");
	  print_line(p);
	  print_matches(mlist);
	}
	for(i=1;i<=max;i++){
	  csav=0;cnt=0;
	  for(m=mlist;m;m=m->next){
	    if(m->max>=i){
	      cnt++;
	      csav+=m->size[i-1]-minsave;
	    }
	  }
	  if(debug) printf("%d times %d: total %d\n",cnt,i,csav);
	  if(csav>maxsav){
	    maxsav=csav;
	    maxn=i;
	  }
	}
	if(debug) printf("best saving: %d: total %d\n",maxn,maxsav);
	
	n=p->prev;
	if(n&&maxsav>0){
	  add_header(last_line);
	  if(last_line->next==last_line) abort();
	  if(debug) printf("copying header:\n");
	  for(i=0;i<maxn;i++){
	    p=n->next;
	    if(debug) print_line(p);
	    p->flags|=LBARRIER;
	    remove_line(p);
	    insert_line(last_line,p);
	  }

	  changed=1;
	  add_jsr(n);
	  
	  add_ret(last_line);
	  
	  labinfos[n->next->l1].usecnt++;
	  if(debug) printf("update usecnt lab %d to %d\n",n->next->l1,labinfos[n->next->l1].usecnt);
	  
	  for(m=mlist;m;m=m->next){
	    if(m->max>=maxn){
	      if(debug) printf("remove copy:\n");
	      p=m->start->prev;
	      for(i=0;i<maxn;i++){
		line *t=p->next;
		if(debug) print_line(t);
		remove_line(t);
		free_line(t);
	      }
	      add_jsr(p);
	      labinfos[p->next->l1].usecnt++;
	      if(debug) printf("update usecnt lab %d to %d\n",p->next->l1,labinfos[p->next->l1].usecnt);
	    }
	  }
	  p=n;
	}
	free_matches(mlist);
      }
    }
    if(changed){
      for(p=first_line;p;p=p->next)
	p->flags&=~LBARRIER;
    }
  }while(changed);

  for(p=first_line;p;p=p->next)
    fprintf(out,"%s",p->code);

  exit(EXIT_SUCCESS);
}
