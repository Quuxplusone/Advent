#include "minicomp.h"

#define clone_type clone_typ
#define free_type freetyp
#define new_icode new_IC
#define add_icode add_IC
#define free_all_icode() free_IC(first_ic)
#define get_first_icode() first_ic
typedef struct IC icode;

static int errors;
var *store_locals;

struct rpair rp;

char *copyright="minicomp/vbcc (c) in 2002 Volker Barthelmann";
char *inname,*outname;
struct struct_declaration *first_sd;
struct Var *first_ext,*merk_varf;
char **target_macros;

FILE *out;

void raus(void)
{
  var *v;
  for(v=first_var[0];v;v=v->next)
    if(v->type->flags!=FUNCTION){
      gen_align(out,falign(v->type));
      gen_var_head(out,v->vbccvar);
      gen_ds(out,szof(v->type),v->type);
    }

  cleanup_cg(out);

  if(errors>0)
    exit(EXIT_FAILURE);
  else
    exit(EXIT_SUCCESS);
}

main(int argc,char **argv)
{
  int i,j,flag;
  type *nt;

  for(i=1;i<argc;i++){
    if(*argv[i]!='-'){
      if(inname)
	error(18,"multiple input files");
      inname=argv[i];
    }else{
      flag=0;
      if((!strcmp("-o",argv[i]))&&i<argc-1){
	flag=1;
	outname=argv[++i];
	continue;
      }
      if(!strncmp("-debug=",argv[i],7)){
	flag=1;
	sscanf(argv[i]+7,"%d",&j);
	DEBUG=j;
	continue;
      }
      if(!strncmp("-unroll-size=",argv[i],13)){
	flag=1;
	sscanf(argv[i]+13,"%d",&j);
	unroll_size=j;
	continue;
      }
      if(!strncmp("-inline-size=",argv[i],13)){
	flag=1;
	sscanf(argv[i]+13,"%d",&j);
	inline_size=j;
	continue;
      }
      if(!strncmp("-maxoptpasses=",argv[i],14)){
	flag=1;
	sscanf(argv[i]+14,"%d",&j);
	maxoptpasses=j;
	continue;
      }
      for(j=0;j<MAXGF&&flag==0;j++){
        size_t l;
        if(!g_flags_name[j]) continue;
        l=strlen(g_flags_name[j]);
        if(l>0&&!strncmp(argv[i]+1,g_flags_name[j],l)){
          flag=1;
          if((g_flags[j]&(USEDFLAG|FUNCFLAG))==USEDFLAG){error(19,"multiple option");break;}
	  g_flags[j]|=USEDFLAG;
          if(g_flags[j]&STRINGFLAG){
            if(argv[i][l+1]!='='){error(21,"string expected");}
            if(argv[i][l+2]||i>=argc-1)
              g_flags_val[j].p=&argv[i][l+2];
            else
              g_flags_val[j].p=&argv[++i][0];
          }
          if(g_flags[j]&VALFLAG){
            if(argv[i][l+1]!='='){error(20,"value expected");}
            if(argv[i][l+2]||i>=argc-1)
              g_flags_val[j].l=atol(&argv[i][l+2]);
            else
              g_flags_val[j].l=atol(&argv[++i][0]);
          }
          if(g_flags[j]&FUNCFLAG) g_flags_val[j].f(&argv[i][l+1]);
	}
      }
      if(!flag) error(23,"unknown option");
    }
  }

  if(!inname)
    error(22,"no input file");
  infile=fopen(inname,"r");
  if(!infile)
    error(18,"could not open output file");
  if(!outname)
    error(23,"no output file");
  out=fopen(outname,"w");
  if(!out){
    error(24,"could not open input file");
  }
  nt=new_type(FUNCTION);
  nt->next=new_type(INT);
  add_var("readInt",clone_type(nt));
  add_var("writeInt",clone_type(nt));
  add_var("writeChar",clone_type(nt));
  add_var("readChar",clone_type(nt));
  add_var("writeReal",nt);
  nt=new_type(FUNCTION);
  nt->next=new_type(REAL);
  add_var("readReal",nt);

  stackalign=l2zm(0L);
  if(!init_cg()) exit(EXIT_FAILURE);
  if(zmeqto(stackalign,l2zm(0L)))
    stackalign=maxalign; 
  for(i=0;i<EMIT_BUF_DEPTH;i++)
    emit_buffer[i]=mymalloc(EMIT_BUF_LEN);
  emit_p=emit_buffer[0];

  multiple_ccs=0;
  optflags=-1;
  maxoptpasses=100;

  yyparse();

  raus();
}

