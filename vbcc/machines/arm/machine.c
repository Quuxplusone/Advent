/*
 * ARM code generator
 * A 32-bit RISC with 16 general purpose registers.
 * Written by Frank Wille <frank@phoenix.owl.de>
 */

#include "supp.h"
#include "vbc.h"

static char FILE_[] = __FILE__;


/*  Public data that MUST be there.                             */

/* Name and copyright. */
char cg_copyright[] = "vbcc code-generator for ARM V0.0 (c) in 2006 by Frank Wille";

/* Commandline-flags the code-generator accepts:
   0: just a flag
   VALFLAG: a value must be specified
   STRINGFLAG: a string can be specified
   FUNCFLAG: a function will be called
   apart from FUNCFLAG, all other versions can only be specified once */
int g_flags[MAXGF] = {
    STRINGFLAG,STRINGFLAG,0,0,0,0,
    0,0,0,0
};

/* the flag-name, do not use names beginning with l, L, I, D or U, because
   they collide with the frontend */
char *g_flags_name[MAXGF] = {
    "cpu","fpu","little-endian","big-endian","arm","thumb",
    "const-in-data","merge-constants","elf","use-commons"
};

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

/*  Names of all registers. */
char *regnames[MAXR+1] = {
    "noreg",
    "r0","r1","r2","r3","r4","r5","r6","r7",
    "r8","r9","r10","r11","r12","sp","lr","pc",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "s8","s9","s10","s11","s12","s13","s14","s15",
    "cpsr",
    "r0/r1","r2/r3","r4/r5","r6/r7","r10/r11",
    "d0","d1","d2","d3","d4","d5","d6","d7"
};

/*  The Size of each register in bytes.                         */
zmax regsize[MAXR+1];

/*  a type which can store each register. */
struct Typ *regtype[MAXR+1];

/*  regsa[reg]!=0 if a certain register is allocated and should */
/*  not be used by the compiler pass.                           */
int regsa[MAXR+1];

/*  Specifies which registers may be scratched by functions.    */
int regscratch[MAXR+1] = {
    0,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,  /* r0-r3,r12 */
    1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,    /* vfp s0-s3 */
    1,                                  /* cpsr */
    1,1,0,0,0,                          /* r0/r1, r2/r3 */
    1,1,0,0,0,0,0,0                     /* vfp d0-d1 */
};

/* specifies the priority for the register-allocator, if the same
   estimated cost-saving can be obtained by several registers, the
   one with the highest priority will be used */
int reg_prio[MAXR+1] = {
    0,10,11,12,13,2,3,4,5,6,7,8,9,14,0,1,0,
    1,2,3,4,0,0,0,0,0,0,0,0,0,0,0,0,
    0,
    10,12,2,4,8,
    1,2,0,0,0,0,0,0
};

/* an empty reg-handle representing initial state */
struct reg_handle empty_reg_handle = {0,0};

/* Names of target-specific variable attributes.                */
char *g_attr_name[] = {
    "__arm","__thumb","__interrupt","__syscall",0
};
#define ARM 1
#define THUMB 2
#define INTERRUPT 4
#define SYSCALL 8


/****************************************/
/*  Private data and functions.         */
/****************************************/

#define LE_MODE (g_flags[2]&USEDFLAG)
#define BE_MODE (g_flags[3]&USEDFLAG)
#define ARM_DEFAULT (g_flags[4]&USEDFLAG)
#define THUMB_DEFAULT (g_flags[5]&USEDFLAG)
#define CONST_IN_DATA (g_flags[6]&USEDFLAG)
#define ELF_LABELS (g_flags[8]&USEDFLAG)
#define USE_COMMONS (g_flags[9]&USEDFLAG)

int arm_le_mode = 1;          /* defaults to little-endian */
static int thumb_default = 0; /* we start in ARM mode */
static int thumb = 0;         /* current mode */

enum {
  AAANY=0,AA2,AA3,AA3M,AA4,AA4T,AA5,AA5T,AA5TE
};
static int aa = AAANY;        /* ARM architecture version */

/* alignment of basic data-types, used to initialize align[] */
static long malign[MAX_TYPE+1] = {
    1,1,2,4,4,8,4,8,8,1,4,1,1,1,4,1
};
/* sizes of basic data-types, used to initialize sizetab[] */
static long msizetab[MAX_TYPE+1] = {
    1,1,2,4,4,8,4,8,8,0,4,0,0,0,4,0
};

/* used to initialize regtyp[] */
static struct Typ ltyp  = {LONG};
static struct Typ lltyp = {LLONG};
static struct Typ ftyp  = {FLOAT};
static struct Typ dtyp  = {DOUBLE};

/* macros defined by the backend */
static char *marray[]={"__section(x,y)=__vattr(\"section(\"#x\",\"#y\")\")",
                       "__ARM__",
                       0};

/* special registers */
static int ip = FIRST_GPR+12;       /* inter-procedural scratch register */
static int sp = FIRST_GPR+13;       /* stack pointer */
static int lr = FIRST_GPR+14;       /* link register */
static int pc = FIRST_GPR+15;       /* program counter */
static int r0 = FIRST_GPR;
static int r1 = FIRST_GPR+1;

/* load/store instructions */
static char *ldts[MAX_TYPE+1] = {
    "ldr","ldrsb","ldrsh","ldr","ldr","ldr","ldr","ldr","ldr","??","ldr"};
static char *ldtu[MAX_TYPE+1] = {
    "ldr","ldrb","ldrh","ldr","ldr","ldr","??","??","??","??","??"};
static char *sdts[MAX_TYPE+1] = {
    "str","strb","strh","str","str","str","str","str","str","??","str"};
static char *sdtu[MAX_TYPE+1] = {
    "str","strb","strh","str","str","str","??","??","??","??","??"};
#define ldt(t) ((t&UNSIGNED) ? ldtu[(t)&NQ] : ldts[(t)&NQ])
#define sdt(t) ((t&UNSIGNED) ? sdtu[(t)&NQ] : sdts[(t)&NQ])

static char *ldstprefix[] = {
    "ld","st"
};

static char *dct[] = {
    "","byte","short","word","word","word","word","word","word"
};

static char *ccs[] = {
    "eq","ne","lt","ge","le","gt",""
};

static char *logicals[] = {
    "orr","eor","and"
};

static char *shifts[2][2] = {
    "lsl","asr","lsl","lsr"
};

/* sections */
#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define SPECIAL 4

#if 0
static long stack;
static int stack_valid;
#endif
static int newobj;
static int section = -1;

static char *codename = "\t.text\n";
static char *dataname = "\t.data\n";
static char *bssname = "";
static char *rodataname = "\t.rodata\n";

/* list of floating point constants to output at end of file */
struct fpconstlist {
  struct fpconstlist *next;
  int label;
  int typ;
  union atyps val;
};
static struct fpconstlist *firstfpc = NULL;

/* data reference pointers at the end of each function */
struct DataRefPtrList {
  struct DataRefPtrList *next;
  int label;
  struct Var *vptr;             /* valid, when label==0 */
};
static struct DataRefPtrList *dataptrs = NULL;
static int drefptr_array_label; /* current array's label */

#define isreg(x) (((x)&(REG|DREFOBJ))==REG)
#define isconst(x) (((x)&(KONST|DREFOBJ))==KONST)

static int q1reg,q2reg,zreg;
static struct Var *current_function;
static int icnt;                /* counts number of lines in cur. function */

#define MAXCODELINES 500        /* emit data-ref. ptr array after that */
#define MAXCOPYINSTS 4          /* max. nb. of load/store copy-instructions */

/* return-instruction */
static char *ret;

/* label at the end of the function (if any) */
static int exit_label;

/* assembly-prefixes for labels and external identifiers */
static char *labprefix="l";
static char *idprefix="_";

/* variables to calculate the size and partitioning of the stack-frame */
static long frameoffset,pushoffs,framesize;
static int needframe;
static long localsize,rsavesize,argsize,rsaveoffs,maxrsaveoffs;



static long real_offset(struct obj *o)
/* calculate the actual current offset of an object relative to the
   stack-pointer; we use a layout like this:
   ------------------------------------------------
   | stack-arguments to this function             |
   ------------------------------------------------
   | caller-save registers [size=rsavesize]       |
   ------------------------------------------------
   | local variables [size=localsize]             |
   ------------------------------------------------
   | arguments to called functions [size=argsize] |
   ------------------------------------------------
   All sizes will be aligned as necessary.
   The stack-pointer will be adjusted at
   function-entry to leave enough space for the arguments and have it
   aligned to 16 bytes. Therefore, when calling a function, the
   stack-pointer is always aligned to 16 bytes.
*/
{
  long off = zm2l(o->v->offset);

  if (off < 0) {
    /* function parameter */
    off = localsize + rsavesize - off - zm2l(maxalign);
  }
  else {
    /* local variable */
    off += argsize;
    off += zm2l(o->val.vmax);
  }

  return off;
}


