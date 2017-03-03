/*  $VER: vbcc (declaration.c) V0.8     */

#include <string.h>

#include "vbcc_cpp.h"
#include "vbc.h"

static char FILE_[]=__FILE__;

#define PLAIN_STORAGE_CLASS 7
#define PARAMETER 8
#define OLDSTYLE 16

void dynamic_init(struct Var *v,struct Typ *t,struct const_list *cl,zmax of,int noconst);
int test_assignment(struct Typ *,np);
int return_sc,return_reg,has_return,return_inline;
char *return_vattr;
static int did_return_label;
#ifdef HAVE_TARGET_ATTRIBUTES
unsigned long return_tattr;
#endif
zmax init_dyn_sz,init_const_sz;
int init_dyn_cnt,init_const_cnt;

void init_sl(struct struct_list *sl);

#ifdef HAVE_ECPP
#define ECPP_CPP_LINKAGE 1
#define ECPP_C_LINKAGE 2
static int ecpp_ret_flags;
static int ecpp_linkage=ECPP_CPP_LINKAGE;
static int ecpp_access;
char* ecpp_mangle_name(struct Typ *t,char *identifier,struct struct_declaration *higher_nesting);
char* ecpp_mangle_arg(char *pos,struct Typ *t);
char* ecpp_mangle_nested_identifier(char *pos,struct struct_declaration *sd);
struct Var *ecpp_find_ext_var(char *identifier);
struct Var *ecpp_find_var(char *identifier);
int ecpp_is_member_struct(struct struct_list *sl,struct struct_declaration *sd);
struct struct_declaration *ecpp_find_struct(char* identifier,struct struct_declaration *scope,int search_flag);
struct struct_declaration *ecpp_find_scope(char* nested_name,char** identifier);
struct struct_list *ecpp_find_member(char* identifier,struct struct_declaration *scope,struct struct_declaration** ret_scope,int search_flag);
void ecpp_add_this_pointer(struct struct_declaration *decl);
struct Typ *ecpp_declarator(struct Typ *t);
int ecpp_linkage_specification();
void ecpp_call_ctor(struct struct_declaration *sd,np this,struct argument_list *al);
void ecpp_auto_dtor(struct Var *v);
void ecpp_call_dtor(struct struct_declaration *sd,np this);
void ecpp_auto_call_dtors();
void ecpp_free_init_list(np *initlist,struct struct_declaration *sd);
void ecpp_gen_set_vtable(struct struct_declaration *class);
np* ecpp_ctor_init_list(struct struct_declaration *ctor_func,int definit);
void ecpp_dtor_prolog();
void ecpp_dtor_epilog();
void ecpp_gen_default_ctor(struct struct_declaration *class);
void ecpp_gen_default_dtor(struct struct_declaration *class);
void ecpp_access_specifier();
void ecpp_add_friend(struct struct_declaration *class,struct struct_declaration *friend);
int ecpp_is_friend(struct struct_declaration *class);
#endif

extern np gen_libcall(char *,np,struct Typ *,np,struct Typ *);
extern int float_used;
extern void optimize(long,struct Var *);

static void clear_main_ret(void)
{
  if(c99&&!strcmp(cur_func,"main")&&return_typ&&ISSCALAR(return_typ->flags)){
    /* in c99, main returns 0 if it falls from back */
    struct IC *new=new_IC();
    new->code=SETRETURN;
    new->q1.val.vmax=l2zm(0L);
    eval_const(&new->q1.val,MAXINT);
    insert_const(&new->q1.val,return_typ->flags&NU);
    new->q1.flags=KONST;
    new->typf=return_typ->flags;
    new->q2.val.vmax=szof(return_typ);
    new->q2.flags=new->z.flags=0;
    new->z.reg=freturn(return_typ);
    add_IC(new);
  }
}

static char *get_string(void)
/* Liest Stringkonstante und liefert Wert als String (malloced) */
{
  np tree;int l;char *p;
  struct const_list *cl;
  killsp();
  if(ctok->type!=T_STRING) {error(74);return 0;}
  tree=string_expression();
  if(!tree||tree->flags!=STRING){
    error(229);
    return 0;
  }
  cl=tree->cl;l=0;
  while(cl){
    l++;
    cl=cl->next;
  }
  p=mymalloc(l);
  cl=tree->cl;l=0;
  while(cl){
    p[l]=zm2l(zc2zm(cl->other->val.vchar));
    l++;
    cl=cl->next;
  }
  if(tree) free_expression(tree);
  killsp();
  return p;
}

/* checks whether string is a valid vector type */
/* returns the dimension */
static int check_vect(char *s,char *base)
{
  int i=strlen(base),dim;
  if(strncmp(s,base,i)) return 0;
  dim=s[i];
  if(dim=='2'||dim=='3'||dim=='4'||dim=='8'){
    if(s[i+1]==0)
      return dim-'0';
    else
      return 0;
  }
  if(s[i]=='1'&&s[i+1]=='6'&&s[i+2]==0)
    return 16;
  return 0;
}

int settyp(int typnew, int typold)
/* Unterroutine fuer declaration_specifiers().              */
{
  if(DEBUG&2) printf("settyp: new=%d old=%d\n",typnew,typold);
  if(typold==LONG&&typnew==FLOAT){ error(203); return DOUBLE;}
  if(typold==LONG&&typnew==DOUBLE) return LDOUBLE;
  if(c99&&typold==LONG&&typnew==LONG) return LLONG;
  if(typold!=0&&typnew!=INT){error(47);return(typnew);}
  if(typold==0&&typnew==INT) return(INT);
  if(typold==0) return(typnew);
  if(typold==SHORT||typold==LONG||typold==LLONG) return(typold);
  error(48);
  return(typnew);
}

#define dsc if(storage_class) error(49); if(typ||type_qualifiers) error(50)
#define XSIGNED 16384

#ifdef HAVE_MISRA
int misra_return_type_unspec= 0;
#endif

struct Typ *declaration_specifiers(void)
/* Erzeugt neuen Typ und gibt Zeiger darauf zurueck,      */
/* parst z.B. unsigned int, struct bla etc.               */
{
  int typ=0,type_qualifiers=0,notdone,storage_class,hard_reg,have_inline;
#ifdef HAVE_ECPP
	int ecpp_flags=0;
  struct struct_declaration* merk_class=0;
  int merk_access;
#endif
  char *imerk,sident[MAXI],sbuff[MAXI],*attr=0,*vattr=0;
  struct Typ *new=new_typ(),*t,*ts;
  struct struct_declaration *ssd;
  struct struct_list (*sl)[];
  size_t slsz;
  struct Var *v;
  int dim;
#ifdef HAVE_TARGET_ATTRIBUTES
  unsigned long tattr=0;
#endif
  storage_class=hard_reg=have_inline=0;
  do{
    killsp();notdone=0;
    if(ctok->type==NAME){
      if(!strcmp("struct",ctok->name)||!strcmp("union",ctok->name)||(ecpp&&!strcmp("class",ctok->name))){
        if(!strcmp("struct",ctok->name)) notdone=STRUCT;
        if(!strcmp("union",ctok->name)) {
          notdone=UNION;
#ifdef HAVE_MISRA
          misra_neu(110,18,4,0);
#endif
        }
#ifdef HAVE_ECPP
        if(ecpp){
          merk_access=ecpp_access;
          if(!strcmp("class",ctok->name)) {ecpp_access=ECPP_PRIVATE;notdone=STRUCT;}
          if(!strcmp("struct",ctok->name)) {ecpp_access=ECPP_PUBLIC;notdone=STRUCT;}
        }
#endif
        next_token();
        killsp();
        if(ctok->type!=LBRA){
          cpbez(sident,1);
          next_token();
          killsp();
          ssd=find_struct(sident,0);
#ifdef HAVE_MISRA
          if (misra_is_reserved(sident)) misra_neu(115,20,2,0);
#endif
          if(ssd&&ctok->type==LBRA&&find_struct(sident,nesting)&&ssd->count>0) error(13,sident);
          if(!ssd||((ctok->type==LBRA||ctok->type==SEMIC)&&!find_struct(sident,nesting))){
            typ=settyp(notdone,typ);
            ssd=mymalloc(sizeof(*ssd));
            ssd->count=0;
            new->exact=ssd=add_sd(ssd,notdone);
            if(!ecpp)add_struct_identifier(sident,ssd);
#ifdef HAVE_ECPP
            if (ecpp){
              if(!current_class)add_struct_identifier(sident,ssd);
              else ssd->identifier=add_identifier(sident,strlen(sident));
            }
#endif
          }else{
            new->exact=ssd;
            typ=settyp(new->flags=notdone,typ);
          }
        }else{
          *sident=0;
          typ=settyp(notdone,typ);
          ssd=mymalloc(sizeof(*ssd));
          ssd->count=0;
          new->exact=ssd=add_sd(ssd,notdone);
        }
        if(ssd->typ!=notdone) error(230,sident);
#ifdef HAVE_ECPP
        if(ecpp&&ctok->type==T_COLON){
          /* derived class */
          int access=ecpp_access;
          struct struct_declaration* base_sd;
          char *id=0;
          next_token();
          killsp();
          if(ctok->type!=NAME){
            error(76);
          }
          if(!strcmp("private",ctok->name)){access=ECPP_PRIVATE;next_token();killsp();}
          else if(!strcmp("protected",ctok->name)){access=ECPP_PROTECTED;next_token();killsp();}
          else if(!strcmp("public",ctok->name)){access=ECPP_PUBLIC;next_token();killsp();}
          base_sd=ecpp_find_scope(ctok->name,&id);
          if(!base_sd||id)error(337,ctok->name);
          if(base_sd->count<=0)error(336,ctok->name);
          ssd->base_class=base_sd;
          ssd->base_access=access;
          next_token();
          killsp();
          if(ctok->type!=LBRA){
            error(335);
          }
        }
#endif        
        if(ctok->type==LBRA){
          int bfoffset,bfsize,flex_array=0,cbfo=0;
          zmax off=l2zm(0L);
          next_token();
          killsp();
          slsz=SLSIZE;
          imerk=ident;
            sl=mymalloc(slsz*sizeof(struct struct_list));
            ssd->count=0;
#ifdef HAVE_ECPP
          if(ecpp){
            merk_class=current_class;
            current_class=new->exact;
            current_class->higher_nesting=merk_class;
          }
          if(ecpp){
						sl=mymalloc(slsz*sizeof(struct struct_list));
						ssd->count=0;
            ssd->sl=sl;
          }
          if(ecpp) ecpp_access_specifier();
#endif
          ts=declaration_specifiers();
          while(ctok->type!=RBRA&&ts){
            if(flex_array) error(231);
            ident=sbuff;
            t=declarator(clone_typ(ts));
            killsp();
            bfsize=bfoffset=-1;
            if(ctok->type==T_COLON){
              np tree;
              if((ts->flags&NQ)!=INT) {
                error(51);
#ifdef HAVE_MISRA
                misra_neu(112,6,4,0);
#endif
              }
              next_token();killsp();tree=assignment_expression();

              if(type_expression(tree)){
                int tsize;
                if(tree->flags!=CEXPR) error(52);
                if(!ISINT(tree->ntyp->flags)) error(52);
                eval_const(&tree->val,tree->ntyp->flags);
                bfsize=(int)zm2l(vmax);
                tsize=(int)zm2l(zmmult(sizetab[t->flags&NQ],char_bit));
                if(bfsize<0||bfsize>tsize||(bfsize==0&&*ident)){
                  error(332);bfsize=1;
                }
                if(bfsize+cbfo>tsize){
                  bfoffset=cbfo=0;
                }else{
                  bfoffset=cbfo;
                }
                if(bfsize!=0)
                  cbfo+=bfsize;
                else
                  cbfo=0;
              }
#ifdef HAVE_MISRA
              if (ts->flags&XSIGNED) {
                if (bfsize < 2) misra_neu(112,6,5,0);
              }
#endif
              if(tree) free_expression(tree);
            }else{
              if(!ecpp&&*ident==0) error(53);
              cbfo=0;
            }
#ifdef HAVE_ECPP
            if(ecpp){
              if(t->ecpp_flags&ECPP_FRIEND){
                if(!t->exact)ierror(0);
                ecpp_add_friend(ssd,t->exact);
                killsp();
                if(ctok->type==COMMA) {next_token();killsp();continue;}
                if(ctok->type!=SEMIC) error(54); else next_token();
                killsp();
                ecpp_access_specifier();
                if(ctok->type!=RBRA){
                  if(ts) freetyp(ts);
                  ts=declaration_specifiers();killsp();
                }
                continue;
              }
            }
#endif
            if(type_uncomplete(t)){
              if(!c99||notdone!=STRUCT||flex_array||(t->flags&NQ)!=ARRAY||type_uncomplete(t->next)){
                error(14,sbuff);
                freetyp(t);
                break;
              }else{
                flex_array=1;
              }
            }
	    if(ISARRAY(t->flags)&&t->dsize){
	      error(352,sbuff);
	    }
            if(ISFUNC(t->flags)){
              if(!ecpp){
                error(15,sbuff);
              }
            }
            if(*ident!=0){
              int i=ssd->count;
              while(--i>=0)
                if(!strcmp((*sl)[i].identifier,ident))
                  error(16,ident);
            }
#ifdef HAVE_MISRA
            if (misra_is_reserved(ident)) misra_neu(115,20,2,0);
#endif
#ifdef HAVE_ECPP
            if(ecpp){
              (*sl)[ssd->count].mangled_identifier=0;
              if(!*ident&&ISSTRUCT(t->flags))t->ecpp_flags|=ECPP_NESTED_CLASS;
              if(ISFUNC(t->flags)){
                struct struct_declaration *decl=t->exact;
                int add_void_arg=0;
                if(t->ecpp_flags&ECPP_STATIC){
                  if(add_void_arg) decl->sl=myrealloc(decl->sl,(decl->count+1)*sizeof(struct struct_list));
                }else{
                  /* the first argument of a class-method is the this pointer */
                  if(add_void_arg) decl->sl=myrealloc(decl->sl,(decl->count+2)*sizeof(struct struct_list));
                  else decl->sl=myrealloc(decl->sl,(decl->count+1)*sizeof(struct struct_list));
                }
                if(add_void_arg){
                  /* add the trailing void FIXME: should be obsolete! */
                  init_sl(&(*decl->sl)[decl->count]);
                  (*decl->sl)[decl->count].styp=new_typ();
                  (*decl->sl)[decl->count].styp->flags=VOID;
                  (*decl->sl)[decl->count].styp->next=0;
                  decl->count++;
                }
                if(*ident==0&&ISSTRUCT(t->next->flags)&&current_class&&!strcmp(t->next->exact->identifier,current_class->identifier)){
                  /* FIXME: obsolete? done in ecpp_declarator */
                  strcpy(ident,current_class->identifier);
                }
                (*sl)[ssd->count].identifier=add_identifier(decl->identifier,strlen(decl->identifier));;
                (*sl)[ssd->count].mangled_identifier=add_identifier(decl->mangled_identifier,strlen(decl->mangled_identifier));;
                if(!(t->ecpp_flags&ECPP_STATIC)){
                  add_var(decl->mangled_identifier,clone_typ(t),EXTERN,0);
                }
              } else if(!ISFUNC(t->flags)&&t->ecpp_flags&ECPP_STATIC){
                  char *mangled_name; 
                (*sl)[ssd->count].identifier=add_identifier(ident,strlen(ident));
                mangled_name=ecpp_mangle_name(t,ident,ssd);
                  (*sl)[ssd->count].mangled_identifier=add_identifier(mangled_name,strlen(mangled_name));
                  (*sl)[ssd->count].align=l2zm(0L);
                strncpy(ident,mangled_name,MAXI);
              } else {
                (*sl)[ssd->count].identifier=add_identifier(ident,strlen(ident));
              }
            }
#endif
	          (*sl)[ssd->count].bfoffset=bfoffset;
            (*sl)[ssd->count].bfsize=bfsize;
            (*sl)[ssd->count].styp=t;
            if(!ecpp) (*sl)[ssd->count].identifier=add_identifier(ident,strlen(ident));
#ifdef HAVE_ECPP
            if(ecpp&&t->ecpp_flags&ECPP_NESTED_CLASS){
              (*sl)[ssd->count].align=l2zm(0L);
            }else
#endif
            if(pack_align>0&&pack_align<falign(t))
              (*sl)[ssd->count].align=pack_align;
            else
              (*sl)[ssd->count].align=falign(t);
            {
              zmax m,al=(*sl)[ssd->count].align;
              zmax sz=szof(t);
              m=off;
              if((zmeqto(al,l2zm(0L))||zmeqto(sz,l2zm(0L)))&&!flex_array){
                if (!ecpp){
                  error(316,ident);
                }
              }else{
                off=zmmult(zmdiv(zmadd(off,zmsub(al,l2zm(1L))),al),al);
                if(!zmeqto(m,off)) error(306,ident);
                if(!flex_array){
                  m=zmmult(zmdiv(zmadd(off,zmsub(sz,l2zm(1L))),sz),sz);
                  if(!zmeqto(m,off)) error(307,ident);
                  off=zmadd(off,sz);
                }
              }
            }
            ssd->count++;
            if(ssd->count>=slsz-1){
              slsz+=SLSIZE;
              sl=myrealloc(sl,slsz*sizeof(struct struct_list));
            }
            killsp();
            if(ctok->type==COMMA) {next_token();killsp();continue;}
            if(ctok->type!=SEMIC) error(54); else next_token();
            killsp();
#ifdef HAVE_ECPP
            if(ecpp)ecpp_access_specifier();
#endif
            if(ctok->type!=RBRA){
              if(ts) freetyp(ts);
              ts=declaration_specifiers();killsp();
            }
          }
          if(ts) freetyp(ts);
          if(ssd->count==0) error(55);
#ifdef HAVE_ECPP
          if(ecpp){
            /* FIXME: overriding virtual methods does not require methods being declared virtual */
            struct struct_list (*ssl)[]=sl;
            int i; int vfcount=0;
            current_class=merk_class;
            for(i=0;i<ssd->count;++i){
              if(ISFUNC((*ssl)[i].styp->flags)&&(*ssl)[i].styp->exact->ecpp_flags&ECPP_VIRTUAL)
                vfcount++;
            }
            if(vfcount>0||ssd->base_class&&ssd->base_class->ecpp_flags&ECPP_VIRTUAL){
              struct Typ *vttyp=new_typ();
              struct struct_declaration *vtsd, *base_vtsd=0;
              struct const_list *vtclist=0;
              struct const_list *vtclist_last=0;
							char *vtname; int base_vfcount=0;
							if(ssd->base_class&&ssd->base_class->ecpp_flags&ECPP_VIRTUAL){
								base_vtsd=(*ssd->base_class->sl)[0].styp->next->exact;
							}
							ssd->ecpp_flags|=ECPP_VIRTUAL;
              vtsd=mymalloc(sizeof(*vtsd));
              vtsd=add_sd(vtsd,STRUCT);
              vtsd->ecpp_flags=ECPP_TV;
							if(base_vtsd){
								base_vfcount=base_vtsd->count;
								vtsd->sl=mymalloc((vfcount+base_vtsd->count)*sizeof(struct struct_list));
							}else{
								vtsd->sl=mymalloc(vfcount*sizeof(struct struct_list));
							}
							vtsd->count=0;
              vttyp->flags=STRUCT;
              vttyp->exact=vtsd;
              for(i=0;i<ssd->count+base_vfcount;++i){
								struct Typ *t=0;struct Var *v;
								if(i>=base_vfcount&&(!ISFUNC((*ssl)[i-base_vfcount].styp->flags)||
									!((*ssl)[i-base_vfcount].styp->exact->ecpp_flags&ECPP_VIRTUAL))){
									continue;
								}
								if(i<base_vfcount){
								  /* append base vf to vtable */
									t=clone_typ((*base_vtsd->sl)[i].styp);
									(*vtsd->sl)[vtsd->count].identifier=
                    add_identifier((*base_vtsd->sl)[i].identifier,strlen((*base_vtsd->sl)[i].identifier));
								}else{
									int j; int override=-1;
									for(j=0;j<base_vfcount;++j){
										if(!strcmp((*base_vtsd->sl)[j].identifier,(*ssl)[i-base_vfcount].identifier)){
											override=j;
											break;
										}
									}
									if(override>=0){
										/* override vf */
										t=new_typ();
										t->flags=POINTER_TYPE((*ssl)[i-base_vfcount].styp->flags);
										t->next=clone_typ((*ssl)[i-base_vfcount].styp);
										freetyp((*vtsd->sl)[override].styp);
										(*vtsd->sl)[override].styp=t;
										continue;
									}else{
										/* append new vf to vtable */
										t=new_typ();
										t->flags=POINTER_TYPE((*ssl)[i-base_vfcount].styp->flags);
										t->next=clone_typ((*ssl)[i-base_vfcount].styp);
										(*vtsd->sl)[vtsd->count].identifier=
                      add_identifier((*ssl)[i-base_vfcount].identifier,strlen((*ssl)[i-base_vfcount].identifier));
									}
								}
								(*vtsd->sl)[vtsd->count].styp=t;
								(*vtsd->sl)[vtsd->count].bfsize=-1;
								(*vtsd->sl)[vtsd->count].bfoffset=-1;
								(*vtsd->sl)[vtsd->count].mangled_identifier=0;
								vtsd->count++;
							}
							for(i=0;i<vtsd->count;++i){
								struct const_list *cl;
								np n; int ret;
								int oldconst;
								(*vtsd->sl)[i].align=falign((*vtsd->sl)[i].styp);
								n=new_node();
								n->flags=ADDRESS;
								n->right=0;
								n->left=new_node();
								n->left->flags=IDENTIFIER;
								n->left->left=n->left->right=0;
								n->left->identifier=(*vtsd->sl)[i].styp->next->exact->mangled_identifier;
                n->left->o.v=ecpp_find_ext_var((*vtsd->sl)[i].styp->next->exact->mangled_identifier);
								ret=type_expression(n);
								if(ret==0)ierror(0);
								cl=mymalloc(sizeof(struct const_list));
								if(!vtclist){vtclist=vtclist_last=cl;}
								else{vtclist_last->next=cl;vtclist_last=cl;}
								cl->other=mymalloc(sizeof(struct const_list));
								cl->other->tree=n;
								oldconst=const_expr;
								const_expr=1;
								gen_IC(n,0,0);
								const_expr=oldconst;
								n->o.v->flags|=USEDASADR;
              }
							vtname=ecpp_mangle_name(new,"_ZTV",0);
							vtname=add_identifier(vtname,strlen(vtname));
              v=add_var(vtname,vttyp,EXTERN,vtclist);
              v->flags|=DEFINED;
              v->dfilename=filename;
              v->dline=line;
              if(slsz<=ssd->count+1)ierror(0);
              memmove(&(*sl)[1],&(*sl)[0],ssd->count*sizeof(struct struct_list));
              init_sl(&(*sl)[0]);
							(*sl)[0].identifier=add_identifier("_ZTV",4);
              (*sl)[0].styp=new_typ();
              (*sl)[0].styp->flags=POINTER_TYPE(vttyp->flags);
              (*sl)[0].styp->next=clone_typ(vttyp);
							(*sl)[0].align=falign(vttyp);
              vtsd->mangled_identifier=add_identifier(vtname,strlen(vtname));
              ssd->count++;
            }
          }
          if(ecpp){
            int i;
            int ispod=1;
            int genctor=0; int gendtor=0;
            if(ssd->base_class)ispod=0;
            else for(i=0;i<ssd->count;++i){
              if(ISFUNC((*sl)[i].styp->flags)&&(*sl)[i].styp->exact->ecpp_flags&ECPP_VIRTUAL){
                ispod=0;break;
              }
              if(ecpp_is_member_struct(&(*sl)[i],ssd)&&!((*sl)[i].styp->exact->ecpp_flags&ECPP_POD)){
                ispod=0;break;
              }
            }
            if(ispod&&ecpp_find_member("__ctor",ssd,0,0))ispod=0;
            if(ispod&&ecpp_find_member("__dtor",ssd,0,0))ispod=0;
            if(ispod)ssd->ecpp_flags|=ECPP_POD;

            if(!(ssd->ecpp_flags&ECPP_POD)&&!ecpp_find_member("__ctor",ssd,0,0)){
              genctor=1;
            }
            if(!(ssd->ecpp_flags&ECPP_POD)&&!ecpp_find_member("__dtor",ssd,0,0)){
              gendtor=1;
            }
            if(ssd->count>=slsz-genctor-gendtor){
              slsz+=genctor+gendtor;
              sl=myrealloc(sl,slsz*sizeof(struct struct_list));
            }
            if(genctor)
              ecpp_gen_default_ctor(ssd);
            if(gendtor)ecpp_gen_default_dtor(ssd);
          }
#endif
          ident=imerk;
					add_sl(ssd,sl);
					free(sl);
          if(ctok->type!=RBRA) error(56); else next_token();
          new->flags=notdone|type_qualifiers;
        }
#ifdef HAVE_ECPP
        if(ecpp&&notdone==STRUCT)ecpp_access=merk_access;
#endif
        notdone=1;
      }else if(!strcmp("enum",ctok->name)){
        /*  enumerations; die Namen werden leider noch ignoriert    */
        next_token();killsp();notdone=1;
        if(ctok->type!=LBRA){cpbez(buff,1);next_token();killsp();}
        if(ctok->type==LBRA){
          /* mode: 0=start 1=first was not init 2=first was init 3=more init */
          zmax val; struct Var *v; struct Typ *t;int mode=0;
          val=l2zm(0L);
          next_token();killsp();
          while(ctok->type!=RBRA){
            cpbez(sident,1);next_token();killsp();
            if(*sident==0) {error(56);break;}
            t=new_typ();
            t->flags=CONST|INT;
            if(find_var(sident,nesting)) error(17,sident);
            v=add_var(sident,t,AUTO,0); /*  AUTO hier klug? */
            if(ctok->type==ASGN){
              if(mode==2) mode=3;
              if(mode==0) mode=2;
#ifdef HAVE_MISRA                                /* MISRA Rule 9.3 checking */
              if(mode==1) misra_neu(32,9,3,0);
#endif
              next_token();killsp();
              v->clist=initialization(v->vtyp,0,0,0,0,0);
              val=zi2zm(v->clist->val.vint);killsp();
            }else{
              if(mode==0) mode=1;
#ifdef HAVE_MISRA                                /* MISRA Rule 9.3 checking */
              if(mode==3) misra_neu(32,9,3,0);
#endif
              v->clist=mymalloc(CLS);
              v->clist->val.vint=zm2zi(val);
              v->clist->next=v->clist->other=0;
              v->clist->tree=0;
            }
            vmax=l2zm(1L);val=zmadd(val,vmax);
            v->vtyp->flags=CONST|ENUM;
            if(ctok->type!=RBRA&&ctok->type!=COMMA) {error(56);break;}
            if(ctok->type==COMMA) next_token();
            killsp();
            if(ctok->type==RBRA) {next_token(); break;}
          }
        }
        killsp();
        typ=settyp(INT,typ);
      }else if(!strcmp("__readsmem",ctok->name)||!strcmp("__writesmem",ctok->name)){
        enum {READS,WRITES} op;
        char *imerk,tbuf[MAXI];
        struct Typ *tmp;
        if(!strcmp("__readsmem",ctok->name))
          op=READS;
        else
          op=WRITES;
        next_token();killsp();
        if(ctok->type==LPAR) next_token(); else error(151);
        imerk=ident;ident=tbuf;
        tmp=declarator(declaration_specifiers());
        ident=imerk;
        if(tmp){
          if(vattr)
            vattr=myrealloc(vattr,strlen(vattr)+25);
          else{
            vattr=mymalloc(25);
            vattr[0]=0;
          }
          sprintf(vattr+strlen(vattr),";%s(%d)",op==READS?"readmem":"writemem",tmp->flags);
        }
        killsp();
        if(ctok->type==RPAR) next_token(); else error(59);
        killsp();
        notdone=1;
      }else if(!strcmp("void",ctok->name)){
        next_token();
        typ=settyp(VOID,typ);notdone=1;
      }else if(!strcmp("char",ctok->name)){
#ifdef HAVE_MISRA
        if(storage_class!=TYPEDEF) misra_neu(13,6,3,0);
        if(!(type_qualifiers&(XSIGNED|UNSIGNED))) misra_neu(14,0,0,0);
#endif
        next_token();
        typ=settyp(CHAR,typ);
	notdone=1;
	if(default_unsigned&&!(type_qualifiers&XSIGNED))
	  type_qualifiers|=UNSIGNED;
      }else if(!strcmp("short",ctok->name)){
#ifdef HAVE_MISRA
        if(storage_class!=TYPEDEF) misra_neu(13,6,3,0);
#endif
        next_token();
        typ=settyp(SHORT,typ);notdone=1;
      }else if(!strcmp("int",ctok->name)){
#ifdef HAVE_MISRA
        if(storage_class!=TYPEDEF) misra_neu(13,6,3,0);
#endif
        next_token();
        typ=settyp(INT,typ);notdone=1;
      }else if(!strcmp("long",ctok->name)){
#ifdef HAVE_MISRA
        if(storage_class!=TYPEDEF) misra_neu(13,6,3,0);
#endif
        next_token();
        typ=settyp(LONG,typ);notdone=1;
      }else if(!strcmp("float",ctok->name)){
#ifdef HAVE_MISRA
                  if(storage_class!=TYPEDEF) misra_neu(13,6,3,0);
#endif
        next_token();
        typ=settyp(FLOAT,typ);notdone=1;
      }else if(!strcmp("double",ctok->name)){
#ifdef HAVE_MISRA
        if(storage_class!=TYPEDEF) misra_neu(13,6,3,0);
#endif
        next_token();
        typ=settyp(DOUBLE,typ);notdone=1;
      }else if(!strcmp("const",ctok->name)){
        next_token();
        if(type_qualifiers&CONST) error(58);
        type_qualifiers|=CONST;notdone=1;
      }else if(!strcmp("volatile",ctok->name)){
        next_token();
        if(type_qualifiers&VOLATILE) error(58);
        type_qualifiers|=VOLATILE;notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"bool"))){
	next_token();
	typ=settyp(VECBOOL+dim-1,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"char"))){
	next_token();
	typ=settyp(VECCHAR+dim-1,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"uchar"))){
	next_token();
	typ=settyp((VECCHAR+dim-1)|UNSIGNED,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"short"))){
	next_token();
	typ=settyp(VECSHORT+dim-1,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"ushort"))){
	next_token();
	typ=settyp((VECSHORT+dim-1)|UNSIGNED,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"int"))){
	next_token();
	typ=settyp(VECINT+dim-1,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"uint"))){
	next_token();
	typ=settyp((VECINT+dim-1)|UNSIGNED,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"long"))){
	next_token();
	typ=settyp(VECLONG+dim-1,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"ulong"))){
	next_token();
	typ=settyp((VECLONG+dim-1)|UNSIGNED,typ);
	notdone=1;
      }else if(opencl&&(dim=check_vect(ctok->name,"float"))){
	next_token();
	typ=settyp(VECFLOAT+dim-1,typ);
	notdone=1;
#if 0
      }else if(c99&&!strcmp("restrict",ctok->name)){
        next_token();
        if(type_qualifiers&RESTRICT) error(58);
        type_qualifiers|=RESTRICT;notdone=1;
#endif
      }else if(!strcmp("unsigned",ctok->name)){
        next_token();
        if(type_qualifiers&(XSIGNED|UNSIGNED)) error(58);
        notdone=1;type_qualifiers|=UNSIGNED;
      }else if(!strcmp("signed",ctok->name)){
        next_token();
        if(type_qualifiers&(XSIGNED|UNSIGNED)) error(58);
        notdone=1;type_qualifiers|=XSIGNED;
      }else if(!strcmp("auto",ctok->name)){
        next_token();
        dsc;storage_class=AUTO;notdone=1;
      }else if(!strcmp("register",ctok->name)){
#ifdef HAVE_MISRA
        misra_neu(28,0,0,0);
#endif
        next_token();
        dsc;storage_class=REGISTER;notdone=1;
      }else if(!strcmp("static",ctok->name)){
        next_token();
        dsc;storage_class=STATIC;notdone=1;
#ifdef HAVE_ECPP
        if(ecpp)ecpp_flags|=ECPP_STATIC;
#endif
      }else if(!strcmp("extern",ctok->name)){
        next_token();
        dsc;storage_class=EXTERN;notdone=1;
      }else if(!strcmp("typedef",ctok->name)){
        next_token();
        dsc;storage_class=TYPEDEF;notdone=1;
      }else if(c99&&!strcmp("inline",ctok->name)){
        next_token();
        have_inline=1;notdone=1;
#ifdef HAVE_ECPP
      }else if(ecpp&&!strcmp("virtual",ctok->name)){
        next_token();
        if(ecpp_flags&ECPP_VIRTUAL)error(58);
				ecpp_flags|=ECPP_VIRTUAL;notdone=1;
      }else if(ecpp&&!strcmp("friend",ctok->name)){
        next_token();
				ecpp_flags|=ECPP_FRIEND;notdone=1;
#endif
			}else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__reg",ctok->name)){
        char *d;
        next_token();killsp(); 
        if(ctok->type==LPAR) next_token(); else error(151);
        killsp();
        if(d=get_string()){
          for(hard_reg=1;hard_reg<=MAXR;hard_reg++){
            if(!strcmp(d,regnames[hard_reg])) break;
          }
          if(hard_reg>MAXR){ hard_reg=0;error(220,d);}
          notdone=1;
          free(d);
        }
        killsp();
        if(ctok->type==RPAR) next_token(); else error(59);
        killsp();
      }else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__attr",ctok->name)){
        char *d;
        next_token();killsp(); 
        if(ctok->type==LPAR) next_token(); else error(151);
        killsp();
        if(d=get_string()){
          if(!attr){
            attr=d;
          }else{
            attr=myrealloc(attr,strlen(attr)+strlen(d)+2);
            strcat(attr,";");
            strcat(attr,d);
            free(d);
          }
          notdone=1;
        }
        killsp();
        if(ctok->type==RPAR) next_token(); else error(59);
        killsp();
      }else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__vattr",ctok->name)){
        char *d;
        next_token();killsp();
        if(ctok->type==LPAR) next_token(); else error(151);
        killsp();
        if(d=get_string()){
          if(!vattr){
            vattr=d;
          }else{
            vattr=myrealloc(vattr,strlen(vattr)+strlen(d)+2);
            strcat(vattr,";");
            strcat(vattr,d);
            free(d);
          }
          notdone=1;
        }
        killsp();
        if(ctok->type==RPAR) next_token(); else error(59);
        killsp();
      }else{
#ifdef HAVE_TARGET_ATTRIBUTES
        int i;
        for(i=0;g_attr_name[i];i++){
          if(!strcmp(g_attr_name[i],ctok->name)){
            if(tattr&(1L<<i)) error(227,ctok->name);
            tattr|=(1L<<i);
            next_token();
            notdone=1;break;
          }
        }
#endif
        if(!notdone&&typ==0&&!(type_qualifiers&(XSIGNED|UNSIGNED))){
          v=find_var(ctok->name,0);
          if(v&&v->storage_class==TYPEDEF){
            free(new);
            new=clone_typ(v->vtyp);
            typ=settyp(new->flags,typ);
            notdone=1;
            next_token();
          }
#ifdef HAVE_ECPP
          else if(ecpp){
            struct struct_declaration *scope;
            char* id=0;
            int done=0;
            int isctor=0;
            scope=ecpp_find_scope(ctok->name,&id);
            if(scope&&!id){
              if(scope==current_class)isctor=1;
              else{
                int clen=strlen(scope->identifier);
                int idlen=strlen(ctok->name);
                if(idlen>clen+1&&ctok->name[idlen-clen-1]==':'){
                  char t[MAXI];
                  strncpy(t,ctok->name,idlen-clen-1);
                  t[idlen-clen-1]=0;
                  if(scope==ecpp_find_scope(t,0))isctor=1;
        }
      }
              if(isctor){
                struct token mtok;
                copy_token(&mtok,ctok);
                next_token();
                if(ctok->type==LPAR){
                  /* found a ctor declaration */
                  /* FIXME: this could also be a pointer to function returning the class-type */
                  free(new);
                  new=new_typ();
                  new->flags=VOID;
                  ecpp_flags|=ECPP_CTOR;
                  typ=settyp(new->flags,typ);
                  done=1;
                }
                push_token(&mtok);
                free(mtok.name);
              }
    }
            if(!done&&scope&&!id){
              /* found a type */
              free(new);
              new=new_typ();
              new->flags=STRUCT;
              new->exact=scope;
              typ=settyp(new->flags,typ);
              next_token();
            }
          }
#endif
        }
      }
    }