yyerror(char *s)
{
  error(1,s);
}

void error(int n,...)
{
  if(n==170||n==172) return;
  errors++;
  fprintf(stderr,"error %d\n",n);
  raus();
}

void *getmem(size_t s)
{
  void *p=malloc(s);
  if(!p){
    error(2,"out of memory");
  }
  return p;
}

#define LABELNESTING 1024
static int labelstack[LABELNESTING],labelidx;

void push_int(int l)
{
  labelstack[labelidx++]=l;
}

int pop_int()
{
  return labelstack[--labelidx];
}

#define NAMENESTING 1024
static char *namestack[NAMENESTING],nameidx;

void push_ptr(void *ptr)
{
  namestack[nameidx++]=ptr;
}

void *pop_ptr()
{
  return namestack[--nameidx];
}


char *typename[]={"    ","int","real","array","function"};


type *new_type(int t)
{
  type *new=new_typ();
  new->flags=t;
  new->next=0;
  return new;
}

type *new_array(type *t,node *p)
{
  type *new=new_typ();
  simplify_tree(p);
  if(p->flags!=NNUMBER)
    error(3,"array size not a constant");
  if(p->type->flags!=INT)
    error(4,"array size not an integer");
  new->flags=ARRAY;
  new->next=t;
  new->size=l2zm((long)p->ivalue);
  free_tree(p);
  return new;
}

int nesting;
int local_offset,parm_offset,framesize;

#define MAXNESTING 128
var *first_var[128];

void free_varlist(var *v)
{
  var *m;
  while(v){
    free_var(v->vbccvar);
    m=v->next;
    free(v);
    v=m;
  }
}

void enter_block()
{
  nesting++;
  first_var[nesting]=0;
  push_int(local_offset);
}

void leave_block()
{
  if(first_var[nesting]){
    var *p=first_var[nesting];
    while(p->next)
      p=p->next;
    p->next=store_locals;
    p->vbccvar->next=merk_varf;
    store_locals=first_var[nesting];
    merk_varf=store_locals->vbccvar;
  }
  nesting--;
  local_offset=pop_int();
}

void gen_func(var *v,icode *first,int framesize)
{
  pric(stdout,get_first_icode());
  vl1=first_var[0]?first_var[0]->vbccvar:0;
  vl2=first_var[1]?first_var[1]->vbccvar:0;
  vl3=merk_varf;
  optimize(optflags,v->vbccvar);
  memset(regs_modified,0,RSIZE);
  pric(stdout,get_first_icode());
  gen_code(out,get_first_icode(),v->vbccvar,max_offset);
}

void enter_func(char *name,type *p)
{
  type *t=new_type(FUNCTION);
  var *v;
  t->next=p;
  t->exact=getmem(sizeof(*t->exact));
  t->exact->count=0;
  v=add_var(name,t);
  local_offset=parm_offset=framesize=0;
  push_ptr(v);
  enter_block();
}

void leave_func()
{
  icode *p,*merk;
  leave_block();
  gen_func(pop_ptr(),get_first_icode(),framesize);
  free_all_icode();
  first_ic=last_ic=0;
  free_varlist(store_locals);
  merk_varf=0;
  store_locals=0;
}

var *find_var(char *p,int minnest)
{
  var *v;
  int i;

  for(i=nesting;i>=minnest;i--){
    for(v=first_var[i];v;v=v->next)
      if(!strcmp(v->name,p))
	return v;
  }
  return 0;
}

