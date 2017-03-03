#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

FILE *infile;
int line=1;

#define INCSIZE 16
FILE **incfile;
char **incname;
int *incline;
int incn,incsize;

#define PP_TSIZE 1024

/*FIXME*/
#define ierror(x) abort();
#define mymalloc(x) malloc(x)
#define myrealloc(x,y) realloc(x,y)

/* must not collide with source character-set */
enum {
  TKKEYWORD=-32,
  TKMARG,
  TKIDENTIFIER,
  TKNUMBER,
  TKPLUSPLUS,
  TKMINMIN,
  TKEOL,
  TKLTLT,
  TKGTGT,
  TKSREF,
  TKLE,
  TKGE,
  TKEQ,
  TKNE,
  TKLAND,
  TKLOR,
  TKMULEQ,
  TKDIVEQ,
  TKMODEQ,
  TKPLUSEQ,
  TKMINEQ,
  TKLTLTEQ,
  TKGTGTEQ,
  TKANDEQ,
  TKOREQ,
  TKXOREQ,
  TKDDD,
  TKNUMNUM
};

static char *tokout[]={"err","err","err","err","++","--","","<<",">>","->","<=",">=","==",
		       "!=","&&","||","*=","/=","%=","+=","-=","<<=",">>=","&=",
		       "|=","^=","...","##"
};

static char *desc[]={
  "TKKEYWORD",
  "TKMARG",
  "TKIDENTIFIER",
  "TKNUMBER",
  "TKPLUSPLUS",
  "TKMINMIN",
  "TKEOL",
  "TKLTLT",
  "TKGTGT",
  "TKSREF",
  "TKLE",
  "TKGE",
  "TKEQ",
  "TKNE",
  "TKLAND",
  "TKLOR",
  "TKMULEQ",
  "TKDIVEQ",
  "TKMODEQ",
  "TKPLUSEQ",
  "TKMINEQ",
  "TKLTLTEQ",
  "TKGTGTEQ",
  "TKANDEQ",
  "TKOREQ",
  "TKXOREQ",
  "TKDDD",
  "TKNUMNUM"
};

typedef struct {
  int size;
  int next;
  int *tokens;
} tokenlist;

#ifndef OBFUSCATOR

#define VARARG_MACRO  1
#define BUILTIN_MACRO 2
#define NEED_PAREN    4

typedef struct {
  char *name;
  int flags;
  int nargs;
  tokenlist repl;
} macro;

#define PP_HSIZE 16384
static int **hash_table;

#define PP_MSIZE 128
static macro *macro_table;
static int macro_cnt=1,macro_size;

void print_tokenlist(FILE *,tokenlist *);

static void copy_tokenlist(tokenlist *new,tokenlist *old)
{
  if(new->size<old->next+new->next){
    new->size=old->next+new->next;
    new->tokens=myrealloc(new->tokens,new->size*sizeof(*new->tokens));
  }
  memcpy(new->tokens+new->next,old->tokens,old->next*sizeof(*new->tokens));
  new->next+=old->next;
}

static int new_macro(char *name,int flags,int nargs,tokenlist *repl)
{
  int i,*p;
  if(macro_cnt>=macro_size){
    macro_size+=PP_MSIZE;
    macro_table=myrealloc(macro_table,macro_size*sizeof(*macro_table));
    macro_table[0].name=0; /* dummy */
  }
  macro_table[macro_cnt].name=mymalloc(strlen(name)+1);
  strcpy(macro_table[macro_cnt].name,name);
  macro_table[macro_cnt].flags=flags;
  macro_table[macro_cnt].nargs=nargs;
  macro_table[macro_cnt].repl.size=0;
  macro_table[macro_cnt].repl.next=0;
  macro_table[macro_cnt].repl.tokens=0;  
  copy_tokenlist(&macro_table[macro_cnt].repl,repl);
  return macro_cnt++;
}

static void free_macro(int n)
{
  free(macro_table[n].name);
  free(macro_table[n].repl.tokens);
  macro_table[n].name=0;
}