static struct obj *cam(int flags,int base,long offset)
/*  Initializes an addressing-mode structure and returns a pointer to
    that object. Will not survive a second call! */
{
  static struct obj obj;
  static struct AddressingMode am;

#if 0
  obj.am = &am;
  am.flags = flags;
  am.base = base;
  am.offset = offset;
#endif
  return &obj;
}


static int objalign(struct obj *o)
/* yields the object's lower two bits, 1 when unknown */
{
  if (o->flags & DREFOBJ)
    return 1;
  if (o->am)
    ierror(0);
  if (!(o->flags & VAR))
    ierror(0);

  if (isstatic(o->v->storage_class) || isextern(o->v->storage_class)) {
    /* all static data should be 32-bits aligned */
    return zm2l(zmand(o->val.vmax,l2zm(3L)));
  }

  if (isauto(o->v->storage_class)) {
    zmax of = o->v->offset;

    if (!zmleq(l2zm(0L),of))
      of = zmsub(l2zm(0L),zmadd(of,maxalign));
    return zm2l(zmand(zmadd(of,o->val.vmax),l2zm(3L)));
  }

  ierror(0);
}


static void title(FILE *f)
/* set file symbol with input file name */
{
  extern char *inname;
  static int done;

  if (!done && f) {
    done = 1;
    emit(f,"\t.file\t\"%s\"\n",inname);
  }
}


static void emitnl(FILE *f)
/* emit a newline character */
{
  emit(f,"\n");
}


static void emit_obj(FILE *f,struct obj *p,int t)
/* prints an object */
{
  if (p->flags & VAR) {
    if (isauto(p->v->storage_class)) {
      emit(f,"[%s,#%ld]",regnames[sp],real_offset(p));
    }
    else {
      if (!zmeqto(l2zm(0L),p->val.vmax)) {
        emitval(f,&p->val,LONG);
        emit(f,"+");
      }
      if (isstatic(p->v->storage_class))
        emit(f,"%s%ld",labprefix,zm2l(p->v->offset));
      else
        emit(f,"%s%s",idprefix,p->v->identifier);
    }
  }

  if (p->flags & REG) {
    emit(f,"%s",regnames[p->reg]);
  }

  if (p->flags & KONST) {
    emit(f,"#");
    emitval(f,&p->val,t&NU);
  }
}


static int special_section(FILE *f,struct Var *v)
/* changes to a special section, used for __section() */
{
  char *sec;

  if (v->vattr) {
    if (sec = strstr(v->vattr,"section(")) {
      sec += strlen("section(");
      emit(f,"\t.section\t");
      while (*sec && *sec!=')')
        emit_char(f,*sec++);
      emitnl(f);
      if (f)
        section = SPECIAL;
      return 1;
    }
  }
  return 0;
}


static int addfpconst(struct obj *o,int t)
/* return label for a float-constant, create if it didn't exist */
{
  struct fpconstlist *p=firstfpc;

  t &= NQ;
  if (t == LDOUBLE)
    t = DOUBLE;

  for (p=firstfpc; p; p=p->next) {
    if (t == p->typ) {
      eval_const(&p->val,t);
      if (t==FLOAT && zldeqto(vldouble,zf2zld(o->val.vfloat)))
        return p->label;
      if (t==DOUBLE && zldeqto(vldouble,zd2zld(o->val.vdouble)))
        return p->label;
    }
  }

  p = mymalloc(sizeof(struct fpconstlist));
  p->next = firstfpc;
  p->label = ++label;
  p->typ = t;
  p->val = o->val;
  firstfpc = p;

  return p->label;
}


static void emit_dataptr_array(FILE *f)
/* emit all data-reference pointers which were collected until now,
   then reset the array */
{
  struct DataRefPtrList *drp,*next;

  if (next = dataptrs) {
    emit(f,"\t.align\t2\n");
    emit(f,"%s%d:\n",labprefix,drefptr_array_label);

    while (drp = next) {
      next = drp->next;
      emit(f,"\t.%s\t",dct[LONG]);
      if (drp->label) {
        emit(f,"%s%d\n",labprefix,drp->label);
      }
      else {
        if (isstatic(drp->vptr->storage_class))
          emit(f,"%s%ld\n",labprefix,zm2l(drp->vptr->offset));
        else
          emit(f,"%s%s\n",idprefix,drp->vptr->identifier);
      }
      free(drp);
    }
    dataptrs = NULL;
  }
  drefptr_array_label = 0;
}


static int cg_getreg(FILE *f,struct IC *ic)
/* allocate a code generator internal general purpose register */
{
  /* alloc_code: 1:compiler, 2:backend, >=4:backend rsave-area offset - 4 */
  int alloc_code = 2;
  int i,p,r;

  /* try to get a free scratch-register or a non-volatile
     register which has to be saved anyway,
     r12 (ip) is reserved to the backend and will be used as well,
     r14 (lr) is available when the function builds a stack frame */
  for (i=FIRST_GPR,p=0,r=0; i<FIRST_GPR+13; i++) {
    if (((i==ip && regs[i]==1) ||
         (i==lr && regs[i]==1 && needframe) ||
         (regs[i]==0 && (regscratch[i] || regused[i]))) &&
        reg_prio[i]+(regused[i]<<8) > p) {
      p = reg_prio[i] + (regused[i] << 8);
      r = i;
    }
  }
  if (!r) {
    /* seems we have to save a new non-volatile register */
    for (i=FIRST_GPR,p=0; i<FIRST_GPR+13; i++) {
      if (regs[i]==0 && reg_prio[i]>p) {
        p = reg_prio[i];
        r = i;
      }
    }
  }
  if (!r) {
    /* no register available - save one to the stack-frame, but
       make sure it is not used in this IC */
    for (i=FIRST_GPR,p=0; i<FIRST_GPR+13; i++) {
      if (regs[i]<2 && reg_prio[i]>p) {
        if ((!isreg(ic->q1.flags) || ic->q1.reg!=i) &&
            (!isreg(ic->q2.flags) || ic->q2.reg!=i) &&
            (!isreg(ic->z.flags) || ic->z.reg!=i)) {
          p = reg_prio[i];
          r = i;
        }
      }
    }
    if (r) {
      if (f)
        emit(f,"\tstr\t%s,[%s,#%ld]\n",
             regnames[r],regnames[sp],argsize+localsize+rsaveoffs);
      rsaveoffs += 4;
      if (rsaveoffs > maxrsaveoffs)
        maxrsaveoffs = rsaveoffs;
      alloc_code = rsaveoffs;
    }
    else
      ierror(0);
  }

  regs[r] = alloc_code;
  regused[r] = 1;
  return r;
}


static int cg_getdpreg(FILE *f,struct IC *ic)
/* allocate a code generator internal double precision FP register */
{
  int i,p,r;
  struct rpair rp;

  /* try to get a free scratch-register or a non-volatile
     register which has to be saved anyway */
  for (i=FIRST_DOUBLE,p=0,r=0; i<=LAST_PAIR; i++) {
    if (regs[i]==0 && (regscratch[i] || regused[i]) &&
        reg_prio[i]+(regused[i]<<8) > p && reg_pair(i,&rp)) {
      if (regs[rp.r1]==0 && regs[rp.r2]==0) {
        p = reg_prio[i] + (regused[i] << 8);
        r = i;
      }
    }
  }
  if (!r) {
    ierror(0); /* @@@ FIXME */
  }

  regs[r] = 2;
  regused[r] = 1;
  if (!reg_pair(i,&rp))
    ierror(0);
  regs[rp.r1] = regs[rp.r2] = 2;
  regused[rp.r1] = regused[rp.r2] = 1;
  return r;
}


static void cg_restorereg(FILE *f,int r)
{
  if (f) {
    if (r <= LAST_GPR)
      emit(f,"\tldr\t%s,[%s,#%ld]\n",
           regnames[r],regnames[sp],argsize+localsize+regs[r]-4);
    else
      ierror(0);  /* @@@ FIXME */
  }
}


static void cg_freereg(FILE *f,int r)
/* free a code generator internal general purpose register */
{
  struct rpair rp;

  if (regs[r] > 1) {
    if (regs[r] > 2)
      cg_restorereg(f,r);
    regs[r] = (r==ip||r==lr) ? 1 : 0;  /* ip/lr need to stay reserved */
  }
  else
    ierror(0);

  if (reg_pair(r,&rp)) {
    if (regs[rp.r1]>1 && regs[rp.r2]>1) {
      if (regs[rp.r1] > 2)
        cg_restorereg(f,rp.r1);
      if (regs[rp.r2] > 2)
        cg_restorereg(f,rp.r2);
      regs[rp.r1] = regs[rp.r2] = 0;
    }
    else
      ierror(0);
  }
}


static void cg_freeall(FILE *f)
/* reset internal register allocations */
{
  int i;

  for (i=1; i<=MAXR; i++) {
    if (regs[i] > 1) {
      if (regs[i] > 2)
        cg_restorereg(f,i);
      regs[i] = (i==ip||i==lr) ? 1 : 0;  /* ip/lr need to stay reserved */
    }
  }
  rsaveoffs = 0;
}