var *add_var(char *name,type *t)
{
  var *new=getmem(sizeof(*new));
  struct Var *vv;
  /*FIXME: add vbccvar */  
  if(find_var(name,nesting))
    error(5,"var %s already defined",name);
  new->nesting=nesting;
  new->name=add_string(name);
  new->type=t;
  if(*name==0){
    new->next=store_locals;
  }else{
    new->next=first_var[nesting];
  }
  if(nesting==1){
    /* parameter */
    new->offset=parm_offset;
    parm_offset+=szof(t);
  }
  if(nesting>1){
    /* local variable */
    new->offset=local_offset;
    local_offset+=szof(t);
    if(local_offset>framesize)
      framesize=local_offset;
  }    
  /* attach vbcc variable */
  vv=new_var();
  vv->vtyp=clone_typ(new->type);
  vv->identifier=add_string(name);
  vv->nesting=(*name!=0?nesting:2);
  vv->storage_class=(vv->nesting==0?EXTERN:AUTO);
  if(nesting==1)
    vv->offset=zmsub(l2zm(0L),zmadd(maxalign,l2zm(new->offset)));
  else
    vv->offset=l2zm((long)local_offset);
  new->vbccvar=vv;

  if(*name==0){
    store_locals=new;
    vv->next=merk_varf;
    merk_varf=vv;
  }else{
    vv->next=first_var[nesting]?first_var[nesting]->vbccvar:0;
    first_var[nesting]=new;
  }
  return new;
}

var *new_temp(int tflags)
{
  char tname[16];
  static int itmps,rtmps,ptmps;
  if(tflags==INT)
    sprintf(tname,"\0 itmp%d",++itmps);
  else if(tflags==REAL)
    sprintf(tname,"\0 rtmp%d",++rtmps);
  else if(tflags==POINTER)
    sprintf(tname,"\0 rtmp%d",++ptmps);
  else
    error(16,"internal");
  return add_var(tname,new_type(tflags));
}

struct Var *add_tmp_var(struct Typ *t)
{
  
  return new_temp(t->flags)->vbccvar;
}

char *add_string(char *s)
{
  char *new=getmem(strlen(s)+1);
  strcpy(new,s);
  return new;
}

char *nodename[]={
  "var","number","add","mul","sub","div","index","equals",
  "lt","gt","leq","geq","neq","and","or",
  "int2real","real2int","assign","call","argument"
};

node *number_node(void)
{
  node *new=getmem(sizeof(*new));
  new->flags=NNUMBER;
  if(strstr(tkname,".")){
    double rval;
    sscanf(tkname,"%lf",&rval);
    new->rvalue=rval;
    new->type=new_type(REAL);
    new->left=new->right=0;
  }else{
    new->ivalue=atoi(tkname);
    new->type=new_type(INT);
    new->left=new->right=0;
  }
  return new;
}

node *var_node(void)
{
  var *v=find_var(tkname,0);
  node *new;
  if(!v){
    error(6,"unknown identifier: %s",tkname);
  }
  new=getmem(sizeof(*new));
  new->flags=NVARIABLE;
  new->var=v;
  new->type=clone_type(v->type);
  new->left=new->right=0;
  return new;
}


node *binary_node(enum nodeflags flags,node *left,node *right)
{
  node *new=getmem(sizeof(*new));
  new->flags=flags;
  new->left=left;
  new->right=right;
  new->type=0;
  if(!left||(flags!=NCALL&&!right))
    error(7,"internal error");
  if(flags==NINDEX){
    if(left->type->flags!=ARRAY)
      error(8,"operand of [] must be array");
    new->type=clone_type(left->type->next);
  }else if(flags==NARG){
    /* nothing to do? */
  }else if(flags==NCALL){
    if(left->type->flags!=FUNCTION)
      error(9,"only functions can be called");
    new->type=clone_type(left->type->next);
  }else{
    if(left->type->flags==ARRAY||right->type->flags==ARRAY)
      error(10,"both operands must be real or int");
    if(left->type->flags==REAL||right->type->flags==REAL){
      new->type=new_type(REAL);
      if(left->type->flags==INT){
	new->left=conv_tree(left,REAL);
      }
      if(right->type->flags==INT){
	new->right=conv_tree(right,REAL);
      }      
    }else
      new->type=new_type(INT);
  }
  return new;
}