static int hash(char *p)
{
  int s=0;
  while(*p)
    s+=*p++;
  return s%PP_HSIZE;
}

static int find_macro(char *name)
{
  int i=hash(name),n,*p;
  if(!(p=hash_table[i])) return 0;
  n=p[0];
  for(i=1;i<=n;i++)
    if(macro_table[p[i]].name&&!strcmp(macro_table[p[i]].name,name))
      return p[i];
  return 0;
}

static void add_macro(int n)
{
  int i=hash(macro_table[n].name),*p;
  if(!(p=hash_table[i])){
    hash_table[i]=p=mymalloc(2*sizeof(*hash_table[i]));
    p[0]=1;
    p[1]=n;
  }else{
    p=hash_table[i]=myrealloc(p,(p[0]+1)*sizeof(*p));
    p[0]++;
    p[p[0]]=n;
printf("hash_collisio, depth=%d\n",p[0]);
  }
}

#endif /* OBFUSCATOR */

static FILE *include_file(const char *name)
{
    if(incn>=incsize){
	incsize+=INCSIZE;
	incfile=myrealloc(incfile,incsize*sizeof(*incfile));
	incname=myrealloc(incname,incsize*sizeof(*incname));
	incline=myrealloc(incline,incsize*sizeof(*incline));
    }
    if(!name){
	incfile[incn]=stdin;
	name="<stdin>";
    }else{
	incfile[incn]=fopen(name,"r");
    }
printf("include file %s\n",name);
    incname[incn]=mymalloc(strlen(name)+1);
    strcpy(incname[incn],name);
    incn++;
    return incfile[incn-1];
}

static FILE *close_file(void)
{
    --incn;
    free(incname[incn]);
    if(incfile[incn])
      fclose(incfile[incn]);
    if(incn<=0)
	return 0;
    return incfile[incn-1];
}

static void add_token(tokenlist *p,int c)
{
  if(p->next>=p->size){
    p->size+=PP_TSIZE;
    p->tokens=myrealloc(p->tokens,p->size*sizeof(*p->tokens));
  }
  p->tokens[p->next]=c;
  p->next++;
}

int read_char(void)
/* Read next input character. Counts line numbers. */
{
  int tmp=getc(infile);
  if(tmp=='\n') line++;
  return tmp;
}

static int push_back=EOF;

static int get_next_char(void)
/* Read next character. Handles trigraphs and line-concatenation. */
{
  static int buf[2],inbuf;
  int tmp,tmp2;
  if(push_back!=EOF){
    tmp=push_back;
    push_back=EOF;
    return tmp;
  }
  if(inbuf==0){
    tmp=read_char();
    if(tmp!='\\'&&tmp!='?'){
      return tmp;
    }else{
      buf[0]=tmp;
      inbuf=1;
      /* fall through */
    }
  }
  if(inbuf==1){
    if(buf[0]!='?'&&buf[0]!='\\'){
      inbuf=0;
      return buf[0];
    }else{
      tmp=read_char();
      if(buf[0]=='\\'){
	if(tmp=='\n'){
	  inbuf=0;
	  return get_next_char();
	}else{
	  buf[0]=tmp;
	  return '\\';
	}
      }else{
	if(tmp=='?'){
	  buf[1]='?';
	  inbuf=2;
	  /* fall through */
	}else{
	  buf[0]=tmp;
	  return '?';
	}
      }
    }
  }
  if(inbuf==2){
    if(buf[0]!='?') ierror(0);
    if(buf[1]!='?'){
      inbuf=1;
      tmp=buf[0];
      buf[0]=buf[1];
      return tmp;
    }else{
      tmp2=tmp=read_char();;
      /* FIXME: only if on */
      if(tmp=='=') tmp2='#';
      else if(tmp=='(') tmp2='[';
      else if(tmp=='/') tmp2='\\';
      else if(tmp==')') tmp2=']';
      else if(tmp=='\'') tmp2='^';
      else if(tmp=='<') tmp2='{';
      else if(tmp=='!') tmp2='|';
      else if(tmp=='>') tmp2='}';
      else if(tmp=='-') tmp2='~';
      if(tmp!=tmp2){
	/* FIXME: warning */
	if(tmp2!='\\'){
	  inbuf=0;
	  return tmp2;
	}else{
	  inbuf=1;
	  buf[0]=tmp2;
	  return get_next_char();
	}
      }else{
	tmp=buf[0];
	buf[0]=buf[1];
	buf[1]=tmp2;
	return tmp;
      }
    }
  }
  ierror(0);
}

