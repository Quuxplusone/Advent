#include "adv0.h"
#include "adv3.h"
#ifdef __STDC__
void p120(void)
#else
void p120(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(417)) return;
   if (bitest(evar(458),5)) {
   p121();
   }
}
#ifdef __STDC__
void p240(void)
#else
void p240()
#endif
{
   if (value[467]==0) {
   set('V',467,'c',1,NULL,NULL);
   }
   p242();
   if (value[467]>0) {
   if ((((((((isnear(91,-1,-1)) ||
   isnear(19,-1,-1)) ||
   isnear(21,-1,-1)) ||
   isnear(23,-1,-1)) ||
   value[16]>0) ||
   isnear(31,-1,-1)) ||
   value[90]>0) ||
   isnear(24,-1,-1)) {
   say(0,549,0);
   } else {
   if (bitest(evar(458),7)) {
   say(0,550,0);
   } else {
   say(0,548,0);
   bitmod('s',evar(458),7);
   }}}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p292(void)
#else
void p292()
#endif
{
   say(0,637,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p459(void)
#else
void p459()
#endif
{
   if (query(515)) {
   set('V',464,'c',1,NULL,NULL);
   p204();
   }
   say(2,465,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p70(void)
#else
void p70()
#endif
{
   say(0,658,0);
   bitmod('s',evar(446),1);
   bitmod('c',evar(446),2);
   value[465]=484;*bitword(465)=-1;
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p329(void)
#else
void p329()
#endif
{
   if ((value[446]==1) ||
   !(bitest(evar(458),5))) {
   say(0,557,0);
   longjmp(loop_back,1);
   }
   if (bitest(evar(445),13)) {
   say(2,479,0);
   move(value[445],-1);
   } else {
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p301(void)
#else
void p301()
#endif
{
   default_to(0,value[HERE],5);
   if (!anyof(21,91,19,6,71,37,77,76,23,16,
   26,90,24,25,-1)) return;
   if (!isnear(value[445],-1,-1)) return;
   if (KEY(21)) {
   p314();
   } else {
   if (KEY(91)) {
   p310();
   } else {
   if (KEY(19)) {
   p309();
   } else {
   if (KEY(6)) {
   p313();
   } else {
   if (KEY(16)) {
   p307();
   } else {
   if (KEY(37)) {
   p304();
   } else {
   if ((KEY(76)) ||
   KEY(77)) {
   p306();
   } else {
   if (KEY(23)) {
   p312();
   } else {
   if (KEY(71)) {
   p305();
   } else {
   if (KEY(26)) {
   p308();
   } else {
   if (KEY(90)) {
   p311();
   } else {
   if (KEY(24)) {
   p303();
   } else {
   if (KEY(25)) {
   p243();
   }}}}}}}}}}}}}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p302(void)
#else
void p302()
#endif
{
   if (value[446]==1) {
   say(0,540,0);
   longjmp(loop_back,1);
   }
}
#ifdef __STDC__
void p189(void)
#else
void p189()
#endif
{
   bitmod('c',evar(446),1);
   bitmod('s',evar(446),2);
   say(2,465,0);
   value[465]=482;*bitword(465)=-1;
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p215(void)
#else
void p215()
#endif
{
   bitmod('c',evar(446),1);
   bitmod('c',evar(446),2);
   value[465]=484;*bitword(465)=-1;
   say(2,465,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p194(void)
#else
void p194()
#endif
{
   if (!anyof(37,21,71,6,91,19,24,90,-1)) return;
   if (isnear(value[445],-1,-1)) {
   if (KEY(37)) {
   if (isnear(80,-1,-1)) {
   say(0,670,0);
   set('E',37,'c',1,NULL,NULL);
   if (have(80,-1,-1)) {
   }
   apport(80,333);
   if (value[85]==1) {
   set('E',85,'c',0,NULL,NULL);
   }
   } else {
   say(0,600,0);
   }
   } else {
   if (KEY(21)) {
   say(0,685,0);
   } else {
   if (KEY(6)) {
   if (have(71,-1,-1)) {
   say(0,599,0);
   apport(71,333);
   } else {
   say(0,600,0);
   }
   } else {
   if (KEY(91)) {
   say(0,601,0);
   bitmod('s',91,11);
   } else {
   if (KEY(71)) {
   say(0,598,0);
   } else {
   if (KEY(19)) {
   if (value[19]==0) {
   say(0,600,0);
   } else {
   say(0,669,0);
   }
   } else {
   if (KEY(24)) {
   if (value[24]<2) {
   say(0,600,0);
   } else {
   say(0,669,0);
   }
   } else {
   if (KEY(90)) {
   say(0,841,0);
   }}}}}}}}
   } else {
   say(12,522,445);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p195(void)
#else
void p195()
#endif
{
   if (value[446]==1) {
   say(0,506,0);
   } else {
   say(0,608,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p481(void)
#else
void p481()
#endif
{
   set('V',464,'c',1,NULL,NULL);
   p229();
   say(13,730,455);
   say(13,731,456);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p299(void)
#else
void p299()
#endif
{
   say(0,726,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p286(void)
#else
void p286()
#endif
{
   say(0,504,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p425(void)
#else
void p425()
#endif
{
   say(0,504,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p496(void)
#else
void p496()
#endif
{
   say(0,547,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p191(void)
#else
void p191()
#endif
{
   if (!anyof(433,432,431,404,-1)) return;
   set('V',460,'c',0,NULL,NULL);
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p192(void)
#else
void p192()
#endif
{
   set('V',460,'c',1,NULL,NULL);
   say(2,465,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p196(void)
#else
void p196()
#endif
{
   if (value[460]==0) {
   set('V',460,'c',2,NULL,NULL);
   say(2,465,0);
   } else {
   set('V',460,'c',0,NULL,NULL);
   say(0,538,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p206(void)
#else
void p206()
#endif
{
   if (value[460]==1) {
   set('V',460,'c',3,NULL,NULL);
   say(2,465,0);
   } else {
   set('V',460,'c',0,NULL,NULL);
   say(0,538,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p207(void)
#else
void p207()
#endif
{
   if (value[460]==2) {
   if (((location[49]==172)) ||
   (location[49]==334)) {
   say(0,538,0);
   } else {
   if (isnear(49,-1,-1)) {
   set('E',49,'c',1,NULL,NULL);
   } else {
   if ((value[HERE]==172)) {
   set('E',49,'c',0,NULL,NULL);
   } else {
   set('E',49,'c',2,NULL,NULL);
   }}
   if (have(49,-1,-1)) {
   apport(49,HERE);
   }
   say(0,49,0);
   set('E',49,'c',0,NULL,NULL);
   if ((location[49]==333)) {
   bitmod('s',21,10);
   if ((value[21]==1) ||
   value[21]==2) {
   if (isnear(22,-1,-1)) {
   set('E',21,'c',5,NULL,NULL);
   say(0,21,0);
   }
   set('E',21,'c',0,NULL,NULL);
   apport(21,199);
   apport(22,333);
   }}
   apport(49,172);
   }
   } else {
   say(0,538,0);
   }
   set('V',460,'c',0,NULL,NULL);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p216(void)
#else
void p216()
#endif
{
   set('V',460,'c',0,NULL,NULL);
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p591(void)
#else
void p591()
#endif
{
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p453(void)
#else
void p453()
#endif
{
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p202(void)
#else
void p202(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(417)) return;
   if ((bitest(evar(442),4)) &&
   !(bitest(100,1))) {
   say(0,553,0);
   } else {
   say(0,554,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p203(void)
#else
void p203()
#endif
{
   if (value[446]>1) {
   if (bitest(evar(445),15)) {
   if (isnear(value[445],-1,-1)) {
   if (have(value[445],-1,-1)) {
   say(0,592,0);
   } else {
   say(0,633,0);
   }
   } else {
   say(0,555,0);
   }
   longjmp(loop_back,1);
   } else {
   if (bitest(evar(445),13)) {
   if ((value[HERE]==value[445])) {
   say(0,635,0);
   } else {
   say(0,555,0);
   }
   } else {
   say(0,557,0);
   }
   longjmp(loop_back,1);
   }}
}
#ifdef __STDC__
void p526(void)
#else
void p526()
#endif
{
   say(0,642,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p63(void)
#else
void p63(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(52)) return;
   if (!isnear(52,-1,-1)) return;
   say(0,702,0);
   if (have(52,-1,-1)) {
   }
   apport(52,333);
   apport(53,value[442]);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p64(void)
#else
void p64(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(88)) return;
   if (!isnear(88,-1,-1)) return;
   p65();
}
#ifdef __STDC__
void p205(void)
#else
void p205(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(52)) return;
   if (!isnear(53,-1,-1)) return;
   say(0,641,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p198(void)
#else
void p198(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(52)) return;
   if (!isnear(52,-1,-1)) return;
   if ((bitest(evar(442),6)) ||
   (value[HERE]==113)) {
   say(0,640,0);
   if (have(52,-1,-1)) {
   }
   apport(52,333);
   apport(53,value[442]);
   } else {
   say(0,639,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p199(void)
#else
void p199(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(81)) return;
   if (!isnear(81,-1,-1)) return;
   if (value[81]==1) {
   if (bitest(evar(442),6)) {
   say(0,605,0);
   set('E',81,'c',0,NULL,NULL);
   if (have(81,-1,-1)) {
   apport(82,INHAND);
   }
   } else {
   if ((value[HERE]==113)) {
   say(0,606,0);
   set('E',81,'c',2,NULL,NULL);
   if (have(81,-1,-1)) {
   apport(83,INHAND);
   }
   } else {
   say(0,604,0);
   }}
   } else {
   say(0,603,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p200(void)
#else
void p200()
#endif
{
   if (value[446]>1) {
   say(0,607,0);
   longjmp(loop_back,1);
   }
}
#ifdef __STDC__
void p456(void)
#else
void p456(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(82)) return;
   if (have(82,-1,-1)) {
   apport(82,333);
   set('E',81,'c',1,NULL,NULL);
   if (isnear(10,-1,-1)) {
   value[10] += 1;
   say(0,10,0);
   value[10] += 1;
   if (value[10]==6) {
   set('E',10,'c',0,NULL,NULL);
   bitmod('s',11,7);
   } else {
   bitmod('c',11,7);
   }
   move(value[442],-1);
   set('E',11,'e',10,NULL,NULL);
   } else {
   if (isnear(5,-1,-1)) {
   say(0,611,0);
   set('E',5,'c',0,NULL,NULL);
   } else {
   say(0,575,0);
   bitmod('s',evar(442),12);
   }}
   } else {
   say(12,717,82);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p457(void)
#else
void p457(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(83)) return;
   if (have(83,-1,-1)) {
   apport(83,333);
   set('E',81,'c',1,NULL,NULL);
   if (isnear(10,-1,-1)) {
   say(0,610,0);
   } else {
   if (isnear(5,-1,-1)) {
   say(0,612,0);
   set('E',5,'c',1,NULL,NULL);
   } else {
   say(0,575,0);
   bitmod('s',evar(442),12);
   }}
   } else {
   say(12,717,445);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p440(void)
#else
void p440()
#endif
{
   if (!anyof(91,6,71,19,21,37,18,23,24,90,-1)) return;
   if (!isnear(value[445],-1,-1)) return;
   say(0,507,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p174(void)
#else
void p174()
#endif
{
   default_to(0,value[HERE],8);
   if (!isnear(89,-1,-1)) return;
   if ((value[446]==1) ||
   KEY(89)) {
   if (have(89,-1,-1)) {
   apport(89,HERE);
   }
   set('E',89,'c',2,NULL,NULL);
   say(0,89,0);
   set('V',469,'c',30,NULL,NULL);
   value[469] += value[471];
   apport(89,333);
   set('V',468,'c',12,NULL,NULL);
   longjmp(loop_back,1);
   }
}
#ifdef __STDC__
void p175(void)
#else
void p175()
#endif
{
   if (!isnear(80,-1,-1)) return;
   if ((value[446]==1) ||
   KEY(80)) {
   if (have(80,-1,-1)) {
   }
   apport(80,333);
   say(0,568,0);
   longjmp(loop_back,1);
   }
}
#ifdef __STDC__
void p176(void)
#else
void p176()
#endif
{
   if (!anyof(91,19,71,6,37,21,10,23,
   24,90,-1)) return;
   if (isnear(value[445],-1,-1)) {
   say(0,567,0);
   } else {
   say(12,522,445);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p177(void)
#else
void p177()
#endif
{
   if (value[446]==1) {
   say(0,676,0);
   longjmp(loop_back,1);
   } else {
   if (bitest(evar(445),15)) {
   if (isnear(value[445],-1,-1)) {
   say(0,574,0);
   longjmp(loop_back,1);
   }}}
}
#ifdef __STDC__
void p472(void)
#else
void p472(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(69)) return;
   if (!isnear(69,-1,-1)) return;
   say(12,572,445);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p473(void)
#else
void p473()
#endif
{
   if (value[446]>1) {
   if (bitest(evar(445),15)) {
   if (isnear(value[445],-1,-1)) {
   say(0,573,0);
   longjmp(loop_back,1);
   }}}
}
#ifdef __STDC__
void p41(void)
#else
void p41()
#endif
{
   if (((bitest(evar(442),3)) ||
   bitest(evar(443),3)) ||
   value[443]==0) {
   say(0,589,0);
   } else {
   move(value[443],-1);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p374(void)
#else
void p374()
#endif
{
   say(0,565,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p479(void)
#else
void p479()
#endif
{
   if (!anyof(425,421,179,422,428,427,424,
   429,420,430,423,435,426,-1)) return;
   if (value[445]<LPROC && value[445]>= 0) (*procs[value[445]])(); else pcall(value[445]);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p480(void)
#else
void p480()
#endif
{
   if (value[446]==2) {
   longjmp(loop_back,1);
   }
}
#ifdef __STDC__
void p477(void)
#else
void p477()
#endif
{
   if (bitest(evar(458),0)) {
   say(0,704,0);
   } else {
   set('V',448,'c',1,NULL,NULL);
   if ((0>0) &&
   !(bitest(evar(458),5))) {
   say(9,745,0);
   if (!(query(703))) {
   set('V',448,'c',0,NULL,NULL);
   }}
   if (value[448]>0) {
   svar(4,&value[466]);
   value[466] *= 60;
   svar(5,&value[448]);
   value[466] += value[448];
   special(1,&value[448]);
   if (value[448]==0) {
   if ((bitest(evar(458),5)) ||
   0==0) {
   if (query(750)) {
   say(2,465,0);
   say(0,483,0);
   longjmp(loop_back,1);
   }}
   say(2,465,0);
   say(0,483,0);
   finita();
   } else {
   say(0,746,0);
   }
   } else {
   say(2,465,0);
   }}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p468(void)
#else
void p468()
#endif
{
   special(5,&value[448]);
   if (value[448]>0) {
   if (!(bitest(evar(458),5))) {
   value[449]=764;*bitword(449)=-1;
   value[449] += value[448];
   say(2,449,0);
   longjmp(loop_back,1);
   }}
   if (bitest(evar(458),5)) {
   set('V',448,'c',1,NULL,NULL);
   } else {
   set('V',448,'c',0,NULL,NULL);
   }
   special(7,&value[448]);
   special(2,&value[448]);
   if (value[448]==0) {
   svar(4,&value[448]);
   svar(5,&value[449]);
   value[448] *= 60;
   value[448] += value[449];
   if (value[448]<value[466]) {
   value[448] += 720;
   value[448] += 720;
   }
   value[448] -= value[466];
   special(8,&value[449]);
   if (value[449]==1) {
   bitmod('s',evar(458),5);
   } else {
   bitmod('c',evar(458),5);
   }
   if ((bitest(evar(458),5)) ||
   value[448]>=0) {
   if (value[69]==1) {
   value[447] += 1;
   }
   if ((bitest(evar(458),5)) ||
   0==0) {
   if (!(query(749))) {
   special(3,&value[448]);
   }
   } else {
   special(3,&value[448]);
   }
   special(4,&value[448]);
   move(value[442],-1);
   say(2,465,0);
   say(0,483,0);
   } else {
   say(9,751,0);
   say(0,483,0);
   finita();
   }
   } else {
   if (value[448]==1) {
   say(0,747,0);
   value[457] -= 1;
   } else {
   say(0,748,0);
   special(3,&value[448]);
   }}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p154(void)
#else
void p154()
#endif
{
   if ((value[446]==1) ||
   KEY(82)) {
   if (bitest(evar(442),6)) {
   say(0,569,0);
   longjmp(loop_back,1);
   } else {
   if ((isnear(81,-1,-1)) &&
   value[81]==0) {
   say(0,571,0);
   apport(82,333);
   set('E',81,'c',1,NULL,NULL);
   longjmp(loop_back,1);
   }}
   if (KEY(82)) {
   say(12,522,445);
   } else {
   say(0,754,0);
   }
   } else {
   if (bitest(evar(445),15)) {
   if (isnear(value[445],-1,-1)) {
   say(0,608,0);
   } else {
   say(12,522,445);
   }
   } else {
   say(0,608,0);
   }}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p394(void)
#else
void p394()
#endif
{
   say(0,486,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p461(void)
#else
void p461()
#endif
{
   if (value[446]>1) {
   if (bitest(evar(445),15)) {
   if (isnear(value[445],-1,-1)) {
   if (KEY(78)) {
   p463();
   } else {
   if (KEY(28)) {
   p464();
   } else {
   if (KEY(8)) {
   p465();
   } else {
   say(12,744,444);
   }}}
   } else {
   say(12,522,445);
   }
   } else {
   say(0,608,0);
   }
   } else {
   return;
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p244(void)
#else
void p244()
#endif
{
   say(0,767,0);
   special(6,&value[448]);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p590(void)
#else
void p590()
#endif
{
   if (query(768)) {
   say(0,769,0);
   input(0);
   say(0,770,0);
   value[454] += 10;
   } else {
   say(2,465,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p138(void)
#else
void p138()
#endif
{
   if (value[476]==0) {
   set('V',476,'c',1,NULL,NULL);
   } else {
   set('V',476,'c',-1,NULL,NULL);
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p238(void)
#else
void p238()
#endif
{
   if (value[476]==1) {
   set('V',476,'c',2,NULL,NULL);
   } else {
   set('V',476,'c',-1,NULL,NULL);
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p282(void)
#else
void p282()
#endif
{
   if (value[476]==2) {
   set('V',476,'c',3,NULL,NULL);
   } else {
   set('V',476,'c',-1,NULL,NULL);
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p502(void)
#else
void p502()
#endif
{
   if (value[476]==3) {
   set('V',476,'c',4,NULL,NULL);
   } else {
   set('V',476,'c',-1,NULL,NULL);
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p217(void)
#else
void p217()
#endif
{
   if (value[476]==4) {
   set('V',476,'c',-1,NULL,NULL);
   bitmod('s',evar(458),5);
   bitmod('c',evar(458),0);
   say(0,484,0);
   } else {
   set('V',476,'c',-1,NULL,NULL);
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p112(void)
#else
void p112()
#endif
{
   say(0,713,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p326(void)
#else
void p326()
#endif
{
   say(0,564,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p370(void)
#else
void p370()
#endif
{
   if ((isnear(30,-1,-1)) &&
   value[30]==0) {
   set('E',30,'c',1,NULL,NULL);
   say(0,830,0);
   } else {
   say(0,538,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p403(void)
#else
void p403(typ0,par0)
int typ0;int par0;
#endif
{
if (!KEY(424)) return;
   if ((((bitest(69,10)) ||
   bitest(evar(442),0)) ||
   !(bitest(278,1))) ||
   !isnear(69,-1,-1)) {
   say(0,538,0);
   } else {
   if (have(69,-1,-1)) {
   say(0,776,0);
   p124();
   } else {
   if (value[447]>40) {
   apport(69,334);
   set('E',38,'c',1,NULL,NULL);
   set('E',69,'c',0,NULL,NULL);
   set('V',447,'c',0,NULL,NULL);
   if (irand(100)<50) {
   say(0,777,0);
   say(0,509,0);
   } else {
   say(0,778,0);
   p124();
   }
   } else {
   say(0,779,0);
   value[447] += 150;
   set('E',69,'c',1,NULL,NULL);
   bitmod('s',69,10);
   }}}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p404(void)
#else
void p404()
#endif
{
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p475(void)
#else
void p475()
#endif
{
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p540(void)
#else
void p540()
#endif
{
   say(0,538,0);
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p316(void)
#else
void p316()
#endif
{
   p426();
}
#ifdef __STDC__
void p595(void)
#else
void p595()
#endif
{
   p426();
}
#ifdef __STDC__
void p315(void)
#else
void p315()
#endif
{
   p426();
}
#ifdef __STDC__
void p510(void)
#else
void p510()
#endif
{
   p426();
}
#ifdef __STDC__
void p60(void)
#else
void p60()
#endif
{
   p426();
}
#ifdef __STDC__
void p469(void)
#else
void p469()
#endif
{
   if (((value[446]==1) ||
   KEY(27)) &&
   isnear(27,-1,-1)) {
   say(0,854,0);
   move(191,-1);
   apport(27,333);
   longjmp(loop_back,1);
   }
}
#ifdef __STDC__
void p434(void)
#else
void p434()
#endif
{
   bitmod('s',26,11);
   if (bitest(evar(442),4)) {
   say(0,538,0);
   } else {
   if (((isnear(81,-1,-1)) &&
   value[81]==0) ||
   bitest(evar(442),6)) {
   if (irand(100)<85) {
   say(0,538,0);
   } else {
   if (irand(100)<95) {
   say(0,864,0);
   p124();
   } else {
   say(0,865,0);
   finita();
   }}
   } else {
   if ((isnear(85,-1,-1)) ||
   isnear(86,-1,-1)) {
   if (isnear(85,-1,-1)) {
   say(0,866,0);
   if (have(85,-1,-1)) {
   }
   apport(85,333);
   }
   if (isnear(86,-1,-1)) {
   say(0,867,0);
   if (have(86,-1,-1)) {
   }
   apport(86,333);
   }
   } else {
   value[448] = irand(3);
   value[448] *= 2;
   if (value[91]>1) {
   value[448] += 1;
   }
   if (isnear(91,-1,-1)) {
   if (irand(100)<70) {
   value[449]=868;*bitword(449)=-1;
   value[448] += value[449];
   say(2,448,0);
   apport(91,333);
   value[461] -= value[91];
   set('E',91,'c',0,NULL,NULL);
   } else {
   value[449]=874;*bitword(449)=-1;
   value[448] += value[449];
   say(2,448,0);
   p124();
   }
   } else {
   say(0,538,0);
   }}}}
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p474(void)
#else
void p474()
#endif
{
   if (bitest(evar(458),5)) {
   set('V',462,'c',0,NULL,NULL);
   move(value[442],-1);
   say(2,465,0);
   } else {
   say(0,557,0);
   }
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p2(void)
#else
void p2()
#endif
{
   say(0,483,0);
   *bitword(448)= -1; value[448]=FOBJ-1; while (++value[448]<=LOBJ) {
   apport(value[448],333);
   value[449] = irand(100);
   }
}
#ifdef __STDC__
void p3(void)
#else
void p3()
#endif
{
   bitmod('s',68,0);
   bitmod('s',69,0);
   bitmod('s',70,0);
   bitmod('s',72,0);
   bitmod('s',74,0);
   bitmod('s',71,0);
   bitmod('s',75,0);
   bitmod('s',76,0);
   bitmod('s',77,0);
   bitmod('s',78,0);
   bitmod('s',79,0);
   bitmod('s',80,0);
   bitmod('s',81,0);
   bitmod('s',82,0);
   bitmod('s',83,0);
   bitmod('s',85,0);
   bitmod('s',37,0);
   bitmod('s',38,0);
   bitmod('s',43,0);
   bitmod('s',44,0);
   bitmod('s',45,0);
   bitmod('s',46,0);
   bitmod('s',47,0);
   bitmod('s',48,0);
   bitmod('s',49,0);
   bitmod('s',50,0);
   bitmod('s',52,0);
   bitmod('s',53,0);
   bitmod('s',54,0);
   bitmod('s',57,0);
   bitmod('s',58,0);
   bitmod('s',59,0);
   bitmod('s',60,0);
   bitmod('s',62,0);
   bitmod('s',63,0);
   bitmod('s',64,0);
   bitmod('s',41,0);
   bitmod('s',51,0);
   bitmod('s',87,0);
   bitmod('s',88,0);
   bitmod('s',89,0);
   bitmod('s',65,0);
   bitmod('s',66,0);
   bitmod('s',67,0);
   bitmod('s',86,0);
   bitmod('s',42,0);
   bitmod('s',55,0);
   bitmod('s',56,0);
   bitmod('s',84,0);
   bitmod('s',73,0);
   bitmod('s',61,0);
}
#ifdef __STDC__
void p4(void)
#else
void p4()
#endif
{
   bitmod('s',71,9);
   bitmod('s',83,9);
   bitmod('s',82,9);
   bitmod('s',37,9);
}
#ifdef __STDC__
void p5(void)
#else
void p5()
#endif
{
   bitmod('s',92,0);
   bitmod('s',93,0);
   bitmod('s',94,0);
   bitmod('s',95,0);
   bitmod('s',96,0);
   bitmod('s',97,0);
   bitmod('s',98,0);
   bitmod('s',195,0);
   bitmod('s',99,0);
   bitmod('s',100,0);
   bitmod('s',101,0);
   bitmod('s',179,0);
   bitmod('s',207,0);
   bitmod('s',278,0);
   bitmod('s',330,0);
   bitmod('s',331,0);
   bitmod('s',211,0);
   bitmod('s',212,0);
   bitmod('s',208,0);
   bitmod('s',209,0);
   bitmod('s',245,0);
   bitmod('s',246,0);
   bitmod('s',250,0);
   bitmod('s',332,0);
   bitmod('s',196,0);
   bitmod('s',197,0);
   bitmod('s',194,0);
}
#ifdef __STDC__
void p6(void)
#else
void p6()
#endif
{
   bitmod('s',100,2);
   bitmod('s',101,2);
   bitmod('s',178,2);
   bitmod('s',179,2);
   bitmod('s',180,2);
   bitmod('s',199,2);
   bitmod('s',200,2);
   bitmod('s',204,2);
   bitmod('s',205,2);
   bitmod('s',206,2);
   bitmod('s',207,2);
   bitmod('s',251,2);
   bitmod('s',252,2);
   bitmod('s',253,2);
   bitmod('s',254,2);
   bitmod('s',198,2);
   bitmod('s',113,2);
   bitmod('s',114,2);
   bitmod('s',124,2);
   bitmod('s',208,2);
   bitmod('s',209,2);
   bitmod('s',210,2);
   bitmod('s',211,2);
   bitmod('s',212,2);
   bitmod('s',213,2);
   bitmod('s',214,2);
   bitmod('s',215,2);
   bitmod('s',216,2);
   bitmod('s',217,2);
   bitmod('s',218,2);
   bitmod('s',219,2);
   bitmod('s',220,2);
   bitmod('s',268,2);
   bitmod('s',221,2);
   bitmod('s',222,2);
   bitmod('s',223,2);
   bitmod('s',224,2);
   bitmod('s',225,2);
   bitmod('s',226,2);
   bitmod('s',227,2);
   bitmod('s',228,2);
   bitmod('s',229,2);
   bitmod('s',230,2);
   bitmod('s',231,2);
   bitmod('s',232,2);
   bitmod('s',233,2);
   bitmod('s',234,2);
   bitmod('s',235,2);
   bitmod('s',236,2);
   bitmod('s',237,2);
   bitmod('s',238,2);
   bitmod('s',239,2);
   bitmod('s',240,2);
   bitmod('s',241,2);
   bitmod('s',242,2);
   bitmod('s',243,2);
   bitmod('s',244,2);
   bitmod('s',245,2);
   bitmod('s',246,2);
   bitmod('s',247,2);
   bitmod('s',248,2);
   bitmod('s',249,2);
   bitmod('s',250,2);
   bitmod('s',192,2);
   bitmod('s',193,2);
   bitmod('s',194,2);
   bitmod('s',269,2);
}
#ifdef __STDC__
void p7(void)
#else
void p7()
#endif
{
   bitmod('s',92,4);
   bitmod('s',93,4);
   bitmod('s',94,4);
   bitmod('s',95,4);
   bitmod('s',96,4);
   bitmod('s',97,4);
   bitmod('s',98,4);
   bitmod('s',99,4);
   bitmod('s',195,4);
}
#ifdef __STDC__
void p8(void)
#else
void p8()
#endif
{
   bitmod('s',128,3);
   bitmod('s',129,3);
   bitmod('s',130,3);
   bitmod('s',131,3);
   bitmod('s',132,3);
   bitmod('s',133,3);
   bitmod('s',134,3);
   bitmod('s',135,3);
   bitmod('s',136,3);
   bitmod('s',137,3);
   bitmod('s',138,3);
   bitmod('s',139,3);
   bitmod('s',140,3);
   bitmod('s',141,3);
   bitmod('s',142,3);
   bitmod('s',143,3);
   bitmod('s',162,3);
   bitmod('s',163,3);
   bitmod('s',164,3);
   bitmod('s',165,3);
   bitmod('s',166,3);
   bitmod('s',167,3);
   bitmod('s',168,3);
   bitmod('s',169,3);
   bitmod('s',190,3);
   bitmod('s',198,3);
   bitmod('s',255,3);
   bitmod('s',256,3);
   bitmod('s',257,3);
   bitmod('s',258,3);
   bitmod('s',259,3);
   bitmod('s',260,3);
   bitmod('s',261,3);
   bitmod('s',262,3);
   bitmod('s',263,3);
   bitmod('s',264,3);
   bitmod('s',182,3);
   bitmod('s',324,3);
   bitmod('s',325,3);
   bitmod('s',192,3);
   bitmod('s',171,3);
   bitmod('s',189,3);
   bitmod('s',199,3);
   bitmod('s',200,3);
   bitmod('s',187,3);
   bitmod('s',178,3);
   bitmod('s',179,3);
   bitmod('s',180,3);
   bitmod('s',158,3);
   bitmod('s',283,3);
   bitmod('s',285,3);
   bitmod('s',287,3);
   bitmod('s',289,3);
   bitmod('s',290,3);
   bitmod('s',291,3);
   bitmod('s',292,3);
   bitmod('s',293,3);
   bitmod('s',294,3);
   bitmod('s',295,3);
   bitmod('s',296,3);
   bitmod('s',297,3);
   bitmod('s',299,3);
   bitmod('s',300,3);
   bitmod('s',301,3);
   bitmod('s',303,3);
   bitmod('s',304,3);
   bitmod('s',305,3);
   bitmod('s',306,3);
   bitmod('s',307,3);
   bitmod('s',308,3);
   bitmod('s',282,3);
   bitmod('s',281,3);
   bitmod('s',226,3);
   bitmod('s',227,3);
   bitmod('s',228,3);
   bitmod('s',229,3);
   bitmod('s',230,3);
   bitmod('s',231,3);
   bitmod('s',232,3);
   bitmod('s',233,3);
   bitmod('s',234,3);
   bitmod('s',235,3);
   bitmod('s',236,3);
   bitmod('s',237,3);
   bitmod('s',238,3);
   bitmod('s',239,3);
   bitmod('s',240,3);
   bitmod('s',241,3);
   bitmod('s',242,3);
   bitmod('s',243,3);
   bitmod('s',244,3);
   bitmod('s',208,3);
   bitmod('s',210,3);
   bitmod('s',211,3);
   bitmod('s',218,3);
   bitmod('s',196,3);
   bitmod('s',332,3);
}
#ifdef __STDC__
void p9(void)
#else
void p9()
#endif
{
   bitmod('s',127,7);
   bitmod('s',128,7);
   bitmod('s',129,7);
   bitmod('s',130,7);
   bitmod('s',131,7);
   bitmod('s',132,7);
   bitmod('s',133,7);
   bitmod('s',134,7);
   bitmod('s',135,7);
   bitmod('s',136,7);
   bitmod('s',137,7);
   bitmod('s',138,7);
   bitmod('s',139,7);
   bitmod('s',140,7);
   bitmod('s',141,7);
   bitmod('s',142,7);
   bitmod('s',143,7);
   bitmod('s',162,7);
   bitmod('s',163,7);
   bitmod('s',164,7);
   bitmod('s',165,7);
   bitmod('s',166,7);
   bitmod('s',167,7);
   bitmod('s',168,7);
   bitmod('s',169,7);
   bitmod('s',186,7);
   bitmod('s',190,7);
   bitmod('s',198,7);
   bitmod('s',255,7);
   bitmod('s',256,7);
   bitmod('s',257,7);
   bitmod('s',258,7);
   bitmod('s',259,7);
   bitmod('s',260,7);
   bitmod('s',261,7);
   bitmod('s',262,7);
   bitmod('s',263,7);
   bitmod('s',264,7);
   bitmod('s',282,7);
   bitmod('s',283,7);
   bitmod('s',284,7);
   bitmod('s',285,7);
   bitmod('s',286,7);
   bitmod('s',287,7);
   bitmod('s',288,7);
   bitmod('s',289,7);
   bitmod('s',290,7);
   bitmod('s',291,7);
   bitmod('s',292,7);
   bitmod('s',293,7);
   bitmod('s',294,7);
   bitmod('s',295,7);
   bitmod('s',296,7);
   bitmod('s',297,7);
   bitmod('s',298,7);
   bitmod('s',299,7);
   bitmod('s',300,7);
   bitmod('s',301,7);
   bitmod('s',302,7);
   bitmod('s',303,7);
   bitmod('s',304,7);
   bitmod('s',305,7);
   bitmod('s',306,7);
   bitmod('s',307,7);
   bitmod('s',308,7);
   bitmod('s',309,7);
   bitmod('s',310,7);
   bitmod('s',311,7);
   bitmod('s',312,7);
   bitmod('s',313,7);
   bitmod('s',314,7);
   bitmod('s',315,7);
   bitmod('s',316,7);
   bitmod('s',317,7);
   bitmod('s',318,7);
}
#ifdef __STDC__
void p10(void)
#else
void p10()
#endif
{
   bitmod('s',109,8);
   bitmod('s',116,8);
   bitmod('s',265,8);
   bitmod('s',120,8);
   bitmod('s',121,8);
   bitmod('s',131,8);
   bitmod('s',132,8);
   bitmod('s',133,8);
   bitmod('s',134,8);
   bitmod('s',139,8);
   bitmod('s',141,8);
   bitmod('s',143,8);
   bitmod('s',147,8);
   bitmod('s',156,8);
   bitmod('s',161,8);
   bitmod('s',163,8);
   bitmod('s',164,8);
   bitmod('s',167,8);
   bitmod('s',168,8);
   bitmod('s',175,8);
   bitmod('s',184,8);
   bitmod('s',191,8);
   bitmod('s',194,8);
   bitmod('s',264,8);
   bitmod('s',269,8);
   bitmod('s',272,8);
   bitmod('s',274,8);
   bitmod('s',275,8);
   bitmod('s',284,8);
   bitmod('s',288,8);
   bitmod('s',292,8);
   bitmod('s',293,8);
   bitmod('s',299,8);
   bitmod('s',300,8);
   bitmod('s',323,8);
   bitmod('s',331,8);
}
#ifdef __STDC__
void p11(void)
#else
void p11()
#endif
{
   bitmod('s',3,6);
   bitmod('s',5,6);
   bitmod('s',76,6);
   bitmod('s',77,6);
   bitmod('s',84,6);
   bitmod('s',62,6);
   bitmod('s',34,6);
   bitmod('s',88,6);
}
#ifdef __STDC__
void p12(void)
#else
void p12()
#endif
{
   bitmod('s',4,7);
   bitmod('s',7,7);
   bitmod('s',9,7);
   bitmod('s',12,7);
   bitmod('s',17,7);
   bitmod('s',18,7);
   bitmod('s',30,7);
   bitmod('s',29,7);
   bitmod('s',31,7);
   bitmod('s',35,7);
   bitmod('s',39,7);
   bitmod('s',91,7);
   bitmod('s',79,7);
   bitmod('s',13,7);
   bitmod('s',11,7);
}
#ifdef __STDC__
void p13(void)
#else
void p13()
#endif
{
   bitmod('s',127,5);
   bitmod('s',128,5);
   bitmod('s',129,5);
   bitmod('s',130,5);
   bitmod('s',131,5);
   bitmod('s',132,5);
   bitmod('s',133,5);
   bitmod('s',134,5);
   bitmod('s',135,5);
   bitmod('s',136,5);
   bitmod('s',137,5);
   bitmod('s',138,5);
   bitmod('s',139,5);
   bitmod('s',140,5);
   bitmod('s',141,5);
   bitmod('s',142,5);
   bitmod('s',143,5);
   bitmod('s',162,5);
   bitmod('s',163,5);
   bitmod('s',164,5);
   bitmod('s',165,5);
   bitmod('s',166,5);
   bitmod('s',167,5);
   bitmod('s',168,5);
   bitmod('s',169,5);
   bitmod('s',186,5);
   bitmod('s',190,5);
   bitmod('s',198,5);
   bitmod('s',255,5);
   bitmod('s',256,5);
   bitmod('s',257,5);
   bitmod('s',258,5);
   bitmod('s',259,5);
   bitmod('s',260,5);
   bitmod('s',261,5);
   bitmod('s',262,5);
   bitmod('s',263,5);
   bitmod('s',264,5);
   bitmod('s',104,5);
   bitmod('s',99,5);
   bitmod('s',110,5);
   bitmod('s',187,5);
   bitmod('s',179,5);
   bitmod('s',178,5);
   bitmod('s',180,5);
   bitmod('s',282,5);
   bitmod('s',283,5);
   bitmod('s',284,5);
   bitmod('s',285,5);
   bitmod('s',286,5);
   bitmod('s',287,5);
   bitmod('s',288,5);
   bitmod('s',289,5);
   bitmod('s',290,5);
   bitmod('s',291,5);
   bitmod('s',292,5);
   bitmod('s',293,5);
   bitmod('s',294,5);
   bitmod('s',295,5);
   bitmod('s',296,5);
   bitmod('s',297,5);
   bitmod('s',298,5);
   bitmod('s',299,5);
   bitmod('s',300,5);
   bitmod('s',301,5);
   bitmod('s',302,5);
   bitmod('s',303,5);
   bitmod('s',304,5);
   bitmod('s',305,5);
   bitmod('s',306,5);
   bitmod('s',307,5);
   bitmod('s',308,5);
   bitmod('s',309,5);
   bitmod('s',310,5);
   bitmod('s',311,5);
   bitmod('s',312,5);
   bitmod('s',313,5);
   bitmod('s',314,5);
   bitmod('s',315,5);
   bitmod('s',316,5);
   bitmod('s',317,5);
   bitmod('s',318,5);
   bitmod('s',211,5);
}
#ifdef __STDC__
void p14(void)
#else
void p14()
#endif
{
   bitmod('s',94,6);
   bitmod('s',92,6);
   bitmod('s',95,6);
   bitmod('s',98,6);
   bitmod('s',124,6);
   bitmod('s',174,6);
   bitmod('s',191,6);
   bitmod('s',192,6);
}
#ifdef __STDC__
void p15(void)
#else
void p15()
#endif
{
   bitmod('s',105,9);
   bitmod('s',107,9);
   bitmod('s',108,9);
   bitmod('s',111,9);
   bitmod('s',112,9);
   bitmod('s',115,9);
   bitmod('s',120,9);
   bitmod('s',121,9);
   bitmod('s',123,9);
   bitmod('s',125,9);
   bitmod('s',142,9);
   bitmod('s',152,9);
   bitmod('s',153,9);
   bitmod('s',157,9);
   bitmod('s',171,9);
   bitmod('s',174,9);
   bitmod('s',177,9);
   bitmod('s',191,9);
   bitmod('s',192,9);
   bitmod('s',189,9);
   bitmod('s',194,9);
   bitmod('s',199,9);
   bitmod('s',200,9);
   bitmod('s',207,9);
   bitmod('s',208,9);
   bitmod('s',216,9);
   bitmod('s',217,9);
   bitmod('s',220,9);
   bitmod('s',221,9);
   bitmod('s',222,9);
   bitmod('s',279,9);
   bitmod('s',280,9);
   bitmod('s',281,9);
   bitmod('s',319,9);
   bitmod('s',330,9);
   bitmod('s',332,9);
}
#ifdef __STDC__
void p16(void)
#else
void p16()
#endif
{
   bitmod('s',43,2);
   bitmod('s',44,2);
   bitmod('s',45,2);
   bitmod('s',46,2);
   bitmod('s',47,2);
   bitmod('s',48,2);
   bitmod('s',49,2);
   bitmod('s',50,2);
   bitmod('s',52,2);
   bitmod('s',54,2);
   bitmod('s',57,2);
   bitmod('s',58,2);
   bitmod('s',59,2);
   bitmod('s',60,2);
   bitmod('s',62,2);
   bitmod('s',67,2);
   bitmod('s',65,2);
   bitmod('s',66,2);
   bitmod('s',63,2);
   bitmod('s',64,2);
   bitmod('s',41,2);
   bitmod('s',51,2);
   bitmod('s',42,2);
   bitmod('s',55,2);
   bitmod('s',56,2);
   bitmod('s',61,2);
}
#ifdef __STDC__
void p17(void)
#else
void p17()
#endif
{
   bitmod('s',48,4);
   bitmod('s',65,4);
   bitmod('s',70,4);
   bitmod('s',67,4);
   bitmod('s',87,4);
   bitmod('s',89,4);
   bitmod('s',41,4);
   bitmod('s',42,4);
   bitmod('s',56,4);
   bitmod('s',73,4);
   bitmod('s',61,4);
   bitmod('s',84,4);
}
#ifdef __STDC__
void p18(void)
#else
void p18()
#endif
{
   bitmod('s',21,5);
   bitmod('s',91,5);
   bitmod('s',19,5);
   bitmod('s',6,5);
   bitmod('s',16,5);
   bitmod('s',37,5);
   bitmod('s',76,5);
   bitmod('s',77,5);
   bitmod('s',23,5);
   bitmod('s',71,5);
   bitmod('s',26,5);
   bitmod('s',90,5);
   bitmod('s',24,5);
   bitmod('s',25,5);
   bitmod('s',80,8);
   bitmod('s',89,8);
}
#ifdef __STDC__
void p19(void)
#else
void p19()
#endif
{
   apport(68,94);
   apport(69,94);
   apport(3,99);
   apport(70,101);
   apport(72,102);
   apport(4,105);
   apport(71,104);
   apport(5,173);
   apport(75,175);
   apport(6,110);
   apport(7,107);
   apport(8,180);
   apport(76,182);
   apport(78,185);
   apport(80,94);
   apport(81,94);
   apport(10,114);
   apport(11,111);
   apport(12,189);
   apport(15,120);
   apport(17,176);
   apport(19,202);
   apport(20,199);
   apport(21,199);
   apport(37,254);
   apport(29,207);
   apport(33,264);
   apport(39,175);
   apport(43,109);
   apport(44,108);
   apport(45,115);
   apport(46,116);
   apport(47,117);
   apport(49,172);
   apport(50,174);
   apport(52,176);
   apport(54,179);
   apport(57,180);
   apport(60,251);
   apport(62,254);
   apport(64,328);
   apport(41,331);
   apport(40,331);
   apport(51,266);
   apport(88,275);
   apport(89,274);
   apport(65,300);
   apport(67,323);
   apport(34,267);
   apport(31,324);
   apport(32,321);
   apport(86,265);
   apport(23,277);
   apport(35,246);
   apport(36,246);
   apport(55,246);
   apport(42,269);
   apport(30,209);
   apport(13,211);
   apport(56,213);
   apport(66,250);
   apport(84,249);
   apport(24,218);
   apport(73,223);
   apport(61,194);
   apport(25,192);
}
#ifdef __STDC__
void p20(void)
#else
void p20()
#endif
{
   bitmod('s',3,3);
   bitmod('s',4,3);
   bitmod('s',7,3);
   bitmod('s',11,3);
   bitmod('s',15,3);
   bitmod('s',19,3);
   bitmod('s',20,3);
   bitmod('s',21,3);
   bitmod('s',22,3);
   bitmod('s',31,3);
   bitmod('s',34,3);
   bitmod('s',13,3);
   bitmod('s',29,3);
   bitmod('s',24,3);
}
#ifdef __STDC__
void p21(void)
#else
void p21()
#endif
{
   set('V',477,'c',35,NULL,NULL);
   set('V',478,'c',25,NULL,NULL);
   move(92,-1);
   value[465]=484;*bitword(465)=-1;
   value[462] = irand(10);
   value[462] += 15;
   set('V',447,'c',300,NULL,NULL);
   value[461] = irand(5);
   value[461] += 4;
   if (irand(100)<5) {
   value[475] = irand(400);
   value[475] += 100;
   } else {
   set('V',475,'c',0,NULL,NULL);
   }
   set('E',62,'c',1,NULL,NULL);
   set('E',13,'c',8,NULL,NULL);
   set('E',84,'c',1,NULL,NULL);
   set('V',452,'c',0,NULL,NULL);
   set('V',468,'c',7,NULL,NULL);
   say(0,485,0);
   special(5,&value[448]);
   if (value[448]>0) {
   value[449]=757;*bitword(449)=-1;
   value[449] += value[448];
   say(2,449,0);
   if (value[448]==1) {
   special(6,&value[449]);
   }
   special(9,&value[449]);
   say(0,483,0);
   say(0,760,0);
   if (query(761)) {
   say(0,483,0);
   say(0,484,0);
   say(0,483,0);
   bitmod('s',evar(458),0);
   } else {
   say(0,762,0);
   finita();
   }}
   if (query(561)) {
   say(0,487,0);
   }
   say(0,483,0);
}
#ifdef __STDC__
void p22(void)
#else
void p22()
#endif
{
   longjmp(loop_back,1);
}
#ifdef __STDC__
void p24(void)
#else
void p24()
#endif
{
   value[448] = irand(100);
   if (bitest(evar(458),1)) {
   p541();
   }
   if (!(bitest(evar(446),0))) {
   if ((isnear(91,-1,-1)) &&
   irand(100)<value[477]) {
   p166();
   }
   return;
   }
   if (((bitest(evar(443),8)) &&
   (location[91]==value[443])) &&
   !(bitest(evar(458),5))) {
   move(value[443],-1);
   bitmod('c',evar(446),0);
   say(0,488,0);
   return;
   }
   if (isnear(13,-1,-1)) {
   p433();
   }
   bitmod('c',evar(446),0);
   value[479] = irand(6);
   value[448]=796;*bitword(448)=-1;
   value[479] += value[448];
   if (irand(100)<5) {
   value[479]=802;*bitword(479)=-1;
   }
   value[459] += 1;
   if (bitest(evar(458),0)) {
   set('V',448,'c',30,NULL,NULL);
   } else {
   set('V',448,'c',800,NULL,NULL);
   }
   if (value[459]==value[448]) {
   say(0,763,0);
   p204();
   }
   if (isnear(69,-1,-1)) {
   if (value[69]==1) {
   value[447] -= 1;
   if ((value[447]==40) ||
   value[447]==0) {
   p318();
   }}}
   if (!((location[90]==333))) {
   apport(90,value[442]);
   if (value[90]>-1) {
   say(0,848,0);
   }
   value[90] += 1;
   }
   if (((bitest(evar(442),1)) &&
   bitest(evar(446),1)) ||
   bitest(evar(446),2)) {
   set('V',449,'c',0,NULL,NULL);
   } else {
   set('V',449,'c',1,NULL,NULL);
   }
   if (((isnear(69,-1,-1)) &&
   value[69]==1) ||
   bitest(evar(442),0)) {
   say(2,442,0);
   if (bitest(evar(442),12)) {
   say(0,576,0);
   }
   if (bitest(evar(442),1)) {
   set('V',450,'c',2,NULL,NULL);
   } else {
   set('V',450,'c',3,NULL,NULL);
   }
   bitmod('s',evar(442),1);
   if (!((location[91]==333))) {
   if (!((bitest(evar(442),4)) ||
   bitest(evar(442),2))) {
   apport(91,value[442]);
   }}
   *bitword(448)= -1; value[448]=FOBJ-1; while (++value[448]<=LOBJ) {
   if ((isnear(value[448],-1,-1)) &&
   !have(value[448],-1,-1)) {
   bitmod('s',evar(448),1);
   if ((value[449]==1) &&
   !(bitest(evar(448),7))) {
   set('V',449,'c',0,NULL,NULL);
   say(0,483,0);
   }
   say(2,448,0);
   }}
   if (have(37,-1,-1)) {
   say(0,636,0);
   }
   } else {
   if (((bitest(evar(443),0)) ||
   irand(100)<75) ||
   bitest(evar(458),6)) {
   say(0,509,0);
   } else {
   say(0,516,0);
   p124();
   }}
   bitmod('c',evar(458),6);
   if ((value[HERE]==118)) {
   if (irand(100)<35) {
   say(0,497,0);
   }}
   if (isnear(90,-1,-1)) {
   value[90] += 1;
   if (value[90]>6) {
   p124();
   }}
   if (!(bitest(evar(442),4))) {
   value[462] -= value[450];
   if (value[462]<1) {
   p113();
   }}
   if (isnear(91,-1,-1)) {
   set('V',478,'c',50,NULL,NULL);
   p166();
   }
   if ((((location[69]==334)) &&
   (value[HERE]==92)) &&
   value[463]<4) {
   say(0,688,0);
   set('V',464,'c',1,NULL,NULL);
   p204();
   }
}
#ifdef __STDC__
void p25(void)
#else
void p25()
#endif
{
   if (bitest(evar(442),5)) {
   value[467] += 1;
   if (((value[467]>30) &&
   !(bitest(evar(442),7))) ||
   value[467]>50) {
   p242();
   }
   } else {
   set('V',467,'c',0,NULL,NULL);
   }
   if (value[16]==16) {
   apport(16,value[442]);
   }
   if (bitest(evar(446),5)) {
   bitmod('c',evar(446),5);
   set('V',452,'c',0,NULL,NULL);
   *bitword(448)= -1; value[448]=FOBJ-1; while (++value[448]<=LOBJ) {
   if ((have(value[448],-1,-1)) &&
   !(bitest(evar(448),9))) {
   value[452] += 1;
   }}}
   set('V',478,'c',25,NULL,NULL);
   set('V',477,'c',35,NULL,NULL);
   input(0);
   value[460] -= 1;
   value[457] += 1;
   if (value[446]==0) {
   longjmp(loop_back,1);
   }
   if (value[444]==9999) {
   say(0,557,0);
   longjmp(loop_back,1);
   }
   if (KEY(336)) {
   p458();
   }
   if (value[446]>1) {
   if (value[445]==9999) {
   value[448]=338;*bitword(448)=-1;
   if (value[444]>value[448]) {
   say(12,556,445);
   longjmp(loop_back,1);
   }}}
   if (value[442]<LPROC && value[442]>= 0) (*procs[value[442]])(); else pcall(value[442]);
   if (bitest(evar(444),13)) {
   if ((value[HERE]==value[444])) {
   say(0,635,0);
   } else {
   say(0,634,0);
   }
   } else {
   if (value[444]<LPROC && value[444]>= 0) (*procs[value[444]])(); else pcall(value[444]);
   if (bitest(evar(444),15)) {
   if (isnear(value[444],-1,-1)) {
   say(12,714,444);
   bitmod('s',evar(446),4);
   } else {
   say(12,522,444);
   }
   } else {
   p42();
   }}
   longjmp(loop_back,1);
}