static long dataptr_offset(int label,struct obj *o)
/* return offset into data-reference-pointer array for label l
   or for object o (mutually exclusive) */
{
  long off = 0;
  struct DataRefPtrList **olddrp = &dataptrs;
  struct DataRefPtrList *drp = dataptrs;

  if (!label) {
    if (!(o->flags & VAR))
      ierror(0);
    if (!isstatic(o->v->storage_class) && !isextern(o->v->storage_class))
      ierror(0);
  }

  /* check if a pointer to this object already exists */
  while (drp) {
    if (label) {
      if (drp->label == label)
        break;
    }
    else {
      if (drp->vptr == o->v)
        break;
    }
    olddrp = &drp->next;
    drp = drp->next;
    off += 4;
  }

  /* create a new entry if it doesn't exist */
  if (drp == NULL) {
    *olddrp = drp = mymalloc(sizeof(struct DataRefPtrList));

    drp->next = NULL;
    if (drp->label = label)
      drp->vptr = NULL;
    else
      drp->vptr = o->v;
  }

  return off;
}


static void load_dataptr(FILE *f,int r,int l,struct obj *o)
/* load data-reference-pointer array entry for label l or object o into r */
{
  BSET(regs_modified,r);

  if (!drefptr_array_label)
    drefptr_array_label = ++label;

  emit(f,"\tldr\t%s,%s%d+%ld\n",
       regnames[r],labprefix,drefptr_array_label,dataptr_offset(l,o));
}


static void ldst64(FILE *f,int store,struct rpair *rp,int base)
/* generate a ldmia or stmia (store=1) instruction to transfer a 64-bit
   value in register pair rp pointed to by base-register base */
{
  emit(f,"%smia\t%s,{%s-%s}\n",
       ldstprefix[store],regnames[base],regnames[rp->r1],regnames[rp->r2]);
}


static void load_address(FILE *f,int r,struct obj *o,int type)
/*  Generates code to load the address of a variable into register r. */
{
  BSET(regs_modified,r);

  if (!(o->flags & VAR))
    ierror(0);

  if (isauto(o->v->storage_class))
    emit(f,"\tadd\t%s,%s,#%ld\n",regnames[r],regnames[sp],real_offset(o));
  else
    load_dataptr(f,r,0,o);
}


static void load_regindir(FILE *f,struct IC *p,int type,
                          int dst,int base,long off)
/*  Load register dst of type type from [base,#off].
    base and dst may be the same.
    Perform size extensions as required by architecture. */
{
  BSET(regs_modified,dst);

  if (aa >= AA4) {
    emit(f,"\t%s\t%s,[%s,#%ld]\n",
         ldt(type),regnames[dst],regnames[base],off);
  }

  else {
    /* this requires more effort on older ARMs */
    int tmp;

    switch (sizetab[type&NQ]) {
      case 1:
        emit(f,"\tldrb\t%s,[%s,#%ld]\n",regnames[dst],regnames[base],off);
        if (!(type & UNSIGNED)) {
          emit(f,"\tmov\t%s,%s,lsl #24\n",regnames[dst],regnames[dst]);
          emit(f,"\tmov\t%s,%s,asr #24\n",regnames[dst],regnames[dst]);
        }
        break;
      case 2:
        tmp = cg_getreg(f,p);
        emit(f,"\tldrb\t%s,[%s,#%ld]\n",regnames[tmp],regnames[base],
                                        off+1-arm_le_mode);
        emit(f,"\tldrb\t%s,[%s,#%ld]\n",regnames[dst],regnames[base],
                                        off+arm_le_mode);
        if (!(type & UNSIGNED)) {
          emit(f,"\tmov\t%s,%s,lsl #24\n",regnames[dst],regnames[dst]);
          emit(f,"\torr\t%s,%s,%s asr #16\n",
               regnames[dst],regnames[tmp],regnames[dst]);
        }
        else {
          emit(f,"\torr\t%s,%s,%s lsl #8\n",
               regnames[dst],regnames[tmp],regnames[dst]);
        }
        cg_freereg(f,tmp);
        break;
      case 4:
        emit(f,"\tldr\t%s,[%s,#%ld]\n",regnames[dst],regnames[base],off);
        break;
      default:
        ierror(0);
    }
  }
}


static void load_reg(FILE *f,struct IC *p,int r,struct obj *o,int type)
/* Generates code to load a memory object into register r. */
{
  type &= NU;
  BSET(regs_modified,r);

  if (o->flags & KONST) {
    /* evaluate and load a constant */

    eval_const(&o->val,type);

    if (ISFLOAT(type)) {
      int lab = addfpconst(o,type);

      load_dataptr(f,r,lab,NULL);
      if (type == FLOAT)
        load_regindir(f,p,type,r,r,0);
      else
        ierror(0);  /* double can only be in a register-pair */
    }

    else {
      /* integer constant */
      emit(f,"\tmov\t%s,",regnames[r]);
      emit_obj(f,o,type);
      emitnl(f);
    }
  }

  else if (isreg(o->flags)) {
    if (r != o->reg)
      emit(f,"\tmov\t%s,%s\n",regnames[r],regnames[o->reg]);
  }

  else if ((o->flags & (REG|DREFOBJ)) == (REG|DREFOBJ)) {
    load_regindir(f,p,type,r,o->reg,0);
  }

  else if (o->flags & VAR) {
    if (isstatic(o->v->storage_class) || isextern(o->v->storage_class)) {
      /* load from a static variable */
      if (o->flags & VARADR) {
        load_address(f,r,o,POINTER);
      }
      else {
        load_dataptr(f,r,0,o);
        load_regindir(f,p,type,r,r,zm2l(o->val.vmax));
      }
    }
    else {
      /* dynamic variable on stack */
      load_regindir(f,p,type,r,sp,real_offset(o));
    }
  }

  else
    ierror(0);
}


static void store_regindir(FILE *f,struct IC *p,int type,
                           int src,int base,long off)
/*  Store register src of type type to [base,#off]. */
{
  if (aa>=AA4 || sizetab[type&NQ]!=2) {
    emit(f,"\t%s\t%s,[%s,#%ld]\n",
         sdt(type),regnames[src],regnames[base],off);
  }

  else {
    /* storing halfwords requires more effort on older ARMs */
    int tmp = cg_getreg(f,p);

    emit(f,"\tstrb\t%s,[%s,#%ld]\n",regnames[src],regnames[base],
                                    off+1-arm_le_mode);
    emit(f,"\tmov\t%s,%s,asr #8\n",regnames[tmp],regnames[src]);
    emit(f,"\tstrb\t%s,[%s,#%ld]\n",regnames[src],regnames[base],
                                    off+arm_le_mode);
    cg_freereg(f,tmp);
  }
}


static void store_reg(FILE *f,struct IC *p,int r,struct obj *o,int type)
/* Generates code to store register r into memory object o. */
{
  if (isreg(o->flags)) {
    if (r != o->reg)
      emit(f,"\tmov\t%s,%s\n",regnames[o->reg],regnames[r]);
  }

  else if ((o->flags & (REG|DREFOBJ)) == (REG|DREFOBJ)) {
    store_regindir(f,p,type,r,o->reg,0);
  }

  else if (o->flags & VAR) {
    if (isstatic(o->v->storage_class) || isextern(o->v->storage_class)) {
      /* store to a static variable */
      int tmp = cg_getreg(f,p);

      load_dataptr(f,tmp,0,o);
      store_regindir(f,p,type,r,tmp,zm2l(o->val.vmax));
      cg_freereg(f,tmp);
    }
    else {
      /* dynamic variable on stack */
      store_regindir(f,p,type,r,sp,real_offset(o));
    }
  }

  else
    ierror(0);
}


static int load_objptr(FILE *f,struct IC *p,struct obj *o)
/* Make sure object o can be dereferenced by a single load.
   Return new base register, or 0 if object was not modified. */
{
  if ((o->flags & (REG|DREFOBJ)) == DREFOBJ) {
    int r = cg_getreg(f,p);

    load_reg(f,p,r,o,POINTER);
    o->flags |= REG;
    o->reg = r;
    return r;
  }

  if ((o->flags & (VAR|REG)) == VAR) {
    if (isstatic(o->v->storage_class) || isextern(o->v->storage_class)) {
      int r = cg_getreg(f,p);

      load_dataptr(f,r,0,o);
      o->reg = r;
      o->flags = REG|DREFOBJ;
      return r;
    }
  }

  return 0;
}


static void load_regpair(FILE *f,struct IC *p,struct rpair *rp,
                         struct obj *o,int type)
