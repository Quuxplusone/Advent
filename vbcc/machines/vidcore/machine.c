/*  Backend for VideoCore IV
    (c) in 2013 by Volker Barthelmann
*/

/* TODO:
   - extended registers
   - floating point
   - long long
   - memcpy
   - addcmp
   - bcc reg
   - cond. ins
   - vector
   - non-pc addressing
*/

#include "supp.h"

static char FILE_[]=__FILE__;

/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[]="vbcc VideoCore IV code-generator V0.1 (c) in 2013 by Volker Barthelmann";

/* the flag-name, do not use names beginning with l, L, I, D or U, because
   they collide with the frontend */
char *g_flags_name[MAXGF]={"use-commons","use-framepointer",
			   "short-double","one-section",
			   "no-delayed-popping"};

int g_flags[MAXGF]={0,0,
		    0};

/* the results of parsing the command-line-flags will be stored here */
union ppi g_flags_val[MAXGF];

/*  Alignment-requirements for all types in bytes.              */
zmax align[MAX_TYPE+1];

/*  Alignment that is sufficient for every object.              */
zmax maxalign;

/*  CHAR_BIT for the target machine.                            */
zmax char_bit;

/*  sizes of the basic types (in bytes) */
zmax sizetab[MAX_TYPE+1];

/*  Minimum and Maximum values each type can have.              */
/*  Must be initialized in init_cg().                           */
zmax t_min[MAX_TYPE+1];
zumax t_max[MAX_TYPE+1];
zumax tu_max[MAX_TYPE+1];

/*  Names of all registers. will be initialized in init_cg(),
    register number 0 is invalid, valid registers start at 1 */
char *regnames[MAXR+1];

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  a type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1];

/* specifies the priority for the register-allocator, if the same
   estimated cost-saving can be obtained by several registers, the
   one with the highest priority will be used */
int reg_prio[MAXR+1];