#ifdef HAVE_ECPP
    if(ecpp&&ctok->type==NOT){
      next_token();killsp();
      if(ctok->type==NAME){
        /* found a dtor declaration */
        free(new);
        new=new_typ();
        new->flags=VOID;
        ecpp_flags|=ECPP_DTOR;
        typ=settyp(new->flags,0);
      }
    }
#endif
    if(DEBUG&2) printf("typ:%d\n",typ);
  }while(notdone);
  killsp();
  return_sc=storage_class;
  return_reg=hard_reg;
  return_vattr=vattr;
  return_inline=have_inline;
#ifdef HAVE_ECPP
	if(ecpp){
    if(return_sc&STATIC)ecpp_flags|=ECPP_STATIC;
    ecpp_ret_flags=ecpp_flags;
  }
#endif
#ifdef HAVE_TARGET_ATTRIBUTES
  return_tattr=tattr;
#endif
  if(typ==0){
    if(storage_class==0&&type_qualifiers==0&&!hard_reg){
      free(new);
      return 0;
    }
#ifdef HAVE_MISRA
        misra_return_type_unspec = 1;
#endif
    typ=INT;
  }
  if(type_qualifiers&(XSIGNED|UNSIGNED))
    if(!ISINT(typ))
      error(58);
  new->flags=typ|type_qualifiers;
  new->attr=attr;
  return new;
}

