/*  Frontend for vbcc                       */
/*  (c) in 1995-2014 by Volker Barthelmann  */
/*  #define AMIGA for Amiga version         */
/*  #define ATARI for Atari version         */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef AMIGA
#pragma amiga-align
#ifdef __amigaos4__
#include <dos/anchorpath.h>
#else
#include <dos/dosasl.h>
#endif
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/libraries.h>
#include <proto/dos.h>
#pragma default-align

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 20
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#if !defined(__amigaos4__) && !defined(__MORPHOS__)
extern struct DosLibrary *DOSBase;
#endif

/*  Must be 32bit-aligned - I know it will be if compiled with vbcc.    */
struct FileInfoBlock fib;
#endif

struct NameList{
    struct NameList *next;
    char *obj;
} *first_obj=0,*last_obj=0,*first_scratch=0,*last_scratch=0;

/*  Limit fuer Laenge der Namen (wegen Wildcards)   */
#define NAMEBUF 1000    /*  MUST BE >= ~TMPNAM_L+7  */
#define USERLIBS 1000

/*  Ab dieser Laenge werden Objektfiles nicht direkt uebergeben,    */
/*  sondern aus einem File an den Linker uebergeben                 */
#ifdef AMIGA
int MAXCLEN=500;
#else
int MAXCLEN=32000;
#endif

#define WPO 16384
#define CROSSMODULE 8192
#define SCHEDULER 4096
#define NOTMPFILE 2048
#define OUTPUTSET 1024
#define NOSTDLIB 512
#define VERBOSE 256
#define VERYVERBOSE 128
#define KEEPSCRATCH 64

#define PPSRC 1
#define CCSRC 2
#define SCSRC 3
#define ASSRC 4
#define OBJ 5

char *vbccenv;
char empty[]="";
/*  Namen der einzelnen Phasen  */
char *ppname=empty,*ccname=empty,*asname=empty,*ldname=empty,*l2name=empty;
char *rmname=empty,*scname=empty;
/*  dasselbe fuer VERBOSE   */
char *ppv=empty,*ccv=empty,*asv=empty,*ldv=empty,*l2v=empty;
char *rmv=empty,*scv=empty;

/*  Linker-Commandfile  */
char *cf="@%s";
char *nodb="";
char *staticflag="";
char *ul="-l%s";

#if defined(AMIGA)
const char *config_name="vc.config";
const char *search_dirs[]={"","ENV:","VBCC:config/","VBCC:"};
#elif defined(_WIN32) || defined(MSDOS)
const char *config_name="vc.cfg";
const char *search_dirs[]={"","%VCCFG%\\"};
#elif defined(ATARI)
const char *config_name="vc.cfg";
const char *search_dirs[]={"","C:\\"};
#else
const char *config_name="vc.config";
const char *search_dirs[]={"","~/","/etc/"};
#endif

/*  String fuer die Default libraries   */
char userlibs[USERLIBS];
char *nomem="Not enough memory!\n";

char *destname="a.out";
char namebuf[NAMEBUF+1],namebuf2[NAMEBUF+1];
char *oldfile;

char *cmfiles,*cmoutput,*cmname;
#if defined(AMIGA)||defined(_WIN32)||defined(MSDOS)
#undef USECMDFILE
#define USECMDFILE 1
#endif
#ifdef USECMDFILE
FILE *cmdfile;
#endif
int final;

char *config;
char **confp;

char *command,*options,*linkcmd,*objects,*libs,*ppopts;
#ifdef AMIGA
struct AnchorPath *ap;
#endif

int linklen=10,flags=0;

#if defined(_WIN32)||defined(MSDOS)||defined(ATARI)
char *tmpnam(char *p)
{
  static int c=1675;
  static char tmp[NAMEBUF];
  char *env;

  env=getenv("TEMP");
  if(!env) env=".";
  if(!p) p=tmp;
  snprintf(p,NAMEBUF,"%s\\vbcc%04x",env,++c);
  return p;
}
#endif