/* an empty reg-handle representing initial state */
struct reg_handle empty_reg_handle={0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[]={"__interrupt",0};


/****************************************/
/*  Private data and functions.         */
/****************************************/

/* addressing modes */
#define AM_IMM_IND 1
#define AM_GPR_IND 2
#define AM_POSTINC 3
#define AM_PREDEC  4

#define USE_COMMONS (g_flags[0]&USEDFLAG)
#define USE_FP      (g_flags[1]&USEDFLAG)
#define SHORT_DOUBLE (g_flags[2]&USEDFLAG)
#define ONESEC      (g_flags[3]&USEDFLAG)
#define NODELAYEDPOP (g_flags[4]&USEDFLAG)


/* alignment of basic data-types, used to initialize align[] */
static long malign[MAX_TYPE+1]=  {1,1,2,4,4,4,4,8,8,1,4,1,1,1,4,1};
/* sizes of basic data-types, used to initialize sizetab[] */
static long msizetab[MAX_TYPE+1]={1,1,2,4,4,8,4,8,8,0,4,0,0,0,4,0};

/* used to initialize regtyp[] */
static struct Typ ltyp={LONG},ldbl={DOUBLE},lchar={CHAR};

/* macros defined by the backend */
static char *marray[]={"__section(x)=__vattr(\"section(\"#x\")\")",
		       "__VIDEOCORE__",
		       0, /* __SHORT_DOUBLE__ */
		       0};

/* special registers */
enum{
  r0=1,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,
  r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,
  r0r1,r2r3,r4r5,r6r7,r8r9,r10r11,r12r13,r14r15,
  r16r17,r18r19,r20r21,r22r23,r24r25,r26r27,r28r29,r30r31
};

static int sd=r24;
static int sp=r25;
static int lr=r26;
static int sr=r30;
static int pc=r31;
static int t1=r15;
static int t2=r14;

static int last_saved;

#define dt(t) (((t)&UNSIGNED)?udt[(t)&NQ]:sdt[(t)&NQ])
#define sdt(t) (udt[(t)&NQ])
static char *sdt[MAX_TYPE+1]={"??","b","hs","","","ll","","d","ld","v",""};
static char *udt[MAX_TYPE+1]={"??","b","h","","","ull","","d","ld","v",""};

static char *dct[]={"","byte","2byte","4byte","4byte","4byte","4byte","4byte","4byte"};

static int no_suffix;

/* sections */
#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define SPECIAL 4

static long stack;
static int stack_valid;
static int section=-1,newobj;
static char *codename="\t.text\n",
  *dataname="\t.data\n",
  *bssname="",
  *rodataname="\t.section\t.rodata\n";

/* conditional execution */
static char *cc;

/* return-instruction */
static char *ret;

/* label at the end of the function (if any) */
static int exit_label;

/* assembly-prefixes for labels and external identifiers */
static char *labprefix="l",*idprefix="_";

#if 0
/* variables to calculate the size and partitioning of the stack-frame
   in the case of FIXED_SP */
static long frameoffset,pushed,maxpushed,framesize;
#else
/* variables to keep track of the current stack-offset in the case of
   a moving stack-pointer */
static long notpopped,dontpop,stackoffset,maxpushed;
#endif

static long localsize,rsavesize,argsize;

static void emit_obj(FILE *f,struct obj *p,int t);

/* calculate the actual current offset of an object relativ to the
   stack-pointer; we use a layout like this:
   ------------------------------------------------
   | arguments to this function                   |
   ------------------------------------------------
   | return-address [size=4]                      |
   ------------------------------------------------
   | caller-save registers [size=rsavesize]       |
   ------------------------------------------------
   | local variables [size=localsize]             |
   ------------------------------------------------
   | arguments to called functions [size=argsize] |
   ------------------------------------------------
   All sizes will be aligned as necessary.
   In the case of FIXED_SP, the stack-pointer will be adjusted at
   function-entry to leave enough space for the arguments and have it
   aligned to 16 bytes. Therefore, when calling a function, the
   stack-pointer is always aligned to 16 bytes.
   For a moving stack-pointer, the stack-pointer will usually point
   to the bottom of the area for local variables, but will move while
   arguments are put on the stack.

   This is just an example layout. Other layouts are also possible.
*/

static void push(long l)
{
  stackoffset-=l;
  if(stackoffset<maxpushed) 
    maxpushed=stackoffset;
  if(-maxpushed>stack)
    stack=-maxpushed;
}

static void pop(long l)
{
  stackoffset+=l;
}

static long real_offset(struct obj *o)
{
  long off=zm2l(o->v->offset);
  if(off<0){
    /* function parameter */
    off=localsize+rsavesize-off-zm2l(maxalign);
  }

  off-=stackoffset;

  off+=zm2l(o->val.vmax);
  return off;
}

/*  Initializes an addressing-mode structure and returns a pointer to
    that object. Will not survive a second call! */
static struct obj *cam(int flags,int base,long offset)
{
  static struct obj obj;
  static struct AddressingMode am;
  obj.am=&am;
  am.flags=flags;
  am.base=base;
  am.offset=offset;
  return &obj;
}

/* changes to a special section, used for __section() */
static int special_section(FILE *f,struct Var *v)
{
  char *sec;
  if(!v->vattr) return 0;
  sec=strstr(v->vattr,"section(");
  if(!sec) return 0;
  sec+=strlen("section(");
  emit(f,"\t.section\t");
  while(*sec&&*sec!=')') emit_char(f,*sec++);
  emit(f,"\n");
  if(f) section=SPECIAL;
  return 1;
}

static unsigned long pushmask;

static int ic_uses_reg(struct IC *p,int r,int onlyuses)
{
  if(reg_pair(r,&rp)){
    return ic_uses_reg(p,rp.r1,onlyuses)||ic_uses_reg(p,rp.r2,onlyuses);
  }
  /*FIXME: handle regpairs */
  if((p->q1.flags&REG)&&p->q1.reg==r) return 1;
  if((p->q2.flags&REG)&&p->q2.reg==r) return 1;
  if(onlyuses){
    if((p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->z.reg==r) return 1;
  }else{
    if((p->z.flags&REG)&&p->z.reg==r) return 1;
  }
  if(p->q1.am){
    if(p->q1.am->base==r) return 1;
    if(p->q1.am->flags==AM_GPR_IND&&p->q1.am->offset==r) return 1;
  }
  if(p->q2.am){
    if(p->q2.am->base==r) return 1;
    if(p->q2.am->flags==AM_GPR_IND&&p->q2.am->offset==r) return 1;
  }
  if(p->z.am){
    if(p->z.am->base==r) return 1;
    if(p->z.am->flags==AM_GPR_IND&&p->z.am->offset==r) return 1;
  }
  return 0;
}

static int reg_is_scratched(struct IC *p,int r)
{
  for(p=p->next;p;p=p->next){
    int c=p->code;
    if(c>=LABEL&&c<=BRA)
      return 0;
    if(c==FREEREG&&p->q1.reg==r)
      return 1;
    if(ic_uses_reg(p,r,1))
      return 0;
    if((p->q1.flags&(REG|DREFOBJ))==REG&&p->z.reg==r)
      return 1;
  }    
  return 0;
}

static int get_reg(FILE *f,struct IC *p)
{
  int r;
  for(r=r0;r<=r31;r++){
    if(!regs[r]&&regused[r]&&!ic_uses_reg(p,r,0)){
      regs[r]=1;
      return r;
    }
  }
  for(r=r0;r<=r31;r++){
    if(!regs[r]&&!ic_uses_reg(p,r,0)){
      regs[r]=1;
      return r;
    }
  }
  for(r=r0;r<=r31;r++){
    if(!ic_uses_reg(p,r,0))
      break;
  }
  argsize+=4;
  emit(f,"\tst\t%s,--(%s)\n",regnames[r],regnames[sp]);
  push(4);
  pushmask|=(1<<(r-1));
  regs[r]=1;
  return r;
}

static void free_reg(FILE *f,int r)
{
  if(pushmask&(1<<(r-1))){
    emit(f,"\tld\t%s,(%s)++\n",regnames[r],regnames[sp]);
    pop(4);
    argsize-=4;
  }else
    regs[r]=0;
  pushmask&=~(1<<(r-1));
}

/* generate code to load the address of a variable into register r */
static void load_address(FILE *f,int r,struct obj *o,int type)
/*  Generates code to load the address of a variable into register r.   */
{
  if(!(o->flags&VAR)) ierror(0);
  if(o->v->storage_class==AUTO||o->v->storage_class==REGISTER){
    long off=real_offset(o);
    emit(f,"\tadd%s\t%s,%s,%ld\n",cc,regnames[r],regnames[sp],off);
  }else{
    emit(f,"\tlea%s\t%s,",cc,regnames[r]);
    emit_obj(f,o,type);
    emit(f,"\n");
  }
}
/* Generates code to load a memory object into register r. tmp is a
   general purpose register which may be used. tmp can be r. */
static void load_reg(FILE *f,int r,struct obj *o,int type)
{
  type&=NU;
  if(reg_pair(r,&rp)) ierror(0);
  if(o->flags&VARADR){
    load_address(f,r,o,POINTER);
  }else if(o->flags&KONST){
    emit(f,"\tmov%s\t%s,",cc,regnames[r]);
    emit_obj(f,o,type);
    emit(f,"\n");
  }else{
    if((o->flags&(REG|DREFOBJ))==REG){
      if(o->reg!=r)
	emit(f,"\tmov%s\t%s,%s\n",cc,regnames[r],regnames[o->reg]);
    }else{
      if(SHORT_DOUBLE&&ISFLOAT(type)) type=FLOAT;
      emit(f,"\tld%s%s\t%s,",dt(type),cc,regnames[r]);
      emit_obj(f,o,type);
      emit(f,"\n");
    }
  }
}

/*  Generates code to store register r into memory object o. */
static void store_reg(FILE *f,int r,struct obj *o,int type)
{
  if(reg_pair(r,&rp)) ierror(0);
  type&=NQ;
  if(SHORT_DOUBLE&&ISFLOAT(type)) type=FLOAT;
  emit(f,"\tst%s%s\t%s,",sdt(type),cc,regnames[r]);
  emit_obj(f,o,type);
  emit(f,"\n");
}

/*  Yields log2(x)+1 or 0. */
static long pof2(zumax x)
{
  zumax p;int ln=1;
  p=ul2zum(1L);
  while(ln<=32&&zumleq(p,x)){
    if(zumeqto(x,p)) return ln;
    ln++;p=zumadd(p,p);
  }
  return 0;
}

static struct IC *preload(FILE *,struct IC *);

static void function_top(FILE *,struct Var *,long);
static void function_bottom(FILE *f,struct Var *,long);

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

static int q1reg,q2reg,zreg;

static char *ccs[]={"eq","ne","lt","ge","le","gt",""};
static char *ccu[]={"eq","ne","cs","cc","ls","hi",""};
static char *logicals[]={"or","eor","and"};
static char *arithmetics[]={"lsl","asr","add","sub","mul","divs","mod"};
static char *uarithmetics[]={"lsl","lsr","add","sub","mul","divu","mod"};
static char *farithmetics[]={"--","--","fadd","fsub","fmul","fdiv","--"};


/* Does some pre-processing like fetching operands from memory to
   registers etc. */
static struct IC *preload(FILE *f,struct IC *p)
{
  int r,load;

  if(isreg(q1))
    q1reg=p->q1.reg;
  else
    q1reg=0;

  if(isreg(q2))
    q2reg=p->q2.reg;
  else
    q2reg=0;

  if(isreg(z)){
    zreg=p->z.reg;
  }else{
    zreg=t1;
  }
  
  if((p->q1.flags&(DREFOBJ|REG))==DREFOBJ&&!p->q1.am){
    p->q1.flags&=~DREFOBJ;
    load_reg(f,t1,&p->q1,POINTER);
    p->q1.reg=t1;
    p->q1.flags|=(REG|DREFOBJ);
  }
  if(p->q1.flags&&!isreg(q1)&&p->code!=ASSIGN&&p->code!=PUSH&&p->code!=SETRETURN/*&&!(p->q1.flags&KONST)*/){
    q1reg=zreg;
    load_reg(f,q1reg,&p->q1,q1typ(p));
    p->q1.reg=q1reg;
    p->q1.flags=REG;
  }

  if((p->q2.flags&(DREFOBJ|REG))==DREFOBJ&&!p->q2.am){
    p->q2.flags&=~DREFOBJ;
    load_reg(f,t2,&p->q2,POINTER);
    p->q2.reg=t2;
    p->q2.flags|=(REG|DREFOBJ);
  }

  if(p->q2.flags){
    load=1;
    if(isreg(q2)) load=0;
    if((p->q2.flags&KONST)&&!ISFLOAT(p->typf)&&(p->code==ADD||p->code==SUB||p->code==ADDI2P||p->code==COMPARE||p->code==SUBIFP||p->code==SUBPFP))
      load=0;
    if(p->code>=OR&&p->code<=MOD&&(p->q2.flags&KONST)&&!ISFLOAT(p->typf)){
      unsigned long v;
      eval_const(&p->q2.val,q2typ(p));
      v=zum2ul(vumax);
      if(v<=31) load=0;
    }
    if(load){
      q2reg=t2;
      load_reg(f,q2reg,&p->q2,q2typ(p));
      p->q2.reg=q2reg;
      p->q2.flags=REG;
    }
  }
  return p;
}

/* save the result (in zreg) into p->z */
void save_result(FILE *f,struct IC *p)
{
  if((p->z.flags&(REG|DREFOBJ))==DREFOBJ&&!p->z.am){
    p->z.flags&=~DREFOBJ;
    load_reg(f,t2,&p->z,POINTER);
    p->z.reg=t2;
    p->z.flags|=(REG|DREFOBJ);
  }
  if(isreg(z)){
    if(p->z.reg!=zreg)
      emit(f,"\tmov%s\t%s,%s\n",cc,regnames[p->z.reg],regnames[zreg]);
  }else{
    store_reg(f,zreg,&p->z,ztyp(p));
  }
}

/* prints an object */
static void emit_obj(FILE *f,struct obj *p,int t)
{
  if((p->flags&DREFOBJ)&&p->am){
    if(p->am->flags==AM_GPR_IND) emit(f,"(%s,%s)",regnames[p->am->base],regnames[p->am->offset]);
    if(p->am->flags==AM_IMM_IND) emit(f,"%ld(%s)",p->am->offset,regnames[p->am->base]);
    if(p->am->flags==AM_POSTINC) emit(f,"(%s)++",regnames[p->am->base]);
    if(p->am->flags==AM_PREDEC) emit(f,"--(%s)",regnames[p->am->base]);
    return;
  }
  if((p->flags&(KONST|DREFOBJ))==(KONST|DREFOBJ)){
    emitval(f,&p->val,p->dtyp&NU);
    return;
  }
  if(p->flags&DREFOBJ) emit(f,"(");
  if(p->flags&REG){
    emit(f,"%s",regnames[p->reg]);
  }else if(p->flags&VAR) {
    if(p->v->storage_class==AUTO||p->v->storage_class==REGISTER)
      emit(f,"%ld(%s)",real_offset(p),regnames[sp]);
    else{
      if(!zmeqto(l2zm(0L),p->val.vmax)){emitval(f,&p->val,LONG);emit(f,"+");}
      if(p->v->storage_class==STATIC){
        emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      }else{
        emit(f,"%s%s",idprefix,p->v->identifier);
      }
      if(!no_suffix)
	emit(f,"(%s)",regnames[pc]);
    }
  }
  if(p->flags&KONST){
    if(ISFLOAT(t)){
      unsigned char *ip;
      eval_const(&p->val,t);
      ip=(unsigned char *)&vfloat;
      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
    }else
      emitval(f,&p->val,t&NU);
  }
  if(p->flags&DREFOBJ) emit(f,")");
}

/*  Test if there is a sequence of FREEREGs containing FREEREG reg.
    Used by peephole. */
static int exists_freereg(struct IC *p,int reg)
{
  while(p&&(p->code==FREEREG||p->code==ALLOCREG)){
    if(p->code==FREEREG&&p->q1.reg==reg) return 1;
    p=p->next;
  }
  return 0;
}

/* search for possible addressing-modes */
static void peephole(struct IC *p)
{
  int c,c2,r;struct IC *p2;struct AddressingMode *am;

  for(;p;p=p->next){
    c=p->code;
    if(c!=FREEREG&&c!=ALLOCREG&&(c!=SETRETURN||!isreg(q1)||p->q1.reg!=p->z.reg)) exit_label=0;
    if(c==LABEL) exit_label=p->typf;

    /* -(ax) */
    if(c==SUBIFP&&isreg(q1)&&isreg(z)&&p->q1.reg==p->z.reg&&p->q1.reg<=8&&isconst(q2)){
      zmax sz;
      int t;
      r=p->q1.reg;
      eval_const(&p->q2.val,q2typ(p));
      sz=zm2l(vmax);
      if(sz==1||sz==2||sz==4){
        for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
          if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r&&(!(p2->q2.flags&REG)||p2->q2.reg!=r)&&(!(p2->z.flags&REG)||p2->z.reg!=r)&&(
c2!=CONVERT||(q1typ(p2)&NQ)<=(ztyp(p2)&NQ))){
            t=(q1typ(p2)&NQ);
            if((ISINT(t)||ISPOINTER(t))&&t!=LLONG&&zmeqto(sizetab[q1typ(p2)&NQ],l2zm(sz))){
              p2->q1.am=am=mymalloc(sizeof(*am));
              p2->q1.val.vmax=l2zm(0L);
              am->base=r;
              am->flags=AM_PREDEC;
              p->code=NOP;
              p->q1.flags=p->q2.flags=p->z.flags=0;
              break;
            }
          }
         if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r&&(!(p2->q1.flags&REG)||p2->q1.reg!=r)&&(!(p2->z.flags&REG)||p2->z.reg!=r)){
            t=(q2typ(p2)&NQ);
            if((ISINT(t)||ISPOINTER(t))&&t!=LLONG&&zmeqto(sizetab[q2typ(p2)&NQ],l2zm(sz))){
              p2->q2.am=am=mymalloc(sizeof(*am));
              p2->q2.val.vmax=l2zm(0L);
              am->base=r;
              am->flags=AM_PREDEC;
              p->code=NOP;
              p->q1.flags=p->q2.flags=p->z.flags=0;
              break;
            }
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r&&(!(p2->q2.flags&REG)||p2->q2.reg!=r)&&(!(p2->q1.flags&REG)||p2->q1.reg!=r)){
            t=(ztyp(p2)&NQ);
            if((ISINT(t)||ISPOINTER(t))&&t!=LLONG&&zmeqto(sizetab[ztyp(p2)&NQ],l2zm(sz))){
              p2->z.am=am=mymalloc(sizeof(*am));
              p2->z.val.vmax=l2zm(0L);
              am->base=r;
              am->flags=AM_PREDEC;
              p->code=NOP;
              p->q1.flags=p->q2.flags=p->z.flags=0;
              break;
            }
          }
          if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
          if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
          if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
          if((p2->z.flags&REG)&&p2->z.reg==r) break;
        }
      }
    }