/* Generates code to load a memory object into the register pair rp.
   tmp is a general purpose register which may be used. */
{
  BSET(regs_modified,rp->r1);
  BSET(regs_modified,rp->r2);

  if (o->flags & KONST) {
    /* evaluate and load a constant */
    eval_const(&o->val,type);

    if (ISFLOAT(type)) {
      int lab = addfpconst(o,type);
      int tmp = cg_getreg(f,p);

      load_dataptr(f,tmp,lab,NULL);
      if (type != FLOAT)
        ldst64(f,0,rp,tmp);
      else
        ierror(0);  /* have to load float in a single register */
      cg_freereg(f,tmp);
    }
    else {
      struct obj cobj;

      cobj.flags = KONST;
      cobj.val.vulong = zum2zul(zumand(vumax,ul2zum(0xffffffff))); 
      load_reg(f,p,arm_le_mode?rp->r1:rp->r2,&cobj,UNSIGNED|LONG);
      cobj.val.vulong = zum2zul(zumand(zumrshift(vumax,ul2zum(32UL)),
                                       ul2zum(0xffffffff)));
      load_reg(f,p,arm_le_mode?rp->r2:rp->r1,&cobj,UNSIGNED|LONG);
    }
  }

  else {
    /* make sure that object can be addressed through a register */
    load_objptr(f,p,o);

    if (isreg(o->flags)) {
      struct rpair qrp;

      if (!reg_pair(o->reg,&qrp))
        ierror(0);
      if (qrp.r1 != rp->r1) {
        emit(f,"\tmov\t%s,%s\n\tmov\t%s,%s\n",regnames[rp->r1],
             regnames[qrp.r1],regnames[rp->r2],regnames[qrp.r2]);
      }
    }
    else if ((o->flags & (REG|DREFOBJ)) == (REG|DREFOBJ)) {
      ldst64(f,0,rp,o->reg);
    }
    else
      ierror(0);
  }
}


static void store_regpair(FILE *f,struct IC *p,struct rpair *rp,
                          struct obj *o,int type)
/* Generates code to store the register pair rp into memory object o.
   tmp is a general purpose register which may be used. */
{
  /* make sure that object can be addressed through a register */
  load_objptr(f,p,o);

  if (isreg(o->flags)) {
    struct rpair zrp;

    if (!reg_pair(o->reg,&zrp))
      ierror(0);
    if (zrp.r1 != rp->r1) {
      emit(f,"\tmov\t%s,%s\n\tmov\t%s,%s\n",regnames[zrp.r1],
           regnames[rp->r1],regnames[zrp.r2],regnames[rp->r2]);
    }
  }
  else if ((o->flags & (REG|DREFOBJ)) == (REG|DREFOBJ)) {
    ldst64(f,1,rp,o->reg);
  }
  else
    ierror(0);
}


static long pof2(zumax x)
/* yields log2(x)+1 or 0 */
{
  zumax p;
  int ln = 1;

  p = ul2zum(1L);
  while (ln<=32 && zumleq(p,x)) {
    if (zumeqto(x,p))
        return ln;
    ln++;
    p = zumadd(p,p);
  }
  return 0;
}


static struct IC *preload(FILE *f,struct IC *p)
/* Does some pre-processing like fetching operands from memory to
   registers etc. */
/* @@@ FIXME - Is this function needed at all ??? */
{
#if 0
  if (isreg(p->q1.flags))
    q1reg = p->q1.reg;
  else
    q1reg = 0;

  if (isreg(p->q2.flags))
    q2reg = p->q2.reg;
  else
    q2reg = 0;
#endif

  if (isreg(p->z.flags)) {
    zreg = p->z.reg;
  }
  else {
    if (ISFLOAT(ztyp(p)))
      zreg = FIRST_PAIR;  /* @@@ VFP? ->f1 */
    else
      zreg = cg_getreg(f,p);
  }
  
#if 0  /* Better use load_objptr() when needed? */
  if ((p->q1.flags & (REG|DREFOBJ)) == DREFOBJ) {
    int tmp = cg_getreg(f,p);

    p->q1.flags &= ~DREFOBJ;
    load_reg(f,p,tmp,&p->q1,q1typ(p));
    p->q1.reg = tmp;
    p->q1.flags |= REG|DREFOBJ;
  }

  if ((p->q2.flags & (REG|DREFOBJ)) == DREFOBJ) {
    int tmp = cg_getreg(f,p);

    p->q2.flags &= ~DREFOBJ;
    load_reg(f,p,tmp,&p->q2,q2typ(p));
    p->q2.reg = tmp;
    p->q2.flags |= REG|DREFOBJ;
  }
#endif

  return p;
}


static void save_result(FILE *f,struct IC *p)
/* save the result (in zreg) into p->z */
{
  if ((p->z.flags&(REG|DREFOBJ)) == DREFOBJ) {
    int tmp = cg_getreg(f,p);

    p->z.flags &= ~DREFOBJ;
    load_reg(f,p,tmp,&p->z,POINTER);
    p->z.reg = tmp;
    p->z.flags |= REG|DREFOBJ;
  }
  if (isreg(p->z.flags)) {
    if (p->z.reg != zreg)
      emit(f,"\tmov\t%s,%s\n",regnames[p->z.reg],regnames[zreg]);
  }
  else {
    store_reg(f,p,zreg,&p->z,ztyp(p));
  }
}


static void registerize(FILE *f,struct IC *p,struct obj *o,int t)
/* make sure object is loaded into a register */
{
  if (!(isreg(o->flags))) {
    int r;

    r = load_objptr(f,p,o);
    if (!r)
      r = cg_getreg(f,p);
    if ((o->flags & (REG|DREFOBJ)) == DREFOBJ)
      ierror(0);
    load_reg(f,p,r,o,t);
    o->reg = r;
  }
  o->flags = REG;
}


static void cg_memcopy(FILE *f,struct IC *p)
/* generates code to copy an object of non-elementary type (ARRAY, STRUCT) */
{
  unsigned long size = opsize(p);
  int a1 = objalign(&p->q1);
  int a2 = (p->code==ASSIGN) ? objalign(&p->z) : 0;
  int b = 1;
  char *ld = ldt(CHAR);
  char *st = sdt(CHAR);
  unsigned long l;
  int srcreg,dstreg,tmpreg,cntreg,ncp;

  if (p->q1.flags & VAR) {
    if (p->q1.flags & DREFOBJ) {
      if (p->q1.v->vtyp->next &&
          zmeqto(p->q2.val.vmax,szof(p->q1.v->vtyp->next)) && (a1&1)) {
        a1 = zm2l(falign(p->q1.v->vtyp->next)) & 3;
        a2 &= a1;
      }
    }
    else {
      if (zmeqto(p->q2.val.vmax,szof(p->q1.v->vtyp)) && (a1&1)) {
        a1 = zm2l(falign(p->q1.v->vtyp)) & 3;
        a2 &= a1;
      }
    }
  }
  if (p->z.flags & VAR) {
    if (p->z.flags & DREFOBJ) {
      if (p->z.v->vtyp->next &&
          zmeqto(p->q2.val.vmax,szof(p->z.v->vtyp->next)) && (a2&1)) {
        a2 = zm2l(falign(p->z.v->vtyp->next)) & 3;
        a1 &= a2;
      }
    }
    else {
      if (zmeqto(p->q2.val.vmax,szof(p->z.v->vtyp)) && (a2&1)) {
        a2 = zm2l(falign(p->z.v->vtyp)) & 3;
        a1 &= a2;
      }
    }
  } 

  /* @@@ implement with ldmia/stmia */
  if (a1>=0 && a2>=0) {
    if (a1==0 && a2==0) {
      /* 32-bit copy */
      b = 4;
      ld = ldt(LONG);
      st = sdt(LONG);
    }
    else if ((a1&1)==0 && (a2&1)==0 && aa>=AA4) {
      /* 16-bit copy for ARM-architecture 4 and better only */
      b = 2;
      ld = ldt(SHORT);
      st = sdt(SHORT);
    }
  }

  srcreg = cg_getreg(f,p);
  BSET(regs_modified,srcreg);

  if (p->q1.flags & DREFOBJ) {
    p->q1.flags &= ~DREFOBJ;
    if (isreg(p->q1.flags))
      emit(f,"\tmov\t%s,%s\n",regnames[srcreg],regnames[p->q1.reg]);
    else
      load_reg(f,p,srcreg,&p->q1,POINTER);
    p->q1.flags |= DREFOBJ;
  }
  else {
    load_address(f,srcreg,&p->q1,POINTER);
  }

  dstreg = cg_getreg(f,p);
  BSET(regs_modified,dstreg);

  if (p->z.flags & DREFOBJ) {
    p->z.flags &= ~DREFOBJ;
    if (isreg(p->z.flags))
      emit(f,"\tmov\t%s,%s\n",regnames[dstreg],regnames[p->z.reg]);
    else
      load_reg(f,p,dstreg,&p->z,POINTER);
    p->z.flags |= DREFOBJ;
  }
  else {
    if (p->code == PUSH) {
      emit(f,"\tadd\t%s,%s,#%ld\n",regnames[dstreg],regnames[sp],pushoffs);
      pushoffs += size;
    }
    else
      load_address(f,dstreg,&p->z,POINTER);
  }

  tmpreg = cg_getreg(f,p);
  BSET(regs_modified,tmpreg);
  l = size/b;

  if (l > MAXCOPYINSTS) { /* @@@ make MAXCOPYINSTS changeable by an option? */
    cntreg = cg_getreg(f,p);
    BSET(regs_modified,cntreg);
    emit(f,"\tmov\t%s,#%lu\n",regnames[cntreg],l);
    emit(f,"%s%d:\n",labprefix,++label);
    ncp = 1;
  }
  else
    ncp = l;
  while (ncp--) {
    emit(f,"\t%s\t%s,[%s],#%d\n",ld,regnames[tmpreg],regnames[srcreg],b);
    emit(f,"\t%s\t%s,[%s],#%d\n",st,regnames[tmpreg],regnames[dstreg],b);
  }
  if (l > MAXCOPYINSTS) {
    emit(f,"\tsubs\t%s,%s,#1\n",regnames[cntreg],regnames[cntreg]);
    emit(f,"\tbne\t%s%d\n",labprefix,label);
    cg_freereg(f,cntreg);
  }

  size = size % b;
  ncp = 0;
  if (size & 2) {
    if (aa >= AA4) {
      emit(f,"\t%s\t%s,[%s],#%d\n",ldt(SHORT),regnames[tmpreg],regnames[srcreg],b);
      emit(f,"\t%s\t%s,[%s],#%d\n",sdt(SHORT),regnames[tmpreg],regnames[dstreg],b);
    }
    else
      ncp = 2;
  }
  ncp += (size & 1);
  while (ncp--) {
    emit(f,"\t%s\t%s,[%s],#%d\n",ldt(CHAR),regnames[tmpreg],regnames[srcreg],b);
    emit(f,"\t%s\t%s,[%s],#%d\n",sdt(CHAR),regnames[tmpreg],regnames[dstreg],b);
  }
}


