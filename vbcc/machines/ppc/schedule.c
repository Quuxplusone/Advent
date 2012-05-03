/*
 * vscppc
 *
 * vbcc PowerPC scheduler
 * (C)1998-2001 by Frank Wille <frank@phoenix.owl.de>
 *
 * vscppc is freeware and part of the portable and retargetable ANSI C
 * compiler vbcc, copyright (c) 1995-98 by Volker Barthelmann.
 * vscppc may be freely redistributed as long as no modifications are
 * made and nothing is charged for it. Non-commercial usage is allowed
 * without any restrictions.
 * EVERY PRODUCT OR PROGRAM DERIVED DIRECTLY FROM MY SOURCE MAY NOT BE
 * SOLD COMMERCIALLY WITHOUT PERMISSION FROM THE AUTHOR.
 *
 * History:
 * V0.5   14-Feb-01
 *        FSCPR is no longer marked as used/modified, which allows the
 *        scheduler to rearrange all FPU instructions. As a consequence, the
 *        only instruction reading FPSCR, mffs, has to be marked as barrier.
 * V0.4   01-Dec-99
 *        603e PID7 seems to have a latency of 20 instead 37 for
 *        the "div" instruction (source: Michal Bartczak).
 * V0.3b  30-Jul-98
 *        crxxx instructions used/modified wrong CCRs.
 * V0.3a  21-Jul-98
 *        "la" instruction was not recognized.
 * V0.3   20-Jul-98
 *        Target-specific options via sched_option() removed. Options
 *        are passed in the assembler source, e.g. "#vsc elf".
 *        Differentiation between 603 and 604 (selected by "#vsc cpu").
 *        Now, scheduling takes place with regard to the real PowerPC
 *        architecture.
 * V0.2   12-Jul-98

 *        Option "-elf" lets the scheduler accept ELF/SVR4 sources,
 *        which only use numbers instead of full register names.
 *        Target-specific options require a modification in the
 *        portable scheduler part vsc.c.
 * V0.1   10-Jul-98
 *        vscppc seems to be stable, after some tests.
 *        However, it still needs a lot of fine tuning (my PowerPC
 *        docs are at home).
 *        A differentiation between the PPC CPUs (603e, 604e) is missing.
 * V0.0   09-Jul-98
 *        File created.
 *
 */

#include "vsc.h"

char tg_copyright[]="PowerPC scheduler V0.5 (c) in 1998-2001 by Frank Wille";

static int elf=0,cpu=604;



int sched_init(void)
{
  return (1);
}


void sched_cleanup(void)
{
}


static void sched_option(char *s)
{
  if (!strncmp(s,"elf",3)) {
    elf = 1;
  }
  else if (!strncmp(s,"cpu ",4)) {
    if (!strncmp(s+4,"603",3))
      cpu = 603;
    else if (!strncmp(s+4,"604",3))
      cpu = 604;
  }
}


static char *strest(char *s,int n)
/* returns ptr to the last n characters of string s */
{
  return (s + strlen(s) - n);
}


static int setlab(struct sinfo *p,char *lab)
/* check for a valid local label */
{
  int i;

  if (sscanf(lab,elf?".l%d":"l%d",&i) == 1) {
    p->label = i;
    return (1);
  }
  return (0);
}


static int lsreg(char *s)
/* checks the operand for load/store addressing mode and returns */
/* the base register on success, -1 otherwise */
{
  char *p = (s+strlen(s))-1;
  int q;

  while (p>s && *p!='(')
    --p;
  if (sscanf(p,elf?"(%d)":"(r%d)",&q) == 1)
    return (q);
  return (-1);
}