    /* (ax)+ in q1 */
    if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&(c!=CONVERT||(q1typ(p)&NQ)<=(ztyp(p)&NQ))){
      int t=(q1typ(p)&NQ);
      long sz=zm2l(sizetab[t]);
      r=p->q1.reg;
      if((sz==1||sz==2||sz==4)&&(ISINT(t)||ISPOINTER(t)||ISFLOAT(t))&&(!(p->q2.flags&REG)||p->q2.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
        for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
          if(c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
            eval_const(&p2->q2.val,q2typ(p2));
            if(zmeqto(vmax,l2zm(sz))){
              p->q1.am=am=mymalloc(sizeof(*am));
              p->q1.val.vmax=l2zm(0L);
	      am->flags=AM_POSTINC;
              am->base=r;
              am->offset=0;
              p2->code=NOP;
              p2->q1.flags=p2->q2.flags=p2->z.flags=0;
              break;
            }
          }
          if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
          if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
          if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
          if((p2->z.flags&REG)&&p2->z.reg==r) break;
        }
      }
    }
    /* (ax)+ in q2 */
    if(!p->q2.am&&(p->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&(c!=CONVERT||(q2typ(p)&NQ)<=(ztyp(p)&NQ))){
      int t=(q2typ(p)&NQ);
      long sz=zm2l(sizetab[t]);
      r=p->q2.reg;
      if((sz==1||sz==2||sz==4)&&(ISINT(t)||ISPOINTER(t)||ISFLOAT(t))&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->z.flags&REG)||p->z.reg!=r)){
        for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
          if(c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
            eval_const(&p2->q2.val,q2typ(p2));
            if(zmeqto(vmax,l2zm(sz))){
              p->q2.am=am=mymalloc(sizeof(*am));
              p->q2.val.vmax=l2zm(0L);
	      am->flags=AM_POSTINC;
              am->base=r;
              am->offset=0;
              p2->code=NOP;
              p2->q1.flags=p2->q2.flags=p2->z.flags=0;
              break;
            }
          }
          if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
          if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
          if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
          if((p2->z.flags&REG)&&p2->z.reg==r) break;
        }
      }
    }
    /* (ax)+ in z */
    if(!p->z.am&&(p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p->z.reg<=8&&(c!=CONVERT||(q2typ(p)&NQ)<=(ztyp(p)&NQ))){
      int t=(ztyp(p)&NQ);
      long sz=zm2l(sizetab[t]);
      r=p->z.reg;
      if((sz==1||sz==2||sz==4)&&(ISINT(t)||ISPOINTER(t)||ISFLOAT(t))&&(!(p->q1.flags&REG)||p->q1.reg!=r)&&(!(p->q2.flags&REG)||p->q2.reg!=r)){
        for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
          if(c2==ADDI2P&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r&&(p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==r&&(p2->q2.flags&(KONST|DREFOBJ))==KONST){
            eval_const(&p2->q2.val,q2typ(p2));
            if(zmeqto(vmax,l2zm(sz))){
              p->z.am=am=mymalloc(sizeof(*am));
              p->z.val.vmax=l2zm(0L);
	      am->flags=AM_POSTINC;
              am->base=r;
              am->offset=0;
              p2->code=NOP;
              p2->q1.flags=p2->q2.flags=p2->z.flags=0;
              break;
            }
          }
          if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
          if((p2->q1.flags&REG)&&p2->q1.reg==r) break;
          if((p2->q2.flags&REG)&&p2->q2.reg==r) break;
          if((p2->z.flags&REG)&&p2->z.reg==r) break;
        }
      }
    }
    /* Try const(reg) */
    if((c==ADDI2P/*||c==SUBIFP*/)&&isreg(z)&&(p->q2.flags&(KONST|DREFOBJ))==KONST){
      int base;zmax of;struct obj *o;
      eval_const(&p->q2.val,p->typf);
      if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
      if(1/*zmleq(l2zm(-32768L),vmax)&&zmleq(vmax,l2zm(32767L))*/){
	r=p->z.reg;
	if(isreg(q1)) base=p->q1.reg; else base=r;
	o=0;
	for(p2=p->next;p2;p2=p2->next){
	  c2=p2->code;
	  if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
	  if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
	  if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
	  if((c2==ASSIGN||c2==PUSH)&&(!ISSCALAR(p2->typf)||!zmeqto(p2->q2.val.vmax,sizetab[p2->typf&NQ]))){
	    if((p->q1.flags&REG)&&p->q1.reg==base)
	      break;
	    if((p->z.flags&REG)&&p->z.reg==base)
	      break;
	  }
	  if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
	    if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
	      if(o) break;
	      o=&p2->q1;
	    }
	    if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
	      if(o) break;
	      o=&p2->q2;
	    }
	    if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
	      if(o) break;
	      o=&p2->z;
	    }
	  }
	  if(c2==FREEREG||(p2->z.flags&(REG|DREFOBJ))==REG){
	    int m;
	    if(c2==FREEREG)
	      m=p2->q1.reg;
	    else
	      m=p2->z.reg;
	    if(m==r){
	      if(o){
		o->am=am=mymalloc(sizeof(*am));
		am->flags=AM_IMM_IND;
		am->base=base;
		am->offset=zm2l(of);
		if(isreg(q1)){
		  p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
		}else{
		  p->code=c=ASSIGN;p->q2.flags=0;
		  p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
		}
	      }
	      break;
	    }
	    if(c2!=FREEREG&&m==base) break;
	    continue;
	  }
        }
      }
    }
    /* Try reg,reg */
    if(c==ADDI2P&&isreg(q2)&&isreg(z)&&(isreg(q1)||p->q2.reg!=p->z.reg)){
      int base,idx;struct obj *o;
      r=p->z.reg;idx=p->q2.reg;
      if(isreg(q1)) base=p->q1.reg; else base=r;
      o=0;
      for(p2=p->next;p2;p2=p2->next){
        c2=p2->code;
        if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
        if(c2!=FREEREG&&(p2->q1.flags&(REG|DREFOBJ))==REG&&p2->q1.reg==r) break;
        if(c2!=FREEREG&&(p2->q2.flags&(REG|DREFOBJ))==REG&&p2->q2.reg==r) break;
        if((p2->z.flags&(REG|DREFOBJ))==REG&&p2->z.reg==idx&&idx!=r) break;
	
        if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
          if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            if(o||(q1typ(p2)&NQ)!=CHAR) break;
            o=&p2->q1;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            if(o||(q2typ(p2)&NQ)!=CHAR) break;
            o=&p2->q2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            if(o||(ztyp(p2)&NQ)!=CHAR) break;
            o=&p2->z;
          }
        }
        if(c2==FREEREG||(p2->z.flags&(REG|DREFOBJ))==REG){
          int m;
          if(c2==FREEREG)
            m=p2->q1.reg;
          else
            m=p2->z.reg;
          if(m==r){
            if(o){
              o->am=am=mymalloc(sizeof(*am));
              am->flags=AM_GPR_IND;
              am->base=base;
              am->offset=idx;
	      if(isreg(q1)){
		p->code=c=NOP;p->q1.flags=p->q2.flags=p->z.flags=0;
	      }else{
		p->code=c=ASSIGN;p->q2.flags=0;
		p->typf=p->typf2;p->q2.val.vmax=sizetab[p->typf2&NQ];
	      }
            }
            break;
          }
          if(c2!=FREEREG&&m==base) break;
          continue;
        }
      }
    }
  }
}