static int exists_freereg(struct IC *p,int reg)
/*  Test if there is a sequence of FREEREGs containing FREEREG reg.
    Used by peephole(). */
{
  while (p && (p->code==FREEREG || p->code==ALLOCREG)) {
    if (p->code==FREEREG && p->q1.reg==reg)
      return 1;
    p=p->next;
  }
  return 0;
}


#if 0
static void peephole(struct IC *p)
/* search for possible addressing-modes */
{
  int c,c2,r;struct IC *p2;struct AddressingMode *am;

  for(;p;p=p->next){
    c=p->code;
    if(c!=FREEREG&&c!=ALLOCREG&&(c!=SETRETURN||!isreg(p->q1.flags)||p->q1.reg!=p->z.reg)) exit_label=0;
    if(c==LABEL) exit_label=p->typf;

    /* Try const(reg) */
    if(IMM_IND&&(c==ADDI2P||c==SUBIFP)&&isreg(p->z.flags)&&isconst(p->q2.flags)){
      int base;zmax of;struct obj *o;
      eval_const(&p->q2.val,p->typf);
      if(c==SUBIFP) of=zmsub(l2zm(0L),vmax); else of=vmax;
      if(1/*zmleq(l2zm(-32768L),vmax)&&zmleq(vmax,l2zm(32767L))*/){
        r=p->z.reg;
        if(isreg(p->q1.flags)) base=p->q1.reg; else base=r;
        o=0;
        for(p2=p->next;p2;p2=p2->next){
          c2=p2->code;
          if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
          if(c2!=FREEREG&&isreg(p2->q1.flags)&&p2->q1.reg==r) break;
          if(c2!=FREEREG&&isreg(p2->q2.flags)&&p2->q2.reg==r) break;
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
          if(c2==FREEREG||isreg(p2->z.flags)){
            int m;
            if(c2==FREEREG)
              m=p2->q1.reg;
            else
              m=p2->z.reg;
            if(m==r){
              if(o){
                o->am=am=mymalloc(sizeof(*am));
                am->flags=IMM_IND;
                am->base=base;
                am->offset=zm2l(of);
                if(isreg(p->q1.flags)){
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
    if(GPR_IND&&c==ADDI2P&&isreg(p->q2.flags)&&isreg(p->z.flags)&&(isreg(p->q1.flags)||p->q2.reg!=p->z.reg)){
      int base,idx;struct obj *o;
      r=p->z.reg;idx=p->q2.reg;
      if(isreg(p->q1.flags)) base=p->q1.reg; else base=r;
      o=0;
      for(p2=p->next;p2;p2=p2->next){
        c2=p2->code;
        if(c2==CALL||c2==LABEL||(c2>=BEQ&&c2<=BRA)) break;
        if(c2!=FREEREG&&isreg(p2->q1.flags)&&p2->q1.reg==r) break;
        if(c2!=FREEREG&&isreg(p2->q2.flags)&&p2->q2.reg==r) break;
        if(isreg(p2->z.flags)&&p2->z.reg==idx&&idx!=r) break;
        
        if(c2!=CALL&&(c2<LABEL||c2>BRA)/*&&c2!=ADDRESS*/){
          if(!p2->q1.am&&(p2->q1.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q1.reg==r){
            if(o||(q1typ(p2)&NQ)==LLONG) break;
            o=&p2->q1;
          }
          if(!p2->q2.am&&(p2->q2.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->q2.reg==r){
            if(o||(q2typ(p2)&NQ)==LLONG) break;
            o=&p2->q2;
          }
          if(!p2->z.am&&(p2->z.flags&(REG|DREFOBJ))==(REG|DREFOBJ)&&p2->z.reg==r){
            if(o||(ztyp(p2)&NQ)==LLONG) break;
            o=&p2->z;
          }
        }
        if(c2==FREEREG||isreg(p2->z.flags)){
          int m;
          if(c2==FREEREG)
            m=p2->q1.reg;
          else
            m=p2->z.reg;
          if(m==r){
            if(o){
              o->am=am=mymalloc(sizeof(*am));
              am->flags=GPR_IND;
              am->base=base;
              am->offset=idx;
              if(isreg(p->q1.flags)){
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
#endif


static void function_top(FILE *f,struct Var *v,long offset)
/* generates the function entry code */
{
  static char ret_instr[64];
  char gprsave[32];
  char *p;
  int i;

  title(f);

  /* determine rsavesize and registers to save */
  rsavesize = 0;
  gprsave[0] = '\0';
  for (i=FIRST_GPR,p=gprsave; i<=LAST_GPR; i++) {
    if (!regsa[i] && !regscratch[i] && regused[i]) {
      p += sprintf(p,"%s,",regnames[i]);
      rsavesize += 4;
    }
  }

  if (!special_section(f,v) && section!=CODE) {
    emit(f,codename);
    section = CODE;
  } 

  if (isextern(v->storage_class)) {
    if ((v->flags & (INLINEFUNC|INLINEEXT)) != INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier);
  }
  else
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));

  if (rsavesize==0 && !needframe) {
    ret = "\tmov\tpc,lr\n";
  }
  else {
    rsavesize += 3*4;  /* sp,lr,pc to be saved as well */
    emit(f,"\tstmfd\t%s!,{%s%s,%s,%s}\n",
         regnames[sp],gprsave,regnames[sp],regnames[lr],regnames[pc]);
    sprintf(ret_instr,"\tldmfd\t%s,{%s%s,%s}\n",
            regnames[sp],gprsave,regnames[sp],regnames[pc]);
    ret = ret_instr;
    if (localsize+argsize > 0) {
      emit(f,"\tsub\t%s,%s,#%ld\n",
           regnames[sp],regnames[sp],localsize+argsize);
    }
  }
}


static void function_bottom(FILE *f,struct Var *v,long offset)
/* generates the function exit code */
{
  if (localsize+argsize > 0) {
    emit(f,"\tadd\t%s,%s,#%ld\n",
         regnames[sp],regnames[sp],localsize+argsize);
  }
  emit(f,ret);
  emit_dataptr_array(f);

  if (isextern(v->storage_class)) {
    emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
    emit(f,"\t.size\t%s%s,.-%s%s\n\n",
         idprefix,v->identifier,idprefix,v->identifier);
  }
  else {
    emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
    emit(f,"\t.size\t%s%ld,.-%s%ld\n\n",
         labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
  }    

  /*if(all_regs&&v->fi) v->fi->flags|=ALL_REGS;*/
}


/****************************************/
/*  End of private data and functions.  */
/****************************************/


int init_cg(void)
/*  Does necessary initializations for the code-generator. Gets called  */
/*  once at the beginning and should return 0 in case of problems.      */
{
  int i;

  /*  Initialize some values which cannot be statically initialized   */
  /*  because they are stored in the target's arithmetic.             */
  maxalign = l2zm(8L);
  stackalign = l2zm(4L);
  char_bit = l2zm(8L);

  for (i=0; i<=MAX_TYPE; i++) {
    sizetab[i] = l2zm(msizetab[i]);
    align[i] = l2zm(malign[i]);
  }

  for (i=FIRST_GPR; i<=LAST_GPR; i++) {
    regsize[i] = l2zm(4L);
    regtype[i] = &ltyp;
  }

  for (i=FIRST_FPR; i<=LAST_FPR; i++) {
    regsize[i] = l2zm(8L);
    regtype[i] = &ftyp;
  }

  for (i=FIRST_CCR; i<=LAST_CCR; i++) {
    regsize[i] = l2zm(4L);
    regtype[i] = &ltyp;
  }

  for (i=FIRST_PAIR; i<FIRST_DOUBLE; i++) {
    regsize[i] = l2zm(8L);
    regtype[i] = &lltyp;
  }

  for (i=FIRST_DOUBLE; i<=LAST_PAIR; i++) {
    regsize[i] = l2zm(8L);
    regtype[i] = &dtyp;
  }

  /*  Initialize the min/max-settings. Note that the types of the     */
  /*  host system may be different from the target system and you may */
  /*  only use the smallest maximum values ANSI guarantees if you     */
  /*  want to be portable.                                            */
  /*  That's the reason for the subtraction in t_min[INT]. Long could */
  /*  be unable to represent -2147483648 on the host system.          */
  t_min[CHAR] = l2zm(-128L);
  t_min[SHORT] = l2zm(-32768L);
  t_min[INT] = zmsub(l2zm(-2147483647L),l2zm(1L));
  t_min[LONG] = t_min(INT);
  t_min[LLONG] = zmlshift(l2zm(1L),l2zm(63L));
  t_min[MAXINT] = t_min(LLONG);
  t_max[CHAR] = ul2zum(127L);
  t_max[SHORT] = ul2zum(32767UL);
  t_max[INT] = ul2zum(2147483647UL);
  t_max[LONG] = t_max(INT);
  t_max[LLONG] = zumrshift(zumkompl(ul2zum(0UL)),ul2zum(1UL));
  t_max[MAXINT] = t_max(LLONG);
  tu_max[CHAR] = ul2zum(255UL);
  tu_max[SHORT] = ul2zum(65535UL);
  tu_max[INT] = ul2zum(4294967295UL);
  tu_max[LONG] = t_max(UNSIGNED|INT);
  tu_max[LLONG] = zumkompl(ul2zum(0UL));
  tu_max[MAXINT] = t_max(UNSIGNED|LLONG);

  /*  Reserve a few registers for use by the code-generator.      */
  regsa[ip] = regsa[sp] = regsa[lr] = regsa[pc] = 1;
  regscratch[ip] = regscratch[sp] = regscratch[lr] = regscratch[pc] = 0;

  if (LE_MODE)
    arm_le_mode = 1;
  if (BE_MODE)
    arm_le_mode = 0;
  if (ARM_DEFAULT)
    thumb_default = 0;
  if (THUMB_DEFAULT)
    thumb_default = 1;
  if (ELF_LABELS) {
    labprefix = ".l";
    idprefix = "";
  }

  target_macros = marray;

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
  int tu = t->flags&NQ;

  /* @@@ handle VFP */

  if (tu==LLONG || tu==DOUBLE)
    return FIRST_PAIR;

  if (zmleq(szof(t),l2zm(4L)))
    return FIRST_GPR;

  return 0;
}


int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r  */
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two   */
/* elements.                                            */
{
  if (r<FIRST_PAIR || r>LAST_PAIR)
    return 0;

  switch (r) {
    case FIRST_PAIR:
      p->r1 = FIRST_GPR;
      p->r2 = FIRST_GPR+1;
      return 1;
    case FIRST_PAIR+1:
      p->r1 = FIRST_GPR+2;
      p->r2 = FIRST_GPR+3;
      return 1;
    case FIRST_PAIR+2:
      p->r1 = FIRST_GPR+4;
      p->r2 = FIRST_GPR+5;
      return 1;
    case FIRST_PAIR+3:
      p->r1 = FIRST_GPR+6;
      p->r2 = FIRST_GPR+7;
      return 1;
    case FIRST_PAIR+4:
      p->r1 = FIRST_GPR+10;
      p->r2 = FIRST_GPR+11;
      return 1;
    case FIRST_DOUBLE:
      p->r1 = FIRST_FPR;
      p->r2 = FIRST_FPR+1;
      return 1;
    case FIRST_DOUBLE+1:
      p->r1 = FIRST_FPR+2;
      p->r2 = FIRST_FPR+3;
      return 1;
    case FIRST_DOUBLE+2:
      p->r1 = FIRST_FPR+4;
      p->r2 = FIRST_FPR+5;
      return 1;
    case FIRST_DOUBLE+3:
      p->r1 = FIRST_FPR+6;
      p->r2 = FIRST_FPR+7;
      return 1;
    case FIRST_DOUBLE+4:
      p->r1 = FIRST_FPR+8;
      p->r2 = FIRST_FPR+9;
      return 1;
    case FIRST_DOUBLE+5:
      p->r1 = FIRST_FPR+10;
      p->r2 = FIRST_FPR+11;
      return 1;
    case FIRST_DOUBLE+6:
      p->r1 = FIRST_FPR+12;
      p->r2 = FIRST_FPR+13;
      return 1;
    case FIRST_DOUBLE+7:
      p->r1 = FIRST_FPR+14;
      p->r2 = FIRST_FPR+15;
      return 1;
    default:
      ierror(0);
  }

  return 0;
}


int cost_savings(struct IC *p,int r,struct obj *o)
/* estimate the cost-saving if object o from IC p is placed in */
/* register r                                                  */
{
  int c = p->code;

  /* @@@ FIXME */
  if (o->flags & VKONST) {
    if (o==&p->q1 && p->code==ASSIGN && (p->z.flags&DREFOBJ))
      return 4;
    else
      return 2;
  }
  if (o->flags & DREFOBJ)
    return 4;
  if (c==SETRETURN && r==p->z.reg && !(o->flags&DREFOBJ))
    return 3;
  if (c==GETRETURN && r==p->q1.reg && !(o->flags&DREFOBJ))
    return 3;
  return 2;
}


int regok(int r,int t,int mode)
/*  Returns 0 if register r cannot store variables of   */
/*  type t. If t==POINTER and mode!=0 then it returns   */
/*  non-zero only if the register can store a pointer   */
/*  and dereference a pointer to mode.                  */
{
  if (r==0)
    return 0;

  if (t==0 && r>=FIRST_CCR && r<=LAST_CCR)
    return 1;

  t &= NQ;

  if (ISFLOAT(t)) {
    /* @@@ handle VFP */
    if (t==FLOAT && r>=FIRST_GPR && r<=LAST_GPR)
      return 1;
    else if (r>=FIRST_PAIR && r<=LAST_PAIR)
      return 1;
  }
  else if (t==POINTER && r>=FIRST_GPR && r<=LAST_GPR)
    return 1;
  else if (t>=CHAR && t<=LONG && r>=FIRST_GPR && r<=LAST_GPR)
    return 1;
  else if (t==LLONG && r>=FIRST_PAIR && r<=LAST_PAIR)
    return 1;

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
  if ((p->q1.flags&DREFOBJ) || (p->q2.flags&DREFOBJ) || (p->z.flags&DREFOBJ))
    return 1;
  /* ARM has no division/modulo instructions */
  return 0;
}


int must_convert(int o,int t,int const_expr)
/*  Returns zero if code for converting np to type t    */
/*  can be omitted.                                     */
/*  On the PowerPC cpu pointers and 32bit               */
/*  integers have the same representation and can use   */
/*  the same registers.                                 */
{
  int op = o & NQ;
  int tp = t & NQ;

  if ((op==INT||op==LONG||op==POINTER) && (tp==INT||tp==LONG||tp==POINTER))
    return 0;
  if (op==DOUBLE && tp==LDOUBLE)
    return 0;
  if (op==LDOUBLE && tp==DOUBLE)
    return 0;
  if(op==tp)
    return 0;
  return 1;
}


char *use_libcall(np p)
/* Return name of library function, if this node should be
   implemented via libcall. */
{
  static char fname[16];
  char *ret = NULL;
  int f = p->ntyp->flags & NU;

  if ((f&NQ) == LLONG) {
    ierror(0);  /* @@@ FIXME */
  }

  else if ((((p->flags==MULT || p->flags==PMULT) && aa<AA2) ||
           (p->flags>MULT && p->flags<=MOD)) && (f&NQ) <= LONG) {
    if ((f&UNSIGNED) && (p->flags==DIV || p->flags==MOD))
      sprintf(fname,"__%s",ename[p->flags]);
    else
      sprintf(fname,"__%ss",ename[p->flags==PMULT?MULT:p->flags]);
    ret = fname;
  }

  if (ret) {
    /* declare function if necessary */
    struct struct_declaration *sd;
    struct Typ *t;
    struct Var *v;

    if (!find_ext_var(ret)) {
      sd = mymalloc(sizeof(*sd));
      sd->count = 0;
      t = new_typ();
      t->flags = FUNKT;
      t->exact = add_sd(sd,FUNKT);
      t->next = clone_typ(p->ntyp);
      add_var(ret,t,EXTERN,0);  
    }
  }

  return ret;
}


void gen_ds(FILE *f,zmax size,struct Typ *t)
/*  This function has to create <size> bytes of storage */
/*  initialized with zero.                              */
{
  title(f);

  if (newobj && section!=SPECIAL)
    emit(f,"%ld\n",zm2l(size));
  else
    emit(f,"\t.space\t%ld\n",zm2l(size));

  newobj = 0;
}


void gen_align(FILE *f,zmax align)
/*  This function has to make sure the next data is     */
/*  aligned to multiples of <align> bytes.              */
{
  title(f);

  if (zm2l(align) > 1)
    emit(f,"\t.align\t2\n");
}


void gen_var_head(FILE *f,struct Var *v)
/*  This function has to create the head of a variable  */
/*  definition, i.e. the label and information for      */
/*  linkage etc.                                        */
{
  int constflag;
  char *sec;

  title(f);

  if(v->clist)
    constflag = is_const(v->vtyp);

  if (isstatic(v->storage_class)) {
    if (ISFUNC(v->vtyp->flags))
      return;
    if (!special_section(f,v)) {
      if (v->clist && (!constflag || CONST_IN_DATA) && section!=DATA) {
        emit(f,dataname);
        if (f)
          section = DATA;
      }
      if (v->clist && constflag && !CONST_IN_DATA && section!=RODATA) {
        emit(f,rodataname);
        if (f)
          section = RODATA;
      }
      if (!v->clist && section!=BSS) {
        emit(f,bssname);
        if (f)
          section = BSS;
      }
    }
    if (v->clist || section==SPECIAL) {
      gen_align(f,falign(v->vtyp));
      emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
    }
    else
      emit(f,"\t.lcomm\t%s%ld,",labprefix,zm2l(v->offset));
    newobj = 1;
  }

  if (isextern(v->storage_class)) {
    emit(f,"\t.globl\t%s%s\n",idprefix,v->identifier);
    if (v->flags & (DEFINED|TENTATIVE)) {
      if (!special_section(f,v)) {
        if (v->clist && (!constflag || CONST_IN_DATA) && section!=DATA) {
          emit(f,dataname);
          if(f)
            section = DATA;
        }
        if (v->clist && constflag && !CONST_IN_DATA && section!=RODATA) {
          emit(f,rodataname);
          if (f)
            section = RODATA;
        }
        if (!v->clist && section!=BSS) {
          emit(f,bssname);
          if (f)
            section = BSS;
        }
      }
      if (v->clist || section==SPECIAL) {
        gen_align(f,falign(v->vtyp));
        emit(f,"%s%s:\n",idprefix,v->identifier);
      }
      else {
        emit(f,"\t.global\t%s%s\n\t.%scomm\t%s%s,",idprefix,
             v->identifier,(USE_COMMONS?"":"l"),idprefix,v->identifier);
      }
      newobj = 1;
    }
  }
}


void gen_dc(FILE *f,int t,struct const_list *p)
/*  This function has to create static storage          */
/*  initialized with const-list p.                      */
{
  title(f);

  if ((t&NQ) == POINTER)
    t = UNSIGNED|LONG;
  emit(f,"\t.%s\t",dct[t&NQ]);

  if (!p->tree) {
    if (ISFLOAT(t)) {
      unsigned char *ip = (unsigned char *)&p->val.vdouble;

      emit(f,"0x%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
      if ((t&NQ) != FLOAT)
        emit(f,",0x%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
    }
    else {
      emitval(f,&p->val,t&NU);
    }
  }
  else {
    emit_obj(f,&p->tree->o,t&NU);
  }

  emitnl(f);
  newobj = 0;
}


/*  The main code-generation routine.                   */
/*  f is the stream the code should be written to.      */
/*  p is a pointer to a doubly linked list of ICs       */
/*  containing the function body to generate code for.  */
/*  v is a pointer to the function.                     */
/*  offset is the size of the stackframe the function   */
/*  needs for local variables.                          */

void gen_code(FILE *f,struct IC *p,struct Var *v,zmax offset)
/*  The main code-generation routine.                   */
{
  FILE *outfile = f;
  struct IC *p_start = p;
  int c,t,i,pass;
  struct rpair rp;

  if (DEBUG & 1)
    printf("gen_code()\n");

  current_function = v;
  icnt = 0;
  drefptr_array_label = 0;
  argsize = 0;
  rsavesize = 0;
  maxrsaveoffs = 0;
  needframe = 0;

  for (c=1; c<=MAXR; c++)
    regs[c] = regsa[c];

  for (p=p_start; p; p=p->next) {
    c = p->code;
    t = p->typf & NU;

    if (c == ALLOCREG) {
      if (reg_pair(p->q1.reg,&rp)) {
        regs[rp.r1] = 1;
        regs[rp.r2] = 1;
      }
      regs[p->q1.reg] = 1;
      continue;
    }

    if (c == FREEREG) {
      if (reg_pair(p->q1.reg,&rp)) {
        regs[rp.r1] = 0;
        regs[rp.r2] = 0;
      }
      regs[p->q1.reg] = 0;
      continue;
    }

    /* try MULT/DIV/MOD with powers of two */
    if ((c==MULT || ((c==DIV || c==MOD) && (t&UNSIGNED))) &&
        (t&NQ)<=LONG && isconst(p->q2.flags)) {
      eval_const(&p->q2.val,t);
      if (i = pof2(vmax)) {
        if (c == MOD) {
          vmax = zmsub(vmax,l2zm(1L));
          p->code = AND;
        }
        else {
          vmax = l2zm(i-1);
          p->code = c==DIV ? RSHIFT : LSHIFT;
        }
        c = p->code;
        gval.vmax = vmax;
        eval_const(&gval,MAXINT);
        if (c == AND) {
          insert_const(&p->q2.val,t);
        }
        else {
          insert_const(&p->q2.val,INT);
          p->typf2 = INT;
        }
      }
    }

    if (c == CALL) {
      needframe = 1;
      if (argsize < zm2l(pushedargsize(p)))
        argsize = zm2l(pushedargsize(p));  /* set max argsize */
    }
  }

  /*peephole(p); @@@ FIXME */

  for (c=i; i<=MAXR; i++) {
    if (regsa[i] || regused[i])
      BSET(regs_modified,i);
    if (!regsa[i] && !regscratch[i] && regused[i])
      needframe = 1;
  }

  /* determine word-aligned space for local variables */
  localsize = ((zm2l(offset) + 3) / 4) * 4;
  if (localsize > 0)
    needframe = 1;

  for (pass=0,f=NULL; pass<2; pass++,f=outfile) {
    struct IC my_ic;
    struct IC *p2;

    if (pass) {
      emit_dataptr_array(NULL);  /* reset ptr-array */
      icnt = 0;
      function_top(f,v,argsize+localsize);
    }

    pushoffs = 0;

    for (p2=p_start; p2; p2=p2->next) {
      if (pass) {
        p = p2;
      }
      else {  /* work on a copy in first pass */
        p = &my_ic;
        *p = *p2;
      }

      c = p->code;
      t = p->typf;

      cg_freeall(f);  /* reset internal register allocations */

      if (icnt > MAXCODELINES) {
        /* function has grown too big, emit all data-reference pointers first */
        emit(f,"\tb\t%s%d\n",labprefix,++label);
        emit_dataptr_array(f);
        emit(f,"%s%d:\n",labprefix,label);
        icnt = 0;
      }

      if (c == NOP) {
        p->z.flags = 0;
        continue;
      }

      if (c == ALLOCREG) {
        if (reg_pair(p->q1.reg,&rp)) {
          regs[rp.r1] = 1;
          regs[rp.r2] = 1;
        }
        regs[p->q1.reg] = 1;
        continue;
      }

      if (c == FREEREG) {
        if (reg_pair(p->q1.reg,&rp)) {
          regs[rp.r1] = 0;
          regs[rp.r2] = 0;
        }
        regs[p->q1.reg] = 0;
        continue;
      }

      if (c == LABEL) {
        emit(f,"%s%d:\n",labprefix,t);
        continue;
      }

      if (c == BRA) {
        if (t==exit_label && framesize==0)
          emit(f,ret);
        else
          emit(f,"\tb\t%s%d\n",labprefix,t);
        continue;
      }

      if (c>=BEQ && c<BRA) {
        emit(f,"\tb%s\t",ccs[c-BEQ]);
        if (isreg(p->q1.flags)) {
          ierror(0);  /* @@@ was ist das? */
          emit_obj(f,&p->q1,0);
          emit(f,",");
        }
        emit(f,"%s%d\n",labprefix,t);
        continue;
      }

      if (c == MOVETOREG) {
        if (p->z.reg>=FIRST_GPR && p->z.reg<=LAST_GPR) {
          load_reg(f,p,p->z.reg,&p->q1,t);
        }
        else if (reg_pair(p->z.reg,&rp)) {
          BSET(regs_modified,p->z.reg);
          load_regpair(f,p,&rp,&p->q1,t);
        }
        else
          ierror(0);  /* @@@ VFP registers */
        p->z.flags = 0;
        continue;
      }

      if (c == MOVEFROMREG) {
        if (p->q1.reg>=FIRST_GPR && p->q1.reg<=LAST_GPR) {
          store_reg(f,p,p->q1.reg,&p->z,t);
        }
        else if (reg_pair(p->q1.reg,&rp)) {
          store_regpair(f,p,&rp,&p->z,t);
        }
        else
          ierror(0);  /* @@@ VFP registers */
        p->z.flags = 0;
        continue;
      }

      if ((c==ASSIGN || c==PUSH) &&
          ((t&NQ)>POINTER || ((t&NQ)==CHAR && zm2l(opsize(p))!=1))) {
        cg_memcopy(f,p);
        p->z.flags = 0;
        continue;
      }

      p = preload(f,p);
      c = p->code;

      if (c == SUBPFP)
        c = SUB;
      else if (c == ADDI2P)
        c = ADD;
      else if (c == SUBIFP)
        c = SUB;

      if (c == CONVERT) {
        if (ISFLOAT(q1typ(p)) || ISFLOAT(ztyp(p)))   /* @@@ */
          ierror(0);

        if (sizetab[q1typ(p)&NQ] < sizetab[ztyp(p)&NQ]) {
          int sh = 0;

          if ((q1typ(p)&NQ) == CHAR)
            sh = 24;
          else if ((q1typ(p)&NQ) == SHORT)
            sh = 16;
          else if (sizetab[ztyp(p)&NQ] > 4)
            ierror(0);  /* @@@ */

          if (sh) {
            registerize(f,p,&p->q1,q1typ(p));
            emit(f,"\tmov\t%s,%s,lsl #%d\n",regnames[zreg],regnames[p->q1.reg],sh);
            emit(f,"\tmov\t%s,%s,%csr #%d\n",regnames[zreg],regnames[zreg],
                                             (q1typ(p)&UNSIGNED)?'l':'a',sh);
          }
        }
        save_result(f,p);
        continue;
      }

      if (c == KOMPLEMENT) {
        registerize(f,p,&p->q1,t);
        emit(f,"\tmvn\t%s,%s\n",regnames[zreg],regnames[p->q1.reg]);
        save_result(f,p);
        continue;
      }

      if (c == SETRETURN) {
        load_reg(f,p,p->z.reg,&p->q1,t);
        BSET(regs_modified,p->z.reg);
        continue;
      }

      if (c == GETRETURN) {
        if (p->q1.reg) {  /* REG-flag is not set!? */
          zreg = p->q1.reg;
          save_result(f,p);
        }
        else
          p->z.flags = 0;
        continue;
      }

      if (c == CALL) {
        if ((p->q1.flags & (VAR|DREFOBJ))==VAR &&
            p->q1.v->fi && p->q1.v->fi->inline_asm) {
          emit_inline_asm(f,p->q1.v->fi->inline_asm);
        }
        else if (p->q1.flags & DREFOBJ) {
          int reg;

          if (p->q1.flags & REG) {
            reg = p->q1.reg;
          }
          else {
            reg = cg_getreg(f,p);
            load_reg(f,p,reg,&p->q1,POINTER);
          }
          emit(f,"\tmov\t%s,%s\n",regnames[lr],regnames[pc]);
          if (aa < AA4T)
            emit(f,"\tmov\t%s,%s\n",regnames[pc],regnames[reg]);
          else
            emit(f,"\tbx\t%s\n",regnames[reg]);
        }
        else {
          emit(f,"\tbl\t");
          emit_obj(f,&p->q1,t);
          emitnl(f);
        }

        pushoffs -= zm2l(pushedargsize(p));

        if ((p->q1.flags & (VAR|DREFOBJ))==VAR &&
            p->q1.v->fi && (p->q1.v->fi->flags & ALL_REGS)) {
          bvunite(regs_modified,p->q1.v->fi->regs_modified,RSIZE);
        }
        else{
          int i;

          for (i=1; i<=MAXR; i++) {
            if (regscratch[i])
              BSET(regs_modified,i);
          }
        }
        continue;
      }

      if (c == PUSH) {
        if (t == 0)
          ierror(0);
        registerize(f,p,&p->q1,t);
        emit(f,"\tstr\t%s,[%s,#%ld]\n",
             regnames[p->q1.reg],regnames[sp],pushoffs);
        pushoffs += zm2l(opsize(p));
        continue;
      }

      if (c == ASSIGN) {
        if (t == 0)
          ierror(0);
        if (isreg(p->q1.flags))
          zreg = p->q1.reg;
        else
          load_reg(f,p,zreg,&p->q1,t);
        save_result(f,p);
        continue;
      }

      if (c == ADDRESS) {
        load_address(f,zreg,&p->q1,POINTER);
        save_result(f,p);
        continue;
      }

      if (c == MINUS) {
        registerize(f,p,&p->q1,t);
        emit(f,"\trsb\t%s,%s,#0\n",regnames[zreg],regnames[p->q1.reg]);
        save_result(f,p);
        continue;
      }

      if (c == TEST) {
        registerize(f,p,&p->q1,t);
        emit(f,"\tteq\t%s,#0\n",regnames[p->q1.reg]);
        continue;
      }

      if (c == COMPARE) {
        if (!isconst(p->q1.flags))
          registerize(f,p,&p->q1,t);
        emit(f,"\tcmp\t%s,",regnames[p->q1.reg]);
        emit_obj(f,&p->q2,t);
        emitnl(f);
        continue;
      }

      if ((c>=OR && c<=AND)) {
        registerize(f,p,&p->q1,t);
        if (!isconst(p->q2.flags))
          registerize(f,p,&p->q2,t);
        emit(f,"\t%s\t%s,%s,",logicals[c-OR],regnames[zreg],
             regnames[p->q1.reg]);
        emit_obj(f,&p->q2,t);
        emitnl(f);
        save_result(f,p);
        continue;
      }

      if (c>=LSHIFT && c<=RSHIFT) {
        registerize(f,p,&p->q1,t);
        if (!isconst(p->q2.flags))
          registerize(f,p,&p->q2,t);
        emit(f,"\tmov\t%s,%s,%s ",regnames[zreg],regnames[p->q1.reg],
             shifts[(t&UNSIGNED)!=0][c-LSHIFT]);
        emit_obj(f,&p->q2,t);
        emitnl(f);
        save_result(f,p);
        continue;
      }

      if (c>=ADD && c<=SUB) {
        registerize(f,p,&p->q1,t);
        if (!isconst(p->q2.flags))
          registerize(f,p,&p->q2,t);
        emit(f,"\t%s\t%s,%s,",c==ADD?"add":"sub",
             regnames[zreg],regnames[p->q1.reg]);
        emit_obj(f,&p->q2,t);
        emitnl(f);
        save_result(f,p);
        continue;
      }

      if (c==MULT && aa>=AA2 && sizetab[t&NQ]<=4) {
        registerize(f,p,&p->q1,t);
        registerize(f,p,&p->q2,t);
        emit(f,"\tmul\t%s,%s,%s\n",
             regnames[zreg],regnames[p->q1.reg],regnames[p->q2.reg]);
        save_result(f,p);
        continue;
      }

      if (pass) {
        pric2(stdout,p);
        ierror(0);
      }
    }
  }

  cg_freeall(f);
  function_bottom(f,v,argsize+localsize+rsavesize);

#if 0  /* @@@ wozu? */
  if (stack_valid) {
    if (!v->fi)
      v->fi = new_fi();
    v->fi->flags |= ALL_STACK;
    v->fi->stack1 = stack;
  }
  emit(f,"; stacksize=%lu%s\n",zum2ul(stack),stack_valid?"":"+??");
#endif
}


int shortcut(int code,int typ)
{
  return 0;
}


int reg_parm(struct reg_handle *m, struct Typ *t,int vararg,struct Typ *d)
{
  int f;

  if (!m || !t)
    ierror(0);

  f = t->flags & NQ;

  if (f<=LONG || f==POINTER) {
    if (m->gregs >= GPR_ARGS)
      return 0;
    else
      return FIRST_GPR + m->gregs++;
  }

  if (f == LLONG) {
    if (m->gregs >= GPR_ARGS-1)
      return 0;
    else
      ierror(0);  /* @@@ check ABI if odd registers are skipped!? */
  }

  if (ISFLOAT(f)) {
#if 0 /* @@@ VFP only! */
    if (m->fregs >= FPR_ARGS)
      return 0;
    else
      return FIRST_DOUBLE + m->fregs++;
#endif
  }

  return 0;
}


int emit_peephole(void)
/* This function will not optimize anything, but just update the
   number of lines counter, icnt, for the current function.
   It is required to estimate if a data-reference-pointer array
   is still reachable with a 12-bit offset. */
{
  int entries;

  entries = emit_f ? EMIT_BUF_DEPTH : emit_l - emit_f + 1;
  icnt += entries;
  return 0;
}


int handle_pragma(const char *s)
{
  return 0;
}


void cleanup_cg(FILE *f)
{
}


void cleanup_db(FILE *f)
{
  if (f)
    section = -1;
}