static void free_namelist(struct NameList *p)
{
    struct NameList *m;
    while(p){
        m=p->next;
        if(flags&VERYVERBOSE){
            puts("free p->obj");
            if(!p->obj) puts("IS ZERO!!"); else puts(p->obj);
        }
        free((void *)p->obj);
        if(flags&VERYVERBOSE){puts("free p"); if(!p) puts("IS ZERO!!");}
        free((void *)p);
        p=m;
    }
}

static void raus(int rc)
{
    if(confp)   free(confp);
    if(config)  free(config);
    if(objects) free(objects);
    if(libs)    free(libs);
    if(command) free(command);
    if(ppopts)  free(ppopts);
    if(options) free(options);
    if(linkcmd) free(linkcmd);
#ifdef AMIGA
    if(ap)
#ifdef __amigaos4__
        FreeDosObject(DOS_ANCHORPATH,ap);
#else
        free(ap);
#endif
#endif
    free_namelist(first_obj);
    free_namelist(first_scratch);
    exit(rc);
}

#ifdef NO_LONGER_NEEDED
/* Launch command from "$VBCC\bin\", when no absolute path was specified. */
static int runcmd(char *cmd)
{
    char *p;

    for(p=cmd;*p!=0&&*p>' ';p++){
        if(*p==':')  /* absolute path */
            goto do_cmd;
    }
    if(vbccenv){
        /* prepend "$VBCC\bin\" in front of our command */
        int len=strlen(vbccenv);
        if(vbccenv[len-1]=='\\') len--;
        memmove(cmd+len+5,cmd,strlen(cmd)+1);
        memcpy(cmd,vbccenv,len);
        memcpy(cmd+len,"\\bin\\",5);
    }
do_cmd:
    return system(cmd);
}
#else
#define runcmd(c) system(c)
#endif

static void del_scratch(struct NameList *p)
{
    while(p){
        sprintf(command,rmname,p->obj);
        if(flags&VERBOSE) printf("%s\n",command);
        if(runcmd(command)){printf("%s failed\n",command);raus(EXIT_FAILURE);}
        p=p->next;
    }
}

static void add_name(char *obj,struct NameList **first,struct NameList **last)
{
    struct NameList *new;
    if(flags&VERYVERBOSE) printf("add_name: %s\n",obj);
    if(!(new=malloc(sizeof(struct NameList))))
        {printf(nomem);raus(EXIT_FAILURE);}
    if(!(new->obj=malloc(strlen(obj)+1)))
        {free((void *)new);printf(nomem);raus(EXIT_FAILURE);}
    if(first==&first_obj) linklen+=strlen(obj)+1;
    strcpy(new->obj,obj);
    new->next=0;
    if(!*first){
        *first=*last=new;
    }else{
        (*last)->next=new;*last=new;
    }
}

static int read_config(const char *cfg_name)
{
    int i,count; long size;
    char *p,*name;
    FILE *file=0;
    for(i=0;i<sizeof(search_dirs)/sizeof(search_dirs[0]);i++){
      name=malloc(strlen(search_dirs[i])+strlen(cfg_name)+1);
      if(!name) {printf(nomem);raus(EXIT_FAILURE);}
      strcpy(name,search_dirs[i]);
      strcat(name,cfg_name);
      file=fopen(name,"r");
      free(name);
      if(file) break;
    }
    if(!file){
      if(p=vbccenv){
        name=malloc(strlen(p)+strlen(cfg_name)+20);
        if(!name){printf(nomem);raus(EXIT_FAILURE);}
        strcpy(name,p);
#if defined(_WIN32)||defined(MSDOS)||defined(ATARI)
        strcat(name,"\\config\\");
        strcat(name,cfg_name);
#elif defined(AMIGA)
        AddPart(name,"config",strlen(p)+strlen(cfg_name)+20);
        AddPart(name,(STRPTR)cfg_name,strlen(p)+strlen(cfg_name)+20);
#else
        strcat(name,"/config/");
        strcat(name,cfg_name);
#endif
        file=fopen(name,"r");
        free(name);
      }
    }
    if(!file) {puts("No config file!");raus(EXIT_FAILURE);}
    if(fseek(file,0,SEEK_END)) return 0;
    size=ftell(file);
    if(fseek(file,0,SEEK_SET)) return 0;
    config=malloc(size+1);
    if(!config){printf(nomem);raus(EXIT_FAILURE);}
    size=fread(config,1,size,file);
    fclose(file);
    count=0;p=config;
    while(p<config+size&&*p){
        count++;
        while(p<config+size&&*p!='\n') p++;
        if(*p=='\n') *p++=0;
    }
    config[size]=0;
    confp=malloc(count*sizeof(char *));
    for(p=config,i=0;i<count;i++){
        confp[i]=p;
        while(*p) p++;
        p++;
    }
    return count;
}