/* generates the function entry code */
static void function_top(FILE *f,struct Var *v,long offset)
{
  int i;
  rsavesize=0;
  if(!special_section(f,v)&&section!=CODE){emit(f,codename);if(f) section=CODE;} 
  emit(f,"\t.align\t1\n");
  if(v->storage_class==EXTERN){
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier);
  }else
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
  last_saved=0;
  for(i=r6;i<t2;i++){
    if(regused[i])
      last_saved=i;
  }
  if(last_saved){
    emit(f,"\tpush\t%s-%s",regnames[r6],regnames[last_saved]);
    rsavesize=(last_saved-r6+1)*4;
    if(function_calls){
      emit(f,",lr");
      rsavesize+=4;
    }
    emit(f,"\n");
  }else if(function_calls){
    emit(f,"\tpush\t%s-%s,lr\n",regnames[r6],regnames[r6]);
    rsavesize+=8;
  }
  if(offset){
    emit(f,"\tsub\t%s,%ld\n",regnames[sp],offset);
  }
}
/* generates the function exit code */
static void function_bottom(FILE *f,struct Var *v,long offset)
{
  if(offset){
    emit(f,"\tadd\t%s,%ld\n",regnames[sp],offset);
  }
  if(last_saved){
    emit(f,"\tpop\t%s-%s",regnames[r6],regnames[last_saved]);
    if(function_calls){
      emit(f,",pc");
    }
    emit(f,"\n");
  }else if(function_calls){
    emit(f,"\tpop\t%s-%s,pc\n",regnames[r6],regnames[r6]);
  }  
  if(!function_calls)
    emit(f,ret);
}

/****************************************/
/*  End of private data and functions.  */
/****************************************/