void print_tree(node *p)
{
  printf("%s(",nodename[p->flags]);
  if(p->left)
    print_tree(p->left);
  if(p->right){
    printf(",");
    print_tree(p->right);
  }
  if(p->flags==NNUMBER){
    if(p->type->flags==INT)
      printf("%d[int]",p->ivalue);
    else
      printf("%g[real]",p->rvalue);
  }
  if(p->flags==NVARIABLE)
    printf("%s[%s]",p->var->name,typename[p->var->type->flags]);
  printf(")");
}

node *conv_tree(node *p,int tflags)
{
  node *new;
  if(p->type->flags==tflags)
    return p;
  new=getmem(sizeof(*new));
  if(tflags==REAL)
    new->flags=NI2R;
  else
    new->flags=NR2I;
  new->type=new_type(p->type->flags);
  new->left=p;
  new->right=0;
  return new;
}

void free_tree(node *p)
{
  if(p->left)
    free_tree(p->left);
  if(p->right)
    free_tree(p->right);
  if(p->type)
    free_type(p->type);
  free(p);
}


static void const_node(node *p,int val)
{
  p->flags=NNUMBER;
  p->ivalue=val;
  if(p->left)
    free_tree(p->left);
  if(p->right)
    free_tree(p->right);
  p->left=p->right=0;
}

void simplify_tree(node *p)
{
  if(p->left)
    simplify_tree(p->left);
  if(p->right)
    simplify_tree(p->right);
  if(p->type&&p->type->flags==INT&&p->left&&p->left->flags==NNUMBER&&p->right&&p->right->flags==NNUMBER){
    switch(p->flags){
    case NADD: const_node(p,p->left->ivalue+p->right->ivalue); break;
    case NMUL: const_node(p,p->left->ivalue*p->right->ivalue); break;
    case NSUB: const_node(p,p->left->ivalue-p->right->ivalue); break;
    case NDIV: 
      if(p->right->ivalue!=0)
	const_node(p,p->left->ivalue/p->right->ivalue); break;
    }
  }
}

void assign_statement(node *left,node *right)
{
  if(left->type->flags==ARRAY||right->type->flags==ARRAY)
    error(11,"array type in assignment");
  if(left->type->flags==FUNCTION||right->type->flags==FUNCTION)
    error(12,"function type in assignment");

  right=conv_tree(right,left->type->flags);
  simplify_tree(left);
  simplify_tree(right);
  if(left->flags==NINDEX){
    icode *n1,*n2,*n3;
    n1=new_icode();
    n1->code=MULT;
    n1->typf=INT;
    n1->q1=*gen_tree(left->right);
    n1->q2.flags=KONST;
    n1->q2.val.vint=zm2zi(sizetab[INT]);
    n1->z.flags=VAR;
    n1->z.v=new_temp(INT)->vbccvar;
    n1->z.val.vmax=l2zm(0L);
    n2=new_icode();
    n2->code=ADDI2P;
    n2->typf=INT;
    n2->typf2=POINTER;
    n2->q1=*gen_tree(left->left);
    n2->q2=n1->z;
    n2->z.flags=VAR;
    n2->z.v=new_temp(POINTER)->vbccvar;
    n2->z.val.vmax=l2zm(0L);
    n3=new_icode();
    n3->code=ASSIGN;
    n3->typf=left->type->flags;
    n3->q1=*gen_tree(right);
    n3->z=n2->z;
    n3->z.flags|=DREFOBJ;
    n3->z.dtyp=POINTER;
    n3->q2.val.vmax=szof(left->type);
    add_icode(n1);
    add_icode(n2);
    add_icode(n3);
#if 0    
    /* leicht anderes Format */
    new->flags=CSTORE;
    new->op1=*gen_tree(right);
    new->op2=*gen_tree(left->left);
    new->dest=*gen_tree(left->right);
#endif
  }else{
    icode *new=new_icode();
    new->code=ASSIGN;
    new->q1=*gen_tree(right);
    new->z=*gen_tree(left);
    new->q2.val.vmax=szof(left->type);
    new->typf=left->type->flags;
    add_icode(new);
  }
  free_tree(left);
  free_tree(right);
}

void return_statement(node *p)
{
  icode *new=new_icode();
  p=conv_tree(p,INT); /*FIXME*/
  simplify_tree(p);
  new->code=SETRETURN;
  new->typf=p->type->flags;
  new->q1=*gen_tree(p);
  new->z.reg=freturn(p->type);
  add_icode(new);
  free_tree(p);
}