static int get_next_token(tokenlist *p)
{
  int c,z,tmp,*cnt;
  while(1){
    c=get_next_char();
    if(c=='"'||c=='\''){
      int term=c,escape=0;
      add_token(p,c);      
      z=p->next;
      add_token(p,0);
      while(1){
	c=get_next_char();
	/* FIXME */
	if(c==EOF||c=='\n'){
	  puts("unterminated string");
	  return 0;
	}
	if(c==term&&!escape) return 0;
	if(escape&&c=='\\')
	    escape=0;
	else
	    escape=(c=='\\');
	add_token(p,c);
	p->tokens[z]++;
      }
    }else if(c=='_'||isalpha((unsigned char)c)){
      add_token(p,TKIDENTIFIER);
      z=p->next;
      add_token(p,1);
      add_token(p,c);
      while((c=get_next_char())=='_'||isalnum((unsigned char)c)){
	add_token(p,c);
	p->tokens[z]++;
      }
      push_back=c;
      return 0;
    }else if(isdigit((unsigned char)c)){
      tmp=c;
      c=TKNUMBER;
    }else if(c=='.'){
      tmp=get_next_char();
      if(isdigit((unsigned char)tmp)){
	push_back=tmp;
	tmp='.';
	c=TKNUMBER;
      }else if(tmp=='.'){
	c=get_next_char();
	if(c!='.'){puts("..x-token");return 0;}/*FIXME*/
	add_token(p,TKDDD);
	return 0;
      }else
	push_back=tmp;
      /* fall through */
    }else if(c=='+'){
      c=get_next_char();
      if(c=='+'){
	add_token(p,TKPLUSPLUS);
      }else if(c=='='){
	add_token(p,TKPLUSEQ);
      }else{
	add_token(p,'+');
	push_back=c;
      }
      return 0;
    }else if(c=='-'){
      c=get_next_char();
      if(c=='-'){
	add_token(p,TKMINMIN);
      }else if(c=='='){
	add_token(p,TKMINEQ);
      }else if(c=='>'){
	add_token(p,TKSREF);
      }else{
	add_token(p,'-');
	push_back=c;
      }
      return 0;
    }else if(c=='|'){
      c=get_next_char();
      if(c=='|'){
	add_token(p,TKLOR);
      }else if(c=='='){
	add_token(p,TKOREQ);
      }else{
	add_token(p,'|');
	push_back=c;
      }
      return 0;
    }else if(c=='&'){
      c=get_next_char();
      if(c=='&'){
	add_token(p,TKLAND);
      }else if(c=='='){
	add_token(p,TKANDEQ);
      }else{
	add_token(p,'&');
	push_back=c;
      }
      return 0;
    }else if(c=='<'){
      c=get_next_char();
      if(c=='<'){
	c=get_next_char();
	if(c=='='){
	  add_token(p,TKLTLTEQ);
	}else{
	  add_token(p,TKLTLT);
	  push_back=c;
	}
      }else if(c=='='){
	add_token(p,TKLE);
      }else{
	add_token(p,'<');
	push_back=c;
      }
      return 0;
    }else if(c=='>'){
      c=get_next_char();
      if(c=='>'){
	c=get_next_char();
	if(c=='=')
	  add_token(p,TKGTGTEQ);
	else{
	  add_token(p,TKGTGT);
	  push_back=c;
	}
      }else if(c=='='){
	add_token(p,TKGE);
      }else{
	add_token(p,'>');
	push_back=c;
      }
      return 0;
    }else if(c=='!'){
      c=get_next_char();
      if(c=='='){
	add_token(p,TKNE);
      }else{
	add_token(p,'!');
	push_back=c;
      }
      return 0;
    }else if(c=='*'){
      c=get_next_char();
      if(c=='='){
	add_token(p,TKMULEQ);
      }else{
	add_token(p,'*');
	push_back=c;
      }
      return 0;
    }else if(c=='%'){
      c=get_next_char();
      if(c=='='){
	add_token(p,TKMODEQ);
      }else{
	add_token(p,'%');
	push_back=c;
      }
      return 0;
    }else if(c=='#'){
      c=get_next_char();
      if(c=='#'){
	add_token(p,TKNUMNUM);
      }else{
	add_token(p,'#');
	push_back=c;
      }
      return 0;
    }else if(c=='/'){
      /*FIXME*/
      c=get_next_char();
      if(c=='*'){
	int cstate=0;
	while(1){
	  c=get_next_char();
	  if(c=='*'){
	    if(cstate==2) /*FIXME*/;
	    cstate=1;
	  }else if(c=='/'){
	    if(cstate==1) break;
	    cstate=2;
	  }else cstate=0;
	}
	if(p->next>=1&&p->tokens[p->next-1]!=' '){
	    add_token(p,' ');
	    return 0;
	}else
	    continue;
      }else if(c=='/'/*FIXME*/){
	while((c=get_next_char())!=EOF&&c!='\n');
	if(c==EOF) ierror(0); /*FIXME*/
	add_token(p,'\n');
	return 0;
      }else if(c=='='){
	add_token(p,TKDIVEQ);
	return 0;
      }
      push_back=c;
      c='/';
      /* fall through */
    }
    if(c==TKNUMBER){
      add_token(p,TKNUMBER);
      z=p->next;
      add_token(p,1);
      add_token(p,tmp);
      while(1){
	c=get_next_char();
	if(c=='e'||c=='E'||/*FIXME?*/c=='p'||c=='P'){
	  add_token(p,c);
	  p->tokens[z]++;
	  tmp=get_next_char();
	  if(tmp=='+'||tmp=='-'){
	    add_token(p,tmp);
	    p->tokens[z]++;
	  }else{
	    push_back=tmp;
	  }
	}else if(c=='.'||c=='_'||isalnum((unsigned char)c)){
	  add_token(p,c);
	  p->tokens[z]++;
	}else{
	  push_back=c;
	  break;
	}
      }
      return 0;
    }
    if(c==EOF||c=='\n'){
      add_token(p,c);
      return c;
    }
    if(!isspace((unsigned char)c)){
      add_token(p,c);
      return 0;
    }else{
      if(p->next>=1&&p->tokens[p->next-1]!=' ')
	add_token(p,' ');
      return 0;
    }
  }
}