/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
int init_cg(void)
{
  int i;
  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign=l2zm(4L);
  char_bit=l2zm(8L);
  stackalign=l2zm(4);

  for(i=0;i<=MAX_TYPE;i++){
    sizetab[i]=l2zm(msizetab[i]);
    align[i]=l2zm(malign[i]);
  }

  if(SHORT_DOUBLE){
    sizetab[DOUBLE]=sizetab[FLOAT];
    align[DOUBLE]=align[FLOAT];
    sizetab[LDOUBLE]=sizetab[FLOAT];
    align[LDOUBLE]=align[FLOAT];
  }

  regnames[0]="noreg";
  for(i=1;i<=32;i++){
    regnames[i]=mymalloc(10);
    sprintf(regnames[i],"r%d",i-1);
    regsize[i]=l2zm(4L);
    regtype[i]=&ltyp;
  }
  for(i=33;i<=48;i++){
    regnames[i]=mymalloc(10);
    sprintf(regnames[i],"r%d/r%d",i-33,i-32);
    regsize[i]=l2zm(8L);
    regtype[i]=&ldbl;
  }

  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR]=l2zm(-128L);
  t_min[SHORT]=l2zm(-32768L);
  t_min[INT]=zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LONG]=t_min(INT);
  t_min[LLONG]=zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT]=t_min(LLONG);
  t_max[CHAR]=ul2zum(127L);
  t_max[SHORT]=ul2zum(32767UL);
  t_max[INT]=ul2zum(2147483647UL);
  t_max[LONG]=t_max(INT);
  t_max[LLONG]=zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT]=t_max(LLONG);
  tu_max[CHAR]=ul2zum(255UL);
  tu_max[SHORT]=ul2zum(65535UL);
  tu_max[INT]=ul2zum(4294967295UL);
  tu_max[LONG]=t_max(UNSIGNED|INT);
  tu_max[LLONG]=zumkompl(ul2zum(0UL));
  tu_max[MAXINT]=t_max(UNSIGNED|LLONG);
  
  /*  Reserve a few registers for use by the code-generator.      */
  /*  This is not optimal but simple.                             */
  regsa[t1]=regsa[t2]=1;
  regsa[pc]=regsa[lr]=regsa[sp]=regsa[sr]=1;
  regscratch[t1]=regscratch[t2]=0;
  regscratch[pc]=regscratch[lr]=regscratch[sp]=regscratch[sr]=1;

  for(i=r16;i<=r31;i++)
    regscratch[i]=regsa[i]=1;
  
  for(i=1;i<=6;i++)
    regscratch[i]=1;

  if(ONESEC){
    bssname=codename;
    dataname=codename;
    rodataname=codename;
  }

  if(SHORT_DOUBLE) marray[2]="__SHORT_DOUBLE__=1";
  target_macros=marray;


  declare_builtin("__mulint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__addint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__subint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__andint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__orint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__eorint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__negint64",LLONG,LLONG,r0r1,0,0,1,0);
  declare_builtin("__lslint64",LLONG,LLONG,r0r1,INT,r2,1,0);

  declare_builtin("__divsint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__divuint64",UNSIGNED|LLONG,UNSIGNED|LLONG,r0r1,UNSIGNED|LLONG,r2r3,1,0);
  declare_builtin("__modsint64",LLONG,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__moduint64",UNSIGNED|LLONG,UNSIGNED|LLONG,r0r1,UNSIGNED|LLONG,r2r3,1,0);
  declare_builtin("__lsrsint64",LLONG,LLONG,r0r1,INT,r2,1,0);
  declare_builtin("__lsruint64",UNSIGNED|LLONG,UNSIGNED|LLONG,r0r1,INT,r2,1,0);
  declare_builtin("__cmpsint64",INT,LLONG,r0r1,LLONG,r2r3,1,0);
  declare_builtin("__cmpuint64",INT,UNSIGNED|LLONG,r0r1,UNSIGNED|LLONG,r2r3,1,0);
  declare_builtin("__sint64toflt32",FLOAT,LLONG,r0r1,0,0,1,0);
  declare_builtin("__uint64toflt32",FLOAT,UNSIGNED|LLONG,r0r1,0,0,1,0);
  declare_builtin("__sint64toflt64",LDOUBLE,LLONG,r0r1,0,0,1,0);
  declare_builtin("__uint64toflt64",LDOUBLE,UNSIGNED|LLONG,r0r1,0,0,1,0);
  declare_builtin("__flt32tosint64",LLONG,FLOAT,r0,0,0,1,0);
  declare_builtin("__flt32touint64",UNSIGNED|LLONG,FLOAT,r0,0,0,1,0);
  declare_builtin("__flt64tosint64",LLONG,LDOUBLE,r0r1,0,0,1,0);
  declare_builtin("__flt64touint64",UNSIGNED|LLONG,LDOUBLE,r0r1,0,0,1,0);

  declare_builtin("__flt32toflt64",LDOUBLE,FLOAT,r0,0,0,1,0);
  declare_builtin("__flt64toflt32",FLOAT,LDOUBLE,r0r1,0,0,1,0);

  declare_builtin("__addflt64",LDOUBLE,LDOUBLE,r0r1,LDOUBLE,r2r3,1,0);
  declare_builtin("__subflt64",LDOUBLE,LDOUBLE,r0r1,LDOUBLE,r2r3,1,0);
  declare_builtin("__mulflt64",LDOUBLE,LDOUBLE,r0r1,LDOUBLE,r2r3,1,0);
  declare_builtin("__divflt64",LDOUBLE,LDOUBLE,r0r1,LDOUBLE,r2r3,1,0);
  declare_builtin("__negflt64",LDOUBLE,LDOUBLE,r0r1,LDOUBLE,r2r3,1,0);
  declare_builtin("__cmpflt64",INT,LDOUBLE,r0r1,LDOUBLE,r2r3,1,0);


  return 1;
}

void init_db(FILE *f)
{
}

int freturn(struct Typ *t)
/*  Returns the register in which variables of type t are returned. */
/*  If the value cannot be returned in a register returns 0.        */
/*  A pointer MUST be returned in a register. The code-generator    */
/*  has to simulate a pseudo register if necessary.                 */
{
  if(SHORT_DOUBLE&&ISFLOAT(t->flags)) return r0;
  if(ISSTRUCT(t->flags)||ISUNION(t->flags)) 
    return 0;
  if(zmleq(szof(t),l2zm(4L))) 
    return r0;
  if(zmeqto(szof(t),l2zm(8L))) 
    return r0r1;
  return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  if(r>=r0r1&&r<=r30r31){
    p->r1=(r-r0r1)*2+r0;
    p->r2=(r-r0r1)*2+r1;
    return 1;
  }
  return 0;
}

/* estimate the cost-saving if object o from IC p is placed in
   register r */
int cost_savings(struct IC *p,int r,struct obj *o)
{
  int c=p->code;
  if(o->flags&VKONST){
    return 0;
  }
  if(o->flags&DREFOBJ)
    return 4;
  if(c==SETRETURN&&r==p->z.reg&&!(o->flags&DREFOBJ)) return 3;
  if(c==GETRETURN&&r==p->q1.reg&&!(o->flags&DREFOBJ)) return 3;
  return 2;
}

int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if(r==0)
    return 0;
  t&=NQ;
  if(ISINT(t)||ISFLOAT(t)||ISPOINTER(t)){
    if(zmeqto(sizetab[t],l2zm(8L))){
      if(r>=r0r1&&r<=r30r31)
	return 1;
      else
	return 0;
    }
    if(r>=r0&&r<=r31)
      return 1;
    else
      return 0;
  }
  return 0;
}

int dangerous_IC(struct IC *p)
/*  Returns zero if the IC p can be safely executed     */
/*  without danger of exceptions or similar things.     */
/*  vbcc may generate code in which non-dangerous ICs   */
/*  are sometimes executed although control-flow may    */
/*  never reach them (mainly when moving computations   */
/*  out of loops).                                      */
/*  Typical ICs that generate exceptions on some        */
/*  machines are:                                       */
/*      - accesses via pointers                         */
/*      - division/modulo                               */
/*      - overflow on signed integer/floats             */
{
  int c=p->code;
  if((p->q1.flags&DREFOBJ)||(p->q2.flags&DREFOBJ)||(p->z.flags&DREFOBJ))
    return 1;
  if((c==DIV||c==MOD)&&!isconst(q2))
    return 1;
  return 0;
}

int must_convert(int o,int t,int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
/*  On the VideCore IV pointers and 32bit               */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
{
  int op=o&NQ,tp=t&NQ;
  if(SHORT_DOUBLE){
    if(op==DOUBLE||op==LDOUBLE) op=FLOAT;
    if(tp==DOUBLE||tp==LDOUBLE) tp=FLOAT;
  }
  if((op==INT||op==LONG||op==POINTER)&&(tp==INT||tp==LONG||tp==POINTER))
    return 0;
  if(op==FLOAT&&tp==FLOAT) return 0;
  if(op==DOUBLE&&tp==LDOUBLE) return 0;
  if(op==LDOUBLE&&tp==DOUBLE) return 0;
  return 1;
}

void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  if(newobj&&!ONESEC&&section!=SPECIAL)
    emit(f,"%ld\n",zm2l(size));
  else
    emit(f,"\t.space\t%ld\n",zm2l(size));
  newobj=0;
}