static int typ(char *fp)
{
    char *p=strrchr(fp,'.');
    if(p&&(!strcmp(p,".c")||!strcmp(p,".C"))) return ppname==empty?CCSRC:PPSRC;
    if(p&&!strcmp(p,".i")) return CCSRC;
    if(p&&(!strcmp(p,".s")||!strcmp(p,".S")||!strcmp(p,".asm"))) return ASSRC;
    if(p&&!strcmp(p,".scs")) return SCSRC;
    if(!p||!strcmp(p,".o")||!strcmp(p,".obj")){
      FILE *f;
      if(f=fopen(fp,"r")){
        if(fgetc(f)==0&&fgetc(f)=='V'&&fgetc(f)=='B'&&fgetc(f)=='C'&&fgetc(f)=='C'){
          fclose(f);     
          return CCSRC;
        }
        fclose(f);
      }
      return OBJ;
    }
    return OBJ;
}

static char *add_suffix(char *s,char *suffix)
{
    static char str[NAMEBUF+3],*p;
    if(strlen(s)+strlen(suffix)>NAMEBUF){printf("string too long\n");raus(EXIT_FAILURE);}
    if(s!=str) strcpy(str,s);
    p=strrchr(str,'.');
    if(!p) p=str+strlen(s);
    if(!p||p==str||(p==str+1&&str[0]=='\"')) p=str+strlen(s);
    strcpy(p,suffix);
    strcat(p,"\"");
    return str;
}

#if defined(_WIN32)||defined(MSDOS)||defined(ATARI)||defined(AMIGA)
static char *convert_path(char *path)
{
    char c,*p,*newpath;
    newpath=p=malloc(strlen(path)+1);
    while(c=*path++){
#ifdef AMIGA
        if(c=='.'){
            if(*path=='/') continue;
            if(*path=='.'&&*(path+1)=='/'){
                path++;
                continue;
            }
        }
#else
        if(c=='/') c='\\';
#endif
        *p++=c;
    }
    *p=0;
    return newpath;
}
#else
#define convert_path(p) (p)
#endif