void while_statement(node *p)
{
  int loop=++label,exit=++label;
  simplify_tree(p);
  gen_cond(p,loop,exit);
  gen_label(loop);
  last_ic->flags|=LOOP_COND_TRUE;
  push_ptr(p);
  push_int(loop);
  push_int(exit);
#if 0
  gen_label(loop);
  gen_cond(p,start,exit);
  free_tree(p);
  gen_label(start);
  push_int(loop);
  push_int(exit);
#endif
}

void while_end()
{
  int loop,exit;
  node *p;
  exit=pop_int();
  loop=pop_int();
  p=pop_ptr();
  gen_cond(p,loop,exit);
  gen_label(exit);
#if 0
  new=new_icode();
  new->code=BRA;
  new->typf=loop;
  add_icode(new);
  gen_label(exit);
#endif
}

void if_statement(node *p)
{
  int true=++label,false=++label;
  simplify_tree(p);
  gen_cond(p,true,false);
  free_tree(p);
  gen_label(true);
  push_int(false);
}

void if_end()
{
  gen_label(pop_int());
}

void if_else()
{
  int endlabel=++label;
  icode *new=new_icode();
  new->code=BRA;
  new->typf=++label;
  add_icode(new);
  gen_label(pop_int());
  push_int(label);
}

int label;

operand *gen_tree(node *p)
{
  icode *new;
  static operand op;

  if(p->flags==NVARIABLE){
    if(p->var->type->flags==ARRAY){
      if(p->var->nesting==0){
	op.flags=VAR|VARADR;
	op.v=p->var->vbccvar;
	op.val.vmax=l2zm(0L);
	return &op;
      }else{
	icode *new=new_icode();
	new->flags=ADDRESS;
	new->q1.flags=VAR;
	new->q1.v=p->var->vbccvar;
	new->q1.val.vmax=l2zm(0L);
	new->z.flags=VAR;
	new->z.v=new_temp(POINTER)->vbccvar;
	new->z.val.vmax=l2zm(0L);
	add_icode(new);
	return &new->z;
      }
    }else{
      op.flags=VAR;
      op.v=p->var->vbccvar;
      op.val.vmax=l2zm(0L);
      return &op;
    }
  }

  if(p->flags==NNUMBER){
    op.flags=KONST;
    if(p->type->flags==INT)
      op.val.vint=p->ivalue;
    else
      op.val.vdouble=zld2zd(d2zld(p->rvalue));
    return &op;
  }

  if(p->flags==NADD||p->flags==NMUL||p->flags==NSUB||p->flags==NDIV||p->flags==NI2R||p->flags==NR2I){
    int ttyp=p->type->flags;
    new=new_icode();
    switch(p->flags){
    case NADD: new->code=ADD;break;
    case NMUL: new->code=MULT;break;
    case NSUB: new->code=SUB;break;
    case NDIV: new->code=DIV;break;
    case NI2R: new->code=CONVERT;ttyp=REAL;new->typf2=INT;break;
    case NR2I: new->code=CONVERT;ttyp=INT;new->typf2=REAL;break;
    }
    new->q1=*gen_tree(p->left);
    if(p->right)
      new->q2=*gen_tree(p->right);
    new->z.flags=VAR;
    new->z.v=new_temp(ttyp)->vbccvar;
    new->z.val.vmax=l2zm(0L);
    new->typf=ttyp;
    add_icode(new);
    return &new->z;
  }

  if(p->flags==NINDEX){
    if(p->type->flags==ARRAY){
      icode *merk,*new=new_icode();
      new->flags=MULT;
      new->typf=INT;
      new->q1=*gen_tree(p->right);
      new->q2.flags=KONST;
      new->q2.val.vint=szof(p->type);
      new->z.flags=VAR;
      new->z.v=new_temp(INT)->vbccvar;
      new->z.val.vmax=l2zm(0L);
      add_icode(new);
      merk=new;
      new=new_icode();
      new->flags=ADDI2P;
      new->typf=INT;
      new->typf2=POINTER;
      new->q1=*gen_tree(p->left);
      new->q2=merk->z;
      new->z.flags=VAR;
      new->z.v=new_temp(POINTER)->vbccvar;
      new->z.val.vmax=l2zm(0L);
      op=new->z;
      add_icode(new);
      return &new->z;
    }else{
      icode *merk,*new=new_icode();
      new->code=MULT;
      new->typf=INT;
      new->q1=*gen_tree(p->right);
      new->q2.flags=KONST;
      new->q2.val.vint=zm2zi(sizetab[INT]);
      new->z.flags=VAR;
      new->z.v=new_temp(INT)->vbccvar;
      new->z.val.vmax=l2zm(0L);
      add_icode(new);
      merk=new;
      new=new_icode();
      new->code=ADDI2P;
      new->q1=*gen_tree(p->left);
      new->q2=merk->z;
      new->typf=INT;
      new->typf2=POINTER;
      new->z.flags=VAR;
      new->z.v=new_temp(POINTER)->vbccvar;
      new->z.val.vmax=l2zm(0L);
      op=new->z;
      op.flags|=DREFOBJ;
      op.dtyp=POINTER;
      add_icode(new);
      return &op;
    }
  }

  if(p->flags==NCALL){
    icode *new=new_icode();
    new->code=CALL;
    new->typf=FUNKT;
    if(p->right)
      new->q2.val.vmax=l2zm(push_arg(p->right));
    else
      new->q2.val.vmax=l2zm(0L);
    new->q1=*gen_tree(p->left);
    add_icode(new);
    new=new_icode();
    new->code=GETRETURN;
    new->z.flags=VAR;
    new->typf=p->type->flags;
    new->z.v=new_temp(p->type->flags)->vbccvar;
    new->z.val.vmax=l2zm(0L);
    new->q1.reg=freturn(p->type);
    add_icode(new);
    return &new->z;
  }

  error(13,"operation not yet supported");
}