void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  if(zm2l(align)>1) emit(f,"\t.align\t2\n");
}

void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;char *sec;
  if(v->clist) constflag=is_const(v->vtyp);
  if(v->storage_class==STATIC){
    if(ISFUNC(v->vtyp->flags)) return;
    if(!special_section(f,v)){
      if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
      if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)&&section!=RODATA){emit(f,rodataname);if(f) section=RODATA;}
      if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
    }
    if(v->clist||ONESEC||section==SPECIAL){
      gen_align(f,falign(v->vtyp));
      emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    }else
      emit(f,"\t.lcomm\t%s%ld,",labprefix,zm2l(v->offset));
    newobj=1;
  }
  if(v->storage_class==EXTERN){
    emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    if(v->flags&(DEFINED|TENTATIVE)){
      if(!special_section(f,v)){
	if(v->clist&&(!constflag||(g_flags[2]&USEDFLAG))&&section!=DATA){emit(f,dataname);if(f) section=DATA;}
	if(v->clist&&constflag&&!(g_flags[2]&USEDFLAG)&&section!=RODATA){emit(f,rodataname);if(f) section=RODATA;}
	if(!v->clist&&section!=BSS){emit(f,bssname);if(f) section=BSS;}
      }
      if(v->clist||ONESEC||section==SPECIAL){
	gen_align(f,falign(v->vtyp));
        emit(f,"%s%s:\n",idprefix,v->identifier);
      }else
        emit(f,"\t.%scomm\t%s%s,",(USE_COMMONS?"":"l"),idprefix,v->identifier);
      newobj=1;
    }
  }
}

void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  if((t&NQ)==POINTER) t=UNSIGNED|LONG;
  emit(f,"\t.%s\t",dct[t&NQ]);
  if(!p->tree){
    if(ISFLOAT(t)){
      /*  auch wieder nicht sehr schoen und IEEE noetig   */
      unsigned char *ip;
      if(SHORT_DOUBLE){
	eval_const(&p->val,t);
	ip=(unsigned char *)&vfloat;
	t=FLOAT;
      }else
	ip=(unsigned char *)&p->val.vdouble;
      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
      if((t&NQ)!=FLOAT){
        emit(f,",0x%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
      }
    }else if((t&NQ)==LLONG){
      zumax tmp;
      eval_const(&p->val,t);
      tmp=vumax;
      vumax=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffffff)); 
      gval.vulong=zum2zul(vumax);
      emitval(f,&gval,UNSIGNED|LONG);
      emit(f,",");
      vumax=zumand(tmp,ul2zum(0xffffffff)); 
      gval.vulong=zum2zul(vumax);
      emitval(f,&gval,UNSIGNED|LONG);
    }else{
      emitval(f,&p->val,t&NU);
    }
  }else{
    no_suffix=1;
    emit_obj(f,&p->tree->o,t&NU);
    no_suffix=0;
  }
  emit(f,"\n");newobj=0;
}


/*  The main code-generation routine.                   */
/*  f is the stream the code should be written to.      */
/*  p is a pointer to a doubly linked list of ICs       */
/*  containing the function body to generate code for.  */
/*  v is a pointer to the function.                     */
/*  offset is the size of the stackframe the function   */
/*  needs for local variables.                          */

void gen_code(FILE *f,struct IC *p,struct Var *v,zmax offset)
/*  The main code-generation.                                           */
{
  int c,t,i,lastcomp;
  int q1wasreg,q2wasreg,zwasreg;
  struct IC *m;
  argsize=0;
  if(DEBUG&1) printf("gen_code()\n");
  for(c=1;c<=MAXR;c++) regs[c]=regsa[c];
  stackoffset=notpopped=dontpop=maxpushed=0;


  /*FIXME*/
  ret="\trts\n";

  cc="";

  for(m=p;m;m=m->next){
    c=m->code;t=m->typf&NU;
    if(c==ALLOCREG) {regs[m->q1.reg]=1;continue;}
    if(c==FREEREG) {regs[m->q1.reg]=0;continue;}

    if(notpopped&&!dontpop){
      int flag=0;
      if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
        emit(f,"\tadd\t%s,%ld\n",regnames[sp],notpopped);
        pop(notpopped);notpopped=0;
      }
    }


    /* convert MULT/DIV/MOD with powers of two */
    if((t&NQ)<=LONG&&(m->q2.flags&(KONST|DREFOBJ))==KONST&&(t&NQ)<=LONG&&(c==MULT||((c==DIV||c==MOD)&&(t&UNSIGNED)))){
      eval_const(&m->q2.val,t);
      i=pof2(vmax);
      if(i){
        if(c==MOD){
          vmax=zmsub(vmax,l2zm(1L));
          m->code=AND;
        }else{
          vmax=l2zm(i-1);
          if(c==DIV) m->code=RSHIFT; else m->code=LSHIFT;
        }
        c=m->code;
	gval.vmax=vmax;
	eval_const(&gval,MAXINT);
	if(c==AND){
	  insert_const(&m->q2.val,t);
	}else{
	  insert_const(&m->q2.val,INT);
	  p->typf2=INT;
	}
      }
    }
    if(c==CALL&&argsize<zm2l(m->q2.val.vmax)) argsize=zm2l(m->q2.val.vmax);
  }
  peephole(p);

  for(c=1;c<=MAXR;c++){
    if(regsa[c]||regused[c]){
      BSET(regs_modified,c);
    }
  }

  localsize=(zm2l(offset)+3)/4*4;
#if FIXED_SP
  /*FIXME: adjust localsize to get an aligned stack-frame */