int main(int argc,char *argv[])
{
    int tfl,i,len=10,pm,count,db=0,staticmode=0;
    char *parm;
    long opt=1;

    for(i=1;i<argc;i++){
        if(argv[i][0]=='+'){
            config_name=argv[i]+1;
            argv[i][0]=0;
            break;
        }
    }
    vbccenv=getenv("VBCC");
    count=read_config(config_name);
#ifdef AMIGA
#if !defined(__amigaos4__) && !defined(__MORPHOS__)
    if(pm=DOSBase->dl_lib.lib_Version>=36)
#else
    if(pm=1)
#endif
    {
#ifdef __amigaos4__
        if(!(ap=(struct AnchorPath *)AllocDosObjectTags(DOS_ANCHORPATH,
                                                        ADO_Strlen,NAMEBUF,
                                                        TAG_DONE)))
          pm=0;
#else
        if(ap=(struct AnchorPath *)calloc(sizeof(struct AnchorPath)+NAMEBUF,1))
            {ap->ap_Strlen=NAMEBUF;ap->ap_BreakBits=0;} else pm=0;
#endif
    }
#endif
    for(i=1;i<argc+count;i++){
        if(i<argc) parm=argv[i]; else parm=confp[i-argc];
        if(!strncmp(parm,"-ul=",4)){ul=parm+4;*parm=0;}
    }
    for(i=1;i<argc+count;i++){
        if(i<argc) parm=argv[i]; else parm=confp[i-argc];
/*        printf("Parameter %d=%s\n",i,parm);*/
        if(!strncmp(parm,"-ldnodb=",8)){nodb=parm+8;*parm=0;}
        if(!strncmp(parm,"-ldstatic=",10)){staticflag=parm+10;*parm=0;}
        if(!strcmp(parm,"-g")) db=1;
        if(!strcmp(parm,"-static")){staticmode=1;*parm=0;}
        if(!strncmp(parm,"-ml=",4)){MAXCLEN=atoi(parm+4);*parm=0;}
        if(!strncmp(parm,"-pp=",4)){ppname=parm+4;*parm=0;}
        if(!strncmp(parm,"-cc=",4)){ccname=parm+4;*parm=0;}
        if(!strncmp(parm,"-as=",4)){asname=parm+4;*parm=0;}
        if(!strncmp(parm,"-ld=",4)){ldname=parm+4;*parm=0;}
        if(!strncmp(parm,"-l2=",4)){l2name=parm+4;*parm=0;}
        if(!strncmp(parm,"-rm=",4)){rmname=parm+4;*parm=0;}
        if(!strncmp(parm,"-ppv=",5)){ppv=parm+5;*parm=0;}
        if(!strncmp(parm,"-ccv=",5)){ccv=parm+5;*parm=0;}
        if(!strncmp(parm,"-asv=",5)){asv=parm+5;*parm=0;}
        if(!strncmp(parm,"-ldv=",5)){ldv=parm+5;*parm=0;}
        if(!strncmp(parm,"-l2v=",5)){l2v=parm+5;*parm=0;}
        if(!strncmp(parm,"-rmv=",5)){rmv=parm+5;*parm=0;}
        if(!strncmp(parm,"-cf=",4)){cf=parm+4;*parm=0;}
        if(!strncmp(parm,"-isc=",5)){scname=parm+5;*parm=0;}
        if(!strncmp(parm,"-iscv=",6)){scv=parm+6;*parm=0;}
        if(!strcmp(parm,"-schedule")) {flags|=SCHEDULER;*parm=0;}
        if(!strcmp(parm,"-notmpfile")) {flags|=NOTMPFILE;*parm=0;}
        /*if(!strcmp(parm,"-E")) {flags|=CCSRC;*parm=0;}*/
        if(!strcmp(parm,"-S")) {flags|=ASSRC;*parm=0;}
        if(!strcmp(parm,"-SCS")) {flags|=SCSRC;*parm=0;}
        if(!strcmp(parm,"-c")) {flags|=OBJ;*parm=0;}
        if(!strcmp(parm,"-v")) {flags|=VERBOSE;*parm=0;}
        if(!strcmp(parm,"-h")) {flags|=VERBOSE;*parm=0;}
        if(!strcmp(parm,"-k")) {flags|=KEEPSCRATCH;*parm=0;}
        if(!strcmp(parm,"-vv")) {flags|=VERBOSE|VERYVERBOSE;*parm=0;}
        if(!strcmp(parm,"-nostdlib")) {flags|=NOSTDLIB;*parm=0;}
        if(parm[0] == ';') {*parm=0;}
        if(!strncmp(parm,"-O",2)){
          static int had_opt;
          if(had_opt){
            puts("Optimization flags specified multiple times");
            exit(EXIT_FAILURE);
          }
          had_opt=1;
          if(parm[2]=='0') opt=0;
          else if(parm[2]=='1'||parm[2]==0) opt=991;
          else if(parm[2]=='2') {opt=1023;flags|=SCHEDULER;}
          else if(parm[2]=='3') {opt=~0;flags|=(SCHEDULER|CROSSMODULE);}
          else if(parm[2]>='4'&&parm[2]<='9') {opt=~0;flags|=(SCHEDULER|WPO);}
          
          else if(parm[2]=='=') opt=atoi(&parm[3]);
          *parm=0;
        }
        if(!strcmp(parm,"-o")&&i<argc-1) {
            *argv[i++]=0;destname=argv[i];
            flags|=OUTPUTSET;argv[i]="";continue;
        }
        if(!strncmp(parm,"-o=",3)){
            destname=parm+3;
            flags|=OUTPUTSET;*parm=0;continue;
        }
        if(parm[0]=='-'&&parm[1]=='l'){
            size_t l=strlen(userlibs);
            if((l+strlen(parm)-2+strlen(ul)+1)>=USERLIBS){puts("Userlibs too long");exit(EXIT_FAILURE);}
            userlibs[l]=' ';
            sprintf(userlibs+l+1,ul,parm+2);
            *parm=0;continue;
        }
        if(parm[0]=='-'&&parm[1]=='L'){
            size_t l=strlen(userlibs);
            if((l+strlen(parm)+1)>=USERLIBS){puts("Userlibs too long");exit(EXIT_FAILURE);}
            userlibs[l]=' ';
            sprintf(userlibs+l+1,"%s",parm);
            *parm=0;continue;
        }
        len+=strlen(parm)+10;
#ifdef ATARI
        if(vbccenv)
          len+=strlen(vbccenv)+5;
#endif
    }
    if(!db&&*nodb){
      if(strlen(userlibs)+2+strlen(nodb)>=USERLIBS){
        puts("Userlibs too long");exit(EXIT_FAILURE);
      }
      strcat(userlibs," ");
      strcat(userlibs,nodb);
    }
    if(staticmode&&*staticflag){
      size_t ulen=strlen(userlibs);
      size_t slen=strlen(staticflag);
      if(ulen+2+slen>=USERLIBS){
        puts("Userlibs too long");exit(EXIT_FAILURE);
      }
      memmove(userlibs+slen+1,userlibs,ulen+1);
      memcpy(userlibs,staticflag,slen);
      userlibs[slen]=' ';
    }
    if(flags&VERBOSE){
      printf("vc frontend for vbcc (c) in 1995-2014 by Volker Barthelmann\n");
#ifdef SPECIAL_COPYRIGHT
      printf("%s\n",SPECIAL_COPYRIGHT);
#endif
    }
    if(!(flags&7)) flags|=OBJ+1;
    tfl=flags&7;
    if((flags&WPO)&&tfl!=OBJ){
      flags&=~WPO;
      flags|=CROSSMODULE;
    }
    if(scname==empty) flags&=~SCHEDULER;
    if(flags&VERYVERBOSE){
      ppname=ppv;ccname=ccv;asname=asv;ldname=ldv;
      rmname=rmv;l2name=l2v;scname=scv;
    }
    if(flags&NOSTDLIB){ldname=l2name;}
    /*  Nummer sicher...    */
    len+=strlen(ppname)+strlen(ccname)+strlen(asname)+
         strlen(rmname)+strlen(scname)+strlen(userlibs)+NAMEBUF+100;
    if(!(command=malloc(len))){printf(nomem);raus(EXIT_FAILURE);}
    if(!(oldfile=malloc(len))){printf(nomem);raus(EXIT_FAILURE);}
    if(!(options=malloc(len))){printf(nomem);raus(EXIT_FAILURE);}
    if(!(ppopts=malloc(len))){printf(nomem);raus(EXIT_FAILURE);}
    *options=0;*ppopts=0;
    for(i=1;i<argc+count;i++){
        if(i<argc) parm=argv[i]; else parm=confp[i-argc];
        if(*parm=='-'){
            if(ppname==empty||(parm[1]!='D'&&parm[1]!='I'&&parm[1]!='+')){
                strcat(options,parm);strcat(options," ");
            }else{
                strcat(ppopts,parm);strcat(ppopts," ");
            }
        }
    }
    if(flags&VERYVERBOSE) printf("flags=%d opt=%ld len=%d\n",flags,opt,len);
    namebuf[0]='\"'; namebuf2[0]='\"';
    for(i=1;i<argc+count+((flags&CROSSMODULE)!=0);i++){
        int t,j;char *file;
#ifdef AMIGA
        BPTR lock;
#endif
        if(i<argc)
          parm=argv[i];
        else if(i<argc+count)
          parm=confp[i-argc];
        else
          parm=cmfiles;
        if(!parm||(*parm=='-'&&parm!=cmfiles)||!*parm) continue;
        if(flags&VERYVERBOSE) printf("Argument %d:%s\n",i,parm);
#ifdef AMIGA
        if(pm&&parm!=cmfiles)
            if(MatchFirst((STRPTR)convert_path(parm),ap)){
                printf("No match for %s\n",parm);continue;
            }
#endif
        do{
            if(parm==cmfiles){
                file=parm;
                t=CCSRC;
            }else{
#ifdef AMIGA
#ifdef __amigaos4__
                if(pm) file=(char *)ap->ap_Buffer; else file=parm;
#else
                if(pm) file=(char *)&ap->ap_Buf[0]; else file=parm;
#endif
#else
                file=convert_path(parm);
#endif
                t=typ(file);
                strcpy(namebuf+1,file);
                strcat(namebuf,"\"");
                file=namebuf;
            }
            if(flags&VERYVERBOSE) printf("File %s=%d\n",file,t);
            if(!cmname&&(flags&CROSSMODULE)&&t<=CCSRC){
                cmname=malloc(NAMEBUF);
                if(!cmname){printf(nomem);exit(EXIT_FAILURE);}
                if(tfl==OBJ){
                    strcpy(cmname,file);
                }else{
                    cmname[0]='\"';
                    tmpnam(cmname+1);
                }
            }
            for(j=t;j<tfl;j++){
                if(j==OBJ){
                    if(j==t) add_name(file,&first_obj,&last_obj);
                    continue;
                }
                strcpy(oldfile,file);
                if(file==cmfiles){
                    file=cmoutput;
                }else{
                    if(j==t&&j!=tfl-1&&!(flags&(NOTMPFILE|KEEPSCRATCH))){
                        file=namebuf2;
                        tmpnam(file+1);
                    }
                    if(j==tfl-1||(flags&WPO)) file=namebuf;
                }
                if(j==PPSRC){
                    file=add_suffix(file,".i");
                    if(tfl==CCSRC&&(flags&OUTPUTSET)) file=destname;
                    sprintf(command,ppname,ppopts,oldfile,file);
                    if((tfl)!=CCSRC) add_name(file,&first_scratch,&last_scratch);
                }
                /* MUST come before CCSRC-handling! */
                if(j==SCSRC){
                    /*if(final) file=cmname;*/
                    file=add_suffix(file,".asm");
                    if(tfl==ASSRC&&(flags&OUTPUTSET)) file=destname;
                    sprintf(command,scname,oldfile,file);
                    if(tfl!=ASSRC) add_name(file,&first_scratch,&last_scratch);
                }
                if(j==CCSRC){
                    if(file!=cmoutput){
                      if(flags&WPO){
                        if(flags&OUTPUTSET)
                          file=destname;
                        else
                          file=add_suffix(file,".o");
                      }else if(flags&SCHEDULER){
                        file=add_suffix(file,".scs");
                      }else{
                        if(++j==tfl-1) file=namebuf;
                        file=add_suffix(file,".asm");
                      }
                      if(tfl==j+1&&(flags&OUTPUTSET)) file=destname;
                      if(flags&CROSSMODULE){
#ifdef USECMDFILE
                        if(!cmdfile){
                          char *s;
                          s=tmpnam(0);
                          cmfiles=malloc(strlen(s)+16);
                          if(!cmfiles){printf(nomem);exit(EXIT_FAILURE);}
                          sprintf(cmfiles,"-cmd= \"%s\"",s);
                          cmdfile=fopen(s,"w");
                          if(!cmdfile){printf("Could not open <%s>!\n",s);exit(EXIT_FAILURE);}
                          add_name(s,&first_scratch,&last_scratch);
                        }
                        fputs(oldfile,cmdfile);
                        fputs("\n",cmdfile);
#else
                        if(!cmfiles){
                          cmfiles=malloc(strlen(oldfile)+3);
                          if(!cmfiles){printf(nomem);exit(EXIT_FAILURE);}
                          strcpy(cmfiles,oldfile);
                        }else{
                          cmfiles=realloc(cmfiles,strlen(cmfiles)+strlen(oldfile)+3);
                          if(!cmfiles){printf(nomem);exit(EXIT_FAILURE);}
                          strcat(cmfiles," ");strcat(cmfiles,oldfile);
                        }
#endif
                        if(!cmoutput){
                          cmoutput=malloc(strlen(file)+1);
                          if(!cmoutput){printf(nomem);exit(EXIT_FAILURE);}
                          strcpy(cmoutput,file);
                        }
                        break;
                      }
                    }
                    if(flags&CROSSMODULE){
#ifdef USECMDFILE
                      fclose(cmdfile);
                      cmdfile=0;
#endif
                      final=1;
                    }
                    if((flags&(CROSSMODULE|SCHEDULER))==CROSSMODULE) j++;
                    sprintf(command,ccname,oldfile,file,options,opt);
                    if(flags&WPO){
                      strcat(command," -wpo");
                      j=OBJ;
                    }else{
                      if(tfl!=j+1) add_name(file,&first_scratch,&last_scratch);
                    }
                }
                if(j==ASSRC){
                    if(final) file=cmname;
                    file=add_suffix(file,".o");
                    if(tfl==OBJ&&(flags&OUTPUTSET)) file=destname;
                    sprintf(command,asname,oldfile,file);
                    add_name(file,&first_obj,&last_obj);
                    if((tfl)!=OBJ) add_name(file,&first_scratch,&last_scratch);
                }
                if(flags&VERBOSE) printf("%s\n",command);
#ifdef AMIGA
#if !defined(__amigaos4__) && !defined(__MORPHOS__)
                if(DOSBase->dl_lib.lib_Version>=36){
#else
                if(1){
#endif
                    if(SystemTags(command,NP_Priority,-2,TAG_DONE)){
                        printf("%s failed\n",command);
                        raus(EXIT_FAILURE);
                    }
                }else
#endif
                if(runcmd(command)){printf("%s failed\n",command);raus(EXIT_FAILURE);}
            }
#ifdef AMIGA
        }while(pm&&!MatchNext(ap));
        if(pm) MatchEnd(ap);
#else
        }while(0);
#endif
    }
    if((tfl)>OBJ){
    /*  Zu Executable linken    */
        struct NameList *p;
        FILE *objfile=0;
        char *tfname;
        objects=malloc(linklen);
        if(!objects){printf(nomem);raus(EXIT_FAILURE);}
        linklen+=strlen(ldname)+strlen(destname)+strlen(userlibs)+10;
#ifdef ATARI
        if(vbccenv) linklen+=strlen(vbccenv)+5;
#endif
        if(flags&VERYVERBOSE) printf("linklen=%d\n",linklen);
        if(!(linkcmd=malloc(linklen))){printf(nomem);raus(EXIT_FAILURE);}
        p=first_obj;
        if(linklen>=MAXCLEN){
            tfname=tmpnam(0);
            sprintf(objects,cf,tfname);
            if(!(objfile=fopen(tfname,"w"))){
                printf("Could not open <%s>!\n",tfname);
                raus(EXIT_FAILURE);
            }
        }else *objects=0;
        while(p){
            if(p->obj){
                if(linklen>=MAXCLEN){
                    fputs(p->obj,objfile);
                    fputs("\n",objfile);
                }else{
                    strcat(objects,p->obj);strcat(objects," ");
                }
            }
            p=p->next;
        }
        if(objfile) fclose(objfile);
        if(*objects){
            sprintf(linkcmd,ldname,objects,userlibs,destname);
            if(flags&VERBOSE) printf("%s\n",linkcmd);
            /*  hier wird objfile bei Fehler nicht geloescht    */
            if(runcmd(linkcmd)){printf("%s failed\n",linkcmd);raus(EXIT_FAILURE);}
#ifdef AMIGA
            if(flags&VERBOSE){
                BPTR l;
                if(l=Lock((STRPTR)destname,-2)){
                    if(Examine(l,&fib)) printf("Size of executable: %lu bytes\n",(unsigned long)fib.fib_Size);
                    UnLock(l);
                }
            }
#endif
        }else puts("No objects to link");
        if(objfile) remove(tfname);
    }
    if(!(flags&KEEPSCRATCH)) del_scratch(first_scratch);
    raus(0);
}