void print_tokenlist(FILE *f,tokenlist *p)
{
  int i,*tr=p->tokens,*end=tr+p->next;
  while(tr<end){
    if(*tr=='"'||*tr=='\''||*tr==TKIDENTIFIER||*tr==TKNUMBER){
      int n;
      if(*tr=='"')
	fprintf(f,"STRING: ");
      else if(*tr=='\'')
	fprintf(f,"CCONST: ");
      else if(*tr==TKNUMBER)
	fprintf(f,"NUMBER: ");
      else
	fprintf(f,"IDENT: ");
      tr++;
      n=*tr++;
      for(i=0;i<n;i++)
	fprintf(f,"%c",*tr++);
      fprintf(f,"\n");
      continue;
    }
    if(*tr==TKMARG){
      fprintf(f,"ARG %d\n",tr[1]);
      tr+=2; continue;
    }
    if(*tr==EOF){
      fprintf(f,"EOF\n");
      tr++; continue;
    }
    if(*tr=='\n'){
      fprintf(f,"NEWLINE\n");
      tr++; continue;
    }
    if(*tr>=TKPLUSPLUS&&*tr<=TKNUMNUM){
      fprintf(f,"%s\n",desc[*tr-TKKEYWORD]);
      tr++; continue;
    }
    fprintf(f,"OTHER: %d(%c)\n",*tr,*tr);
    tr++;
  }
}