int sched_info(struct sinfo *p)
{
  char buf[16],oper[40],c;
  int q1,q2,q3,z,i,x,n;

  /* check for vscppc specific options in the source */
  if (!strncmp(p->txt,"#vsc ",5)) {
    sched_option(p->txt+5);
    p->flags = BARRIER;
    return (1);
  }

  if (!strncmp(p->txt,"#barrier",8)) {
    /* an artificial barrier, used e.g. for inline-code */
    p->flags = BARRIER;
    return (1);
  }

  if (sscanf(p->txt," .%15s %39s",buf,oper) >= 1) {
    /* assembler directive or macro */
    p->flags = BARRIER;
    return (1);
  }

  if (sscanf(p->txt,elf?".l%d:":"l%d:",&i) == 1) {
    /* a local label */
    p->label = i;
    p->flags = LABEL;
    return (1);
  }

  if (sscanf(p->txt," mffs%15s %39s",buf,oper) >= 1) {
    /* mffs reads FPSCR and therefore has to be treated as a barrier, */
    /* because FPSCR is never marked as used by the scheduler */
    p->flags = BARRIER;
    return (1);
  }

  if ((n = sscanf(p->txt," b%15s %39s",buf,oper)) >= 1) {
    /* branch instruction */
    p->latency = 1;
    BSET(p->pipes,BPU);

    if (n == 1) {
      /* branch unconditional: b label */
      if (setlab(p,buf)) {
        p->flags = UNCOND_BRANCH;
        return (1);
      }
    }
    else {
      char *a = strest(buf,3);

      /* reject branch instructions ending with */
      /* "lr", "ctr", "a" or "l", but accept "bnl" */
      if ((strncmp(a+1,"lr",2) && strncmp(a,"ctr",3) &&
           *(a+2)!='l' && *(a+2)!='a') ||
          !strcmp(buf,"nl")) {

        if (*buf == 'd') {
          /* bdz... or bdnz... */
          a = oper;
          if (strcmp(buf,"dz") && strcmp(buf,"dnz")) {
            while (*a && *a!=',')
              a++;
            if (*a == ',')
              a++;
            else
              a = 0;
          }
          if (a) {
            if (setlab(p,a)) {
              p->flags = COND_BRANCH;
              BSET(p->modifies,CTR);
              /* @@@ unable to determine the used CCRs - set all to used */
              for (x=CCR; x<(CCR+8); x++)
                BSET(p->uses,x);
              return (1);
            }
          }
        }

        else if (*buf == 'c') {
          if (sscanf(oper,"%d,%d,%s",&q1,&q2,buf) == 3) {
            /* conditional branch: bc m,n,label */
            if (setlab(p,buf)) {
              p->flags = COND_BRANCH;
              BSET(p->uses,CCR+(q2>>2));
              return (1);
            }
          }
        }

        else {
          /* normal conditional branch: b<cond> [crN,]label */
          a = buf;
          if (sscanf(oper,elf?"%d,%s":"cr%d,%s",&i,buf) != 2) {
            i = 0;
            a = oper;
          }
          if (setlab(p,a)) {
            p->flags = COND_BRANCH;
            BSET(p->uses,CCR+i);
            return (1);
          }
        }
      }
    }

    p->flags = BARRIER;
    return (1);
  }


  if (cpu == 603) {
    /* scheduling for the PowerPC 603 */

    if ((!elf && sscanf(p->txt," %15s %c%d,%39s",buf,&c,&z,oper) == 4) ||
        (elf && sscanf(p->txt," %15s %d,%39s",buf,&z,oper) == 3)) {
      if (elf) {
        if ((buf[0]=='l' && buf[1]=='f') || (buf[0]=='s' && buf[2]=='f'))
          c = 'f';
        else
          c = 'r';
      }
      if ((q1 = lsreg(oper))>=0 && (c=='r'||c=='f')) {
        /* load/store instruction ...,d(rQ1) */

        if (!strcmp(buf,"la")) {
          /* la rZ,d(rQ1) is the same as: addi rZ,rQ1,d */
          p->latency = 1;
          BSET(p->pipes,IU);
          BSET(p->pipes,SRU);  /* addi may also execute in SRU */
          BSET(p->uses,GPR+q1);
          BSET(p->modifies,GPR+z);
          return (1);
        }

        BSET(p->pipes,LSU);
        if (*(strest(buf,1)) == 'u')  /* update instruction? */
          BSET(p->modifies,GPR+q1);
        else
          BSET(p->uses,GPR+q1);

        if (c == 'r') {
          /* integer load/store */

          if (!strncmp(strest(buf,2),"mw",2)) {
            /* load/store multiple word rz,i(rq1) */

            if (*buf == 'l') {
              p->latency = 2+(32-z);
              for (x=z; x<32; x++)
                BSET(p->modifies,GPR+x);
              BSET(p->uses,MEM);
            }
            else {
              p->latency = 1+(32-z);
              for (x=z; x<32; x++)
                BSET(p->uses,GPR+x);
              BSET(p->modifies,MEM);
            }
          }
          else {
            /* load/store integer rz,i(rq1) */

            p->latency = 3;
            if(*buf == 'l') {
              BSET(p->modifies,GPR+z);
              BSET(p->uses,MEM);
            }
            else{
              BSET(p->uses,GPR+z);
              BSET(p->modifies,MEM);
            }
          }
        }

        else {
          /* load/store float fz,d(rQ1) */

          p->latency = 3;
          if(*buf == 'l') {
            BSET(p->modifies,FPR+z);
            BSET(p->uses,MEM);
          }
          else{
            BSET(p->uses,FPR+z);
            BSET(p->modifies,MEM);
          }
        }

        return (1);
      }
    }

    if ((!elf && sscanf(p->txt," %15s %c%d,r%d,r%d",buf,&c,&z,&q1,&q2) == 5) ||
        (elf && sscanf(p->txt," %15s %d,%d,%d",buf,&z,&q1,&q2) == 4)) {
      if (*buf=='l' || (buf[0]=='s' && buf[1]=='t')) {
        if (elf) {
          if ((buf[0]=='l' && buf[1]=='f') || (buf[0]=='s' && buf[2]=='f'))
            c = 'f';
          else
            c = 'r';
        }

        BSET(p->pipes,LSU);
        BSET(p->uses,GPR+q2);
        if (!strncmp(strest(buf,2),"ux",2))  /* update instruction? */
          BSET(p->modifies,GPR+q1);
        else
          BSET(p->uses,GPR+q1);

        if (c == 'r') {
          /* integer load/store */

          if (!strncmp(buf,"lsw",3) || !strncmp(buf,"stsw",4)) {
            /* load/store string word rz,rq1,rq2/imm */
            p->flags = BARRIER;  /* too complex... ;) */
            return (1);
          }
          else {
            /* load/store integer indexed rz,rq1,rq2 */

            p->latency = 3;
            if(*buf == 'l') {
              BSET(p->modifies,GPR+z);
              BSET(p->uses,MEM);
            }
            else{
              if (!strcmp(buf,"stwcx."))
                p->latency = 8;
              BSET(p->uses,GPR+z);
              BSET(p->modifies,MEM);
            }
            return (1);
          }
        }

        else if (c == 'f') {
          /* load/store float indexed fz,rq1,rq2 */
          p->latency = 3;
          if(*buf == 'l') {
            BSET(p->modifies,FPR+z);
            BSET(p->uses,MEM);
          }
          else{
            BSET(p->uses,FPR+z);
            BSET(p->modifies,MEM);
          }
          return (1);
        }
      }
    }

    if (sscanf(p->txt,elf ? " fcm%15s %d,%d,%d" : " fcm%15s cr%d,f%d,f%d",
               buf,&z,&q1,&q2) == 4) {
      /* floating point compare CR0 */
      p->latency = 3;
      BSET(p->pipes,FPU);
      BSET(p->modifies,CCR+z);
      /*BSET(p->modifies,FPSCR);*/
      BSET(p->uses,FPR+q1);
      BSET(p->uses,FPR+q2);
      return (1);
    }
    if (sscanf(p->txt,elf ? " fcm%15s %d,%d" : " fcm%15s f%d,f%d",
               buf,&q1,&q2) == 3) {
      /* floating point compare */
      p->latency = 3;
      BSET(p->pipes,FPU);
      BSET(p->modifies,CCR);
      /*BSET(p->modifies,FPSCR);*/
      BSET(p->uses,FPR+q1);
      BSET(p->uses,FPR+q2);
      return (1);
    }

    if ((n = sscanf(p->txt,elf ? " f%15s %d,%d,%d,%d" :
                    " f%15s f%d,f%d,f%d,f%d",buf,&z,&q1,&q2,&q3))>=3) {
      /* floating point operation with 2, 3 or 4 operands */

#if 0
      if (strncmp(buf,"abs",3) &&
          strncmp(buf,"mr",2) &&
          strncmp(buf,"nabs",4) &&
          strncmp(buf,"neg",3) &&
          strncmp(buf,"sel",3))
        BSET(p->modifies,FPSCR);  /* only some instr. don't alter FPSCR */
#endif

      if (!strncmp(buf,"divs",4) ||
          !strncmp(buf,"res",3))
        p->latency = 18;
      else if (!strncmp(buf,"div",3))
        p->latency = 33;
      else if (!strncmp(buf,"mul",3) ||
               !strncmp(buf,"madd",4) ||
               !strncmp(buf,"msub",4) ||
               !strncmp(buf,"nmadd",5) ||
               !strncmp(buf,"nmsub",5))
        p->latency = 4;
      else
        p->latency = 3;

      if (*(strest(buf,1)) == '.')
        BSET(p->modifies,CCR+1);
      BSET(p->pipes,FPU);
      BSET(p->uses,FPR+q1);
      if (n >= 4) {
        BSET(p->uses,FPR+q2);
        if (n == 5)
          BSET(p->uses,FPR+q3);
      }
      BSET(p->modifies,FPR+z);
      return (1);
    }

    if (sscanf(p->txt,elf ? " cm%15s %d,%d,%d" : " cm%15s cr%d,r%d,r%d",
        buf,&z,&q1,&q2) == 4) {
      /* integer compare instruction */
      p->latency = 1;
      BSET(p->pipes,IU);
      BSET(p->pipes,SRU);
      BSET(p->modifies,CCR+z);
      BSET(p->uses,GPR+q1);
      if (*(strest(buf,1)) != 'i')
        BSET(p->uses,GPR+q2);
      return (1);
    }
    if (sscanf(p->txt,elf ? " cm%15s %d,%d" : " cm%15s r%d,r%d",
        buf,&q1,&q2) == 3) {
      /* integer compare instruction CR0 */
      p->latency = 1;
      BSET(p->pipes,IU);
      BSET(p->pipes,SRU);
      BSET(p->modifies,CCR);
      BSET(p->uses,GPR+q1);
      if (*(strest(buf,1)) != 'i')
        BSET(p->uses,GPR+q2);
      return (1);
    }

    if (!elf) {
      if (sscanf(p->txt," cm%15s cr%d,r%d,%d",buf,&z,&q1,&i) == 4) {
        /* immediate integer compare instruction */
        p->latency = 1;
        BSET(p->pipes,IU);
        BSET(p->pipes,SRU);
        BSET(p->modifies,CCR+z);
        BSET(p->uses,GPR+q1);
        return (1);
      }
      if (sscanf(p->txt," cm%15s r%d,%d",buf,&q1,&i) == 3) {
        /* immediate integer compare instruction CR0 */
        p->latency = 1;
        BSET(p->pipes,IU);
        BSET(p->pipes,SRU);
        BSET(p->modifies,CCR+z);
        BSET(p->uses,GPR+q1);
        return (1);
      }
    }

    if ((n = sscanf(p->txt," cr%15s %d,%d,%d",buf,&z,&q1,&q2)) >= 2) {
      /* condition code register operation (vbcc uses this version) */
      p->latency = 1;
      BSET(p->pipes,SRU);
      BSET(p->modifies,CCR+(z>>4));
      if (n >= 3) {
        BSET(p->uses,CCR+(q1>>4));
        if (n == 4)
          BSET(p->uses,CCR+(q2>>4));
      }
      return (1);
    }

    if (sscanf(p->txt,elf ? " rlw%15s %d,%d,%d,%d,%d" :
               " rlw%15s r%d,r%d,%d,%d,%d",buf,&z,&q1,&i,&n,&x) == 6) {
      /* rotate left: rlwimi, rlwinm, rlwnm r1,r2,x,y,z */
      p->latency = 1;
      BSET(p->pipes,IU);
      if (*(strest(buf,1)) == '.')
        BSET(p->modifies,CCR);
      BSET(p->uses,GPR+q1);
      BSET(p->modifies,GPR+z);
      return (1);
    }

    if (sscanf(p->txt,elf ? " %15s %d,%d,%c" : " %15s r%d,r%d,%c",
        buf,&z,&q1,&c) == 4) {
      /* op r1,r2,imm */
      if (!strncmp(buf,"addi",4) ||
          !strncmp(buf,"andi",4) ||
          !strncmp(buf,"mulli",5) ||
          !strncmp(buf,"ori",3) ||
          !strncmp(buf,"slwi",4) ||
          !strncmp(buf,"srwi",4) ||
          !strncmp(buf,"srawi",5) ||
          !strncmp(buf,"subi",4) ||
          !strncmp(buf,"xori",4)) {
        char *a = strest(buf,1);

        if (*buf == 'm')  /* mulli */
          p->latency = 3;
        else
          p->latency = 1;
        BSET(p->pipes,IU);
        if (!strncmp(buf,"add",3) && *(buf+4)!='c')
          BSET(p->pipes,SRU);  /* addi/addis may also execute in SRU */
        if (*a == '.') {
          BSET(p->modifies,CCR);
          --a;
        }
        if (*a == 'c')
          BSET(p->modifies,XER);
        BSET(p->uses,GPR+q1);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " %15s %d,0,%d" : " %15s r%d,0,r%d",
        buf,&z,&q2) == 3) {
      /* op r1,0,r3 */
      if (!strncmp(buf,"add",3) ||
          !strncmp(buf,"sub",3)) {
        p->latency = 1;
        BSET(p->pipes,IU);
        if (*(strest(buf,1)) == '.')
          BSET(p->modifies,CCR);
        else
          BSET(p->pipes,SRU);  /* add/addo may also execute in SRU */
        BSET(p->uses,GPR+q2);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " %15s %d,%d,%d" : " %15s r%d,r%d,r%d",
        buf,&z,&q1,&q2) == 4) {
      /* op r1,r2,r3 */
      if (!strncmp(buf,"add",3) ||
          !strncmp(buf,"and",3) ||
          !strncmp(buf,"div",3) ||
          !strncmp(buf,"eqv",3) ||
          !strncmp(buf,"mul",3) ||
          !strncmp(buf,"nand",4) ||
          !strncmp(buf,"nor",3) ||
          !strncmp(buf,"or",2) ||
          !strncmp(buf,"sl",2) ||
          !strncmp(buf,"sr",2) ||
          !strncmp(buf,"sub",3) ||
          !strncmp(buf,"xor",3)) {
        char *a = strest(buf,1);

        if (!strncmp(buf,"mul",3)) {
          if (*(buf+5) == 'u')
            p->latency = 6;  /* mulhwu needs 6 cycles */
          else
            p->latency = 5;
          if (*(buf+3) == 'l')
            BSET(p->modifies,XER);
        }
        else if (!strncmp(buf,"div",3)) {
/*          p->latency = 37;*/
          p->latency = 20;  /* 603e has 20 since PID7 */
          BSET(p->modifies,XER);
        }
        else
          p->latency = 1;
        BSET(p->pipes,IU);
        if (!strcmp(buf,"add") || !strcmp(buf,"addo"))
          BSET(p->pipes,SRU);  /* add/addo may also execute in SRU */

        if (*a == '.') {
          BSET(p->modifies,CCR);
          --a;
        }
        if (*a == 'o') {
          BSET(p->modifies,XER);
          --a;
        }
        if (*a == 'c') {
          BSET(p->modifies,XER);
          --a;
        }
        if (*a == 'e')
          BSET(p->uses,XER);
        BSET(p->uses,GPR+q1);
        BSET(p->uses,GPR+q2);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " l%15s %d,%c" : " l%15s r%d,%c",
        buf,&z,&c) == 3) {
      if (*buf == 'i') {
        /* li, lis -> addi, addis */
        p->latency = 1;
        BSET(p->pipes,IU);
        BSET(p->pipes,SRU);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " %15s %d,%d" : " %15s r%d,r%d",
        buf,&z,&q1) == 3) {
      /* op r1,r2 */
      if (!strncmp(buf,"add",3) ||
          !strncmp(buf,"exts",4) ||
          !strncmp(buf,"mr",2) ||
          !strncmp(buf,"neg",3) ||
          !strncmp(buf,"sub",3)) {
        char *a = strest(buf,1);

        p->latency = 1;
        if (*buf=='a' || *buf=='s')
          BSET(p->uses,XER);  /* addme/addze/subfme/subfze/... */
        BSET(p->pipes,IU);
        if (*a == '.') {
          BSET(p->modifies,CCR);
          --a;
        }
        if (*a == 'o') {
          BSET(p->modifies,XER);
        }
        BSET(p->uses,GPR+q1);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf?" m%15s %d":" m%15s r%d",buf,&z) == 2) {
      /* mtxxx, mfxxx: move from/to special registers */
      int reg=0;
  
      if (!strcmp(&buf[1],"xer"))
        reg = XER;
      else if (!strcmp(&buf[1],"ctr"))
        reg = CTR;
      else if (!strcmp(&buf[1],"lr"))
        reg = LR;
      else if (!strncmp(&buf[1],"fs",2))
        reg = FPSCR;
      if (reg) {
        if (reg == FPSCR) {
          p->latency = 3;
          BSET(p->pipes,FPU);
          /*if (*buf == 'f') {
            BSET(p->uses,reg);
            BSET(p->modifies,z);
          }
          else {*/
            BSET(p->uses,z);
            BSET(p->modifies,reg);
          /*}*/
        }
        else {
          BSET(p->pipes,SRU);
          if (*buf == 'f') {
            p->latency = 1;
            BSET(p->uses,reg);
            BSET(p->modifies,z);
          }
          else {
            p->latency = 2;
            BSET(p->uses,z);
            BSET(p->modifies,reg);
          }
        }
        return (1);
      }
    }
  }


  else if (cpu == 604) {
    /* scheduling for the PowerPC 604 */

    if ((!elf && sscanf(p->txt," %15s %c%d,%39s",buf,&c,&z,oper) == 4) ||
        (elf && sscanf(p->txt," %15s %d,%39s",buf,&z,oper) == 3)) {
      if (elf) {
        if ((buf[0]=='l' && buf[1]=='f') || (buf[0]=='s' && buf[2]=='f'))
          c = 'f';
        else
          c = 'r';
      }
      if ((q1 = lsreg(oper))>=0 && (c=='r'||c=='f')) {
        /* load/store instruction ...,d(rQ1) */

        if (!strcmp(buf,"la")) {
          /* la rZ,d(rQ1) is the same as: addi rZ,rQ1,d */
          p->latency = 1;
          BSET(p->pipes,SCIU1);
          BSET(p->pipes,SCIU2);
          BSET(p->uses,GPR+q1);
          BSET(p->modifies,GPR+z);
          return (1);
        }

        BSET(p->pipes,LSU);
        if (*(strest(buf,1)) == 'u')  /* update instruction? */
          BSET(p->modifies,GPR+q1);
        else
          BSET(p->uses,GPR+q1);

        if (c == 'r') {
          /* integer load/store */

          if (!strncmp(strest(buf,2),"mw",2)) {
            /* load/store multiple word rz,i(rq1) */

            p->latency = 2+(32-z);
            if (*buf == 'l') {
              for (x=z; x<32; x++)
                BSET(p->modifies,GPR+x);
              BSET(p->uses,MEM);
            }
            else {
              for (x=z; x<32; x++)
                BSET(p->uses,GPR+x);
              BSET(p->modifies,MEM);
            }
          }
          else {
            /* load/store integer rz,i(rq1) */

            if(*buf == 'l') {
              p->latency = 2;
              BSET(p->modifies,GPR+z);
              BSET(p->uses,MEM);
            }
            else{
              p->latency = 3;
              BSET(p->uses,GPR+z);
              BSET(p->modifies,MEM);
            }
          }
        }

        else {
          /* load/store float fz,d(rQ1) */

          p->latency = 3;
          if(*buf == 'l') {
            BSET(p->modifies,FPR+z);
            BSET(p->uses,MEM);
          }
          else{
            BSET(p->uses,FPR+z);
            BSET(p->modifies,MEM);
          }
        }

        return (1);
      }
    }

    if ((!elf && sscanf(p->txt," %15s %c%d,r%d,r%d",buf,&c,&z,&q1,&q2) == 5) ||
        (elf && sscanf(p->txt," %15s %d,%d,%d",buf,&z,&q1,&q2) == 4)) {
      if (*buf=='l' || (buf[0]=='s' && buf[1]=='t')) {
        if (elf) {
          if ((buf[0]=='l' && buf[1]=='f') || (buf[0]=='s' && buf[2]=='f'))
            c = 'f';
          else
            c = 'r';
        }

        BSET(p->pipes,LSU);
        BSET(p->uses,GPR+q2);
        if (!strncmp(strest(buf,2),"ux",2))  /* update instruction? */
          BSET(p->modifies,GPR+q1);
        else
          BSET(p->uses,GPR+q1);

        if (c == 'r') {
          /* integer load/store */

          if (!strncmp(buf,"lsw",3) || !strncmp(buf,"stsw",4)) {
            /* load/store string word rz,rq1,rq2/imm */
            p->flags = BARRIER;  /* too complex... ;) */
            return (1);
          }
          else {
            /* load/store integer indexed rz,rq1,rq2 */

            if(*buf == 'l') {
              p->latency = 2;
              BSET(p->modifies,GPR+z);
              BSET(p->uses,MEM);
            }
            else{
              p->latency = 3;
              BSET(p->uses,GPR+z);
              BSET(p->modifies,MEM);
            }
            return (1);
          }
        }

        else if (c == 'f') {
          /* load/store float indexed fz,rq1,rq2 */
          p->latency = 3;
          if(*buf == 'l') {
            BSET(p->modifies,FPR+z);
            BSET(p->uses,MEM);
          }
          else{
            BSET(p->uses,FPR+z);
            BSET(p->modifies,MEM);
          }
          return (1);
        }
      }
    }

    if (sscanf(p->txt,elf ? " fcm%15s %d,%d,%d" : " fcm%15s cr%d,f%d,f%d",
               buf,&z,&q1,&q2) == 4) {
      /* floating point compare CR0 */
      p->latency = 3;
      BSET(p->pipes,FPU);
      BSET(p->modifies,CCR+z);
      /*BSET(p->modifies,FPSCR);*/
      BSET(p->uses,FPR+q1);
      BSET(p->uses,FPR+q2);
      return (1);
    }
    if (sscanf(p->txt,elf ? " fcm%15s %d,%d" : " fcm%15s f%d,f%d",
               buf,&q1,&q2) == 3) {
      /* floating point compare */
      p->latency = 3;
      BSET(p->pipes,FPU);
      BSET(p->modifies,CCR);
      /*BSET(p->modifies,FPSCR);*/
      BSET(p->uses,FPR+q1);
      BSET(p->uses,FPR+q2);
      return (1);
    }

    if ((n = sscanf(p->txt,elf ? " f%15s %d,%d,%d,%d" :
                    " f%15s f%d,f%d,f%d,f%d",buf,&z,&q1,&q2,&q3))>=3) {
      /* floating point operation with 2, 3 or 4 operands */

#if 0
      if (strncmp(buf,"abs",3) &&
          strncmp(buf,"mr",2) &&
          strncmp(buf,"nabs",4) &&
          strncmp(buf,"neg",3) &&
          strncmp(buf,"sel",3))
        BSET(p->modifies,FPSCR);  /* only some instr. don't alter FPSCR */
#endif

      if (!strncmp(buf,"divs",4) ||
          !strncmp(buf,"res",3))
        p->latency = 18;
      else if (!strncmp(buf,"div",3))
        p->latency = 32;
      else
        p->latency = 3;

      if (*(strest(buf,1)) == '.')
        BSET(p->modifies,CCR+1);
      BSET(p->pipes,FPU);
      BSET(p->uses,FPR+q1);
      if (n >= 4) {
        BSET(p->uses,FPR+q2);
        if (n == 5)
          BSET(p->uses,FPR+q3);
      }
      BSET(p->modifies,FPR+z);
      return (1);
    }

    if (sscanf(p->txt,elf ? " cm%15s %d,%d,%d" : " cm%15s cr%d,r%d,r%d",
        buf,&z,&q1,&q2) == 4) {
      /* integer compare instruction */
      p->latency = 1;
      BSET(p->pipes,SCIU1);
      BSET(p->pipes,SCIU2);
      BSET(p->modifies,CCR+z);
      BSET(p->uses,GPR+q1);
      if (*(strest(buf,1)) != 'i')
        BSET(p->uses,GPR+q2);
      return (1);
    }
    if (sscanf(p->txt,elf ? " cm%15s %d,%d" : " cm%15s r%d,r%d",
        buf,&q1,&q2) == 3) {
      /* integer compare instruction CR0 */
      p->latency = 1;
      BSET(p->pipes,SCIU1);
      BSET(p->pipes,SCIU2);
      BSET(p->modifies,CCR);
      BSET(p->uses,GPR+q1);
      if (*(strest(buf,1)) != 'i')
        BSET(p->uses,GPR+q2);
      return (1);
    }

    if (!elf) {
      if (sscanf(p->txt," cm%15s cr%d,r%d,%d",buf,&z,&q1,&i) == 4) {
        /* immediate integer compare instruction */
        p->latency = 1;
        BSET(p->pipes,SCIU1);
        BSET(p->pipes,SCIU2);
        BSET(p->modifies,CCR+z);
        BSET(p->uses,GPR+q1);
        return (1);
      }
      if (sscanf(p->txt," cm%15s r%d,%d",buf,&q1,&i) == 3) {
        /* immediate integer compare instruction CR0 */
        p->latency = 1;
        BSET(p->pipes,SCIU1);
        BSET(p->pipes,SCIU2);
        BSET(p->modifies,CCR+z);
        BSET(p->uses,GPR+q1);

        return (1);
      }
    }

    if ((n = sscanf(p->txt," cr%15s %d,%d,%d",buf,&z,&q1,&q2)) >= 2) {
      /* condition code register operation (vbcc uses this version) */
      p->latency = 1;
      BSET(p->pipes,CRU);
      BSET(p->modifies,CCR+(z>>4));
      if (n >= 3) {
        BSET(p->uses,CCR+(q1>>4));
        if (n == 4)
          BSET(p->uses,CCR+(q2>>4));
      }
      return (1);
    }

    if (sscanf(p->txt,elf ? " rlw%15s %d,%d,%d,%d,%d" :
               " rlw%15s r%d,r%d,%d,%d,%d",buf,&z,&q1,&i,&n,&x) == 6) {
      /* rotate left: rlwimi, rlwinm, rlwnm r1,r2,x,y,z */
      p->latency = 1;
      BSET(p->pipes,SCIU1);
      BSET(p->pipes,SCIU2);
      if (*(strest(buf,1)) == '.')
        BSET(p->modifies,CCR);
      BSET(p->uses,GPR+q1);
      BSET(p->modifies,GPR+z);
      return (1);
    }

    if (sscanf(p->txt,elf ? " %15s %d,%d,%c" : " %15s r%d,r%d,%c",
        buf,&z,&q1,&c) == 4) {
      /* op r1,r2,imm */
      if (!strncmp(buf,"addi",4) ||
          !strncmp(buf,"andi",4) ||
          !strncmp(buf,"mulli",5) ||
          !strncmp(buf,"ori",3) ||
          !strncmp(buf,"slwi",4) ||
          !strncmp(buf,"srwi",4) ||
          !strncmp(buf,"srawi",5) ||
          !strncmp(buf,"subi",4) ||
          !strncmp(buf,"xori",4)) {
        char *a = strest(buf,1);

        if (*buf == 'm') { /* mulli */
          p->latency = 3;
          BSET(p->pipes,MCIU);
        }
        else {
          p->latency = 1;
          BSET(p->pipes,SCIU1);
          BSET(p->pipes,SCIU2);
        }
        if (*a == '.') {
          BSET(p->modifies,CCR);
          --a;
        }
        if (*a == 'c')
          BSET(p->modifies,XER);
        BSET(p->uses,GPR+q1);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " %15s %d,0,%d" : " %15s r%d,0,r%d",
        buf,&z,&q2) == 3) {
      /* op r1,0,r3 */
      if (!strncmp(buf,"add",3) ||
          !strncmp(buf,"sub",3)) {
        p->latency = 1;
        BSET(p->pipes,SCIU1);
        BSET(p->pipes,SCIU2);
        if (*(strest(buf,1)) == '.')
          BSET(p->modifies,CCR);
        else
          BSET(p->pipes,SRU);  /* add/addo may also execute in SRU */
        BSET(p->uses,GPR+q2);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " %15s %d,%d,%d" : " %15s r%d,r%d,r%d",
        buf,&z,&q1,&q2) == 4) {
      /* op r1,r2,r3 */
      if (!strncmp(buf,"add",3) ||
          !strncmp(buf,"and",3) ||
          !strncmp(buf,"div",3) ||
          !strncmp(buf,"eqv",3) ||
          !strncmp(buf,"mul",3) ||
          !strncmp(buf,"nand",4) ||
          !strncmp(buf,"nor",3) ||
          !strncmp(buf,"or",2) ||
          !strncmp(buf,"sl",2) ||
          !strncmp(buf,"sr",2) ||
          !strncmp(buf,"sub",3) ||
          !strncmp(buf,"xor",3)) {
        char *a = strest(buf,1);

        if (!strncmp(buf,"mul",3)) {
          p->latency = 4;
          BSET(p->pipes,MCIU);
          if (*(buf+3) == 'l')
            BSET(p->modifies,XER);
        }
        else if (!strncmp(buf,"div",3)) {
          p->latency = 20;
          BSET(p->pipes,MCIU);
          BSET(p->modifies,XER);
        }
        else {
          p->latency = 1;
          BSET(p->pipes,SCIU1);
          BSET(p->pipes,SCIU2);
        }
        if (*a == '.') {
          BSET(p->modifies,CCR);
          --a;
        }
        if (*a == 'o') {
          BSET(p->modifies,XER);
          --a;
        }
        if (*a == 'c') {
          BSET(p->modifies,XER);
          --a;
        }
        if (*a == 'e')
          BSET(p->uses,XER);
        BSET(p->uses,GPR+q1);
        BSET(p->uses,GPR+q2);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " l%15s %d,%c" : " l%15s r%d,%c",
        buf,&z,&c) == 3) {
      if (*buf == 'i') {
        /* li, lis -> addi, addis */
        p->latency = 1;
        BSET(p->pipes,SCIU1);
        BSET(p->pipes,SCIU2);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf ? " %15s %d,%d" : " %15s r%d,r%d",
        buf,&z,&q1) == 3) {
      /* op r1,r2 */
      if (!strncmp(buf,"add",3) ||
          !strncmp(buf,"exts",4) ||
          !strncmp(buf,"mr",2) ||
          !strncmp(buf,"neg",3) ||
          !strncmp(buf,"sub",3)) {
        char *a = strest(buf,1);

        p->latency = 1;
        BSET(p->pipes,SCIU1);
        BSET(p->pipes,SCIU2);
        if (*buf=='a' || *buf=='s')
          BSET(p->uses,XER);  /* addme/addze/subfme/subfze/... */
        if (*a == '.') {
          BSET(p->modifies,CCR);
          --a;
        }
        if (*a == 'o') {
          BSET(p->modifies,XER);
        }
        BSET(p->uses,GPR+q1);
        BSET(p->modifies,GPR+z);
        return (1);
      }
    }

    if (sscanf(p->txt,elf?" m%15s %d":" m%15s r%d",buf,&z) == 2) {
      /* mtxxx, mfxxx: move from/to special registers */
      int reg=0;
  
      if (!strcmp(&buf[1],"xer"))
        reg = XER;
      else if (!strcmp(&buf[1],"ctr"))
        reg = CTR;
      else if (!strcmp(&buf[1],"lr"))
        reg = LR;
      else if (!strncmp(&buf[1],"fs",2))
        reg = FPSCR;
      if (reg) {
        if (reg == FPSCR) {
          p->latency = 3;
          BSET(p->pipes,FPU);
          /*if (*buf == 'f') {
            BSET(p->uses,reg);
            BSET(p->modifies,z);
          }
          else {*/
            BSET(p->uses,z);
            BSET(p->modifies,reg);
          /*}*/
        }
        else {
          BSET(p->pipes,MCIU);
          if (*buf == 'f') {
            p->latency = 3;
            BSET(p->uses,reg);
            BSET(p->modifies,z);
          }
          else {
            p->latency = 1;
            BSET(p->uses,z);
            BSET(p->modifies,reg);
          }
        }
        return (1);
      }
    }
  }

  p->flags = BARRIER;
  return (1);
}