#endif

  function_top(f,v,localsize);

  for(;p;p=p->next){
    c=p->code;t=p->typf;
    if(c==NOP) {p->z.flags=0;continue;}
    if(c==ALLOCREG) {regs[p->q1.reg]=1;continue;}
    if(c==FREEREG) {regs[p->q1.reg]=0;continue;}

    if(DEBUG&256){emit(f,"# "); pric2(f,p);}
    if(DEBUG&512) emit(f,"# stackoffset=%ld, notpopped=%ld, dontpop=%ld\n",stackoffset,notpopped,dontpop);
    if(notpopped&&!dontpop){
      int flag=0;
      if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
        emit(f,"\tadd\t%s,%ld\n",regnames[sp],notpopped);
        pop(notpopped);notpopped=0;
      }
    }

    if(c==LABEL) {emit(f,"%s%d:\n",labprefix,t);continue;}
    if(c==BRA){
      if(0/*t==exit_label&&framesize==0*/)
	emit(f,ret);
      else
	emit(f,"\tb\t%s%d\n",labprefix,t);
      continue;
    }
    if(c>=BEQ&&c<BRA){
      if(lastcomp&UNSIGNED)
	emit(f,"\tb%s\t",ccu[c-BEQ]);
      else
	emit(f,"\tb%s\t",ccs[c-BEQ]);
      if(isreg(q1)){
	emit_obj(f,&p->q1,0);
	emit(f,",");
      }
      emit(f,"%s%d\n",labprefix,t);
      continue;
    }
    if(c==MOVETOREG){
      load_reg(f,p->z.reg,&p->q1,regtype[p->z.reg]->flags);
      continue;
    }
    if(c==MOVEFROMREG){
      store_reg(f,p->q1.reg,&p->z,regtype[p->q1.reg]->flags);
      continue;
    }
    if(c==PUSH)
      dontpop+=zm2l(p->q2.val.vmax);
    if((c==ASSIGN||c==PUSH)&&((t&NQ)>POINTER||((t&NQ)==CHAR&&zm2l(p->q2.val.vmax)!=1))){
      int i,cnt,unit,lab,treg,acnt=0,atreg=0;
      long size;
      char *ut;
      if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&reg_is_scratched(p,p->q1.reg)){
	q1reg=p->q1.reg;
	if(p->q1.am) ierror(0);
      }else{
	q1reg=t1;
	if(p->q1.flags&(REG|DREFOBJ)){
	  p->q1.flags&=~DREFOBJ;
	  load_reg(f,q1reg,&p->q1,POINTER);
	}else
	  load_address(f,q1reg,&p->q1,POINTER);
      }
      if((p->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&reg_is_scratched(p,p->z.reg)){
	zreg=p->z.reg;
	if(p->z.am) ierror(0);
      }else if(c==PUSH){
	zreg=sp;
      }else{
	if(q1reg==t1)
	  zreg=t2;
	else
	  zreg=t1;
	if(p->z.flags&(REG|DREFOBJ)){
	  p->z.flags&=~DREFOBJ;
	  load_reg(f,zreg,&p->z,POINTER);
	}else
	  load_address(f,zreg,&p->z,POINTER);
      }      
      if(zreg==t2){
	treg=get_reg(f,p);
	atreg=1;
      }else if(zreg==t1||q1reg==t1)
	treg=t2;
      else
	treg=t1;

      unit=1;ut="b";
      size=zm2l(p->q2.val.vmax);

      if(c==PUSH)
	emit(f,"\tadd\t%s,%ld\n",regnames[q1reg],size);

      if(size/unit>=8){
	if(treg!=t2&&zreg!=t2)
	  cnt=t2;
	else{
	  cnt=get_reg(f,p);
	  acnt=1;
	}
	emit(f,"\tmov\t%s,%ld\n",regnames[cnt],(size/unit)>>2);
	emit(f,"%s%d:\n",labprefix,++label);
      }

      for(i=0;i<((size/unit>4)?4:size/unit);i++){
	if(c!=PUSH){
	  emit(f,"\tld%s\t%s,(%s)++\n",ut,regnames[treg],regnames[q1reg]);
	  emit(f,"\tst%s\t%s,(%s)++\n",ut,regnames[treg],regnames[zreg]);
	}else{
	  emit(f,"\tld%s\t%s,--(%s)\n",ut,regnames[treg],regnames[q1reg]);
	  emit(f,"\tst%s\t%s,--(%s)\n",ut,regnames[treg],regnames[zreg]);
	}
      }
      if(size/unit>=8){
	emit(f,"\taddcmpbne\t%s,-1,0,%s%d\n",regnames[cnt],labprefix,label);
      }
      for(i=0;i<((size/unit)&3);i++){
	if(c!=PUSH){
	  emit(f,"\tld%s\t%s,(%s)++\n",ut,regnames[treg],regnames[q1reg]);
	  emit(f,"\tst%s\t%s,(%s)++\n",ut,regnames[treg],regnames[zreg]);
	}else{
	  emit(f,"\tld%s\t%s,--(%s)\n",ut,regnames[treg],regnames[q1reg]);
	  emit(f,"\tst%s\t%s,--(%s)\n",ut,regnames[treg],regnames[zreg]);
	}
      }
      if(c==PUSH)
	push(size);
      if(acnt) free_reg(f,cnt);
      if(atreg) free_reg(f,treg);
      continue;
    }

    if(c==CALL){
      int reg;
      /*FIXME*/
#if 0      
      if(stack_valid&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_STACK)){
	if(framesize+zum2ul(p->q1.v->fi->stack1)>stack)
	  stack=framesize+zum2ul(p->q1.v->fi->stack1);
      }else
	stack_valid=0;
#endif
      /*FIXME: does not work with fixed parameters on stack */
      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp("__va_start",p->q1.v->identifier)){
	emit(f,"\tadd\t%s,%s,%ld\n",regnames[r0],regnames[sp],localsize+rsavesize-stackoffset);
	continue;
      }

      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
        emit_inline_asm(f,p->q1.v->fi->inline_asm);
      }else{
	if((p->q1.flags&(REG|DREFOBJ))==DREFOBJ){
	  p->q1.flags&=~DREFOBJ;
	  load_reg(f,t1,&p->q1,POINTER);
	  p->q1.flags|=(DREFOBJ|REG);
	  p->q1.reg=t1;
	}
	emit(f,"\tbl\t");
	if((p->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)){
	  emit(f,"%s",regnames[p->q1.reg]);
	}else{
	  no_suffix=1;
	  emit_obj(f,&p->q1,t);
	  no_suffix=0;
	}
	emit(f,"\n");
      }

      if(!zmeqto(p->q2.val.vmax,l2zm(0L))){
        notpopped+=zm2l(p->q2.val.vmax);
        dontpop-=zm2l(p->q2.val.vmax);
        if(!NODELAYEDPOP&&!vlas&&stackoffset==-notpopped){
          /*  Entfernen der Parameter verzoegern  */
        }else{
	  emit(f,"\tadd\t%s,%ld\n",regnames[sp],zm2l(p->q2.val.vmax));
          pop(zm2l(p->q2.val.vmax));
          notpopped-=zm2l(p->q2.val.vmax);
        }
      }

      if((p->q1.flags&(VAR|DREFOBJ))==VAR&&p->q1.v->fi&&(p->q1.v->fi->flags&ALL_REGS)){
	bvunite(regs_modified,p->q1.v->fi->regs_modified,RSIZE);
      }else{
	int i;
	for(i=1;i<=MAXR;i++){
	  if(regscratch[i]) BSET(regs_modified,i);
	}
      }
      continue;
    }

    if(c==ADDRESS){
      if(isreg(z))
	zreg=p->z.reg;
      else
	zreg=t1;
      load_address(f,zreg,&p->q1,POINTER);
      save_result(f,p);
      continue;
    }

    if(isreg(q1)) q1wasreg=1; else q1wasreg=0;
    if(isreg(q2)) q2wasreg=1; else q2wasreg=0;
    if(isreg(z)) zwasreg=1; else zwasreg=0;

    p=preload(f,p);
    c=p->code;
    if(c==SUBPFP) c=SUB;
    if(c==ADDI2P) c=ADD;
    if(c==SUBIFP) c=SUB;
    if(c==CONVERT){
      if(ISFLOAT(q1typ(p))){
	if(ztyp(p)&UNSIGNED) ierror(0);
	if(ISFLOAT(ztyp(p)))
	  emit(f,"\tmov%s\t%s,%s\n",cc,regnames[zreg],regnames[q1reg]);
	else
	  emit(f,"\tftrunc%s\t%s,%s\n",cc,regnames[zreg],regnames[q1reg]);
	save_result(f,p);
	continue;
      }
      if(ISFLOAT(ztyp(p))){
	emit(f,"\tflt%c%s\t%s,%s\n",(q1typ(p)&UNSIGNED)?'u':'s',cc,regnames[zreg],regnames[q1reg]);
	save_result(f,p);
	continue;
      }
      if(sizetab[q1typ(p)&NQ]<sizetab[ztyp(p)&NQ]){
	int mask;
	if((q1typ(p)&NQ)==CHAR)
	  mask=7;
	else
	  mask=15;
	if(q1typ(p)&UNSIGNED)
	  mask++;
	if(q1wasreg||(q1typ(p)&NU)==CHAR)
	  emit(f,"\text%c%s\t%s,%s,%d\n",(q1typ(p)&UNSIGNED)?'u':'s',cc,regnames[zreg],regnames[q1reg],mask);
      }else{
	zreg=q1reg;
      }	  
      save_result(f,p);
      continue;
    }
    if(c==KOMPLEMENT){
      emit(f,"\tmvn%s\t%s,%s\n",cc,regnames[zreg],regnames[q1reg]);
      save_result(f,p);
      continue;
    }
    if(c==SETRETURN){
      if(isreg(q1)){
	if(p->q1.reg!=p->z.reg)
	  emit(f,"\tmov%s\t%s,%s\n",cc,regnames[p->z.reg],regnames[p->q1.reg]);
      }else
	load_reg(f,p->z.reg,&p->q1,t);
      BSET(regs_modified,p->z.reg);
      continue;
    }
    if(c==GETRETURN){
      if(p->q1.reg){
        zreg=p->q1.reg;
	save_result(f,p);
      }else
        p->z.flags=0;
      continue;
    }

    if(c==ASSIGN||c==PUSH){
      if(t==0) ierror(0);
      if(c==PUSH){
	if(!q1reg){
	  q1reg=t1;
	  load_reg(f,q1reg,&p->q1,t);
	}
	if(SHORT_DOUBLE&&ISFLOAT(t)) t=FLOAT;
	emit(f,"\tst%s%s\t%s,--(%s)\n",sdt(t),cc,regnames[q1reg],regnames[sp]);
	push(zm2l(p->q2.val.vmax));
	continue;
      }
      if(c==ASSIGN){
	if(isreg(q1)){
	  if(isreg(z))
	    emit(f,"\tmov%s\t%s,%s\n",cc,regnames[zreg],regnames[p->q1.reg]);
	  else
	    zreg=p->q1.reg;
	}else
	  load_reg(f,zreg,&p->q1,t);
	save_result(f,p);
      }
      continue;
    }
    if(c==MINUS){
      load_reg(f,zreg,&p->q1,t);
      emit(f,"\tneg%s\t%s,%s\n",cc,regnames[zreg],regnames[zreg]);
      save_result(f,p);
      continue;
    }
    if(c==TEST){
      emit(f,"\tcmp%s\t",cc);
      emit_obj(f,&p->q1,t);
      emit(f,",0\n");
      lastcomp=t;
      continue;
    }
    if(c==COMPARE){
      if(ISFLOAT(t))
	emit(f,"\tfcmp%s\t%s,",cc,regnames[t1]);
      else
	emit(f,"\tcmp%s\t",cc);
      emit_obj(f,&p->q1,t);
      emit(f,",");
      emit_obj(f,&p->q2,t);
      emit(f,"\n");
      lastcomp=t;
      continue;
    }
    if(c==MOD){
      /* TODO: is there a faster way? */
      int m;
      if(q2reg!=t2)
	m=t2;
      else
	m=get_reg(f,p);
      if(!isreg(q1)||p->q1.reg!=zreg)
	load_reg(f,zreg,&p->q1,t);
      emit(f,"\tdiv%c%s\t%s,%s,",(t&UNSIGNED)?'u':'s',cc,regnames[m],regnames[zreg]);
      emit_obj(f,&p->q2,t);
      emit(f,"\n");
      emit(f,"\tmul%s\t%s,",cc,regnames[m]);
      emit_obj(f,&p->q2,t);
      emit(f,"\n");
      emit(f,"\tsub%s\t%s,%s,%s\n",cc,regnames[zreg],regnames[zreg],regnames[m]);
      free_reg(f,m);
      save_result(f,p);
      continue;
    }      
    if((c>=OR&&c<=AND)||(c>=LSHIFT&&c<MOD)){
      if(c>=OR&&c<=AND)
	emit(f,"\t%s%s\t%s,",logicals[c-OR],cc,regnames[zreg]);
      else{
	if(ISFLOAT(t))
	  emit(f,"\t%s%s\t%s,",farithmetics[c-LSHIFT],cc,regnames[zreg]);
	else
	  emit(f,"\t%s%s\t%s,",(t&UNSIGNED)?uarithmetics[c-LSHIFT]:arithmetics[c-LSHIFT],cc,regnames[zreg]);
      }
      if(!isreg(q1)||p->q1.reg!=zreg||*cc||c==DIV||ISFLOAT(t)){
	emit_obj(f,&p->q1,t);
	emit(f,",");
      }
      emit_obj(f,&p->q2,t);
      emit(f,"\n");
      save_result(f,p);
      continue;
    }
    pric2(stdout,p);
    ierror(0);
  }

  if(notpopped){
    emit(f,"\tadd\t%s,%ld\n",regnames[sp],notpopped);
    pop(notpopped);notpopped=0;
  }

  function_bottom(f,v,localsize);
  if(stack_valid){
    if(!v->fi) v->fi=new_fi();
    v->fi->flags|=ALL_STACK;
    v->fi->stack1=stack;
  }
  /*emit(f,"# stacksize=%lu%s\n",zum2ul(stack),stack_valid?"":"+??");*/
}