void print_code(FILE *f,tokenlist *p)
{
  int i,*tr=p->tokens,*end=tr+p->next;
  while(tr<end){
    if(*tr=='"'||*tr=='\''||*tr==TKIDENTIFIER||*tr==TKNUMBER){
      int n,c;
      if(*tr==TKIDENTIFIER||*tr==TKNUMBER)
	c=0;
      else
	c=*tr;
      tr++;
      if(c)
	  fprintf(f,"%c",c);
      n=*tr++;
      for(i=0;i<n;i++)
	fprintf(f,"%c",*tr++);
      if(c)
	fprintf(f,"%c",c);
      continue;
    }
    if(*tr==EOF){
	return;
    }
    if(*tr=='\n'){
      fprintf(f,"\n");
      tr++; continue;
    }
    if(*tr>=TKPLUSPLUS&&*tr<=TKNUMNUM){
      fprintf(f,"%s",tokout[*tr-TKKEYWORD]);
      tr++; continue;
    }
    fprintf(f,"%c",*tr);
    tr++;
  }
}

char *token2string(int *p)
{
  int n=p[1];char *s;
  static char *buf,len;
  if(n+1>len){
    len=n+1;
    buf=mymalloc(len);
  }
  s=buf;
  p+=2;
  while(--n>=0) *s++=*p++;
  *s=0;
  return buf;
}

#ifndef OBFUSCATOR

static int *get_macro_arg(tokenlist *arg,int *p,int vararg)
{
  int par=0;
  while(1){
    if(*p==EOF||*p=='\n') puts("unterminated macro invocation"); /*FIXME*/
    if(*p==','&&par==0&&!vararg) return p;
    if(*p==')'){
      if(par>0){
	add_token(arg,*p++);
	par--;
	continue;
      }else
	return p;
    }
    if(*p==TKIDENTIFIER||*p==TKNUMBER||*p=='"'||*p=='\''){
      int n;
      add_token(arg,*p++);
      n=*p;
      add_token(arg,*p++);
      while(--n>=0) add_token(arg,*p++);
      continue;
    }
    if(*p=='(') par++;
    add_token(arg,*p++);
  }
}

void stringize(tokenlist *new,tokenlist *src)
{
  int z,f,n;
  int *p=src->tokens,*end=p+src->next;
puts("1");
  add_token(new,'"');
  z=new->next;
  add_token(new,0);
  while(p<end){
puts("2");
    f=*p;
    if(f==TKIDENTIFIER||f==TKNUMBER||f=='"'||f=='\''){
      if(f=='"'){
	add_token(new,'\\');
	add_token(new,'"');
	new->tokens[z]+=2;
      }else if(f=='\''){
	add_token(new,'\'');
	new->tokens[z]++;
      }
      p++;
      n=*p++;
      while(--n>=0){
	if((f=='"'||f=='\'')&&(*p=='\\'||*p=='"')){
	  add_token(new,'\\');
	  new->tokens[z]++;
	}
	add_token(new,*p++);
	new->tokens[z]++;
      }
      continue;
    }
puts("3b");
    add_token(new,*p++);
    new->tokens[z]++;
  }
}