struct Typ *declarator(struct Typ *a)
/* Erzeugt einen neuen Typ, auf Basis des Typs a.           */
/* a wird hiermit verkettet.                                */
{
  struct Typ *t;
  killsp();*ident=0;
  t=direct_declarator(pointer(a));
  if(!a)
    {if(t) freetyp(t);return 0;}
#ifdef HAVE_ECPP
  if(ecpp)t=ecpp_declarator(t);
#endif
#ifdef HAVE_EXT_TYPES
  conv_typ(t);
#endif  
  return t;
}
struct Typ *pointer(struct Typ *a)
/* Unterroutine fuer declarator(), behandelt Zeiger auf Typ.   */
{
  struct Typ *t;char *attr;int notdone;
  if(!a) return(0);
  killsp();
  while(ctok->type==STAR){
    next_token();
    t=new_typ();
    t->flags=POINTER_TYPE(a);
    t->next=a;
    attr=0;
    a=t;
    do{
      killsp();
      notdone=0;
      if(ctok->type==NAME&&!strcmp("const",ctok->name)){
	a->flags|=CONST;
	notdone=1;
	next_token();
      }else if(ctok->type==NAME&&!strcmp("volatile",ctok->name)){
	a->flags|=VOLATILE;
	notdone=1;
	next_token();
      }else if(c99&&ctok->type==NAME&&!strcmp("restrict",ctok->name)){
	a->flags|=RESTRICT;
	notdone=1;
	next_token();
      }else if(ctok->type==NAME&&!strcmp("__attr",ctok->name)){
	char *d;
	next_token();
	killsp();
	if(ctok->type==LPAR) next_token(); else error(151);
	killsp();
	if(d=get_string()){
	  if(!attr){
	    attr=d;
	  }else{
	    attr=myrealloc(attr,strlen(attr)+strlen(d)+2);
	    strcat(attr,";");
	    strcat(attr,d);
	    free(d);
	  }
	  notdone=1;
	}
	killsp();
	if(ctok->type==RPAR) next_token(); else error(59);
	killsp();
      }
    }while(notdone);
    a->attr=attr;
  }
  return a;
}
struct Typ *direct_declarator(struct Typ *a)
/*  Unterroutine zu declarator()                    */
/*  behandelt [],(funkt),(dekl).                     */
{
  struct Typ *rek=0,*merk,*p,*t=0,*first,*last=0;
  struct struct_declaration *fsd;
  struct struct_list (*sl)[];
  size_t slsz;char *imerk;
  char fbuff[MAXI];
  killsp();
  if(ctok->type!=NAME&&ctok->type!=LPAR&&ctok->type!=LBRK)
    return a;
  if(ctok->type==NAME){
    cpbez(ident,1);
    next_token();
    if(!a) return(0);
#ifdef HAVE_ECPP
    if(ctok->type==NOT){
      next_token();
      if(ctok->type!=NAME)return(0);
      snprintf(ident,MAXI,"%s~%s",ident,ctok->name);
    }
#endif
  }
  else if(ctok->type==LPAR&&a){
    /* Rekursion */
    struct token mtok;
    copy_token(&mtok,ctok);
    next_token(); killsp();
    if(ctok->type!=RPAR&&*ident==0&&!declaration(0)){
      merk=a;
      rek=declarator(a);
      if(ctok->type!=RPAR) error(59); else next_token();
    }else

      push_token(&mtok);
    free(mtok.name);
  }
  if(!a)return(0);
  killsp();
  while(ctok->type==LBRK||ctok->type==LPAR){
    if(ctok->type==LBRK){
      next_token();
      killsp();
      p=new_typ();
      p->flags=ARRAY;
      p->next=0;
      if(ctok->type==RBRK){
        p->size=l2zm(0L);
      }else{
        np tree;
        tree=expression();
        if(!type_expression(tree)){
          /*                    error("incorrect constant expression");*/
        }else{
          if(tree->sidefx&&!c99) error(60);
          if(tree->flags!=CEXPR||!ISINT(tree->ntyp->flags)){
	    if(!c99)
	      error(19);
	    else{
	      struct Typ *st;struct IC *new;
	      st=new_typ();
	      st->flags=HAVE_INT_SIZET?(UNSIGNED|INT):(UNSIGNED|LONG);
	      p->dsize=add_tmp_var(st);
	      gen_IC(tree,0,0);
	      convert(tree,st->flags);
	      new=new_IC();
	      new->code=ASSIGN;
	      new->typf=st->flags;
	      new->q1=tree->o;
	      new->z.flags=VAR;
	      new->z.v=p->dsize;
	      new->z.val.vmax=l2zm(0L);
	      new->q2.val.vmax=szof(st);
	      add_IC(new);
	    }
          }else{
            eval_constn(tree);
            p->size=vmax;
            if(zmeqto(p->size,l2zm(0L))) {error(61);p->size=l2zm(1L);}
          }
        }
        free_expression(tree);
      }
      if(ctok->type!=RBRK) error(62); else next_token();
      if(last){
        last->next=p;
        last=p;
      }else{
        first=last=p;
      }
    }
#ifdef HAVE_ECPP
    if(ecpp&&current_func&&ctok->type==LPAR&&ISSTRUCT(a->flags)){
      /* don't parse this as function decl, but as ctor call - in var_declaration() */
      return a;
    }
#endif
    if(ctok->type==LPAR){
      int komma,firstparm,oldstyle=0;
#ifdef HAVE_REGPARMS
      struct reg_handle reg_handle=empty_reg_handle;
      struct Typ rpointer={0};
      if(!ffreturn(a)&&(a->flags&NQ)!=VOID){
	rpointer.flags=POINTER_TYPE(a);
        rpointer.next=a;
        reg_parm(&reg_handle,&rpointer,0,0);
      }
#endif
#ifdef HAVE_MISRA
	  if (misra_return_type_unspec) misra_neu(75,8,2,0);
	  misra_return_type_unspec = 0;
#endif
      next_token();
      killsp();
#ifdef HAVE_MISRA
      if(ctok->type==RPAR) misra_neu(76,16,5,0);
#endif
      fsd=mymalloc(sizeof(*fsd));
      slsz=SLSIZE;
      sl=mymalloc(sizeof(struct struct_list)*slsz);
      fsd->count=0;
      imerk=ident;komma=0;
      enter_block();
      firstparm=1;
      while(ctok->type!=RPAR&&ctok->type!=MDOTS){
        int hard_reg;
#ifdef HAVE_ECPP
				int ecpp_merk_flags=ecpp_ret_flags;
#endif
	if(!firstparm&&!komma) error(57);
	komma=firstparm=0;
        ident=fbuff;*fbuff=0;
        t=declaration_specifiers();
        hard_reg=return_reg;
        t=declarator(t);
        if(!t){
          oldstyle=1;
          if(*ident==0) {error(20);freetyp(t);continue;}
        }
        if(fsd->count){
          if((t&&!(*sl)[fsd->count-1].styp)||
             (!t&&(*sl)[fsd->count-1].styp))
            error(63);
        }
#ifdef HAVE_ECPP
        if(ecpp){
					if(ecpp_ret_flags&ECPP_VIRTUAL)error(339);
					ecpp_ret_flags=ecpp_merk_flags;
				}
        /*if(return_sc&STATIC){
          (*sl)[fsd->count].styp->ecpp_flags|=ECPP_STATIC;
          return_sc&=~STATIC;
        }*/
#endif
        if(!return_sc) return_sc=AUTO;
        if(return_sc!=AUTO&&return_sc!=REGISTER)
          {error(21);return_sc=AUTO;}
        (*sl)[fsd->count].styp=t;
        (*sl)[fsd->count].storage_class=return_sc;
	{
	  int m=nesting;
	  nesting=0;
	  (*sl)[fsd->count].identifier=add_identifier(ident,strlen(ident));
	  nesting=m;
	}
        if(t){
          if(((*sl)[fsd->count].styp->flags&NQ)==VOID&&fsd->count!=0)
            error(22);
          /*  Arrays in Zeiger umwandeln  */
          if(ISARRAY((*sl)[fsd->count].styp->flags))
            (*sl)[fsd->count].styp->flags=POINTER_TYPE((*sl)[fsd->count].styp->next);
	  /*  Funktionen in Zeiger auf Funktionen umwandeln   */
	  if(ISFUNC((*sl)[fsd->count].styp->flags)){
	    struct Typ *new;
	    new=new_typ();
	    new->next=(*sl)[fsd->count].styp;
	    new->flags=POINTER_TYPE(new->next);
	    (*sl)[fsd->count].styp=new;
	  }
	}
        if(hard_reg&&regok(hard_reg,t->flags,-1)<=0) error(217,regnames[hard_reg]);
#ifdef HAVE_REGPARMS
        if(t) (*sl)[fsd->count].reg=reg_parm(&reg_handle,t,0,0);
        if(hard_reg) (*sl)[fsd->count].reg=hard_reg;
#else
        (*sl)[fsd->count].reg=hard_reg;
#endif
        fsd->count++;
        if(fsd->count>=slsz-2){     /*  eins Reserve fuer VOID  */
          slsz+=SLSIZE;
          sl=myrealloc(sl,slsz*sizeof(struct struct_list));
        }
        killsp(); /* Hier Syntaxpruefung strenger machen */
        if(ctok->type==COMMA) {next_token();komma=1; killsp();}
      }
      ident=imerk;
#ifdef HAVE_MISRA
      if(ctok->type==MDOTS) misra_neu(69,16,1,0);
#endif
      if(ctok->type!=MDOTS||!komma){
        int ecpp_addvoid=0;
#ifdef HAVE_ECPP
        ecpp_addvoid=ecpp&&ctok->type!=MDOTS&&(fsd->count==0||(fsd->count>0&&(!(*sl)[fsd->count-1].styp||((*sl)[fsd->count-1].styp->flags&NQ)!=VOID)));
#endif
        if(ecpp_addvoid||(!ecpp&&fsd->count>0&&(!(*sl)[fsd->count-1].styp||((*sl)[fsd->count-1].styp->flags&NQ)!=VOID))){
          (*sl)[fsd->count].styp=new_typ();
          (*sl)[fsd->count].styp->flags=VOID;
          (*sl)[fsd->count].styp->next=0;
          (*sl)[fsd->count].identifier=empty;
#ifdef HAVE_ECPP
          (*sl)[fsd->count].mangled_identifier=0;
#endif
          fsd->count++;
        }
        if(ecpp&&ctok->type==MDOTS){next_token();killsp();}
      }else if(komma){
        next_token();komma=0;
        if(oldstyle) error(221);
      }
      p=new_typ();
      p->flags=FUNKT;
      {
        int m=nesting;
        nesting=0;
        p->exact=add_sd(fsd,FUNKT);
        add_sl(fsd,sl);
        free(sl);
        nesting=m;
      }
      killsp();
      if(komma) error(59);
      if(ctok->type!=RPAR) error(59); else next_token();
      killsp();
      if(ctok->type==COMMA||ctok->type==SEMIC||ctok->type==RPAR||ctok->type==ASGN)
	leave_block();
      if(last){
        last->next=p;
        last=p;
      }else{
        first=last=p;
      }
    }
    killsp();
  }
  if(last){last->next=a;last=a;a=first;}
  if(rek!=0&&rek!=merk){
    /* Zweite Liste anhaengen */
    p=rek;
    while(p->next!=merk) p=p->next;
    if(p) p->next=a; else ierror(0);
    return rek;
  }
  return a;
}
int declaration(int offset)
/*  Testet, ob eine Typangabe kommt. Wenn offset!=0 ist,    */
/*  muss s auf '(' zeigen und es wird getestet, ob nach der */
/*  Klammer eine Typangabe kommt.                           */
/*  In jedem Fall zeigt s danach wieder auf dieselbe Stelle */
/*  im Source.                                              */
{
  struct Var *v;struct token mtok;
  int fl=0;
  if(offset){
    copy_token(&mtok,ctok);
    next_token();
    killsp();
  }
  if(ctok->type==NAME){
    if(!strcmp("auto",ctok->name)) fl=1;
    else if(!strcmp("char",ctok->name)) fl=1;
    else if(!strcmp("const",ctok->name)) fl=1;
    else if(!strcmp("double",ctok->name)) fl=1;
    else if(!strcmp("enum",ctok->name)) fl=1;
    else if(!strcmp("extern",ctok->name)) fl=1;
    else if(!strcmp("float",ctok->name)) fl=1;
    else if(!strcmp("int",ctok->name)) fl=1;
    else if(!strcmp("long",ctok->name)) fl=1;
    else if(!strcmp("register",ctok->name)) fl=1;
    else if(c99&&!strcmp("restrict",ctok->name)) fl=1;
    else if(!strcmp("short",ctok->name)) fl=1;
    else if(!strcmp("signed",ctok->name)) fl=1;
    else if(!strcmp("static",ctok->name)) fl=1;
    else if(!strcmp("struct",ctok->name)) fl=1;
    else if(!strcmp("typedef",ctok->name)) fl=1;
    else if(!strcmp("union",ctok->name)) fl=1;
    else if(!strcmp("unsigned",ctok->name)) fl=1;
    else if(!strcmp("void",ctok->name)) fl=1;
    else if(!strcmp("volatile",ctok->name)) fl=1;
    else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__reg",ctok->name)) fl=1;
    else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__attr",ctok->name)) fl=1;
    else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__vattr",ctok->name)) fl=1;
    else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__readsmem",ctok->name)) fl=1;
    else if(/*!(c_flags[7]&USEDFLAG)&&*/!strcmp("__writesmem",ctok->name)) fl=1;
    else{
      v=find_var(ctok->name,0);
      if(v&&v->storage_class==TYPEDEF) fl=1;
    }
  }
#ifdef HAVE_ECPP
  if(ecpp&&!fl&&ctok->type==NAME){
    char *id=0;
    struct struct_declaration *sd;
    sd=ecpp_find_scope(ctok->name,&id);
    if(sd&&!id)  fl=1;
  }
#endif
  if(offset){
    push_token(&mtok);
    free(mtok.name);
  }
  return fl;
}
void init_sl(struct struct_list *sl){
  if(!sl){ierror(0);return;}
  sl->identifier=0;
  sl->styp=0;
  sl->align=0;
  sl->bfoffset=-1;
  sl->bfsize=-1;
  sl->reg=0;
  sl->storage_class=0;
#ifdef HAVE_ECPP
  sl->mangled_identifier=0;
#endif
}
void add_sl(struct struct_declaration *sd,struct struct_list (*sl)[])
/*  Fuegt ein struct_list-Array in eine struct_declaration ein.     */
/*  Das Array muss mind. sd->count Elements haben und wird kopiert. */
{
  size_t sz=sizeof(struct struct_list)*sd->count;
  sd->sl=mymalloc(sz);
  memcpy(sd->sl,sl,sz);
}
struct struct_declaration *add_sd(struct struct_declaration *new,int typ)
/*  Fuegt eine struct Declaration in Liste ein.     */
{
  new->next=0;
  new->label=0;
  new->typ=typ;
  new->identifier=0;
  new->tunit=last_tunit;
#ifdef HAVE_ECPP
  new->higher_nesting=0;
  new->base_class=0;
	new->ecpp_flags=0;
  new->mangled_identifier=0;
  new->num_friends=0;
  new->friends=0;
#endif
  if(first_sd[nesting]==0){
    first_sd[nesting]=last_sd[nesting]=new;
  }else{
    last_sd[nesting]->next=new;
    last_sd[nesting]=new;
  }
  return new;
}
void free_sd(struct struct_declaration *p)
/*  Gibt eine struct_declaration-List inkl. struct_lists und    */
/*  allen Typen jeder struct_list frei, nicht aber identifier.  */
{
  int i;struct struct_declaration *merk;
  while(p){
    merk=p->next;
    if(p->sl){
      for(i=0;i<p->count;i++){
	if((*p->sl)[i].styp) freetyp((*p->sl)[i].styp);
      }
      if(p->count>0) free(p->sl);
    }
#ifdef HAVE_ECPP
    if(ecpp){
      if(p->friends)free(p->friends);
    }
#endif
    free(p);
    p=merk;
  }
}
char *add_identifier(char *identifier,int length)
/*  Kopiert identifier an sicheren Ort, der spaeter zentral     */
/*  freigegeben werden kann.                                    */
/*  Sollte noch einbauen, dass ueberprueft wird, ob schon       */
/*  vorhanden und dann nicht zweimal speichern.                 */
{
  struct identifier_list *new;
  if((*identifier==0&&length==0)||identifier==empty) return(empty);
  new=mymalloc(sizeof(struct identifier_list));
  new->identifier=mymalloc(length+1);
  memcpy(new->identifier,identifier,length+1);
  new->next=0;new->length=length;
  if(last_ilist[nesting]){
    last_ilist[nesting]->next=new;
    last_ilist[nesting]=new;
  }else{
    last_ilist[nesting]=first_ilist[nesting]=new;
  }
  return(new->identifier);
}
void free_ilist(struct identifier_list *p)
/*  Gibt eine verkettete identifier_liste und saemtliche darin  */
/*  gespeicherten Identifier frei.                              */
{
  struct identifier_list *merk;
  while(p){
    merk=p->next;
    if(p->identifier) free(p->identifier);
    free(p);
    p=merk;
  }
}
int type_uncomplete(struct Typ *p)
/*  Testet, ob Typ unvollstaendig ist. Momentan gelten nur      */
/*  unvollstaendige Strukturen und Arrays von solchen als       */
/*  unvollstaendig, aber keine Zeiger oder Funktionen darauf.   */
{
  struct struct_declaration *sd;
  if(!p){ierror(0);return(0);}
  if(ISSTRUCT(p->flags)||ISUNION(p->flags))
    if(p->exact->count<=0) return 1;
  if(ISARRAY(p->flags)){
    if(!p->dsize&&zmleq(p->size,l2zm(0L))) return 1;
if(!p->next) ierror(0);
    if(type_uncomplete(p->next)) return 1;
  }
  return 0;
}
void add_struct_identifier(char *identifier,struct struct_declaration *sd)
/*  Erzeugt neuen struct_identifier, fuegt ihn in Liste an und  */
/*  vervollstaendigt unvollstaendige Typen dieser Struktur.     */
{
  struct struct_identifier *new;
/*    struct Typ *t;*/
  if(DEBUG&1) printf("add_si %s (nesting=%d)->%p\n",identifier,nesting,(void *)sd);
#ifdef HAVE_MISRA
  if(misracheck){
    if(find_var(identifier,0)||find_label(identifier))
      misra_neu(12,5,6,0,identifier);
    if(find_struct(identifier,0)) misra_neu(21,5,2,0,identifier);
  }
#endif
  new=mymalloc(sizeof(struct struct_identifier));
  new->identifier=add_identifier(identifier,strlen(identifier));
  new->sd=sd;
  new->next=0; 
  if(first_si[nesting]==0){
    first_si[nesting]=new;last_si[nesting]=new;
  }else{
    last_si[nesting]->next=new;last_si[nesting]=new;
  }
  sd->identifier=new->identifier;
}
void free_si(struct struct_identifier *p)
/*  Gibt eine struct_identifier-Liste frei, aber nicht die      */
/*  identifiers und struct_declarations.                        */
{
  struct struct_identifier *merk;
  while(p){
    merk=p->next;
    p->sd->identifier="<prototype-only>";
    free(p);
    p=merk;
  }
}
struct struct_declaration *find_struct(char *identifier,int endnesting)

/*  Sucht angegebene Strukturdefinition und liefert             */
/*  entsprechende struct_declaration.                           */
{
  struct struct_identifier *si; int i,l;
  if(misracheck) l=strlen(identifier);
  for(i=nesting;i>=endnesting;i--){
    si=first_si[i];
    while(si){
      if(!strcmp(si->identifier,identifier)){
        if(DEBUG&1) printf("found struct tag <%s> at nesting %d->%p\n",identifier,i,(void *)si->sd);
        return(si->sd);
      }
#ifdef HAVE_MISRA
      if(misracheck&&l>31){
		if(!strncmp(si->identifier,identifier,31))
			misra_neu(11,5,1,0,si->identifier,identifier);
	  }
#endif
	si=si->next;
	}
  }
  if(DEBUG&1) printf("didn't find struct tag <%s>\n",identifier);
  return(0);
}

/* generate code to create a variable-lenght-array */
static void create_allocvl(struct Var *v)
{
  np tree,ds;
  struct IC *new;struct Var *fv;

  /* check if we got a frame-pointer */
  if(FPVLA_REG&&!regsa[FPVLA_REG]){
    if(regused[FPVLA_REG])
      ierror(0);
    else
      regsa[FPVLA_REG]=regscratch[FPVLA_REG]=regused[FPVLA_REG]=1;
  }

  /* if this is the first vla in this block, save old sp */
  if(!block_vla[nesting]){
    /* declare function - may be done by the backend, if necessary */
    if(!(fv=find_ext_var("__oldvlasp"))){
      struct Typ *t;
      static struct Typ voidt={VOID};
      struct struct_declaration *sd=mymalloc(sizeof(*sd));
      t=new_typ();
      t->flags=FUNKT;

      t->next=new_typ();
      t->next->flags=POINTER_TYPE(&voidt);
      t->next->next=clone_typ(&voidt);
      sd->count=0;
      t->exact=add_sd(sd,FUNKT);
      fv=add_var("__oldvlasp",t,EXTERN,0);
      fv->fi=new_fi();
      fv->fi->flags|=(ALL_USES|ALL_MODS);
      fv->fi->inline_asm=mystrdup(OLDSPVLA_INLINEASM);
    } 
    block_vla[nesting]=add_tmp_var(clone_typ(fv->vtyp->next));
    tree=gen_libcall("__oldvlasp",0,0,0,0);
    new=new_IC();
    new->code=ASSIGN;
    new->z.flags=VAR;
    new->z.v=block_vla[nesting];
    new->z.val.vmax=l2zm(0L);
    new->q1=tree->o;
    new->q2.flags=0;
    new->typf=block_vla[nesting]->vtyp->flags;
    new->q2.val.vmax=sizetab[new->typf];
    add_IC(new);
    free_expression(tree);
  }

  /* make room on the stack */
  ds=new_node();
  ds->flags=IDENTIFIER;
  ds->identifier=empty;
  ds->dsize=vlength_szof(v->vtyp);
  ds->val.vmax=l2zm(0L);
  ds->ntyp=clone_typ(ds->dsize->vtyp);
  ds->o.flags=VAR;
  ds->o.v=ds->dsize;
  ds->o.val.vmax=l2zm(0L);
  ds->left=ds->right=0;

  if(!find_ext_var("__allocvla")){
    /* declare function */
    struct struct_declaration *sd=mymalloc(sizeof(*sd));
    struct Typ *t=new_typ();
    sd->count=1;
    sd->sl=mymalloc(sizeof(struct struct_list));
    (*sd->sl)[0].storage_class=AUTO;
    (*sd->sl)[0].styp=clone_typ(ds->dsize->vtyp);
    (*sd->sl)[0].reg=ALLOCVLA_REG;
    t->flags=FUNKT;
    t->exact=add_sd(sd,FUNKT);
    t->next=new_typ();
    t->next->flags=POINTER_TYPE(v->vtyp);
    t->next->next=new_typ();
    t->next->next->flags=v->vtyp->flags;
    fv=add_var("__allocvla",t,EXTERN,0);
    fv->fi=new_fi();
    fv->fi->flags|=(ALL_USES|ALL_MODS);
    fv->fi->inline_asm=mystrdup(ALLOCVLA_INLINEASM);
  }

  if(!type_expression(ds)) ierror(0);
  tree=gen_libcall("__allocvla",ds,0,0,0);
  new=new_IC();
  new->code=ASSIGN;
  new->z.flags=VAR;
  new->z.v=v;
  new->z.val.vmax=l2zm(0L);
  new->q1=tree->o;
  new->q2.flags=0;
  new->typf=POINTER_TYPE(v->vtyp);
  new->q2.val.vmax=sizetab[new->typf];
  add_IC(new);
  free_expression(tree);
  vlas=1;
}

/* reset sp to remove variable-length-arrays */
void freevl(void)
{
  np tree,ds;
  struct Var *fv;
  dontdelete=1; /* never remove them, otherwise, fix_vla_jump get confused */
  if(!find_ext_var("__resetvlasp")){
    struct struct_declaration *sd=mymalloc(sizeof(*sd));
    struct Typ *t=new_typ();
    sd->count=1;
    sd->sl=mymalloc(sizeof(struct struct_list));
    (*sd->sl)[0].storage_class=AUTO;
    (*sd->sl)[0].styp=clone_typ(block_vla[nesting]->vtyp);
    (*sd->sl)[0].reg=FREEVLA_REG;
    t->flags=FUNKT;
    t->exact=add_sd(sd,FUNKT);
    t->next=new_typ();
    t->next->flags=VOID;
    fv=add_var("__resetvlasp",t,EXTERN,0);
    fv->fi=new_fi();
    fv->fi->flags|=(ALL_USES|ALL_MODS);
    fv->fi->inline_asm=mystrdup(FREEVLA_INLINEASM);
  }
  if(nesting==1){
    clear_main_ret();
    gen_label(return_label);
    did_return_label=1;
  }
  ds=new_node();
  ds->flags=IDENTIFIER;
  ds->identifier=empty;
  ds->dsize=block_vla[nesting];
  ds->val.vmax=l2zm(0L);
  ds->ntyp=clone_typ(ds->dsize->vtyp);
  ds->left=ds->right=0;
  ds->o.flags=VAR;
  ds->o.v=ds->dsize;
  ds->o.val.vmax=l2zm(0L);
  if(!type_expression(ds)) ierror(0);
  tree=gen_libcall("__resetvlasp",ds,0,0,0);
  free_expression(tree);
  dontdelete=0;
}  

void clearvl(void)
{
  struct llist *p,*n;
  struct vlaadjust_list *vl,*vn;
  /* look for stack-adjusts that have to be removed */
  vl=vlaadjusts[nesting];
  while(vl){
    int ln;
    struct IC *ic=vl->branch;
    if(ic){
      ln=ic->typf;
      if(ic->code!=BRA) ierror(0);
      p=vladeflabels[nesting];
      while(p){
	if(p->label==ln) break;
	p=p->next;
      }
      if(!p){
	ic=ic->prev;
	while(ic!=vl->first->prev){
	  if(!ic) ierror(0);
	  ic->code=NOP;
	  ic->q1.flags=ic->q2.flags=ic->z.flags=0;
	  ic=ic->prev;
	}
      }
    }
    vn=vl->next;
    free(vl);
    vl=vn;
  }
  freevl();
  block_vla[nesting]=0;
  p=vladeflabels[nesting];
  while(p){
    n=p->next;
    free(p);
    p=n;
  }
  p=vlajmplabels[nesting];
  while(p){
    n=p->next;
    free(p);
    p=n;
  }
  vladeflabels[nesting]=vlajmplabels[nesting]=0;
  vlaadjusts[nesting]=0;
}