int shortcut(int code,int typ)
{
  return 0;
}

int reg_parm(struct reg_handle *m, struct Typ *t,int vararg,struct Typ *d)
{
  int f;

  if(vararg)
    return 0;

  f=t->flags&NQ;
  if(ISINT(f)||ISFLOAT(f)||f==POINTER){
    if(zmeqto(sizetab[f],l2zm(8L))){
      if(m->gregs<=r4){
	m->gregs=(m->gregs+1)&0xfe;
	m->gregs+=2;
	if(m->gregs==r1)
	  return r0r1;
	else if(m->gregs==r3)
	  return r2r3;
	else if(m->gregs==r5)
	  return r4r5;
	else
	  ierror(0);
      }else
	return 0;
    }else{
      if(m->gregs<=r4)
	return ++m->gregs;
      else
	return 0;
    }
  }
  return 0;
}

int handle_pragma(const char *s)
{
}
void cleanup_cg(FILE *f)
{
}
void cleanup_db(FILE *f)
{
  if(f) section=-1;
}

char *use_libcall(int c,int t,int t2)
/* Return name of library function, if this node should be
   implemented via libcall. */
{
  static char fname[16];
  char *ret = NULL;

  if(c==COMPARE){
    if((t&NQ)==DOUBLE) t=(SHORT_DOUBLE)?FLOAT:LDOUBLE;
    if((t&NQ)==LLONG||t==LDOUBLE){
      sprintf(fname,"__cmp%s%s%ld",(t&UNSIGNED)?"u":"s",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8);
      ret=fname;
    }
  }else{
    t&=NU;
    t2&=NU;
    if(SHORT_DOUBLE){
      if(ISFLOAT(t)) t=FLOAT;
      if(ISFLOAT(t2)) t2=FLOAT;
    }else{
      if(t==DOUBLE) t=LDOUBLE;
      if(t2==DOUBLE) t2=LDOUBLE;
    }
    if(c==CONVERT){
      if(t==t2) return 0;
      if(t==FLOAT&&t2==LDOUBLE) return "__flt64toflt32";
      if(t==LDOUBLE&&t2==FLOAT) return "__flt32toflt64";
      if(t!=LDOUBLE&&t2!=LDOUBLE&&(t&NQ)!=LLONG&&(t2&NQ)!=LLONG) return 0;
      if(ISFLOAT(t)){
        sprintf(fname,"__%cint%ldtoflt%d",(t2&UNSIGNED)?'u':'s',zm2l(sizetab[t2&NQ])*8,(t==FLOAT)?32:64);
        ret=fname;
      }
      if(ISFLOAT(t2)&&(t&NU)==LLONG){
        sprintf(fname,"__flt%dto%cint%ld",((t2&NU)==FLOAT)?32:64,(t&UNSIGNED)?'u':'s',zm2l(sizetab[t&NQ])*8);
        ret=fname;
      }
    }
    if((t&NQ)==LLONG||t==DOUBLE){
      if((c>=LSHIFT&&c<=MOD)||(c>=OR&&c<=AND)||c==KOMPLEMENT||c==MINUS){
        if(t==(UNSIGNED|LLONG)&&(c==DIV||c==MOD||c==RSHIFT)){
          sprintf(fname,"__%suint64",ename[c]);
          ret=fname;
        }else if((t&NQ)==LLONG){
          sprintf(fname,"__%sint64",ename[c]);
          ret=fname;
        }else{
          sprintf(fname,"__%s%s%s%ld",ename[c],(t&UNSIGNED)?"u":"",ISFLOAT(t)?"flt":"int",zm2l(sizetab[t&NQ])*8);
          ret=fname;
        }
      }
    }
  }

  return ret;
}