int expand(tokenlist *dest,tokenlist *old,tokenlist *scratch1,tokenlist *scratch2)
{
  int n,i,*p,*end;
  int did_replace=0,notdone;
  tokenlist *new=scratch1,*src=old,t1={0},t2={0};
  do{
puts("starting expand iteration");
    notdone=0;
    p=src->tokens;
    end=p+src->next;
    while(p<end){
      if(*p==TKIDENTIFIER||*p==TKNUMBER||*p=='"'||*p=='\''){
	if(*p==TKIDENTIFIER&&(n=find_macro(token2string(p)))!=0){
	  tokenlist *repl=&macro_table[n].repl;
	  if(!(macro_table[n].flags&NEED_PAREN)){
	    n=repl->next;
	    for(i=0;i<n;i++) add_token(new,repl->tokens[i]);
	    p+=p[1]+2;
	    did_replace=notdone=1;
	    continue;
	  }else if(p[p[1]+2]=='('){
	    tokenlist **args=mymalloc((macro_table[n].nargs+1)*sizeof(*args));
	    /*	  tokenlist *scratch=mymalloc(sizeof(*scratch));*/
	    int *r,*end;
	    p+=p[1]+3;
	    for(i=0;i<macro_table[n].nargs;i++){
	      if(i!=0){
		if(*p==',') p++; else puts(", expect");/*FIXME*/
	      }
	      args[i]=mymalloc(sizeof(**args));
	      args[i]->size=0;
	      args[i]->next=0;
	      args[i]->tokens=0;
	      p=get_macro_arg(args[i],p,0);
	      printf("arg %d:\n",i);
	      print_tokenlist(stdout,args[i]);
	    }
	    if(macro_table[n].flags&VARARG_MACRO){
	      args[i]=mymalloc(sizeof(**args));
	      args[i]->size=0;
	      args[i]->next=0;
	      args[i]->tokens=0;
	      if(*p==',') p++; else puts(", expect");/*FIXME*/
	      p=get_macro_arg(args[i],p,1);
	      printf("vararg:\n");
	      print_tokenlist(stdout,args[i]);
	    }
	    if(*p==')') p++; else puts(") expect");/*FIXME*/
	    
	    r=macro_table[n].repl.tokens;
	    end=r+macro_table[n].repl.next;
	    while(r<end){
	      if(*r==TKIDENTIFIER||*r==TKNUMBER||*r=='"'||*r=='\''){
		n=r[1];
		puts("insert idnt");
		r+=2;
		while(--n>=0) add_token(new,*r++);
		continue;
	      }
	      if(*r=='#'){
		r++;
		if(*r!=TKMARG) {puts("huch?");ierror(0);/*FIXME*/}
		r++;
		n=*r++;
		stringize(new,args[n]);
		continue;
	      }
	      if(*r==TKMARG){
		t1.next=t2.next=0;
		expand(new,args[r[1]],&t1,&t2);
		r+=2;
		continue;
	      }
	      puts("insert other");
	      add_token(new,*r++);
	    }
	    
	    for(i=0;i<macro_table[n].nargs;i++) free(args[i]);
	    if(macro_table[n].flags&VARARG_MACRO) free(args[i]);
	    free(args);
	    did_replace=notdone=1;
	    continue;
	  }
	}
	add_token(new,*p++);
	n=*p++;
	add_token(new,n);
	for(i=0;i<n;i++) add_token(new,*p++);
      }else
	add_token(new,*p++);
    }
    if(!notdone){
puts("expand done, copying");
print_tokenlist(stdout,new);
      copy_tokenlist(dest,new);
puts("copy done");
    }else{
puts("need another iteration");
      src=new;
      if(new==scratch1)
	new=scratch2;
      else
	new=scratch1;
      new->next=0;
    }
  }while(notdone);
  return did_replace;
}

#endif /* OBFUSCATOR */