/* Handle a stack of stored sp variables when traversing an IC list; */
void vla_nesting(struct IC *p,struct Var **vn,int *nest)
{
  if(p->code==CALL&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp(p->q1.v->identifier,"__oldvlasp")){
    struct IC *p2=p->next;
    while(p2&&(p2->code==ALLOCREG||p2->code==FREEREG)) p2=p2->next;
    if(!p2||p2->code!=GETRETURN||(p2->z.flags&(VAR|DREFOBJ))!=VAR) ierror(0);
    /*printf("found save sp to %p\n",p2->z.v);*/
    vn[*nest]=p2->z.v;
    (*nest)++;
    vn[*nest]=0;
    if(*nest>=MAXN) ierror(0);
    return;
  }
  if(p->code==CALL&&(p->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp(p->q1.v->identifier,"__resetvlasp")){ 
    /*printf("found reset\n");*/
    if(*nest<=0) ierror(0);
    (*nest)--;
    return;
  }
}

static int return_vla_nest;
static struct Var *return_last_vlasp;

/* Find the stack pointer that is needed when jumping to label lab */
struct Var *vla_find_sp(int lab)
{
  struct IC *p;int nest=0;
  static struct Var *vn[MAXN];
  for(p=first_ic;p;p=p->next){
    if(p->code==LABEL&&p->typf==lab){
      return_vla_nest=nest;
      return_last_vlasp=vn[nest];
      if(nest<=0)
	return 0;
      else
	return vn[nest-1];
    }
    vla_nesting(p,vn,&nest);
  }
  ierror(0);
}

void vla_jump_fix(void)
{
  struct IC *p;int nest=0;
  static struct Var *vn[MAXN],*savedsp;
  if(DEBUG&1) printf("searching for vla-jump-fixes\n");
  for(p=first_ic;p;p=p->next){
    /*pric2(stdout,p);*/
    if(p->code>=BEQ&&p->code<=BRA){
      /*printf("jump found\n");*/
      p->savedsp=0;
      if(1/*nest>0*/){
	/*printf("is in vla context!\n");*/
	savedsp=vla_find_sp(p->typf);
	if(return_vla_nest>nest||(nest>0&&return_vla_nest==nest&&savedsp!=vn[nest-1])){
	  err_ic=p;
	  error(351);
	}else if(nest==0||savedsp==vn[nest-1]){
	  /*printf("jump within the same context\n");*/
	}else{
	  if(vn[nest]){
	    if(DEBUG&1) printf("have to set sp to %p\n",return_last_vlasp);
	    p->savedsp=return_last_vlasp;
	  }else{
	    int ndiff=nest-return_vla_nest-1;
	    struct IC *p2;
	    /*printf("have to search oldsp ndiff=%d\n",ndiff);*/
	    for(p2=p->prev;p2;p2=p2->prev){
	      if(p2->code==CALL&&(p2->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp(p2->q1.v->identifier,"__oldvlasp")){
		if(ndiff==0){
		  /*printf("found savesp\n");*/
		  p2=p2->next;
		  while(p2&&(p2->code==ALLOCREG||p2->code==FREEREG)) p2=p2->next;
		  if(!p2||p2->code!=GETRETURN) ierror(0);
		  if((p2->z.flags&(VAR|DREFOBJ))!=VAR) ierror(0);
		  /*printf("found oldsp %p\n",p2->z.v);*/
		  p->savedsp=p2->z.v;
		  break;
		}
		ndiff--;
	      }
	      if(p2->code==CALL&&(p2->q1.flags&(VAR|DREFOBJ))==VAR&&!strcmp(p2->q1.v->identifier,"__resetvlasp")){
		ndiff++;
	      }
	    }
	    if(!p2) ierror(0);
	  }
	}
      }else{
	/*printf("not in vla context\n");*/
      }
    }
    vla_nesting(p,vn,&nest);
  }
  for(p=first_ic;p;p=p->next){
    if(p->code>=BEQ&&p->code<=BRA){  
      if(p->savedsp){
	struct IC *merkfic,*merklic,*newcode,*m,*new,*setr=0;
	np ds,tree;
	if(DEBUG&1) printf("generating sp-adjust\n");
        merkfic=first_ic;merklic=last_ic;
	first_ic=0;last_ic=0;
	ds=new_node();
	ds->flags=IDENTIFIER;
	ds->identifier=empty;
	ds->dsize=p->savedsp;
	ds->val.vmax=l2zm(0L);
	ds->ntyp=clone_typ(ds->dsize->vtyp);
	ds->left=ds->right=0;
	ds->o.flags=VAR;
	ds->o.v=ds->dsize;
	ds->o.val.vmax=l2zm(0L);
	if(!type_expression(ds)) ierror(0);
	tree=gen_libcall("__resetvlasp",ds,0,0,0);
	free_expression(tree);
	newcode=first_ic;
	first_ic=merkfic;last_ic=merklic;
	if(p->code==BRA){
	  /* check if the branch was preceded by a SETRETURN */
	  struct IC *p2=p->prev;
	  while(p2&&(p2->code==FREEREG||p2->code==ALLOCREG))
	    p2=p2->prev;
	  if(p2->code==SETRETURN&&p2->z.reg)
	    setr=p2;
	}else{
	  new=new_IC();
	  new->typf=++label;
	  if(p->code==BEQ) new->code=BRA;
	  else if(p->code==BNE) new->code=BEQ;
	  else if(p->code==BLT) new->code=BGE;
	  else if(p->code==BGT) new->code=BLE;
	  else if(p->code==BLE) new->code=BGT;
	  else if(p->code==BGE) new->code=BLT;
	  insert_IC(p->prev,new);
	}
	while(newcode){
	  m=newcode->next;
	  insert_IC(p->prev,newcode);
	  newcode=m;
	}
	if(p->code!=BRA){
	  p->code=BRA;
	  new=new_IC();
	  new->code=LABEL;
	  new->typf=label;
	  insert_IC(p,new);
	}
	if(setr){
	  /* save the return value to save it from being overwritten */
	  /* could be optimized further */
	  struct Var *v;
	  if(ISSCALAR(setr->typf)){
	    static struct Typ t={0};
	    t.flags=setr->typf;
	    v=add_tmp_var(clone_typ(&t));
	  }else ierror(0);
	  new=new_IC();
	  *new=*setr;
	  new->q1.flags=VAR;
	  new->q1.v=v;
	  new->q1.val.vmax=l2zm(0L);
	  setr->z=new->q1;
	  setr->code=ASSIGN;
	  insert_IC(p->prev,new);
	}
      }
    }
  }
}  
  

struct Var *add_tmp_var(struct Typ *t)
{
  t->flags&=NU;
  return add_var(empty,t,AUTO,0);
}
struct Var *add_var(char *identifier, struct Typ *t, int storage_class,struct const_list *clist)
/*  Fuegt eine Variable mit Typ in die var_list ein.            */
/*  In der storage_class werden die Flags PARAMETER und evtl.   */
/*  OLDSTYLE und REGPARM erkannt.                               */
{
  struct Var *new;int f;
  struct struct_declaration *sd;
  static zmax paroffset;
  zmax al;
  /*if(*identifier==0) return;*/ /* sollte woanders bemaekelt werden */
  if(DEBUG&2) printf("add_var(): %s\n",identifier);
#ifdef HAVE_MISRA
  if(misracheck){
    if(find_struct(identifier,0)||find_label(identifier))
      misra_neu(12,5,6,0,identifier);
    if(find_var(identifier,0)) misra_neu(21,5,2,0,identifier);
		if (misra_is_reserved(identifier)) misra_neu(115,20,2,0);
  }
#endif
  if(ISFUNC(t->flags&NQ)&&(ISARRAY(t->next->flags)||ISFUNC(t->next->flags)))
    error(25);
  new=mymalloc(sizeof(struct Var));
  if(!*identifier&&identifier!=empty) ierror(0);
  new->clist=clist;
  new->vtyp=t;
  new->storage_class=storage_class&PLAIN_STORAGE_CLASS;
  new->reg=0;
  new->vattr=0;
  new->next=0;
  new->flags=0;
  new->inr=0;
  new->fi=0;
  new->nesting=nesting;
  new->filename=filename;
  new->line=line;
  new->dfilename=0;
  new->dline=0;
  new->description=0;
  new->tunit=last_tunit;
#ifdef HAVE_TARGET_ATTRIBUTES
  new->tattr=0;
#endif
  /*    if((storage_class&PLAIN_STORAGE_CLASS)==STATIC||(storage_class&PLAIN_STORAGE_CLASS)==EXTERN) new->flags=USEDASSOURCE|USEDASDEST;*/
  if(DEBUG&2) printf("storage_class=%d\n",storage_class);
  if(storage_class&PARAMETER) new->flags|=USEDASDEST;
  if(storage_class&REGPARM){
    new->flags|=REGPARM;
    if(!(storage_class&DBLPUSH)){
      if((storage_class&OLDSTYLE)&&(t->flags&NQ)==FLOAT)
	new->flags|=CONVPARAMETER;
      storage_class&=~PARAMETER;
    }
  }
  if(DEBUG&2) printf("storage_class=%d\n",storage_class);
  if(DEBUG&2) printf("max_offset=%ld\n",zm2l(max_offset));
  if(is_vlength(t)&&storage_class!=AUTO&&storage_class!=REGISTER)
    error(315);
  if((storage_class&PLAIN_STORAGE_CLASS)==REGISTER) new->priority=registerpri; else new->priority=0;
  if(/*nesting==0&&*/new->storage_class==EXTERN){
    int m=nesting;
    nesting=0;
    new->identifier=add_identifier(identifier,strlen(identifier));
    nesting=m;
    if(last_ext){
      last_ext->next=new;
      last_ext=new;
    }else{
      first_ext=last_ext=new;
      vl0=first_ext;
    }
  }else{
    new->identifier=add_identifier(identifier,strlen(identifier));
    if(last_var[nesting]){
      new->offset=zmadd(last_var[nesting]->offset,szof(last_var[nesting]->vtyp));
      last_var[nesting]->next=new;
      last_var[nesting]=new;
    }else{
      new->offset=l2zm(0L);
      paroffset=l2zm(0L);;
      first_var[nesting]=last_var[nesting]=new;
      if(nesting==0) vl1=new;
      if(nesting==1) vl2=new;
    }
  }
  f=t->flags&NQ;
  if((storage_class&PLAIN_STORAGE_CLASS)==AUTO||(storage_class&PLAIN_STORAGE_CLASS)==REGISTER){
    if(DEBUG&2) printf("auto\n");
    al=falign(t);
    if((c_flags_val[0].l&2)&&nesting==1&&!(storage_class&PARAMETER)){
      new->offset=max_offset;
    }else{
      if(storage_class&PARAMETER){
        new->offset=paroffset;
	if(align_arguments)
	  new->offset=zmmult(zmdiv(zmadd(new->offset,zmsub(al,l2zm(1L))),al),al);
      }else{
        new->offset=local_offset[nesting];
	new->offset=zmmult(zmdiv(zmadd(new->offset,zmsub(al,l2zm(1L))),al),al);
      }
    }
    if(storage_class&PARAMETER){
      new->offset=zmmult(zmdiv(zmadd(new->offset,zmsub(stackalign,l2zm(1L))),stackalign),stackalign);
      if(ISINT(f)&&f<INT&&!short_push){
        /*  Integer-Erweiterungen fuer alle Funktionsparameter  */
        paroffset=zmadd(new->offset,sizetab[INT]);
      }else{
        if(f==FLOAT&&(storage_class&OLDSTYLE)){
          /*  Bei alten Funktionen werden FLOAT als DOUBLE uebergeben */
          new->offset=zmmult(zmdiv(zmadd(new->offset,zmsub(align[DOUBLE],l2zm(1L))),align[DOUBLE]),align[DOUBLE]);
          paroffset=zmadd(new->offset,sizetab[DOUBLE]);
        }else{
          paroffset=zmadd(new->offset,szof(new->vtyp));
        }
      }
    }else{
      local_offset[nesting]=zmadd(new->offset,szof(new->vtyp));
    }
    if(!(storage_class&PARAMETER))
      if(zmleq(max_offset,local_offset[nesting])) max_offset=local_offset[nesting];
    if(DEBUG&2) printf("max_offset=%ld\n",zm2l(max_offset));
  }
  if((storage_class&PLAIN_STORAGE_CLASS)==STATIC) new->offset=l2zm((long)++label);
  if(storage_class&PARAMETER){

    if(DEBUG&2) printf("parameter\n");

    if(ISINT(f)&&f<INT&&!zmleq(sizetab[INT],sizetab[f])){
      if(BIGENDIAN){
        new->offset=zmadd(new->offset,zmsub(sizetab[INT],sizetab[f]));
      }else{
        if(!LITTLEENDIAN)
          ierror(0);
      }
    }
    if((storage_class&OLDSTYLE)&&f==FLOAT){
      /*  Bei alten Funktionen werden DOUBLE nach FLOAT konvertiert   */
      if(!(storage_class&REGPARM)){
#if HAVE_LIBCALLS
	static struct Typ dt={DOUBLE},ft={FLOAT};
	static struct node n,nn;
	struct IC *conv=new_IC();
	n.flags=REINTERPRET;
	n.left=&nn;
	n.ntyp=&dt;
	nn.flags=IDENTIFIER;
	nn.identifier=identifier;
	nn.ntyp=&ft;
	nn.o.flags=VAR|DONTREGISTERIZE;
	nn.o.v=new;
	nn.o.val.vmax=l2zm(0L);
	n.o=nn.o;
	convert(&n,FLOAT);
	

	conv->code=ASSIGN;
	conv->typf=FLOAT;
	conv->q1=n.o;
	conv->z.v=new;
	conv->z.flags=VAR;
	conv->z.val.vmax=l2zm(0L);
	conv->q2.val.vmax=sizetab[FLOAT];
	conv->z.v=new;
	add_IC(conv);	
#else
	struct IC *conv=new_IC();
	conv->code=CONVERT;
	conv->typf=FLOAT;
	conv->typf2=DOUBLE;
	conv->q1.flags=VAR|DONTREGISTERIZE;
	conv->z.flags=VAR;
	conv->q2.flags=0;
	conv->q1.v=conv->z.v=new;
	conv->q1.val.vmax=conv->z.val.vmax=l2zm(0);
	add_IC(conv);
#endif
      }
      new->flags|=CONVPARAMETER;
    }
    new->offset=zmsub(l2zm(0L),zmadd(maxalign,new->offset));
  }
  if((storage_class&PLAIN_STORAGE_CLASS)==EXTERN){
    if(!strcmp("fprintf",identifier)) new->flags|=PRINTFLIKE;
    if(!strcmp("printf",identifier))  new->flags|=PRINTFLIKE;
    if(!strcmp("sprintf",identifier)) new->flags|=PRINTFLIKE;
    if(!strcmp("fscanf",identifier))  new->flags|=SCANFLIKE;
    if(!strcmp("scanf",identifier))   new->flags|=SCANFLIKE;
    if(!strcmp("sscanf",identifier))  new->flags|=SCANFLIKE;
  }
  if(is_vlength(new->vtyp))
    create_allocvl(new);
  return(new);
}
void free_var(struct Var *p)
/*  Gibt Variablenliste inkl. Typ, aber ohne Identifier frei.   */
{
  struct Var *merk;
  while(p){
    if(!*p->identifier&&p->identifier!=empty) ierror(0);
    free(p->description);
    free(p->vattr);
    merk=p->next;
    if(!(p->flags&USEDASADR)&&(p->storage_class==AUTO||p->storage_class==REGISTER)){
      if(*p->identifier&&!(p->flags&USEDASDEST)&&ISSCALAR(p->vtyp->flags)) error(64,p->identifier);
      if(*p->identifier&&!(p->flags&USEDASSOURCE)&&ISSCALAR(p->vtyp->flags)) error(65,p->identifier);
    }
    if(DEBUG&2) printf("free_var %s, pri=%d\n",p->identifier,p->priority);
    if(p->vtyp) freetyp(p->vtyp);
    if(p->clist) free_clist(p->clist);
    if(p->fi){
      if(DEBUG&2) printf("free_fi of function %s\n",p->identifier);
      free_fi(p->fi);
      if(DEBUG&2) printf("end free_fi of function %s\n",p->identifier);
    }
    free(p);
    p=merk;
  }
}
struct Var *find_ext_var(char *identifier)
{
  struct Var *v;int l;
  if(misracheck) l=strlen(identifier);
  for(v=first_ext;v;v=v->next){
    if(!strcmp(v->identifier,identifier)) return v;
#ifdef HAVE_MISRA
    if(misracheck&&l>31){
      if(!strncmp(v->identifier,identifier,31))
		misra_neu(11,5,1,0,v->identifier,identifier);
    }
#endif

  }
  return 0;
}
struct Var *find_var(char *identifier,int endnesting)
/*  Sucht Variable mit Bezeichner und liefert Zeiger zurueck    */
/*  es werden nur Variablen der Bloecke endnesting-nesting      */
/*  durchsucht.                                                 */
{
  int i,l;struct Var *v;
  if(identifier==0||*identifier==0) return 0;
  if(misracheck) l=strlen(identifier);
  for(i=nesting;i>=endnesting;i--){
    for(v=first_var[i];v;v=v->next){
      if(!strcmp(v->identifier,identifier))
	return v;
#ifdef HAVE_MISRA
      if(misracheck&&l>31){
		if(!strncmp(v->identifier,identifier,31))
			misra_neu(11,5,1,0,v->identifier,identifier);
      }
#endif
    }
  }
  if(endnesting==0){
    v=find_ext_var(identifier);
    if(v&&!(v->flags&NOTINTU))
      return v;
    else
      return 0;
  }else
    return 0;
}


#ifdef HAVE_MISRA
int misra_92_violation = 0;
int misra_92_violation_line = 0;
#endif



int check_zero_initialisation(struct const_list* cl, int typ) 
{
  
  if (cl->next) return 0;
  if (cl->tree) return 0;
  if (cl->other)  {
    return check_zero_initialisation(cl->other,typ);
  } else {
    eval_const(&cl->val,typ);
    if ( (zmeqto(vmax,l2zm(0L))) && (zumeqto(vumax,ul2zum(0UL))) && (zldeqto(vldouble,d2zld(0.0))) ) {
      return 1;
    }
  }
  return 0;
}

/* decide whether a initialization shall be performed only be generated code
   or if a table copy should be used first */
int use_only_dyn_init(zmax sz,zmax dyn_sz,zmax const_sz,int dyn_cnt,int const_cnt)
{
  if(zmleq(sz,l2zm(32L)))
    return 1;
  if(zmeqto(dyn_sz,l2zm(0L)))
    return 0;
  if(!zmleq(zmdiv(sz,dyn_sz),l2zm(2L)))
    return 0;
  else
    return 1;
}

void init_local_compound(struct Var *v)
{
  if(v->storage_class==AUTO||v->storage_class==REGISTER){
    struct IC *new;
    /*  Initialisierung von auto-Variablen  */
    new=new_IC();
    new->code=ASSIGN;
    new->typf=v->vtyp->flags;
    new->q2.flags=0;
    new->q2.val.vmax=szof(v->vtyp);
    new->z.flags=VAR;
    new->z.v=v;
    new->z.val.vmax=l2zm(0L);
    if(v->clist->tree){
      /*  einzelner Ausdruck  */
      gen_IC(v->clist->tree,0,0);
      convert(v->clist->tree,v->vtyp->flags&NU);
      new->q1=v->clist->tree->o;
      add_IC(new);
      /*                        v->clist=0;*/
    }else{
      /*  Array etc.  */
      struct Var *nv;
      if(!use_only_dyn_init(szof(v->vtyp),init_dyn_sz,init_const_sz,init_dyn_cnt,init_const_cnt)){
	nv=add_var(empty,clone_typ(v->vtyp),STATIC,v->clist);
	nv->flags|=DEFINED;
	nv->dfilename=filename;
	nv->dline=line;
	nv->vtyp->flags|=CONST;
	/*                        v->clist=0;*/
	new->q1.flags=VAR;
	new->q1.v=nv;
	new->q1.val.vmax=l2zm(0L);
	
	add_IC(new);
	
	dynamic_init(v,v->vtyp,v->clist,0,1);
      }else{
	dynamic_init(v,v->vtyp,v->clist,0,0);
      }
    }   
  }
}

#ifdef HAVE_MISRA
int misra_oldstyle_def = 0;
int	misra_return_count = 0;
int	misra_statement_after_return = 0;
#endif

void var_declaration(void)
/*  Bearbeitet eine Variablendeklaration und erzeugt alle       */
/*  noetigen Strukturen.                                        */
{
  struct Typ *ts,*t,*old=0,*om=0;char *imerk,vident[MAXI];
  int mdef=0,makeint=0,notdone,storage_class,msc,extern_flag,isfunc,
    had_decl,hard_reg,mhr,diffunit=0,inline_flag;
  struct Var *v;
  char *vattr;
  int base_type;
#ifdef HAVE_TARGET_ATTRIBUTES
  unsigned long tattr;
#endif
#ifdef HAVE_ECPP
  struct argument_list *ecpp_ctor_args=0;
  if(ecpp){
    if(ecpp_linkage_specification())return;
  }
#endif
  ts=declaration_specifiers();notdone=1;

  storage_class=return_sc;hard_reg=return_reg;vattr=return_vattr;
  inline_flag=return_inline;
#ifdef HAVE_ECPP
  if(ecpp&&ecpp_ret_flags&ECPP_VIRTUAL)error(339);
#endif
  if(for_decl&&storage_class!=0&&storage_class!=AUTO&&storage_class!=REGISTER){
    error(299);
    storage_class=AUTO;
  }

#ifdef HAVE_TARGET_ATTRIBUTES
  tattr=return_tattr;
#endif
  if(storage_class==EXTERN) extern_flag=1; else extern_flag=0;
  killsp();
  if(ctok->type==SEMIC){
    if(!ts) error(0);
    if(storage_class||(!ISSTRUCT(ts->flags)&&!ISUNION(ts->flags)&&(ts->flags&NQ)!=INT))
      error(36);
    freetyp(ts);
    next_token();killsp();
    return;
  }
  if(nesting==0&&(storage_class==AUTO||storage_class==REGISTER))
    {error(66);storage_class=EXTERN;}
  if(!ts){
    if(nesting<=1){
      ts=new_typ();
      ts->flags=INT;ts->next=0;
      makeint=1;
      if(!storage_class) storage_class=EXTERN;
#ifdef HAVE_MISRA
      misra_neu(75,8,2,0);
#endif
      error(67);
    }else{
      ierror(0);return;
    }
  }
  if(storage_class==0){
    if(nesting==0) 
      storage_class=EXTERN;
    else
      storage_class=AUTO;
  }
  msc=storage_class;mhr=hard_reg;
  while(notdone){
    int oldnesting=nesting;
    imerk=ident;ident=vident;*vident=0;  /* merken von ident hier vermutlich */
    storage_class=msc;hard_reg=mhr;
    if(old) {freetyp(old);old=0;}
    t=declarator(clone_typ(ts));
#ifdef HAVE_ECPP
    if(ecpp&&current_func&&ctok->type==LPAR&&ISSTRUCT(t->flags)){
      /* ctor call (e.g. A a(23,21);)*/
      /* FIXME - this could still be a local function declaration returning a class type! */
      ecpp_ctor_args=argument_list_expression();
    }
#endif
#ifdef HAVE_EXT_TYPES
    conv_typ(t);
#endif
    if(!ISFUNC(t->flags)){
        isfunc=0;
        if(inline_flag) error(301);
    }else{
        isfunc=1;
#ifdef HAVE_MISRA
        if(oldnesting!=0) misra_neu(68,8,6,0);
#endif
        if(storage_class!=STATIC&&storage_class!=TYPEDEF) storage_class=EXTERN;
    }
    ident=imerk;                    /* nicht unbedingt noetig ?         */
    if(!*vident){
      free(ts);free(t);
      error(36);return;
    }
    v=find_var(vident,oldnesting);
    if(!v&&/*oldnesting==0&&*/storage_class==EXTERN&&cross_module&&(v=find_ext_var(vident))){
      v->flags&=~NOTINTU;
      diffunit=1;
      /*FIXME: check auf doppelte Def. */
    }
#ifdef HAVE_MISRA
    if ( (v) && (ISFUNC(v->vtyp->flags)) ) {
      if (v->vtyp->exact->count == 0) misra_oldstyle_def=1;
    }

    if(misracheck&&extern_flag&&storage_class==EXTERN&&cross_module){
      struct Var *ov=find_ext_var(vident);
      if(ov&&strcmp(ov->filename,filename))
        misra_neu(27,8,8,0,vident);
    }
#endif
    if(v){
      had_decl=1;
      if(storage_class==TYPEDEF){
        error(226,v->identifier);
      }else{
        if(nesting>0&&(v->flags&DEFINED)&&!extern_flag&&!isfunc){
          error(27,vident);
        }else{
          if(t&&v->vtyp&&!compatible_types(v->vtyp,t,NU|CONST|VOLATILE)){
	    if(ISFUNC(t->flags)&&!ISFUNC(v->vtyp->flags))
	      error(361,vident);
	    else
	      error(68,vident);
#ifdef HAVE_MISRA
            misra_neu(26,8,4,0,vident);
#endif
          }
          if((storage_class!=v->storage_class&&!extern_flag)||hard_reg!=v->reg)
            error(28,v->identifier);
          if(!isfunc&&!extern_flag) v->flags|=TENTATIVE;
        }
#ifdef HAVE_TARGET_ATTRIBUTES
        {
          int i;
          for(i=0;g_attr_name[i];i++){
            if((v->tattr&(1L<<i))!=(tattr&(1L<<i))) error(228,vident,g_attr_name[i]);
          }
          v->tattr=tattr;
        }
#endif
        if(vattr){
          if(v->vattr){
            v->vattr=myrealloc(v->vattr,strlen(v->vattr)+strlen(vattr)+2);
            strcat(v->vattr,";");
            strcat(v->vattr,vattr);
          }else v->vattr=vattr;
          if(ISFUNC(v->vtyp->flags)) fi_from_attr(v);
        }
        if(!isfunc){
          if(!ISARRAY(t->flags)||!zmeqto(t->size,l2zm(0L))){
            free(v->vtyp);
            v->vtyp=t;
          }
        }else{
          om=v->vtyp;
          if(t->exact->count>0){
            old=v->vtyp;v->vtyp=t;
          }
        }
      }
    }else{
      had_decl=0;
#ifdef HAVE_MISRA
      if ((isfunc) && (ctok->type!=SEMIC)) misra_neu(71,8,1,0);
#endif
      if(isfunc&&ctok->type!=COMMA&&ctok->type!=SEMIC&&ctok->type!=RPAR&&ctok->type!=ASGN&&nesting>0) nesting--;
      v=add_var(vident,t,storage_class,0);
#ifdef HAVE_ECPP
      if(ecpp&&ISSTRUCT(v->vtyp->flags)&&!(v->vtyp->exact->ecpp_flags&ECPP_POD)){
        np p=new_node();
        p->flags=ADDRESS;
        p->left=new_node();
        p->left->flags=IDENTIFIER;
        p->left->identifier=add_identifier(v->identifier,strlen(v->identifier));
        ecpp_call_ctor(v->vtyp->exact,p,ecpp_ctor_args);
        ecpp_auto_dtor(v);
      }
#endif
      v->reg=hard_reg;
      v->vattr=vattr;
      if(ISFUNC(v->vtyp->flags))
        fi_from_attr(v);
#ifdef HAVE_TARGET_ATTRIBUTES
      v->tattr=tattr;
#endif
      if(isfunc&&ctok->type!=COMMA&&ctok->type!=SEMIC&&ctok->type!=RPAR&&ctok->type!=ASGN&&nesting>=0) nesting++;
      if(!v) ierror(0);
      else{
        if(!isfunc&&!extern_flag){
          v->flags|=TENTATIVE;
          if(nesting>0){
            v->flags|=DEFINED;
            v->dfilename=filename;
            v->dline=line;
          }
        }
      }
      om=0;
    }
    if(isfunc&&inline_flag){
      v->flags|=INLINEFUNC;
      if(extern_flag) v->flags|=INLINEEXT;
    }
#ifdef HAVE_MISRA
    if(isfunc&&misracheck&&t->exact->count>1){
      int i,noid=!*(*t->exact->sl)[0].identifier;
      for(i=1;i<t->exact->count;i++) {
        if(i==t->exact->count-1&&(((*t->exact->sl)[i].styp->flags&NQ))==VOID)
          break;
        if(!(* (*t->exact->sl)[i].identifier)!=noid){
          misra_neu(73,0,0,0);
          break;
        }
      }
    } else if (isfunc&&misracheck) {
      int i;
      for (i=0;i<t->exact->count;i++) {
        if(i==t->exact->count-1&&(((*t->exact->sl)[i].styp->flags&NQ))==VOID)
          break;
        if(!(* (*t->exact->sl)[i].identifier)){
          misra_neu(73,16,3,0);
          break;
        }                
      }
    }
#endif
    if(disallow_statics&&v->storage_class==STATIC&&*v->identifier&&!is_const(v->vtyp))
      error(302,v->identifier);
    killsp();
    /*  Inline-Assembler-Code in Funktionsdeklarationen */
    if(ctok->type==ASGN&&ISFUNC(v->vtyp->flags)&&(header_cnt>0||!(c_flags[7]&USEDFLAG))){
      np tree;
      next_token();killsp();
      if(v->fi){free(v->fi->inline_asm);v->fi->inline_asm=0;}
      if(!v->fi) v->fi=new_fi();
      v->fi->inline_asm=get_string();
      mdef=1;
    }else{
      /*if(v->fi){free(v->fi->inline_asm);v->fi->inline_asm=0;}*/
    }
    /*  Initialisierung von Variablen bei Deklaration   */
    if(ctok->type==ASGN){

      next_token();killsp();
      if(!had_decl&&v->nesting==0&&v->storage_class==EXTERN&&strcmp("main",v->identifier))
        error(168,v->identifier);
      if(v->flags&DEFINED){
        if(nesting==0) error(30,v->identifier);
      }else{
        v->flags|=DEFINED;
        v->dfilename=filename;
        v->dline=line;
#ifdef HAVE_MISRA
        {
          char* teststr = v->dfilename;
          teststr+=(strlen(teststr)-2);
          if (strcmp(teststr,".c")) misra_neu(87,8,5,0);
        }
#endif
      }
      if(v->storage_class==TYPEDEF) error(114,v->identifier);
      if(extern_flag){
        if(nesting==0)
          error(118,v->identifier);
        else
          error(207,v->identifier);
        if(v->storage_class!=EXTERN){ error(77);v->storage_class=EXTERN;}
      }
#ifdef HAVE_MISRA
      misra_92_violation = 0;                        /* No MISRA Rule 9.2 violation so far*/
      misra_92_violation_line = 0;                /* Store line information for error reporting */
#endif
      init_dyn_sz=l2zm(0L);
      init_dyn_cnt=0;
      init_const_sz=l2zm(0L);
      init_const_cnt=0;
      v->clist=initialization(v->vtyp,v->storage_class==AUTO||v->storage_class==REGISTER,0,0,0,0);
                /* MISRA Rule 9.2 violation checking and error reporting */
#ifdef HAVE_MISRA
      if ((misracheck) && (misra_92_violation)){
        if (v->clist) {
          misra_92_violation = !(check_zero_initialisation(v->clist,get_first_base_type(v->vtyp)));
        }
        if (misra_92_violation) misra_neu(31,9,2,misra_92_violation_line);
      }
#endif
      if(v->clist){
        if(ISARRAY(v->vtyp->flags)&&zmeqto(v->vtyp->size,l2zm(0L))){
          struct const_list *p=v->clist;
          while(p){v->vtyp->size=zmadd(p->idx,l2zm(1L));p=p->next;}
          if(v->storage_class==AUTO||v->storage_class==REGISTER){
            local_offset[nesting]=zmadd(local_offset[nesting],szof(v->vtyp));
            if(zmleq(max_offset,local_offset[nesting])) max_offset=local_offset[nesting];
          }
        }
        if(v->storage_class==AUTO||v->storage_class==REGISTER){

	  init_local_compound(v);

        }else if(c_flags[19]&USEDFLAG){
          /*  Ohne Optimierung gleich erzeugen; das ist noch  */
          /*  etwas von der genauen Implementierung der Liste */
          /*  der Variablen abhaengig.                        */
          struct Var *merk=v->next;
          v->next=0;
          gen_vars(v);
          v->next=merk;
          v->clist=0;
        }
      }
    }else{
      if((v->flags&DEFINED)&&type_uncomplete(v->vtyp)) error(202,v->identifier);
      if((v->vtyp->flags&CONST)&&(v->storage_class==AUTO||v->storage_class==REGISTER))
        error(119,v->identifier);
    }
    if(ctok->type==COMMA) {next_token();killsp();mdef=1;} else notdone=0;
  }
  freetyp(ts);

  if(ISFUNC(t->flags)&&v->reg)
    t->next->reg=v->reg;

  if(!mdef&&t&&(t->flags&NQ)==FUNKT&&ctok->type!=SEMIC){
    /*  Funktionsdefinition*/
    int i,oldstyle=0;

#ifdef HAVE_MISRA
    if (misra_oldstyle_def) misra_neu(71,8,1,0);
    misra_oldstyle_def = 0;
#endif
#ifdef HAVE_REGPARMS
    struct reg_handle reg_handle;
#endif
    if(DEBUG&1) printf("Funktionsdefinition! %s %p\n",v->identifier,(void *)v);
    {
      int i;
      for(i=1;i<=MAXR;i++) {regs[i]=regused[i]=regsa[i];regsbuf[i]=0;}
    }
    cur_func=v->identifier;
    if(only_inline==2) only_inline=0;
    if(nesting<1) enter_block();
    if(nesting>1) error(32);
    if(v->flags&DEFINED){
      if(!inline_flag||!cross_module){
                                error(33,v->identifier);
      }else{
        if(v->fi->first_ic){
          free_IC(v->fi->first_ic);
          v->fi->first_ic=0;
        }
      }
    }else{
      v->flags|=DEFINED;
      v->dfilename=filename;
      v->dline=line;
#ifdef HAVE_MISRA
      {
        char* teststr = v->dfilename;
        teststr+=(strlen(teststr)-2);
        if (strcmp(teststr,".c")) misra_neu(87,8,5,0);
      }
#endif
    }
    if(storage_class!=EXTERN&&storage_class!=STATIC) error(34);
    if(extern_flag&&!inline_flag) error(120);
    if(storage_class==EXTERN&&!strcmp(v->identifier,"main")&&(!t->next||t->next->flags!=INT)) error(121);
    if(!had_decl&&v->nesting==0&&v->storage_class==EXTERN&&strcmp("main",v->identifier))
      error(168,v->identifier);
    while(!ecpp&&ctok->type!=LBRA){
      /*  alter Stil  */
      struct Typ *nt=declaration_specifiers();notdone=1;oldstyle=OLDSTYLE;
      if(!ts) {error(35);}
      while(notdone){
        int found=0;
        imerk=ident;ident=vident;*vident=0;
        ts=declarator(clone_typ(nt));
        ident=imerk;
        if(!ts) {error(36);}
        else{
          for(i=0;i<t->exact->count;i++){
            if(!strcmp((*t->exact->sl)[i].identifier,vident)){
              found=1;
              if((*t->exact->sl)[i].styp){
                error(69,vident);
                freetyp((*t->exact->sl)[i].styp);
              }
              /*  typ[] in *typ   */
              if(ISARRAY(ts->flags)) ts->flags=POINTER_TYPE(ts);
              /*  typ() in *typ() */
              if(ISFUNC(ts->flags)){
                struct Typ *new=new_typ();
                new->flags=POINTER_TYPE(ts);
                new->next=ts;
                ts=new;
              }
              if(!return_sc) return_sc=AUTO;
              if(return_sc!=AUTO&&return_sc!=REGISTER)
                {error(122);return_sc=AUTO;}
              (*t->exact->sl)[i].storage_class=return_sc;
              (*t->exact->sl)[i].reg=return_reg;
              if(return_reg) error(219);
              (*t->exact->sl)[i].styp=ts;
            }
          }
        }
        if(!found) {error(37,vident);}
        killsp();
        if(ctok->type==COMMA) {next_token();killsp();} else notdone=0;
      }
      if(nt) freetyp(nt);
      if(ctok->type==SEMIC){
        next_token();killsp();
      }else{
        error(54);
        while(ctok->type!=LBRA&&ctok->type!=SEMIC){next_token();killsp();}
      }
    }
    if(!ecpp&&t->exact->count==0){
      struct struct_list sl[1];
      if(DEBUG&1) printf("prototype converted to (void)\n");
      t->exact->count=1;
      sl[0].identifier=empty;
      sl[0].storage_class=AUTO;
      sl[0].styp=new_typ();
      sl[0].styp->flags=VOID;
      sl[0].styp->next=0;
      nesting--;
      add_sl(t->exact,&sl);
      nesting++;
    }
    if(om&&om->exact&&!compare_sd(om->exact,t->exact)) {
      error(123);
#ifdef HAVE_MISRA
      misra_neu(72,8,3,0);
#endif
    }
    nocode=0;currentpri=1;
    /*        enter_block();*/
    local_offset[1]=l2zm(0L);
    return_var=0;
    if(!v->vtyp) ierror(0);
#ifdef HAVE_REGPARMS
    reg_handle=empty_reg_handle;
#endif
    if(v->vtyp->next->flags==VOID){
      return_typ=0;
    }else{
      return_typ=v->vtyp->next;
      if(!ffreturn(return_typ)&&(return_typ->flags&NQ)!=VOID){
        /*  Parameter fuer die Rueckgabe von Werten, die nicht in einem */
        /*  Register sind.                                              */
        struct Typ *rt=new_typ();int reg;
        rt->flags=POINTER_TYPE(return_typ);rt->next=return_typ;
#ifdef HAVE_REGPARMS
        reg=reg_parm(&reg_handle,rt,0,v->vtyp);
        if(!reg){
	  return_var=add_var(empty,clone_typ(rt),AUTO|PARAMETER|oldstyle,0);
	}else{
	  return_var=add_var(empty,clone_typ(rt),reg<0?(AUTO|PARAMETER|REGPARM|DBLPUSH|oldstyle):(AUTO|PARAMETER|REGPARM|oldstyle),0);
	  return_var->reg=reg;
	}
#else
        return_var=add_var(empty,clone_typ(rt),AUTO|PARAMETER|oldstyle,0);
#endif
        return_var->flags|=DEFINED;
        return_var->dfilename=filename;
        return_var->dline=line;
        free(rt);
      }
    }
    first_ic=last_ic=0;ic_count=0;max_offset=l2zm(0L);
    for(i=0;i<t->exact->count;i++){
#ifdef HAVE_REGPARMS
      int didrp=0;
      if((*t->exact->sl)[i].styp){
        int tr;
        tr=reg_parm(&reg_handle,(*t->exact->sl)[i].styp,0,t);
        didrp=1;
        if(!(*t->exact->sl)[i].reg) (*t->exact->sl)[i].reg=tr;
      }
#endif
      if(!(*t->exact->sl)[i].styp&&*(*t->exact->sl)[i].identifier){
        struct Typ *nt;
#ifdef HAVE_MISRA
        misra_neu(71,8,1,0);
#endif
        nt=new_typ();
        nt->flags=INT;
        (*t->exact->sl)[i].styp=nt;
        (*t->exact->sl)[i].storage_class=AUTO;
        (*t->exact->sl)[i].reg=0;
        error(124);
      }
      if(*(*t->exact->sl)[i].identifier){
        struct Var *tmp;int sc,tr;
        sc=((*t->exact->sl)[i].storage_class|PARAMETER|oldstyle);
#ifdef HAVE_REGPARMS
        if(!didrp){
          if(!t->exact->sl) ierror(0);
          if(!(*t->exact->sl)[i].styp) ierror(0);
          tr=reg_parm(&reg_handle,(*t->exact->sl)[i].styp,0,t);
          if(!(*t->exact->sl)[i].reg) (*t->exact->sl)[i].reg=tr;
        }
#endif
        if((*t->exact->sl)[i].reg>0) sc|=REGPARM;
        if((*t->exact->sl)[i].reg<0) sc|=(REGPARM|DBLPUSH);
        tmp=add_var((*t->exact->sl)[i].identifier,clone_typ((*t->exact->sl)[i].styp),sc,0);
        tmp->reg=(*t->exact->sl)[i].reg;
        tmp->flags|=DEFINED;
        tmp->dfilename=filename;
        tmp->dline=line;
        if(oldstyle){
#ifdef HAVE_MISRA
          misra_neu(71,8,1,0);
#endif
          freetyp((*t->exact->sl)[i].styp);
          (*t->exact->sl)[i].styp=0; /*  Prototype entfernen */
        }
      }
    }
    if(oldstyle) t->exact->count=0; /*  Prototype entfernen */
    /*        local_offset[1]=l2zm(0L);*/
    return_label=++label;
    function_calls=0;float_used=0;has_return=0;goto_used=0;vlas=0;
    did_return_label=0;
    for(i=1;i<=MAXR;i++) simple_scratch[i]=0;
    if(v->storage_class==EXTERN&&(v->flags&INLINEFUNC))
      disallow_statics=1;
#ifdef HAVE_ECPP
    current_class=0;
    if(ecpp) current_func=v->vtyp;
    if(ecpp&&ISFUNC(v->vtyp->flags)&&v->vtyp->exact->ecpp_flags&ECPP_CTOR){
      ecpp_ctor_init_list(v->vtyp->exact,0);
    }
    if(ecpp&&current_func->ecpp_flags&ECPP_DTOR){
      ecpp_dtor_prolog();
    }
#endif
#ifdef HAVE_MISRA
    misra_return_count = 0;
    misra_statement_after_return = 0;
#endif

    if(c99){
      /* c99 predefined __func__ */
      struct Typ *ft=new_typ();
      struct Var *fnc;

      /* create type */
      ft->flags=ARRAY;
      ft->size=l2zm((long)strlen(cur_func)+1);
      ft->next=new_typ();
      ft->next->flags=CONST|CHAR;

      /* use string_expression() to create const_list */
      fnc=add_var("__func__",ft,STATIC,cl_from_string(cur_func,cur_func+strlen(cur_func)));
      fnc->flags|=DEFINED;
    }

    /* Generate intermediate code for function */
    compound_statement();

#ifdef HAVE_MISRA
    if ((misra_statement_after_return) || (misra_return_count>1)) misra_neu(82,14,7,0); 
#endif
#ifdef HAVE_ECPP
    if(ecpp){
      ecpp_auto_call_dtors();
      if(current_func->ecpp_flags&ECPP_DTOR){
        ecpp_dtor_epilog();
      }
      current_func=0;
    }
#endif
    disallow_statics=0;
    if(block_vla[nesting]) clearvl();
    if((v->vtyp->next->flags&NQ)!=VOID&&!has_return){
      if(strcmp(v->identifier,"main")) error(173,v->identifier);
      else error(174,v->identifier);
    }
#if 0
    {int i;
    for(i=1;i<=MAXR;i++) if(regs[i]!=regsa[i]) {printf("Register %s:\n",regnames[i]);ierror(0);}
    }
#endif
    if(!did_return_label){
      clear_main_ret();
      gen_label(return_label);
    }
    /* backpatch code for jumps out of vla-scope if necessary */
    if(vlas)
      vla_jump_fix();
    if(cross_module){
      if(!v->fi) v->fi=new_fi();
      v->fi->statics=first_var[0];
    }else if(first_ic&&errors==0){
      if((c_flags[2]&USEDFLAG)&&ic1){fprintf(ic1,"function %s\n",v->identifier); pric(ic1,first_ic);}
      vl1=first_var[0];
      vl2=first_var[1];
      vl3=merk_varf;
      optimize(optflags,v);
      if((c_flags[3]&USEDFLAG)&&ic2){fprintf(ic2,"function %s\n",v->identifier); pric(ic2,first_ic);}
      if(out&&!only_inline&&!(c_flags[5]&USEDFLAG)){
        memset(regs_modified,0,RSIZE);
        gen_code(out,first_ic,v,max_offset);
        static_stack_check(v);
        v->flags|=GENERATED;
#ifdef HAVE_REGS_MODIFIED
        if(!v->fi) v->fi=new_fi();
        if(v->fi->flags&ALL_REGS)
        {
          int i;
          for(i=1;i<=MAXR;i++){
            if(reg_pair(i,&rp)){
              if(BTST(regs_modified,i)){
                BSET(regs_modified,rp.r1);
                BSET(regs_modified,rp.r2);
              }else{
                if(BTST(regs_modified,rp.r1)||BTST(regs_modified,rp.r2))
                  BSET(regs_modified,i);
              }
            }
          }
          memcpy(v->fi->regs_modified,regs_modified,RSIZE);
          v->fi->flags|=ALL_REGS;
        }
#endif
      }
      /*if(DEBUG&8192){fprintf(ic2,"function %s, after gen_code\n",v->identifier); pric(ic2,first_ic);}*/
      free_IC(first_ic);
      first_ic=last_ic=0;
    }
    if(cross_module){
      if(!v->fi) v->fi=new_fi();
      v->fi->first_ic=first_ic;
      v->fi->last_ic=last_ic;
      first_ic=last_ic=0;
    }
    if(v->fi&&v->fi->first_ic){
      struct Var *vp;
      if(DEBUG&1) printf("leave block %d (inline-version)\n",nesting);
      if(block_vla[nesting]) clearvl();
      if(nesting!=1) ierror(0);
      if(merk_varl) merk_varl->next=first_var[nesting]; else merk_varf=first_var[nesting];
      if(last_var[nesting]) merk_varl=last_var[nesting];
      if(merk_sil) merk_sil->next=first_si[nesting]; else merk_sif=first_si[nesting];
      if(last_si[nesting]) merk_sil=last_si[nesting];
      if(merk_sdl) merk_sdl->next=first_sd[nesting]; else merk_sdf=first_sd[nesting];
      if(last_sd[nesting]) merk_sdl=last_sd[nesting];
      if(merk_ilistl) merk_ilistl->next=first_ilist[nesting]; else merk_ilistf=first_ilist[nesting];
      if(last_ilist[nesting]) merk_ilistl=last_ilist[nesting];
      
      if(merk_varf&&!only_inline&&!cross_module) gen_vars(merk_varf);
      if(first_llist) free_llist(first_llist);
      first_llist=0;
      if(first_clist) free_clist(first_clist);
      first_clist=0;
      if(merk_sif) free_si(merk_sif);
      /*  struct-declarations erst ganz am Schluss loeschen. Um zu vermeiden,     */
      /*  dass struct-declarations in Prototypen frei werden und dann eine        */
      /*  spaetere struct, dieselbe Adresse bekommt und dadurch gleich wird.      */
      /*  Nicht sehr schoen - wenn moeglich noch mal aendern.                     */
      /*            if(merk_sdf) free_sd(merk_sdf);*/
      /*  hier noch was ueberlegen    */
      /*            if(merk_ilistf) free_ilist(merk_ilistf);*/
      nesting--;
      v->fi->vars=merk_varf;
      /*            v->fi->vars=first_var[1];*/
      /*  keine echten Parameter=>keine negativen Offsets */
      /*            vp=first_var[1];*/
      if(!cross_module){
        vp=merk_varf;
        while(vp){
          if(vp->storage_class==AUTO||vp->storage_class==REGISTER){
            /*if(DEBUG&1024) printf("handling variable %s(%ld)/%p\n",vp->identifier,zm2l(vp->offset),(void*)vp);*/
            if(!zmleq(l2zm(0L),vp->offset)){
              vp->offset=l2zm(0L);
              if(DEBUG&1024) printf("converted parameter <%s>(%ld) for inlining\n",vp->identifier,(long)zm2l(vp->offset));
            }else vp->offset=l2zm(4L);  /*  Dummy, da recalc_offsets?   */
          }
          vp=vp->next;
        }
      }
    }else{
      leave_block();
    }
    if(only_inline==2) only_inline=0;
    cur_func="oops, I forgot it";
  }else{
    if(makeint) error(125);
    if(ctok->type==SEMIC) next_token(); else error(54);
    if(ISFUNC(t->flags)&&t->exact){
      struct struct_declaration *sd=t->exact;int i,f;
      for(f=0,i=0;i<sd->count;i++)
        if(!(*sd->sl)[i].styp){error(126);f=1;}
      if(f){
        for(i=0;i<sd->count;i++) if((*sd->sl)[i].styp) freetyp((*sd->sl)[i].styp);
        sd->count=0;
      }
    }
  }
  if(old) freetyp(old);
}
int compatible_types(struct Typ *a,struct Typ *b,int qual)
/*  Vergleicht, ob Typ beider Typen gleich ist, const/volatile      */
/*  werden laut ANSI nicht beruecksichtigt.                         */
{
  struct struct_declaration *sd;
  int af=a->flags&qual,bf=b->flags&qual;
  if(af!=bf) return(0);
  af&=NQ;bf&=NQ;
  if(ISFUNC(af)){
    if(a->exact->count&&!compare_sd(a->exact,b->exact)) return(0);
  }
  if(ISSTRUCT(af)||ISUNION(af)){
    if(cross_module&&a->exact->tunit!=b->exact->tunit) return 1;
    if(a->exact!=b->exact) return(0);
  }
  if(ISARRAY(af)){
    if(!zmeqto(a->size,l2zm(0L))&&!zmeqto(b->size,l2zm(0L))&&!zmeqto(a->size,b->size)) return(0);
  }
  if(a->next==0&&b->next!=0) return(0);
  if(a->next!=0&&b->next==0) return(0);
  if(a->next==0&&b->next==0) return(1);
  if(qual!=NQ) qual=(NU|CONST|VOLATILE);
  return(compatible_types(a->next,b->next,qual));
}
int compare_sd(struct struct_declaration *a,struct struct_declaration *b)
/*  Vergleicht, ob zwei struct_declarations identisch sind          */
/*  Wird nur nur fuer Prototypen benutzt, leere Liste immer gleich. */
{
  int i;
  if(!a->count||!b->count) return(1);
  if(a->count!=b->count) return(0);
  for(i=0;i<a->count;i++){
    if((*a->sl)[i].styp&&(*b->sl)[i].styp&&!compatible_types((*a->sl)[i].styp,(*b->sl)[i].styp,NU)) return(0);
#ifdef HAVE_MISRA
    if(misracheck&&*(*a->sl)[i].identifier&&*(*b->sl)[i].identifier&&strcmp((*a->sl)[i].identifier,(*b->sl)[i].identifier))
      misra_neu(74,16,4,0);
#endif
  }
  return(1);
}
void free_clist(struct const_list *p)
/*  Gibt clist frei.                                        */
{
  struct const_list *merk;
  return;
  while(p){
    merk=p->next;
    if(p->other) free_clist(p->other);
    if(p->tree) free_expression(p->tree);
    free(p);
    p=merk;
  }
}
void gen_clist(FILE *,struct Typ *,struct const_list *);

void gen_vars(struct Var *v)
/*  Generiert Variablen.                                    */
{
  int mode,al;struct Var *p;
  if(errors!=0||(c_flags[5]&USEDFLAG)) return;
  if(optsize)
    al=zm2l(maxalign);
  else
    al=0;
  for(;al>=0;al--){
    for(mode=0;mode<3;mode++){
      int i,flag;
      for(p=v;p;p=p->next){
        if(optsize&&zm2l(falign(p->vtyp))!=al)
          continue;
        if(cross_module&&!(p->flags&REFERENCED)) continue;
        if(DEBUG&2) printf("gen_var(): %s\n",p->identifier);
        if(p->storage_class==STATIC||p->storage_class==EXTERN){
          if(!(p->flags&GENERATED)){
            if(p->storage_class==EXTERN&&!(p->flags&(USEDASSOURCE|USEDASDEST))&&!(p->flags&(TENTATIVE|DEFINED))) continue;
	if(p->storage_class==STATIC&&p->nesting>0&&!(p->flags&(USEDASSOURCE|USEDASDEST))) continue;
            /*  erst konstante initialisierte Daten */
            if(mode==0){
              if(!p->clist) continue;
              if(!(p->vtyp->flags&(CONST|STRINGCONST))){
                struct Typ *t=p->vtyp;int f=0;
                do{
                  if(t->flags&(CONST|STRINGCONST)) break;
                  if(!ISARRAY(t->flags)){f=1;break;}
                  t=t->next;
                }while(1);
                if(f) continue;
              }
            }
            /*  dann initiolisierte */
            if(mode==1&&!p->clist) continue;
            /*  und dann der Rest   */
            if(mode==2&&p->clist) continue;
            if(!(p->flags&(TENTATIVE|DEFINED))){
              if(!((p->vtyp->flags&NQ)==FUNKT)||!p->fi||!p->fi->inline_asm)
                gen_var_head(out,p);
              if(p->storage_class==STATIC&&(!p->fi||!p->fi->inline_asm)) error(127,p->identifier);
              continue;
            }else{
              /*gen_align(out,falign(p->vtyp));*/
            }
            if(!(p->vtyp->flags&NQ)==FUNKT||!p->fi||!p->fi->inline_asm)
              gen_var_head(out,p);
            if(!p->clist){
              if(type_uncomplete(p->vtyp)) error(202,p->identifier);
              gen_ds(out,szof(p->vtyp),p->vtyp);
            }else{
              gen_clist(out,p->vtyp,p->clist);
            }
            p->flags|=GENERATED;
          }else if(p->flags&INLINEEXT){
            /* a function was declared extern inline and defined;
               we have to create external linkage */
            int m=p->flags;
            /* pretend, it was only declared, but not defined and remove
               INLINEEXT */
            p->flags&=~(DEFINED|INLINEEXT);
            gen_var_head(out,p);
            p->flags|=DEFINED;
          }
        }
      }
    }
  }
}

/* creates code that dynamically initializes a variable */
void dynamic_init(struct Var *v,struct Typ *t,struct const_list *cl,zmax of,int noconst)
{
  int f=t->flags;
  if(ISARRAY(f)){
    zmax i;
    for(i=l2zm(0L);!zmleq(t->size,i);i=zmadd(i,l2zm(1L))){
      if(cl&&zmeqto(cl->idx,i)){
	dynamic_init(v,t->next,cl->other,of,noconst);
	cl=cl->next;
      }else{
	dynamic_init(v,t->next,0,of,noconst);
      }
      of=zmadd(of,szof(t->next));
    }
  }else if(ISUNION(f)&&(!cl||!cl->tree)){
    dynamic_init(v,(*t->exact->sl)[cl?zm2l(cl->idx):0].styp,cl?cl->other:0,of,noconst);
  }else if(ISSTRUCT(f)&&(!cl||!cl->tree)){
    zmax al;int fl;struct Typ *st;
    int bfo,i;
    for(i=0;i<t->exact->count&&cl;i++){
      if(!cl->other){ierror(0);return;}
      st=(*t->exact->sl)[i].styp;
      al=(*t->exact->sl)[i].align;
      if(!(*t->exact->sl)[i].identifier) ierror(0);
      bfo=(*t->exact->sl)[i].bfoffset;
      if(!zmeqto(zmmod(of,al),l2zm(0L))){
        of=zmadd(of,zmsub(al,zmmod(of,al)));
      }
      if(bfo>=0){
	int bfs=(*t->exact->sl)[i].bfsize;
	static struct obj dest = {0};
	  
	dest.flags=VAR;
	dest.v=v;
	dest.val.vmax=of;

	if(bfo==0){
	  /* first clear entire bitfield */
	  struct IC *new=new_IC();
	  new->code=ASSIGN;
	  new->z=dest;
	  new->typf=st->flags;
	  new->q2.val.vmax=sizetab[st->flags&NQ];
	  new->q1.flags=KONST;
	  new->q1.val.vmax=l2zm(0L);
	  eval_const(&new->q1.val,MAXINT);
	  insert_const(&new->q1.val,st->flags&NU);
	  add_IC(new);
	}

	/* bitfield */
	if(!zmeqto(cl->idx,l2zm(i))||!cl->other){
	  /* nothing to do, initialized before */
	}else if(cl->other->tree){
	  struct obj dummy;
	  gen_IC(cl->other->tree,0,0);
	  convert(cl->other->tree,st->flags);
	  insert_bitfield(&dest,&cl->other->tree->o,&cl->other->tree->o,bfs,bfo,st->flags,1);
	  cl=cl->next;
	}else{
	  static struct obj val = {0};
	  val.flags=KONST;
	  val.val=cl->other->val;
	  insert_bitfield(&dest,&val,&val,bfs,bfo,st->flags,1);
	  cl=cl->next;
	}
	if(i+1>=t->exact->count||(*t->exact->sl)[i+1].bfoffset<=0||!cl){
	  of=zmadd(of,szof(st));
	}
      }else{
	if(zmeqto(l2zm((long)i),cl->idx)){
	  dynamic_init(v,st,cl->other,of,noconst);
	  cl=cl->next;
	}else
	  dynamic_init(v,st,0,of,noconst);
        of=zmadd(of,szof(st));
      }
    }
    for(;i<t->exact->count;i++){
      st=(*t->exact->sl)[i].styp;
      al=(*t->exact->sl)[i].align;
      bfo=(*t->exact->sl)[i].bfoffset;
      if(bfo>0) continue;
      if(!zmeqto(zmmod(of,al),l2zm(0L))){
        of=zmadd(of,zmsub(al,zmmod(of,al)));
      }
      dynamic_init(v,st,0,of,noconst);
      of=zmadd(of,szof(st));
    }
  }else{
    struct IC *new;
    if(noconst&&(!cl||!cl->tree))
      return;
    new=new_IC();
    new->code=ASSIGN;
    new->z.flags=VAR;
    new->z.v=v;
    new->z.val.vmax=of;
    new->typf=t->flags;
    new->q2.val.vmax=szof(t);
    if(!cl){
      new->q1.flags=KONST;
      gval.vmax=l2zm(0L);
      eval_const(&gval,MAXINT);
      insert_const(&new->q1.val,t->flags&NU);
    }else if(cl->tree){
      gen_IC(cl->tree,0,0);
      if(ISSCALAR(t->flags))
	convert(cl->tree,t->flags);
      new->q1=cl->tree->o;
    }else{
      new->q1.flags=KONST;
      new->q1.val=cl->val;
    }
    add_IC(new);
  }
}


void gen_clist(FILE *f,struct Typ *t,struct const_list *cl)
/*  Generiert dc fuer const_list.                           */
{
  int i,bfo,bfs;zmax sz;zumax bfval=ul2zum(0UL);
  if(ISARRAY(t->flags)){
    for(sz=l2zm(0L);!zmleq(t->size,sz)&&cl;cl=cl->next){
      if(!cl->other){ierror(0);return;}
      if(!zmeqto(sz,cl->idx))
	gen_ds(f,zmmult(zmsub(cl->idx,sz),szof(t->next)),t->next);

      gen_clist(f,t->next,cl->other);
      sz=zmadd(cl->idx,l2zm(1L));
    }
    if(!zmleq(t->size,sz)) gen_ds(f,zmmult(zmsub(t->size,sz),szof(t->next)),t->next);
    return;
  }
  if(ISUNION(t->flags)){
    int i=zm2l(cl->idx);
    if(cl&&cl->tree){
      /* union initialized by another union */
      gen_ds(f,szof(t),t);
    }else{
      gen_clist(f,(*t->exact->sl)[i].styp,cl->other);
      sz=zmsub(szof(t),szof((*t->exact->sl)[i].styp));
      if(!zmeqto(sz,l2zm(0L))) gen_ds(f,sz,0);
    }
    return;
  }
  if(ISSTRUCT(t->flags)){
    zmax al;int fl;struct Typ *st;
    sz=l2zm(0L);
    if(cl&&cl->tree){
      /* struct initialized by another struct */
      gen_ds(f,szof(t),t);
      sz=zmadd(sz,szof(t));
    }else{
      for(i=0;i<t->exact->count&&cl;i++){
	if(!cl->other){ierror(0);return;}
	st=(*t->exact->sl)[i].styp;
	al=(*t->exact->sl)[i].align;
	if(!(*t->exact->sl)[i].identifier) ierror(0);
	bfo=(*t->exact->sl)[i].bfoffset;
	if(!zmeqto(zmmod(sz,al),l2zm(0L))){
	  gen_ds(f,zmsub(al,zmmod(sz,al)),0);
	  sz=zmadd(sz,zmsub(al,zmmod(sz,al)));
	}
	if(bfo>=0){
	  /* bitfield */
	  if((*t->exact->sl)[i].identifier[0]){
	    bfs=(*t->exact->sl)[i].bfsize;
	    if(zmeqto(l2zm((long)i),cl->idx)){
	      eval_const(&cl->other->val,st->flags);
	      cl=cl->next;
	    }else{
	      vumax=ul2zum(0UL);
	    }
	    vumax=zumand(vumax,zumsub(zumlshift(ul2zum(1UL),ul2zum((unsigned long)bfs)),ul2zum(1UL)));
	    bfval=zumor(bfval,zumlshift(vumax,ul2zum((unsigned long)bflayout(bfo,bfs,st->flags))));
	  }          
	  if(i+1>=t->exact->count||(*t->exact->sl)[i+1].bfoffset<=0||!cl){
	    /* last bitfield in integer */
	    struct const_list bfcl;
	    gval.vumax=bfval;
	    eval_const(&gval,UNSIGNED|MAXINT);
	    insert_const(&bfcl.val,st->flags&NU);
	    bfcl.tree=0;
	    gen_dc(f,st->flags&NU,&bfcl);
	    bfval=ul2zum(0L);
	    sz=zmadd(sz,szof(st));
	  }
	}else{
	  if(zmeqto(l2zm((long)i),cl->idx)){
	    gen_clist(f,st,cl->other);
	    cl=cl->next;
	  }else
	    gen_ds(f,szof(st),st);
	  sz=zmadd(sz,szof(st));
	}
      }
      for(;i<t->exact->count;i++){
	st=(*t->exact->sl)[i].styp;
	al=(*t->exact->sl)[i].align;
	bfo=(*t->exact->sl)[i].bfoffset;
	if(bfo>0) continue;
	if(!zmeqto(zmmod(sz,al),l2zm(0L))){
	  gen_ds(f,zmsub(al,zmmod(sz,al)),0);
	  sz=zmadd(sz,zmsub(al,zmmod(sz,al)));
	}
	gen_ds(f,szof((*t->exact->sl)[i].styp),(*t->exact->sl)[i].styp);
	sz=zmadd(sz,szof(st));
      }
    }
    al=falign(t);
    if(!zmeqto(zmmod(sz,al),l2zm(0L)))
      gen_ds(f,zmsub(al,zmmod(sz,al)),0);
    return;
  }

  if(cl->tree) cl->tree->o.am=0;

  if(zmeqto(cl->idx,l2zm(-1L)))
    gen_ds(f,szof(t),t);
  else
    gen_dc(f,t->flags&NU,cl);
}

/* finds the correct place in a const list to insert new initializer */
struct const_list *insert_cl(struct const_list *old,zmax idx)
{
  struct const_list *p,*cl=0;
  if(old&&zmleq(old->idx,idx)){
    p=old;
    do{
      if(zmeqto(p->idx,idx)){
	/* found existing entry */
	cl=p;
	break;
      }
      if(!p->next||!zmleq(p->next->idx,idx))
	break;
      p=p->next;
    }while(p);
  }else
    p=0;
  /* we need a new entry */
  if(!cl){
    cl=mymalloc(CLS);
    cl->tree=0;
    cl->other=0;
    cl->idx=idx;
    if(p){
      cl->next=p->next;
      p->next=cl;
    }else
      cl->next=old;
  }
  return cl;
}

struct const_list *designator(struct Typ *t,struct const_list *cl)
{
  int f=t->flags&NQ;
  np tree;
  if(!c99) return 0;
  if(ISARRAY(f)&&ctok->type==LBRK){
    next_token();
    killsp();
    tree=expression();
    if(ctok->type!=RBRK)
      error(62);
    else
      {next_token();killsp();}
    if(!type_expression(tree)){
      /*                    error("incorrect constant expression");*/
    }else{
      if(tree->sidefx) error(60);
      if(tree->flags!=CEXPR||!ISINT(tree->ntyp->flags)){
	error(19);
      }else{
	/* check index for valid range */
	eval_constn(tree);
	if(!zmleq(l2zm(0L),vmax)) {error(354);vmax=l2zm(0L);}
	if(!zmeqto(t->size,l2zm(0L))&&zmleq(t->size,vmax)) {error(354);vmax=l2zm(0L);}

	cl=insert_cl(cl,vmax);

	return cl;
      }
    }
  }else if((ISSTRUCT(f)||ISUNION(f))&&ctok->type==DOT){
    next_token();
    killsp();
    if(ctok->type!=NAME){
      error(53);
    }else{
      int i,n=-1;
      for(i=0;i<t->exact->count;i++)
	if(!strcmp((*t->exact->sl)[i].identifier,ctok->name)) n=i;
      if(n<0){
	error(23,ctok->name);
	return 0;
      }
      next_token();
      killsp();

      if(!ISUNION(f)||!cl)
	cl=insert_cl(cl,l2zm((long)n));

      return cl;
    }
  }
  return 0;
}

/* declare a builtin function with up to two scalar arguments */
struct Var *declare_builtin(char *name,int ztyp,int q1typ,int q1reg,int q2typ,int q2reg,int nosidefx,char *asm)
{
  struct struct_declaration *sd;
  struct Typ *t;
  struct Var *v;
  int args;
  if(!(v=find_ext_var(name))){
    sd=mymalloc(sizeof(*sd));
    if(q1typ==0) args=1;
    else if(q2typ!=0) args=3;
    else args=2;
    sd->sl=mymalloc(args*sizeof(struct struct_list));
    memset(sd->sl,0,args*sizeof(struct struct_list));
    sd->count=args;
    if(q1typ){
      (*sd->sl)[0].styp=new_typ();
      (*sd->sl)[0].styp->flags=q1typ;
      (*sd->sl)[0].reg=q1reg;
    }
    if(q2typ){
      (*sd->sl)[1].styp=new_typ();
      (*sd->sl)[1].styp->flags=q2typ;
      (*sd->sl)[1].reg=q2reg;
    }
    (*sd->sl)[args-1].styp=new_typ();
    (*sd->sl)[args-1].styp->flags=VOID;
    t=new_typ();
    t->flags=FUNKT;
    t->exact=add_sd(sd,FUNKT);
    t->next=new_typ();
    t->next->flags=ztyp;
    v=add_var(name,t,EXTERN,0);
    v->flags|=BUILTIN;
    if(asm||nosidefx){
      v->fi=new_fi();
      if(asm) v->fi->inline_asm=asm;
      if(nosidefx){
	v->fi->call_cnt=v->fi->use_cnt=v->fi->change_cnt=0;
	v->fi->flags=ALL_CALLS|ALL_USES|ALL_MODS|ALWAYS_RETURNS|NOSIDEFX;
      }
    }
  }
  return v;
}


struct const_list *initialization(struct Typ *t,int noconst,int level,int desi,struct struct_declaration *fstruct,struct const_list *first)
/*  Traegt eine Initialisierung in eine const_list ein.         */
{
  struct const_list *cl,**prev;
  np tree,tree2;
  int bracket,desi_follows=0;
  zmax i;
  struct token mtok;
  
  int f=t->flags&NQ;
  if(ISFUNC(f)){error(42);return(0);}
  if(ctok->type==LBRA){next_token();killsp();bracket=1;} else bracket=0;
  if(ISARRAY(f)){
#ifdef HAVE_MISRA
    if(!bracket) { misra_92_violation = 1; if (!misra_92_violation_line) misra_92_violation_line = ctok->line; } /* possible MISRA Rule 9.2 violation */
#endif
    if(t->dsize){
      error(358);
    }else if(zmeqto(t->size,l2zm(0L))&&level!=0){
      error(359);
    }else if(ctok->type==T_STRING&&t->next&&(t->next->flags&NQ)==CHAR){
      killsp();
      tree=string_expression();
      first=tree->cl;
      free_expression(tree);
    }else{
      struct const_list *last=first;
      if(level==0&&!bracket) error(157);
      for(i=l2zm(0L);desi_follows||((zmeqto(t->size,l2zm(0L))||!zmleq(t->size,i)||ctok->type==LBRK)&&ctok->type!=RBRA);i=zmadd(i,l2zm(1L))){
        if(!zmleq(i,0)){
          if(ctok->type==COMMA){next_token();killsp();} else break;
          if(ctok->type==RBRA) break;
        }
	/* check for first designated initializer */
	cl=designator(t,first);
	if(!cl){
	  /* no designator */
	  cl=insert_cl(last,i);
	  cl->other=initialization(t->next,c99?noconst:0,level+1,0,fstruct,cl->other);
	  if(cl->other&&zmeqto(cl->other->idx,l2zm(-2L))){
	    first=cl->other;
	    first->other=0;
	    break;
	  }
	}else{
	  /* designator: store current object and handle remaining designators */
	  i=cl->idx;
	  if(ctok->type!=ASGN){
	    if(ctok->type!=LBRK&&ctok->type!=DOT)
	      error(355);
	  }else{
	    cl->other=0;
	    next_token();killsp();
	  }
	  cl->other=initialization(t->next,c99?noconst:0,level+1,1,fstruct,cl->other);
	}
	if(cl->next==first) first=cl;
	last=cl;
        killsp();
	if(desi&&!bracket)
	  break;
	desi_follows=0;
	if(ctok->type==COMMA){
	  copy_token(&mtok,ctok);
	  next_token();killsp();
	  if(ctok->type==LBRK)
	    desi_follows=1;
	  push_token(&mtok);
	}
      }

      if(bracket&&zmeqto(i,l2zm(0L))) error(360);

#ifdef HAVE_MISRA
          /* MISRA 9.2 -- Check if all array fields are initialised. Can be removed without problems if not needed */
          if ( (zmleq(i,t->size)) && (!zmeqto(t->size,i)) ) {   /* not all array-fields are initialised */
                misra_92_violation =1; if (!misra_92_violation_line) misra_92_violation_line = ctok->line;
          }
#endif
    }
  }else if(ISSTRUCT(f)&&(bracket||!noconst||c99)){
    if(t->exact->count<=0)
      {error(43);return(0);}
#ifdef HAVE_ECPP
    if(ecpp&&t->exact->ecpp_flags&ECPP_VIRTUAL){error(342,t->exact->identifier);return(0);}
#endif
    prev=0;
    if(level==0&&!bracket&&!c99) error(157);
    for(i=l2zm(0L);desi_follows||(!zmleq(t->exact->count,i)&&ctok->type!=RBRA);i=zmadd(i,l2zm(1L))){
      if(!desi_follows&&(*t->exact->sl)[zm2l(i)].identifier[0]==0) continue; /* unnamed bitfield */
      if(!zmleq(i,0)){
        if(ctok->type==COMMA){next_token();killsp();} else break;
        if(ctok->type==RBRA) break;
      }
      cl=designator(t,first);

      if(!cl){
	cl=insert_cl(first,l2zm((long)i));
	cl->other=initialization((*t->exact->sl)[zm2l(i)].styp,c99?noconst:0,level+1,0,!fstruct&&!bracket&&zmeqto(i,l2zm(0L))?t->exact:fstruct,cl->other);
	if(cl->other&&zmeqto(cl->other->idx,l2zm(-2L))){
	  if(fstruct){
	    first=cl->other;
	    first->other=0;
	  }else{
	    *cl=*cl->other;
	    cl->idx=l2zm(-1L);
	    cl->other=0;
	    first=cl;
	  }
	  break;
	}
      }else{
	i=zm2l(cl->idx);
	if(ctok->type!=ASGN){
	  if(ctok->type!=LBRK&&ctok->type!=DOT)
	    error(355);
	}else{
	  next_token();killsp();
	  cl->other=0;
	}
	cl->other=initialization((*t->exact->sl)[zm2l(i)].styp,c99?noconst:0,level+1,1,fstruct,cl->other);
      }
      if(cl->next==first) first=cl;
      if(desi&&!bracket)
	break;
      desi_follows=0;
      if(ctok->type==COMMA){
	copy_token(&mtok,ctok);
	next_token();killsp();
	if(ctok->type==DOT)
	  desi_follows=1;
	push_token(&mtok);
      }
    }

    if(bracket&&zmeqto(i,l2zm(0L))) error(360);

#ifdef HAVE_MISRA
          /* MISRA 9.2 -- Check if all struct fields are initialised. Can be removed without problems if not needed */
          if ( (zmleq(i,t->exact->count)) && (!zmeqto(t->exact->count,i)) ) {   /* not all struct-fields are initialised */
                misra_92_violation =1; if (!misra_92_violation_line) misra_92_violation_line = ctok->line;
          }
#endif

  }else if(ISUNION(f)&&(c99||bracket||!noconst)){
    if(t->exact->count<=0)
      {error(44);return(0);}

    if(level==0&&!bracket&&!c99) error(157);
    desi_follows=1;
    while(desi_follows){
      cl=designator(t,first);

      if(!cl){
	cl=insert_cl(first,l2zm((long)0));
	cl->other=initialization((*t->exact->sl)[0].styp,c99?noconst:0,level+1,0,!fstruct&&!bracket?t->exact:fstruct,cl->other);
	if(cl->other&&zmeqto(cl->other->idx,l2zm(-2L))){
	  if(fstruct){
	    first=cl->other;
	    first->other=0;
	  }else{
	    *cl=*cl->other;
	    cl->other=0;
	    cl->idx=l2zm(-1L);
	    first=cl;
	  }
	  break;
	}
      }else{
	i=zm2l(cl->idx);
	if(ctok->type!=ASGN){
	  if(ctok->type!=LBRK&&ctok->type!=DOT)
	    error(355);
	}else{
	  next_token();killsp();
	  cl->other=0;
	}
	cl->other=initialization((*t->exact->sl)[zm2l(i)].styp,c99?noconst:0,level+1,1,fstruct,cl->other);
      }
      first=cl;
      if(desi&&!bracket)
	break;
      desi_follows=0;
      if(ctok->type==COMMA){
	copy_token(&mtok,ctok);
	next_token();killsp();
	if(ctok->type==DOT)
	  desi_follows=1;
	else
	  push_token(&mtok);
      }
    }
  }else{
    int oldconst=const_expr;
    tree2=tree=assignment_expression();

    if(!tree){error(45);return(0);}
    if(!noconst) const_expr=1;
    if(!type_expression(tree)){free_expression(tree); const_expr=oldconst;return 0;}
    const_expr=oldconst;

    tree=makepointer(tree);

    /* check for complete struct assignment in dynamic initialization */
    if(noconst&&(ISSTRUCT(tree->ntyp->flags)||ISUNION(tree->ntyp->flags))&&fstruct==tree->ntyp->exact){
      first=mymalloc(CLS);
      first->tree=tree;
      first->next=first->other=0;
      first->idx=l2zm(-2L);
      return first;
    }

    if(!test_assignment(t,tree)){free_expression(tree); return 0;}
    if(!noconst){
      /*  nur Konstanten erlaubt (bei Arrays/Strukturen etc. oder static) */
      if(tree->flags!=CEXPR){
        /*while(tree->flags==CAST) tree=tree->left;*/
        if(!tree->sidefx/*tree->flags==ADDRESS||tree->flags==ADDRESSS||tree->flags==ADDRESSA*/){
          const_expr=1;
          gen_IC(tree,0,0);
          const_expr=oldconst;
          if(!(tree->o.flags&VARADR)){
            /*  hier fehlen noch viele Pruefungen   */
            free_expression(tree);error(46);
            return(0);
          }
          tree->o.v->flags|=USEDASADR;
          first=mymalloc(CLS);
          first->next=first->other=0;
          first->tree=tree;
	  first->idx=l2zm(0L);
          killsp();
        }else{
          free_expression(tree);error(46);
          return(0);
        }
      }else{
        first=mymalloc(CLS);
	first->idx=l2zm(0L);
        first->next=first->other=0;
        first->tree=0;
        eval_constn(tree);
        tree->ntyp->flags=t->flags;
        insert_constn(tree);
        first->val=tree->val;
        free_expression(tree2);
        killsp();
      }
    }else{
      /*  auch anderes erlaubt    */
      first=mymalloc(CLS);
      first->next=first->other=0;
      first->tree=tree;
      killsp();
      if(!tree->sidefx){
	if(tree->flags==CEXPR){
	  eval_constn(tree);
	  tree->ntyp->flags=t->flags;
	  insert_constn(tree);
	  first->val=tree->val;
	  first->tree=0;
	  first->idx=l2zm(0L);
	  init_const_sz=zmadd(init_const_sz,szof(tree->ntyp));
	  init_const_cnt++;
	  free_expression(tree2);
	}else{
	  first->idx=l2zm(-1L);
	  init_dyn_sz=zmadd(init_dyn_sz,szof(tree->ntyp));
	  init_dyn_cnt++;
	}
      }else{
	init_dyn_sz=zmadd(init_dyn_sz,szof(tree->ntyp));
	init_dyn_cnt++;
      }
    }
  }
  if(bracket){
    if(ctok->type==COMMA){next_token();killsp();}
    if(ctok->type==RBRA){next_token();killsp();} else error(128);
  }
  return(first);
}
#ifdef HAVE_ECPP
char* ecpp_mangle_name(struct Typ *t,char *identifier,struct struct_declaration *higher_nesting) {
  /* FIXME: mangled names don't have a static limit to their size*/
  /* buff should grow when required */
	/* if identifier=="_ZTV", the name for a virtual table will be created */
  static char buff[2048];
  char *pos=buff;
  int i;
  if(!higher_nesting&&t->exact&&t->exact->higher_nesting){
    higher_nesting=t->exact->higher_nesting;
  }
  /*if(t->exact&&t->exact->higher_nesting){
    if(higher_nesting)ierror(0);
    higher_nesting=t->exact->higher_nesting;
  }*/
  if(identifier&&!strcmp(identifier,"_ZTV")){
    /* vtable */
    memset(buff,0,sizeof(char)*2048);
    *pos++='_';*pos++='Z';*pos++='T';*pos++='V';
    pos=ecpp_mangle_nested_identifier(pos,t->exact);
    return buff;
  }
  if(!ISFUNC(t->flags)&&!higher_nesting&&!t->ecpp_flags&ECPP_STATIC){
    /* no mangling */
    strcpy(buff,identifier);
    return buff;
  }
  memset(buff,0,sizeof(char)*2048);
  *pos++='_';
  *pos++='Z';
  if(higher_nesting){
    *pos++='N';
    pos=ecpp_mangle_nested_identifier(pos,higher_nesting);
    if(t->ecpp_flags&ECPP_CTOR){
      *pos++='C'; *pos++='1';
    }else if(t->ecpp_flags&ECPP_DTOR){
      *pos++='D'; *pos++='1';
    }else{
    pos+=sprintf(pos,"%d%s",strlen(identifier),identifier);
    }
    *pos++='E';
  }else{
    pos+=sprintf(pos,"%d%s",strlen(identifier),identifier);
  }
  if(ISFUNC(t->flags)){
    int hasthisp=0;
    hasthisp=(t->exact->higher_nesting&&!(t->ecpp_flags&ECPP_STATIC));
    if(t->exact->count==hasthisp){
      *pos++='z';
    }else if(t->exact->count==1+hasthisp&&(*t->exact->sl)[hasthisp].styp->flags==VOID){
      *pos++='v';
    }else{
      for(i=hasthisp;i<t->exact->count-1;++i){
        pos=ecpp_mangle_arg(pos,(*t->exact->sl)[i].styp);
      }
      if((*t->exact->sl)[t->exact->count-1].styp->flags!=VOID){
        pos=ecpp_mangle_arg(pos,(*t->exact->sl)[i].styp);
        *pos++='z';
      }
    }
  }
   return buff;
}
char* ecpp_mangle_arg(char *pos,struct Typ *t) {
  int f;
  int i;
  struct struct_declaration *higher_nesting=0;
  if(!t)return pos;
  f=t->flags&NQ;
  if(t->exact&&t->exact->higher_nesting)higher_nesting=t->exact->higher_nesting;
  if(t->flags&KONST)*pos++='K';
  switch(f){
    case CHAR:
      if(t->flags&UNSIGNED)*pos++='h';
      else *pos++='c';
      break;
    case SHORT:
      if(t->flags&UNSIGNED)*pos++='t';
      else *pos++='s';
      break;
    case INT:
      if(t->flags&UNSIGNED)*pos++='j';
      else *pos++='i';
      break;
    case LONG:
     if(t->flags&UNSIGNED)*pos++='m';
      else *pos++='l';
      break;
    case LLONG:
     if(t->flags&UNSIGNED)*pos++='y';
      else *pos++='x';
      break;
    case FLOAT: *pos++='f';break;
    case DOUBLE: *pos++='d';break;
    case LDOUBLE: *pos++='e';break;
    case VOID: *pos++='v';break;
  case POINTER:
      *pos++='P';
      pos=ecpp_mangle_arg(pos,t->next);
      break;
    case ARRAY:
      *pos++='A';
      pos+=sprintf(pos,"%d",t->size);
      *pos++='_';
      pos=ecpp_mangle_arg(pos,t->next);
      break;
    case STRUCT:
    case UNION:
    case ENUM:
      if(higher_nesting){
        *pos++='N';
        pos=ecpp_mangle_nested_identifier(pos,higher_nesting);
      }
      pos+=sprintf(pos,"%d%s",strlen(t->exact->identifier),t->exact->identifier);
      if(higher_nesting){
        *pos++='E';
      }
      break;
    case FUNKT:
      *pos++='F';
      if(higher_nesting){
        *pos++='N';
        pos=ecpp_mangle_nested_identifier(pos,higher_nesting);
        *pos++='E';
      }
      if(t->exact->count==1){
        *pos++='v';
      }else for(i=0;i<t->exact->count-1;++i){
        ecpp_mangle_arg(pos,(*t->exact->sl)[i].styp);
      }
      *pos++='E';
      break;
  }
  return pos;
}
char* ecpp_mangle_nested_identifier(char *pos,struct struct_declaration *sd){
  if(sd->higher_nesting){
    pos=ecpp_mangle_nested_identifier(pos,sd->higher_nesting);
  }
	if(!sd->identifier){ierror(0);}
	pos+=sprintf(pos,"%d%s",strlen(sd->identifier),sd->identifier);
  return pos;
}
struct Var *ecpp_find_ext_var(char *identifier)
{
  struct Var *v;int l;
  for(v=first_ext;v;v=v->next){
    if(ISFUNC(v->vtyp->flags)&&!v->vtyp->exact->higher_nesting&&
    v->vtyp->exact->identifier&&!strcmp(v->vtyp->exact->identifier,identifier)){
      /* for non-method functions search for the unmangled name */
      return v;
    }else{
      if(!strcmp(v->identifier,identifier)) return v;
    }
  }
  return 0;
}
struct Var *ecpp_find_var(char *identifier)
/*  Searches local variables */
{
  int i;struct Var *v;
  int endnesting=0;
  if(identifier==0||*identifier==0) return 0;
  for(i=nesting;i>=endnesting;i--){
    for(v=first_var[i];v;v=v->next){
      if(ISFUNC(v->vtyp->flags)){
        if(!strcmp(v->vtyp->exact->identifier,identifier)) return v;
      }else{
        if(!strcmp(v->identifier,identifier)) return v;
      }
    }
  }
  return 0;
}
int ecpp_is_member_struct(struct struct_list *sl,struct struct_declaration *sd)
{
  if(!ISSTRUCT(sl->styp->flags))return 0;
  if(sl->styp->exact->higher_nesting==sd&&(!sl->identifier||!*sl->identifier))return 0;
  return 1;
}
struct struct_declaration *ecpp_find_struct(char* identifier,struct struct_declaration *scope,int search_flag)
/* search_flag: 0->only search scope, 1->scope and its bases */
/* 2->scope,bases,higher_nestings and bases of those */
{
  int i;
  struct struct_declaration* leaf_scope;
  leaf_scope=scope;
  if(!identifier||*identifier==0){ierror(0);return 0;}
  if(!scope) return find_struct(identifier,0);
  if(!strcmp(scope->identifier,identifier))return scope;
  while(scope){
    for(i=0;i<scope->count;++i){
      if((*scope->sl)[i].styp->ecpp_flags&ECPP_NESTED_CLASS&&!strcmp((*scope->sl)[i].styp->exact->identifier,identifier))
        return (*scope->sl)[i].styp->exact;
    }
    if((search_flag==1||search_flag==2)&&scope->base_class){
      scope=scope->base_class;
    }else if(search_flag==2){
      leaf_scope=leaf_scope->higher_nesting;
      scope=leaf_scope;
    }
    else scope=0;
  }
  return 0;
}
struct struct_declaration *ecpp_find_scope(char* nested_name,char** identifier)
/* if <identifier> is given, it will contain the "rightmost" identifier of the name */
/* if the name is a type, <identifier> will contain 0 */
/* e.g. if g is a global function, A is a class, f is a member of A, B is a class nested in A: */
/* <nested_name> -> (<return-value>, <*identifier>) */
/* g->(0,g); A->(A,0); A:f->(A,f); A:B->(B,0); A:A->(A,A); */
{
  static char buff[MAXI];
  char *pos=buff;
  struct struct_declaration *scope;
  struct struct_declaration *cc;
  int searchflag=2;
  if(!nested_name||!nested_name[0]){ierror(0);return;}
  memset(buff,0,MAXI*sizeof(char));
  scope=0;
  if(current_func)cc=current_func->exact->higher_nesting;
  else cc=current_class;
  for(;;){
    if(*nested_name==0){
      struct struct_declaration *merk_scope=scope;
      *pos=0;
      if(identifier)*identifier=buff;
      if(cc){
        scope=ecpp_find_struct(buff,cc,searchflag);
        if(scope){if(identifier)*identifier=0;return scope;}
        if(ecpp_find_member(buff,cc,&scope,searchflag))return scope;
      }
      scope=ecpp_find_struct(buff,scope,searchflag);
      if(scope){if(identifier)*identifier=0;return scope;}
      return merk_scope;
    }else if(*nested_name==':'){
      *pos=0;
      if(pos==buff){
        scope=0;cc=0;
      }else{
        if(cc){
          scope=ecpp_find_struct(buff,cc,searchflag);
          cc=0;
          if(!scope)scope=ecpp_find_struct(buff,0,searchflag);
        }else{
          scope=ecpp_find_struct(buff,scope,searchflag);
        }
        if(*(nested_name+1)==0||*(nested_name+1)==':'){
          /*error(341);*/return scope;}
        if(!scope){
          error(337,buff);
          return 0;
        }
      }
      searchflag=1;
      pos=buff;nested_name++;
    }else{
      *pos++=*nested_name++;
    }
  }
  return 0;
}
struct struct_list *ecpp_find_member(char* identifier,struct struct_declaration *scope,struct struct_declaration** ret_scope,int search_flag)
/* search_flag: 0->only search scope, 1->scope and its bases */
/* 2->scope,bases,higher_nestings and bases of those */
{
  int i;
  struct struct_declaration* leaf_scope;
  if(!scope||!identifier||*identifier==0){ierror(0);return 0;}
  leaf_scope=scope;
  while(scope){
    for(i=0;i<scope->count;++i){
      if(!strcmp((*scope->sl)[i].identifier,identifier)){
        if(ret_scope)*ret_scope=scope;
        return &(*scope->sl)[i];
      }
    }
    if((search_flag==1||search_flag==2)&&scope->base_class){
      scope=scope->base_class;
    }else if(search_flag==2){
      leaf_scope=leaf_scope->higher_nesting;
      scope=leaf_scope;
    }
    else scope=0;
  }
  if(ret_scope)*ret_scope=0;
  return 0;
}
void ecpp_add_this_pointer(struct struct_declaration *decl){
  if(!decl->higher_nesting){ierror(0);return;}
  decl->count++;
  decl->sl=myrealloc(decl->sl,decl->count*sizeof(struct struct_list));
  memmove(&(*decl->sl)[1],&(*decl->sl)[0],(decl->count-1)*sizeof(struct struct_list));
  (*decl->sl)[0].identifier=add_identifier("this",4);
  (*decl->sl)[0].styp=new_typ();
  (*decl->sl)[0].styp->flags=POINTER;
  (*decl->sl)[0].styp->next=new_typ();
  (*decl->sl)[0].styp->next->flags=STRUCT;
  (*decl->sl)[0].styp->next->exact=decl->higher_nesting;
  (*decl->sl)[0].align=falign((*decl->sl)[0].styp);
  (*decl->sl)[0].bfoffset=-1;
  (*decl->sl)[0].bfsize=-1;
  (*decl->sl)[0].storage_class=AUTO;
  (*decl->sl)[0].reg=0;
}
struct Typ *ecpp_declarator(struct Typ *t)
/* Subroutine for declarator(), returns 0 on error */
/* creates the mangled identifier, creates this pointer arg and sets nesting */
{
  struct struct_declaration *scope;
  char *mname;
  char *id;
  int merk_nesting;
  int domangle=0;
  int addthisarg=0;
  t->ecpp_flags|=ecpp_access;
  t->ecpp_flags|=ecpp_ret_flags;
  if(!ident||!*ident)return t;
  if(t->exact) t->exact->ecpp_flags|=ecpp_ret_flags;
  ecpp_ret_flags&=~(ECPP_CTOR|ECPP_DTOR);
  if(!ISFUNC(t->flags)&&t->ecpp_flags&ECPP_VIRTUAL)error(339);
  scope=ecpp_find_scope(ident,&id);
  if(id)strncpy(ident,id,MAXI);
  if(t->ecpp_flags&ECPP_CTOR||(id&&scope&&!strcmp(id,scope->identifier))){
    t->ecpp_flags|=ECPP_CTOR;
    strcpy(ident,"__ctor");
    if((t->next->flags&NQ)!=VOID&&((t->next->flags&NQ)!=INT))ierror(0); /* FIXME: assert that NO return type was specified */
    t->next->flags=VOID;
  }
  if(t->ecpp_flags&ECPP_DTOR){
    strcpy(ident,"__dtor");
    if((t->next->flags&NQ)!=VOID&&((t->next->flags&NQ)!=INT))ierror(0); /* FIXME: assert that NO return type was specified */
  }
  if(!scope)scope=current_class;
  if(!current_class&&scope&&id){
    /* external method/static member: assert previous declaration and inherit flags */
    struct struct_list *sl;
    int hasthisp;
    if(ISFUNC(t->flags)){
      int i;
      int ok=0;
      for(i=0;i<scope->count;++i){
        int j;
        sl=&(*scope->sl)[i];
        if(strcmp(ident,sl->identifier))continue;
        if(!ISFUNC(sl->styp->flags)){error(348);return t;}
        hasthisp=!(sl->styp->ecpp_flags&ECPP_STATIC);
        if(t->exact->count!=sl->styp->exact->count-hasthisp)continue;
        ok=1;
        for(j=0;j<t->exact->count;++j){
          struct Typ *t1,*t2;
          t1=(*t->exact->sl)[j].styp;
          t2=(*sl->styp->exact->sl)[j+hasthisp].styp;
          if(!compatible_types(t1,t2,NU|CONST|VOLATILE)){ok=0;break;}
        }
        if(ok)break;
      }
      if(!ok){error(349);return t;}
      t->ecpp_flags=sl->styp->ecpp_flags;
      t->exact->ecpp_flags|=sl->styp->ecpp_flags;
      /* FIXME: free t->exact */
      for(i=0;i<t->exact->count;++i){
        char *id=(*t->exact->sl)[i].identifier;
        (*sl->styp->exact->sl)[i+hasthisp].identifier=add_identifier(id,strlen(id));
      }
      t->exact=sl->styp->exact;
      strncpy(ident,t->exact->mangled_identifier,MAXI);
      return t;
    }else{
      sl=ecpp_find_member(ident,scope,0,0);
      if(!sl)return t; /* local var/undeclared identifier*/
      /* FIXME: check compatability */
      t->ecpp_flags=sl->styp->ecpp_flags;
    }
  }
  if(!scope){
    if(ISFUNC(t->flags)){
      if(ecpp_linkage==ECPP_C_LINKAGE){
        t->exact->identifier=add_identifier(ident,strlen(ident));
        t->exact->mangled_identifier=0;
      }else{
        struct Var *v;
        v=ecpp_find_var(ident);
        if(!v)v=ecpp_find_ext_var(ident);
        if(!v){
          domangle=1;
        }
          domangle=1;
      }
    }else{}
  }else{
    if(ISFUNC(t->flags)&&t->ecpp_flags&ECPP_FRIEND){
      struct Var *v;
      if(scope==current_class){
        v=ecpp_find_var(ident);
        if(!v)v=ecpp_find_ext_var(ident);
        if(!v){
          mname=ecpp_mangle_name(t,ident,0);
          v=add_var(mname,t,EXTERN,0);
          merk_nesting=nesting;
          nesting=0;
          t->exact->identifier=add_identifier(ident,strlen(ident));
          t->exact->mangled_identifier=add_identifier(mname,strlen(mname));
          nesting=merk_nesting;
          strncpy(ident,mname,MAXI);
        }else{
          /* TODO: check compatabiblity, find correct overloaded version */
          freetyp(t);
          t=clone_typ(v->vtyp);
          t->ecpp_flags|=ECPP_FRIEND;
        }
      }else{
        struct struct_list *sl;
        sl=ecpp_find_member(ident,scope,0,1);
        if(!sl)ierror(0); /* FIXME: member not found */
        if(!ISFUNC(sl->styp->flags))ierror(0);
        v=find_ext_var(sl->styp->exact->mangled_identifier);
        /* TODO: check compatabiblity, find correct overloaded version */
        freetyp(t);
        t=clone_typ(v->vtyp);
        t->ecpp_flags|=ECPP_FRIEND;
      }
    }else{
      if(ISFUNC(t->flags)){
        domangle=1;
      }
      if(t->ecpp_flags&ECPP_STATIC&&!current_class){
        domangle=1;
      }
      if(ISFUNC(t->flags)) t->exact->higher_nesting=scope;
      if(ISFUNC(t->flags)&&!(t->ecpp_flags&ECPP_STATIC)){
        addthisarg=1;
      }
    }
  }
  if(addthisarg) ecpp_add_this_pointer(t->exact);
  if(domangle){
    if(ISFUNC(t->flags)) mname=ecpp_mangle_name(t,ident,0);
    else mname=ecpp_mangle_name(t,ident,scope);
    merk_nesting=nesting;
    nesting=0;
    if(ISFUNC(t->flags)){
      t->exact->identifier=add_identifier(ident,strlen(ident));
      t->exact->mangled_identifier=add_identifier(mname,strlen(mname));
    }
    nesting=merk_nesting;
    strncpy(ident,mname,MAXI);
  }
  return t;
}
int ecpp_linkage_specification()
/* handles a linkage specification (e.g. extern "C" ...) */
/* FIXME: when not followed by {...}, the single declaration autom. has */
/* storage_class extern and must have no other storage_class specifiers */
/* FIXME: redeclaration of the same name with different linkage is an error */
{
  struct token mtok;
  if(ctok->type!=NAME||strcmp(ctok->name,"extern"))return 0;
  copy_token(&mtok,ctok);
  next_token();
  killsp();
  if(ctok->type==T_STRING){
    if(strcmp(ctok->name,"\"C\"")){error(343,ctok->name);return 1;}
    next_token();
    killsp();
    if(ctok->type==LBRA){
      int merk_linkage=ecpp_linkage;
      next_token();
      killsp();
      ecpp_linkage=ECPP_C_LINKAGE;
      while(declaration(0)){
        var_declaration();
        killsp();
      }
      ecpp_linkage=merk_linkage;
      if(ctok->type!=RBRA){error(0);return 1;}
      next_token();
      killsp();
      return 1;
    }else{
      int merk_linkage=ecpp_linkage;
      if(!declaration(0)){error(0);return 1;}
      ecpp_linkage=ECPP_C_LINKAGE;
      var_declaration();
      ecpp_linkage=merk_linkage;
      return 1;
    }
  }
  push_token(&mtok);
  free(mtok.name);
  return 0;
}
void ecpp_call_ctor(struct struct_declaration *sd,np this,struct argument_list *al)
/* must be called after v has been added, makes the required ctor call */
{
  np p;
  if(sd->ecpp_flags&ECPP_TV)return;
  if(sd->ecpp_flags&ECPP_POD)return;
  p=new_node();
  p->flags=CALL;
  p->alist=al;
  p->left=new_node();
  p->left->flags=DSTRUCT;
  p->left->left=new_node();
  p->left->left->flags=CONTENT;
  p->left->left->left=this;
  p->left->right=new_node();
  p->left->right->flags=MEMBER;
  p->left->right->identifier=add_identifier("__ctor",6);

  if(!type_expression(p)){ierror(0);return;}
  gen_IC(p,0,0);
  if(p&&(p->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->o.reg);
  free_expression(p);
}
void ecpp_auto_dtor(struct Var *v)
{
  struct ecpp_dtor_list *dl;
  dl=mymalloc(sizeof(struct ecpp_dtor_list));
  dl->var=v;
  dl->next=ecpp_dlist[nesting];
  ecpp_dlist[nesting]=dl;
 }
void ecpp_call_dtor(struct struct_declaration *sd,np this)
{
  np p;
  if(sd->ecpp_flags&ECPP_POD)return;
  p=new_node();
  p->flags=CALL;
  p->left=new_node();
  p->left->flags=DSTRUCT;
  p->left->left=new_node();
  p->left->left->flags=CONTENT;
  p->left->left->left=this;
  p->left->right=new_node();
  p->left->right->flags=MEMBER;
  p->left->right->identifier=add_identifier("__dtor",6);

  if(!type_expression(p)){ierror(0);return;}
  gen_IC(p,0,0);
  if(p&&(p->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->o.reg);
  free_expression(p);
}
void ecpp_auto_call_dtors()
{
  while(ecpp_dlist[nesting]){
    struct ecpp_dtor_list *merk=ecpp_dlist[nesting];
    np p;
    p=new_node();
    p->flags=ADDRESS;
    p->left=new_node();
    p->left->flags=IDENTIFIER;
    p->left->identifier=add_identifier(ecpp_dlist[nesting]->var->identifier,strlen(ecpp_dlist[nesting]->var->identifier));
    ecpp_call_dtor(ecpp_dlist[nesting]->var->vtyp->exact,p);
    ecpp_dlist[nesting]=ecpp_dlist[nesting]->next;
    free(merk);
  }
}
void ecpp_free_init_list(np *initlist,struct struct_declaration *sd)
{
  int i;
  for(i=0;i<=sd->count;++i)
    if(initlist[i])free_expression(initlist[i]);
  free(initlist);
}
void ecpp_gen_set_vtable(struct struct_declaration *class)
{
  char *vname;
  struct struct_list *sl;
  if(!class)ierror(0);
  np p=new_node();
  p->flags=ASSIGN;
  p->left=new_node();
  p->left->flags=DSTRUCT;
  p->left->left=new_node();
  p->left->left->flags=CONTENT;
  p->left->left->left=new_node();
  p->left->left->left->flags=IDENTIFIER;
  p->left->left->left->identifier=add_identifier("this",4);
  p->left->right=new_node();
  p->left->right->flags=MEMBER;
  p->left->right->identifier=add_identifier("_ZTV",4);
  p->right=new_node();
  p->right->flags=ADDRESS;
  p->right->left=new_node();
  p->right->left->flags=IDENTIFIER;
  sl=&(*class->sl)[0];
  vname=sl->styp->next->exact->mangled_identifier;
  p->right->left->identifier=add_identifier(vname,strlen(vname));
  if(!type_expression(p)){ierror(0);return;}
  gen_IC(p,0,0);
  if(p&&(p->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(p->o.reg);
}
np* ecpp_ctor_init_list(struct struct_declaration *ctor_func,int definit)
/* parses a ctor-initializer */
{
  np *initlist; int bail=0;
  char id[MAXI];
  struct struct_list *sl;
  struct struct_declaration *sd;
  struct argument_list *al;
  struct argument_list *base_args=0;
  struct Var *v;
  int i;
  sd=ctor_func->higher_nesting;
  if(!sd)ierror(0);
  if(ctok->type!=T_COLON)definit=1;
  if(!definit){
    initlist=mymalloc(NODES*(sd->count));
    memset(initlist,0,NODES*(sd->count));
    next_token();killsp();
    for(;;){
      if(ctok->type!=NAME){error(76);bail=1;break;}
      strncpy(id,ctok->name,MAXI);
      next_token();killsp();
      if(ctok->type!=LPAR){error(151);bail=1;break;}
      al=argument_list_expression();
      if(sd->base_class&&!strcmp(id,sd->base_class->identifier)){
        base_args=al;
      }else{
        for(i=0;i<sd->count;++i){
          if(!strcmp((*sd->sl)[i].identifier,id)) break;
        }
        if(i>=sd->count){error(23,ctok->name);bail=1;break;}
        sl=&(*sd->sl)[i];
        if(initlist[i]){error(344,id);bail=1;break;}
        if(ISSCALAR(sl->styp->flags)||ISSTRUCT(sl->styp->flags)){
          np dstruct;
          dstruct=new_node();
          dstruct->flags=DSTRUCT;
          dstruct->left=new_node();
          dstruct->left->flags=CONTENT;
          dstruct->left->left=new_node();
          dstruct->left->left->flags=IDENTIFIER;
          dstruct->left->left->identifier=add_identifier("this",4);
          dstruct->right=new_node();
          dstruct->right->flags=MEMBER;
          dstruct->right->identifier=add_identifier(id,strlen(id));
          if(ISSCALAR(sl->styp->flags)){
            initlist[i]=new_node();
            initlist[i]->flags=ASSIGN;
            initlist[i]->left=dstruct;
            if(al&&al->next){error(345,id);bail=1;break;}
            if(al){
              initlist[i]->right=al->arg;
            }else{
              initlist[i]->right=new_node();
              initlist[i]->right->flags=CEXPR;
              initlist[i]->right->val.vmax=l2zm(0L);
              initlist[i]->right->ntyp=new_typ();
              initlist[i]->right->ntyp->flags=INT;
            }
          }else if(ecpp_is_member_struct(sl,sd)){
            initlist[i]=new_node();
            initlist[i]->flags=ADDRESS;
            initlist[i]->left=dstruct;
            initlist[i]->alist=al;
          }
        }
      }
    if(ctok->type==COMMA){next_token();killsp();}
      else break;
    }
    if(bail){ecpp_free_init_list(initlist,sd); return 0;}
  }
  if(sd->base_class){
    np p=new_node();
    p->flags=IDENTIFIER;
    p->identifier=add_identifier("this",4);
    ecpp_call_ctor(sd->base_class,p,base_args);
  }
  if(sd->ecpp_flags&ECPP_VIRTUAL){
    ecpp_gen_set_vtable(sd);
  }
  for(i=0;i<sd->count;++i){
    if(!definit&&initlist[i]){
      if(ISSCALAR((*sd->sl)[i].styp->flags)){
        if(!type_expression(initlist[i])){ierror(0);return;}
        gen_IC(initlist[i],0,0);
        if(initlist[i]&&(initlist[i]->o.flags&(SCRATCH|REG))==(SCRATCH|REG)) free_reg(initlist[i]->o.reg);
      }else if(ecpp_is_member_struct(&(*sd->sl)[i],sd)){
        al=initlist[i]->alist;initlist[i]->alist=0;
        ecpp_call_ctor((*sd->sl)[i].styp->exact,initlist[i],al);
      }
    }else if(ecpp_is_member_struct(&(*sd->sl)[i],sd)&&!((*sd->sl)[i].styp->exact->ecpp_flags&ECPP_POD)){
      np p=new_node();
      p->flags=ADDRESS;
      p->left=new_node();
      p->left->flags=IDENTIFIER;
      p->left->identifier=add_identifier((*sd->sl)[i].identifier,strlen((*sd->sl)[i].identifier));
      ecpp_call_ctor((*sd->sl)[i].styp->exact,p,0);
    }
  }
}
void ecpp_dtor_prolog()
{
  if(current_func->exact->higher_nesting->ecpp_flags&ECPP_VIRTUAL){
    ecpp_gen_set_vtable(current_func->exact->higher_nesting);
  }
}
void ecpp_dtor_epilog()
{
  struct struct_declaration *sd;
  int i;
  sd=current_func->exact->higher_nesting;
  ecpp_auto_call_dtors();
  for(i=sd->count-1;i>=0;--i){
    if(ecpp_is_member_struct(&(*sd->sl)[i],sd)&&!((*sd->sl)[i].styp->exact->ecpp_flags&ECPP_POD)){
      np p=new_node();
      p->flags=ADDRESS;
      p->left=new_node();
      p->left->flags=IDENTIFIER;
      p->left->identifier=add_identifier((*sd->sl)[i].identifier,strlen((*sd->sl)[i].identifier));
      ecpp_call_dtor((*sd->sl)[i].styp->exact,p);
   }
  }
  if(sd->base_class){
    np p;
    p=new_node();
    p->flags=IDENTIFIER;
    p->identifier=add_identifier("this",4);
    ecpp_call_dtor(sd->base_class,p);
  }
}
void ecpp_gen_default_ctor(struct struct_declaration *class)
{
  struct struct_list *sl;
  struct Var *v;
  struct Typ *t;
  struct struct_declaration *sd;
  char *mname;
  if(!class){ierror(0);return;}
  t=new_typ();
  t->flags=FUNKT;
  t->ecpp_flags=ECPP_CTOR;
  t->next=new_typ();
  t->next->flags=VOID;
  sd=mymalloc(sizeof(struct struct_declaration));
  t->exact=sd=add_sd(sd,FUNKT);
  sd->ecpp_flags=ECPP_CTOR;
  sd->higher_nesting=class;
  sd->count=2;
  sd->sl=mymalloc(2*sizeof(struct struct_list));
  init_sl(&(*sd->sl)[0]);
  (*sd->sl)[0].identifier=add_identifier("this",4);
  (*sd->sl)[0].styp=new_typ();
  (*sd->sl)[0].styp->flags=POINTER;
  (*sd->sl)[0].styp->next=new_typ();
  (*sd->sl)[0].styp->next->flags=STRUCT;
  (*sd->sl)[0].styp->next->exact=class;
  init_sl(&(*sd->sl)[1]);
  (*sd->sl)[1].styp=new_typ();
  (*sd->sl)[1].styp->flags=VOID;
  sd->identifier=add_identifier("__ctor",6);
  mname=ecpp_mangle_name(t,0,class);
  sd->mangled_identifier=add_identifier(mname,strlen(mname));
  init_sl(&(*class->sl)[class->count]);
  (*class->sl)[class->count].identifier=add_identifier("__ctor",6);
  (*class->sl)[class->count].mangled_identifier=add_identifier(mname,strlen(mname));
  (*class->sl)[class->count].styp=clone_typ(t);
  class->count++;
  v=add_var(mname,t,EXTERN,0);
  current_func=t;
  enter_block();
  first_ic=last_ic=0;ic_count=0;max_offset=l2zm(0L);
  {
    struct Var *tmp;
    tmp=add_var((*sd->sl)[0].identifier,clone_typ((*sd->sl)[0].styp),AUTO|PARAMETER,0);
    tmp->reg=(*t->exact->sl)[0].reg;
    tmp->flags|=DEFINED;
    tmp->dfilename=filename;
    tmp->dline=line;
  }
  ecpp_ctor_init_list(sd,1);
  if((c_flags[2]&USEDFLAG)&&ic1){fprintf(ic1,"function %s\n",v->identifier); pric(ic1,first_ic);}
  vl1=first_var[0];
  vl2=first_var[1];
  vl3=merk_varf;
  optimize(optflags,v);
  if((c_flags[3]&USEDFLAG)&&ic2){fprintf(ic2,"function %s\n",v->identifier); pric(ic2,first_ic);}
  if(out&&!only_inline&&!(c_flags[5]&USEDFLAG)){
    memset(regs_modified,0,RSIZE);
    gen_code(out,first_ic,v,max_offset);
    static_stack_check(v);
    v->flags|=GENERATED;
  }
  free_IC(first_ic);
  first_ic=last_ic=0;
  leave_block();
  current_func=0;
}
void ecpp_gen_default_dtor(struct struct_declaration *class)
{
  struct struct_list *sl;
  struct Var *v;
  struct Typ *t;
  struct struct_declaration *sd;
  char *mname;
  if(!class){ierror(0);return;}
  t=new_typ();
  t->flags=FUNKT;
  t->ecpp_flags=ECPP_DTOR;
  t->next=new_typ();
  t->next->flags=VOID;
  sd=mymalloc(sizeof(struct struct_declaration));
  t->exact=sd=add_sd(sd,FUNKT);
  sd->ecpp_flags=ECPP_DTOR;
  sd->higher_nesting=class;
  sd->count=2;
  sd->sl=mymalloc(2*sizeof(struct struct_list));
  init_sl(&(*sd->sl)[0]);
  (*sd->sl)[0].identifier=add_identifier("this",4);
  (*sd->sl)[0].styp=new_typ();
  (*sd->sl)[0].styp->flags=POINTER;
  (*sd->sl)[0].styp->next=new_typ();
  (*sd->sl)[0].styp->next->flags=STRUCT;
  (*sd->sl)[0].styp->next->exact=class;
  init_sl(&(*sd->sl)[1]);
  (*sd->sl)[1].styp=new_typ();
  (*sd->sl)[1].styp->flags=VOID;
  sd->identifier=add_identifier("__dtor",6);
  mname=ecpp_mangle_name(t,0,class);
  sd->mangled_identifier=add_identifier(mname,strlen(mname));
  init_sl(&(*class->sl)[class->count]);
  (*class->sl)[class->count].identifier=add_identifier("__dtor",6);
  (*class->sl)[class->count].mangled_identifier=add_identifier(mname,strlen(mname));
  (*class->sl)[class->count].styp=clone_typ(t);
  class->count++;
  v=add_var(mname,t,EXTERN,0);
  current_func=t;
  enter_block();
  first_ic=last_ic=0;ic_count=0;max_offset=l2zm(0L);
  {
    struct Var *tmp;
    tmp=add_var((*sd->sl)[0].identifier,clone_typ((*sd->sl)[0].styp),AUTO|PARAMETER,0);
    tmp->reg=(*t->exact->sl)[0].reg;
    tmp->flags|=DEFINED;
    tmp->dfilename=filename;
    tmp->dline=line;
  }
  ecpp_dtor_prolog();
  ecpp_dtor_epilog();
  if((c_flags[2]&USEDFLAG)&&ic1){fprintf(ic1,"function %s\n",v->identifier); pric(ic1,first_ic);}
  vl1=first_var[0];
  vl2=first_var[1];
  vl3=merk_varf;
  optimize(optflags,v);
  if((c_flags[3]&USEDFLAG)&&ic2){fprintf(ic2,"function %s\n",v->identifier); pric(ic2,first_ic);}
  if(out&&!only_inline&&!(c_flags[5]&USEDFLAG)){
    memset(regs_modified,0,RSIZE);
    gen_code(out,first_ic,v,max_offset);
    static_stack_check(v);
    v->flags|=GENERATED;
  }
  free_IC(first_ic);
  first_ic=last_ic=0;
  leave_block();
  current_func=0;
}
void ecpp_access_specifier()
{
  while(ctok->type==NAME){
    if(!strcmp("private",ctok->name)){ecpp_access=ECPP_PRIVATE;}
    else if(!strcmp("protected",ctok->name)){ecpp_access=ECPP_PROTECTED;}
    else if(!strcmp("public",ctok->name)){ecpp_access=ECPP_PUBLIC;}
    else return;
    next_token();
    killsp();
    if(ctok->type!=T_COLON){error(70);return;}
    next_token();killsp();
  }
}
void ecpp_add_friend(struct struct_declaration *class,struct struct_declaration *friend)
{
  static const int REALLOC_SIZE=4;
  if(class->num_friends%REALLOC_SIZE==0){
    int newsz=(class->num_friends/REALLOC_SIZE+1)*REALLOC_SIZE;
    if(class->friends==0)class->friends=mymalloc(newsz*sizeof(struct struct_declaration**));
    else class->friends=myrealloc(class->friends,newsz*sizeof(struct struct_declaration**));
  }
  class->friends[class->num_friends]=friend;
  class->num_friends++;
}
int ecpp_is_friend(struct struct_declaration *class)
/* returns 1, if current_func is a friend of class */
{
  int i;
  for(i=0;i<class->num_friends;++i){
    if(ISFUNC(class->friends[i]->typ)&&!strcmp(class->friends[i]->mangled_identifier,current_func->exact->mangled_identifier))return 1;
    if(current_func->exact->higher_nesting&&class->friends[i]==current_func->exact->higher_nesting)return 1;
  }
  return 0;
}
#endif