void gen_label(int l)
{
  icode *new=new_icode();
  new->code=LABEL;
  new->typf=l;
  add_icode(new);
}

void gen_cond(node *p,int true_label,int false_label)
{
  icode *new;
  if(p->flags==NAND){
    int tmp=++label;
    gen_cond(p->left,tmp,false_label);
    gen_label(tmp);
    gen_cond(p->right,true_label,false_label);
    return;
  }
  if(p->flags==NOR){
    int tmp=++label;
    gen_cond(p->left,true_label,tmp);
    gen_label(tmp);
    gen_cond(p->right,true_label,false_label);
    return;
  }
  new=new_icode();
  new->code=COMPARE;
  new->typf=p->type->flags;
  new->q1=*gen_tree(p->left);
  new->q2=*gen_tree(p->right);
  add_icode(new);
  new=new_icode();
  switch(p->flags){
  case NEQUALS: new->code=BEQ;break;
  case NNEQ: new->code=BNE;break;
  case NLT: new->code=BLT;break;
  case NGT: new->code=BGT;break;
  case NLEQ: new->code=BLE;break;
  case NGEQ: new->code=BGE;break;
  default: error(14,"internal");
  }
  new->typf=true_label;
  add_icode(new);

  new=new_icode();
  new->code=BRA;
  new->typf=false_label;
  add_icode(new);
}

int push_arg(node *p)
{
  if(p->flags==NARG){
    int s;
    s=push_arg(p->right);
    return s+push_arg(p->left);
  }else{
    icode *new=new_icode();
    if(p->type->flags==ARRAY)
      error(15,"arrays cannot be passed");
    new->code=PUSH;
    new->q2.val.vmax=szof(p->type);
    new->z.val.vmax=new->q2.val.vmax;
    new->typf=p->type->flags;
    new->q1=*gen_tree(p);
    add_icode(new);
    return szof(p->type);
  }
}

void free_var(struct Var *v)
{
  free(v);
  /*FIXME*/
}

void add_IC(icode *new)
{
  if(last_ic&&new->code==ASSIGN&&new->q1.flags==VAR&&last_ic->z.flags==VAR&&
     new->q1.v==last_ic->z.v&&zmeqto(new->q1.val.vmax,last_ic->z.val.vmax)&&
     *new->q1.v->identifier==0){
    last_ic->z=new->z;
    free(new);
    return;
  }
  insert_IC(last_ic,new);
}