main()
{
  int i,lastline=line,*p;
  tokenlist tl={0},t2={0},t3={0},t4={0};
  infile=include_file(0);
#ifndef OBFUSCATOR  
  hash_table=mymalloc(PP_HSIZE*sizeof(*hash_table));
  for(i=0;i<PP_HSIZE;i++) hash_table[i]=0;
#endif  
  while(1){
    tl.next=0;
    while(!(i=get_next_token(&tl)));
    add_token(&tl,TKEOL);
    print_code(stdout,&tl);
    if(i==EOF){
	infile=close_file();
	if(!infile) exit(0);
    }
#ifndef OBFUSCATOR
    p=&tl.tokens[0];
    while(*p==' ') p++;
    if(*p=='#'&&(p[1]==TKIDENTIFIER||(p[1]==' '&&p[2]==TKIDENTIFIER))){
      char *s;
      p++;
      if(*p!=TKIDENTIFIER) p++;
      s=token2string(p);
      if(!strcmp(s,"include")){
	puts("include found");
	*p=TKNUMBER; /* prevent expansion */
	p+=2+p[1];
	i=p-&tl.tokens[0];
	t2.next=0;
	t3.next=0;
	t4.next=0;
	expand(&t2,&tl,&t3,&t4);
	p=&t2.tokens[i];
	while(*p==' ') p++;
printf("*p=%d\n",*p);
	if(*p=='\"'){
	  s=token2string(p);
	  infile=include_file(s);
	  if(!infile) infile=close_file();
	  if(!infile) {exit(EXIT_FAILURE);/*FIXME*/}
	}
	
	continue;
      }else if(!strcmp(s,"define")){
	puts("define found");
	p+=2+p[1];
	while(*p==' ') p++;
	if(*p!=TKIDENTIFIER){
	  puts("#define needs ID");
	}else{
	  char *mname;
	  int flags=0,nargs=0;
	  static char **pnames;
	  static int pnsize;

	  s=token2string(p);
	  p+=2+p[1];
	  mname=mymalloc(strlen(s)+1);
	  strcpy(mname,s);
	  printf("id=%s\n",mname);
	  while(*p==' ') p++;
	  if(*p=='('){
	    puts("function-like");
	    p++;
	    while(*p==' ') p++;
	    while(*p==TKIDENTIFIER){
	      flags=0;
	      s=token2string(p);
	      p+=p[1]+2;
	      printf("parameter %s\n",s);
	      if(nargs>=pnsize){
		pnsize+=PP_MSIZE;
		pnames=myrealloc(pnames,pnsize*sizeof(*pnames));
	      }
	      for(i=0;i<nargs;i++)
		if(!strcmp(pnames[i],s)) puts("double parameter");
	      pnames[nargs]=mymalloc(strlen(s)+1);
	      strcpy(pnames[nargs],s);
	      nargs++;
	      while(*p==' ') p++;
	      if(*p==','){ p++;flags=1;}
	      while(*p==' ') p++;
	    }
	    if(flags){
	      flags=NEED_PAREN;
	      if(*p==TKDDD){p++;flags|=VARARG_MACRO;}else puts(".../,");/*FIXME*/
	      while(*p==' ') p++;
	      if(*p==')') p++; else puts(") expect");/*FIXME*/
	    }else{
	      flags=NEED_PAREN;
	      if(*p==')') p++; else puts(") expect");/*FIXME*/
	    }
	  }
	  /*FIXME: # must be followed by parameter */
	  t2.next=0;
	  while(*p==' ') p++;
	  while(p<tl.tokens+tl.next&&*p!='\n'){
	    if(*p==TKIDENTIFIER){
	      s=token2string(p);
	      for(i=0;i<nargs;i++){
		if(!strcmp(pnames[i],s)) break;
	      }
	      if(i<nargs||((flags&VARARG_MACRO)&&!strcmp("__VA_ARGS__",s))){
		p+=p[1]+2;
		add_token(&t2,TKMARG);
		add_token(&t2,i);
		continue;
	      }
	    }
	    add_token(&t2,*p++);
	  }
	  for(i=0;i<nargs;i++) free(pnames[i]);
	  i=new_macro(mname,flags,nargs,&t2);
	  add_macro(i);
	  printf("replacement-list for %s\n",mname);
	  print_tokenlist(stdout,&t2);
	}
      }
    }else{
      t2.next=0;
      t3.next=0;
      t4.next=0;
      expand(&t2,&tl,&t3,&t4);
      /*      print_tokenlist(stdout,&t2);*/
      print_code(stdout,&t2);
    }
#endif /* OBFUSCATOR */    
  }
}
