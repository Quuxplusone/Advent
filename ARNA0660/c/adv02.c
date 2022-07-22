/* adv02.c: Adv660 - copyright Mike Arnautov 2022. Licensed under
 * GPL (version 3 or later) or the Modified BSD Licence, whichever
 * is asserted by the supplied LICENCE file.*/

#include "adv0.h"
#include "adv3.h"
int p83(void)
{
value[0]=(bitest(evar(672),16));
if(value[0]){
value[702]=value[value[672]];*bitword(702)= *bitword(672);value[0]=(value[702]>1);
if(value[0]){
value[0]=(KEY(64));
if(value[0]){
set('E',64,'c',1,NULL,NULL);
}else{
say(76,1613,672);
 if (loop) return(0);}}}
return 0;
}
int p250(void)
{
value[0]=(value[699]==1);
if(value[0]){
value[0]=(isnear(114,0,4));
value[0]|=(isnear(111,0,4));
value[0]|=(ishere(113,-1,-1));
if(value[0]){
bitmod('s',evar(699),5);
say(76,1022,113);
 if (loop) return(0);}
say(76,819,113);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p251(void)
{
value[0]=(value[699]==1);
if(value[0]){
value[0]=(isnear(114,0,3));
value[0]|=(isnear(111,0,3));
value[0]|=(bitest(evar(673),9));
if(value[0]){
bitmod('s',evar(699),5);
say(76,1022,671);
 if (loop) return(0);}
say(76,819,671);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p308(void)
{
set('V',701,'c',1,NULL,NULL);
value[0]=(isnear(99,0,1));
value[0]|=(bitest(evar(673),3));
if(value[0]){
set('V',701,'c',0,NULL,NULL);
}else{
value[0]=(isnear(95,0,3));
if(value[0]){
set('V',701,'c',0,NULL,NULL);
}}
return 0;
}
int p653(void)
{
apport(101,485);
value[0]=(getloc(7)==155);
if(value[0]){
bitmod('s',155,8);
}
return 0;
}
int p622(void)
{
value[0]=(value[760]<135);
if(value[0]){
say(0,869,0);
}else{
say(2,701,0);
}
value[701] = irand(150);
value[760] += value[702];
if (*bitword(702*VARSIZE)==-1) *bitword(760*VARSIZE)= -1;
value[760] += value[701];
if (*bitword(701*VARSIZE)==-1) *bitword(760*VARSIZE)= -1;
value[0]=(value[760]>1500);
if(value[0]){
set('V',760,'c',1500,NULL,NULL);
}
loop=1; return(0);
return 0;
}
int p350(void)
{
value[716] += 1;
value[0]=(getloc(110)==484);
value[0]&=!(value[HERE]==412);
if(value[0]){
apport(110,412);
bitmod('c',110,4);
}
return 0;
}
int p85(void)
{
set('V',688,'c',0,NULL,NULL);
value[0]=(value[716]<1);
if(value[0]){
return 0;
}
set('V',704,'v',761,NULL,NULL);
value[704] *= 100;
value[704] /= value[716];
value[704] *= value[134];
value[0]=(irand(100)<value[704]);
if(value[0]){
value[703] = irand(value[761]);
value[703] += 1;
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(bitest(evar(677),3));
value[0]&=(getloc(value[677])==435);
if(value[0]){
value[703] -= 1;
value[0]=(value[703]==0);
if(value[0]){
set('V',688,'v',677,NULL,NULL);
return 0;
}}}}
return 0;
}
int p319(void)
{
*bitword(705)= -1; value[705]=FOBJ-1; while (++value[705]<=LOBJ) {
value[0]=(value[688]==0);
if(value[0]){
p85();
if (loop) return (0);
}value[0]=(
value[688]==0);
if(!value[0]){
apport(value[688],value[673]);
value[761] -= 1;
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
say(2,688,0);
}
set('V',688,'c',0,NULL,NULL);
}
value[134] -= 1;
value[716] -= 1;
value[0]=(value[134]==0);
if(value[0]){
apport(134,484);
return 0;
}
value[706] -= 1;
value[0]=(value[706]==0);
if(value[0]){
return 0;
}}
return 0;
}
int p649(void)
{
value[701] = irand(5);
value[0]=(value[701]==0);
if(value[0]){
value[678]=168;*bitword(678)= -1;
}else{
value[0]=(value[701]==1);
if(value[0]){
value[678]=192;*bitword(678)= -1;
}else{
value[0]=(value[701]==2);
if(value[0]){
value[678]=151;*bitword(678)= -1;
}else{
value[0]=(value[701]==3);
if(value[0]){
value[678]=212;*bitword(678)= -1;
}else{
value[678]=319;*bitword(678)= -1;
}}}}
say(0,1236,0);
set('V',703,'c',0,NULL,NULL);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(ishere(value[677],-1,-1));
value[0]&=!(value[677]==78);
value[0]&=(bitest(evar(677),3));
if(value[0]){
say(0,1238,0);
apport(value[677],value[678]);
set('V',703,'c',1,NULL,NULL);
}}value[0]=(
value[703]==0);
if(value[0]){
say(0,1237,0);
}
move(value[678],-2); if (loop) return(0);
return 0;
}
int p499(void)
{
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(ishere(value[677],1,3));
if(value[0]){
apport(value[677],value[678]);
}}
return 0;
}
int p414(void)
{
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],1,15));
if(value[0]){
say(64,1272,0);
 if (loop) return(0);}}
return 0;
}
int p580(void)
{
value[0]=(bitest(evar(672),15));
if(value[0]){
apport(value[672],HERE);
say(12,1496,671);
say(76,1497,672);
 if (loop) return(0);}
value[679]=1234;*bitword(679)= -1;
value[0]=(irand(100)<10);
value[0]&=(getloc(78)==393);
if(value[0]){
value[679]=1235;*bitword(679)= -1;
apport(78,419);
apport(79,485);
}
say(12,1551,672);
value[0]=(bitest(evar(672),11));
if(value[0]){
say(11,679,1);
}else{
say(11,679,0);
}
apport(value[672],419);
loop=1; return(0);
return 0;
}
int p613(void)
{
value[0]=(value[734]==11);
if(value[0]){
set('V',701,'c',0,NULL,NULL);
return 0;
}
set('V',701,'v',734,NULL,NULL);
value[0]=(value[701]==value[735]);
if(value[0]){
value[0]=(irand(100)<10);
if(value[0]){
set('V',703,'c',11,NULL,NULL);
value[703] -= value[701];
value[702] = irand(value[703]);
value[701] += value[702];
if (*bitword(702*VARSIZE)==-1) *bitword(701*VARSIZE)= -1;
}else{
value[0]=(irand(100)<80);
if(value[0]){
value[701]=0;*bitword(701)= -1;
return 0;
}}
}else{
set('V',735,'v',701,NULL,NULL);
}
return 0;
}
int p433(void)
{
say(0,1303,0);
say(0,777,0);
set('V',678,'v',673,NULL,NULL);
value[703] = irand(3-1+1)+1;
value[0]=(irand(100)<50);
if(value[0]){
value[678] += value[703];
if (*bitword(703*VARSIZE)==-1) *bitword(678*VARSIZE)= -1;
}else{
value[701] -= value[703];
}value[0]=(
bitest(evar(678),3));
value[0]|=(bitest(evar(678),7));
value[0]|=(bitest(evar(678),5));
if(value[0]){
set('V',678,'v',684,NULL,NULL);
}
apport(85,value[678]);
value[0]=(isnear(85,-1,-1));
if(value[0]){
apport(85,205);
}
return 0;
}
int p432(void)
{
value[0]=(irand(100)<value[703]);
if(value[0]){
say(0,1527,0);
return 0;
}value[0]=(
irand(100)<value[702]);
if(value[0]){
say(0,1305,0);
return 0;
}
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
say(0,1304,0);
}else{
value[0]=(isnear(87,-1,-1));
if(value[0]){
say(0,1307,0);
set('V',734,'c',4,NULL,NULL);
apport(87,485);
set('E',86,'c',1,NULL,NULL);
value[0]=(value[115]==2);
if(value[0]){
set('E',115,'c',0,NULL,NULL);
}
}else{
value[0]=(isnear(134,-1,-1));
if(value[0]){
say(0,1307,0);
set('V',706,'e',134,NULL,NULL);
p319();
if (loop) return (0);
}else{
value[0]=(isnear(20,0,0));
if(value[0]){
say(0,1474,0);
apport(85,485);
}else{
say(0,1306,0);
}}}
value[684]=getloc(85);*bitword(684)= -1;
apport(85,value[673]);
}
return 0;
}
int p53(void)
{
set('V',704,'v',672,NULL,NULL);
value[0]=(value[699]==1);
value[0]|=(513<=value[671] && value[671]<=522);
if(value[0]){
set('V',704,'v',671,NULL,NULL);
}value[0]=(
value[703]==0);
if(value[0]){
bitmod('s',evar(673),8);
move(value[680],-1);
}else{
value[0]=(value[703]==value[704]);
if(value[0]){
move(value[680],-2); if (loop) return(0);
}
move(value[681],-2); if (loop) return(0);
}
return 0;
}
int p79(void)
{
value[0]=(bitest(evar(673),7));
if(value[0]){
value[701]=1700;*bitword(701)= -1;
set('V',702,'c',200,NULL,NULL);
p622();
if (loop) return (0);
}
value[760] = irand(750-600+1)+600;
apport(81,485);
bitmod('s',evar(712),9);
value[0]=(bitest(64,4));
if(value[0]){
say(64,1367,0);
 if (loop) return(0);}
say(0,1277,0);
value[0]=(query(1278)); if (loop>1) return (0);
if(value[0]){
apport(82,value[673]);
say(64,1282,0);
 if (loop) return(0);}
say(64,1279,0);
 if (loop) return(0);return 0;
}
int p7(void)
{
set('V',703,'c',1,NULL,NULL);
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
return 0;
}
set('V',703,'c',0,NULL,NULL);
value[0]=(bitest(evar(699),7));
value[0]|=(value[718]>0);
value[0]|=(bitest(evar(673),7));
value[0]|=(bitest(evar(673),5));
value[0]|=(bitest(evar(673),11));
value[0]|=(bitest(19,13));
value[0]|=(isnear(134,-1,-1));
value[0]|=(isnear(20,-1,-1));
value[0]|=(isnear(22,-1,-1));
value[0]|=(isnear(7,-1,-1));
value[0]|=(isnear(32,-1,-1));
value[0]|=(value[715]>0);
if(value[0]){
set('V',703,'c',1,NULL,NULL);
}
return 0;
}
int p398(void)
{
value[757] -= 1;
value[0]=(value[757]<1);
value[0]&=(irand(100)<25);
if(value[0]){
p7();
if (loop) return (0);
value[0]=(value[703]==1);
if(value[0]){
return 0;
}
set('V',756,'c',-1,NULL,NULL);
value[0]=(bitest(evar(756),0));
value[0]|=(irand(100)<75);
value[0]|=(getloc(81)==485);
value[0]|=(bitest(64,4));
if(!value[0]){
set('V',756,'c',0,NULL,NULL);
bitmod('s',evar(756),0);
}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),1));
value[0]|=(irand(100)<75);
value[0]|=(bitest(385,4));
if(!value[0]){
set('V',756,'c',1,NULL,NULL);
bitmod('s',evar(756),1);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),9));
value[0]|=(irand(100)<75);
value[0]|=(bitest(119,4));
value[0]|=!(bitest(116,4));
if(!value[0]){
set('V',756,'c',9,NULL,NULL);
bitmod('s',evar(756),9);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),2));
value[0]|=(irand(100)<75);
value[0]|=(bitest(253,4));
value[0]|=!(bitest(250,4));
if(!value[0]){
set('V',756,'c',2,NULL,NULL);
bitmod('s',evar(756),2);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),3));
value[0]|=(irand(100)<75);
value[0]|=!(bitest(259,4));
value[0]|=(bitest(27,13));
if(!value[0]){
set('V',756,'c',3,NULL,NULL);
bitmod('s',evar(756),3);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),4));
value[0]|=(irand(100)<75);
value[0]|=(bitest(253,4));
if(!value[0]){
set('V',756,'c',4,NULL,NULL);
bitmod('s',evar(756),4);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),6));
value[0]|=(irand(100)<75);
value[0]|=(getloc(95)==140);
value[0]|=!(bitest(95,4));
if(!value[0]){
set('V',756,'c',6,NULL,NULL);
bitmod('s',evar(756),6);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),7));
value[0]|=(irand(100)<75);
value[0]|=(bitest(462,4));
value[0]|=!(bitest(271,4));
if(!value[0]){
set('V',756,'c',7,NULL,NULL);
bitmod('s',evar(756),7);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),8));
value[0]|=(irand(100)<75);
value[0]|=!(bitest(324,4));
if(!value[0]){
set('V',756,'c',8,NULL,NULL);
bitmod('s',evar(756),8);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),11));
value[0]|=(irand(100)<75);
value[0]|=(bitest(58,4));
value[0]|=(bitest(228,8));
if(!value[0]){
set('V',756,'c',11,NULL,NULL);
bitmod('s',evar(756),11);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),10));
value[0]|=(irand(100)<75);
value[0]|=!(bitest(35,4));
if(!value[0]){
set('V',756,'c',10,NULL,NULL);
bitmod('s',evar(756),10);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),12));
value[0]|=(irand(100)<75);
value[0]|=!(bitest(415,4));
value[0]|=(bitest(420,4));
if(!value[0]){
set('V',756,'c',12,NULL,NULL);
bitmod('s',evar(756),12);
}}value[0]=(
value[756]==-1);
if(value[0]){
value[0]=(bitest(evar(756),5));
value[0]|=(irand(100)<85);
if(!value[0]){
set('V',756,'c',5,NULL,NULL);
bitmod('s',evar(756),5);
}}
value[757] = irand(100-20+1)+20;
value[0]=(value[756]>-1);
if(value[0]){
value[679]=1286;*bitword(679)= -1;
value[679] += value[756];
if (*bitword(756*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
say(0,777,0);
say(2,679,0);
value[757] += 100;
}}
return 0;
}
int p345(void)
{
value[0]=(irand(100)<20);
if(value[0]){
move(434,-2); if (loop) return(0);
}
move(432,-2); if (loop) return(0);
return 0;
}
int p473(void)
{
value[0]=(bitest(116,13));
if(value[0]){
say(64,1339,0);
 if (loop) return(0);}
bitmod('s',116,13);
say(64,1338,0);
 if (loop) return(0);return 0;
}
int p239(void)
{
value[0]=(value[716]<1);
if(value[0]){
return 0;
}
set('V',702,'c',0,NULL,NULL);
bitmod('s',95,15);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(bitest(evar(677),3));
if(value[0]){
bitmod('c',evar(677),14);
value[0]=(bitest(evar(677),4));
value[0]&=!(bitest(evar(677),15));
value[0]&=!(isnear(value[677],-1,-1));
if(value[0]){
value[678]=getloc(value[677]);*bitword(678)= -1;
value[0]=(bitest(evar(678),7));
value[0]|=(bitest(evar(678),5));
if(!value[0]){
value[0]=(bitest(evar(678),10));
if(!value[0]){
value[702] += 1;
bitmod('s',evar(677),14);
}}}}}
bitmod('c',95,15);
value[0]=(value[702]>0);
if(value[0]){
value[703] = irand(value[702]);
value[703] += 1;
*bitword(676)= -1; value[676]=FOBJ-1; while (++value[676]<=LOBJ) {
value[0]=(value[703]>0);
value[0]&=(bitest(evar(676),3));
value[0]&=(bitest(evar(676),14));
if(value[0]){
value[703] -= 1;
value[0]=(value[703]==0);
if(value[0]){
value[678]=getloc(value[676]);*bitword(678)= -1;
set('V',677,'v',676,NULL,NULL);
}}}
set('V',701,'v',761,NULL,NULL);
value[701] *= 100;
value[701] /= value[716];
value[701] *= value[134];
value[0]=(irand(100)<value[701]);
if(value[0]){
value[0]=(bitest(evar(677),5));
if(!value[0]){
return 0;
}
value[703] = irand(value[761]);
value[703] += 1;
*bitword(676)= -1; value[676]=FOBJ-1; while (++value[676]<=LOBJ) {
value[0]=(value[703]>0);
value[0]&=(getloc(value[676])==435);
if(value[0]){
value[703] -= 1;
value[0]=(value[703]==0);
if(value[0]){
value[0]=(bitest(evar(676),5));
value[0]|=(value[676]==value[688]);
if(value[0]){
return 0;
}
value[761] -= 1;
apport(value[676],value[678]);
}}}}
value[761] += 1;
apport(value[677],435);
}
return 0;
}
int p558(void)
{
value[0]=(value[699]==1);
if(value[0]){
set('V',702,'v',671,NULL,NULL);
}else{
set('V',702,'v',672,NULL,NULL);
}value[0]=(
513<=value[702] && value[702]<=522);
if(value[0]){
value[702] += 4;
value[703]=522;*bitword(703)= -1;
value[0]=(value[702]<value[703]);
if(!value[0]){
value[702] -= 8;
}value[0]=(
value[699]==1);
if(value[0]){
special(9,&value[702]);
}else{
special(10,&value[702]);
}}
return 0;
}
int p81(void)
{
set('V',675,'c',0,NULL,NULL);
value[0]=(value[699]==1);
if(value[0]){
set('V',677,'v',671,NULL,NULL);
}else{
set('V',677,'v',672,NULL,NULL);
}value[0]=(
bitest(evar(677),0));
if(!value[0]){
return 0;
}value[0]=(
isnear(value[677],-1,-1));
if(value[0]){
return 0;
}value[0]=(
KEY(102));
if(value[0]){
value[0]=(bitest(102,4));
value[0]&=(isnear(104,-1,-1));
value[0]&=!(isnear(102,-1,-1));
if(value[0]){
return 0;
}}value[0]=(
bitest(evar(677),13));
value[0]|=(bitest(evar(677),14));
if(!value[0]){
return 0;
}value[0]=(
KEY(27));
if(value[0]){
return 0;
}value[0]=(
bitest(evar(677),13));
if(value[0]){
value[0]=(value[HERE]==378);
if(!value[0]){
return 0;
}
}else{
value[0]=(value[HERE]==379);
if(!value[0]){
return 0;
}}value[0]=(
KEY(533));
if(value[0]){
value[0]=(KEY(134));
if(value[0]){
say(64,916,0);
 if (loop) return(0);}value[0]=(
bitest(evar(677),4));
if(value[0]){
say(76,1385,672);
 if (loop) return(0);}value[0]=(
value[707]<value[722]);
if(value[0]){
bitmod('s',evar(677),4);
}else{
flush_command();
say(64,898,0);
 if (loop) return(0);}}
value[675]=getloc(value[677]);*bitword(675)= -1;
apport(value[677],value[673]);
return 0;
}
int p545(void)
{
value[0]=(value[675]==0);
if(!value[0]){
value[0]=(value[699]==1);
if(value[0]){
set('V',677,'v',671,NULL,NULL);
}else{
set('V',677,'v',672,NULL,NULL);
}value[0]=(
bitest(evar(677),0));
if(!value[0]){
return 0;
}value[0]=(
have(value[677],-1,-1));
if(!value[0]){
apport(value[677],value[675]);
}}
return 0;
}
int p117(void)
{
value[0]=(value[HERE]==147);
value[0]|=(value[HERE]==419);
value[0]|=(value[HERE]==370);
value[0]|=(value[HERE]==380);
if(value[0]){
say(64,1384,0);
 if (loop) return(0);}
say(76,830,147);
 if (loop) return(0);return 0;
}
int p484(void)
{
value[0]=(KEY(533));
if(value[0]){
value[0]=(ishere(102,-1,-1));
if(value[0]){
return 0;
}value[0]=(
have(104,-1,-1));
if(value[0]){
say(76,814,672);
 if (loop) return(0);}value[0]=(
isnear(104,-1,-1));
if(!value[0]){
value[0]=(value[HERE]==379);
if(value[0]){
value[0]=(bitest(104,4));
if(value[0]){
say(76,1385,672);
 if (loop) return(0);}
}else{
return 0;
}}value[0]=(
value[707]<value[722]);
if(value[0]){
apport(104,INHAND);
bitmod('s',104,4);
set('V',675,'c',0,NULL,NULL);
say(12,1496,671);
say(76,1497,672);
 if (loop) return(0);}
flush_command();
say(64,898,0);
 if (loop) return(0);}value[0]=(
KEY(534));
value[0]|=(KEY(556));
if(value[0]){
value[0]=(have(102,-1,-1));
if(value[0]){
return 0;
}value[0]=(
have(104,-1,-1));
if(value[0]){
apport(104,HERE);
say(12,1496,671);
say(76,1497,672);
 if (loop) return(0);}
say(76,1026,672);
 if (loop) return(0);}value[0]=(
KEY(546));
if(value[0]){
value[0]=(have(102,-1,-1));
value[0]|=(have(104,-1,-1));
if(value[0]){
say(64,829,0);
 if (loop) return(0);}
say(76,1026,672);
 if (loop) return(0);}value[0]=(
KEY(570));
if(value[0]){
value[0]=(isnear(102,-1,-1));
value[0]&=!(have(104,-1,-1));
value[0]|=(have(102,-1,-1));
if(value[0]){
say(76,1548,672);
 if (loop) return(0);}else{
bitmod('s',evar(699),4);
say(0,104,0);
bitmod('c',evar(699),4);
loop=1; return(0);
}
}else{
say(64,1403,0);
 if (loop) return(0);}
return 0;
}
int p434(void)
{
value[0]=(value[718]==3);
if(!value[0]){
say(64,1077,0);
 if (loop) return(0);}value[0]=(
have(107,-1,-1));
if(!value[0]){
say(64,1078,0);
 if (loop) return(0);}value[0]=(
bitest(107,15));
if(value[0]){
say(64,1404,0);
 if (loop) return(0);}
say(9,1394,20);
value[0]=(query(1395)); if (loop>1) return (0);
if(value[0]){
bitmod('s',107,15);
value[709] += 20;
say(64,1396,0);
 if (loop) return(0);}
say(64,1405,0);
 if (loop) return(0);return 0;
}
int p237(void)
{
value[0]=(value[719]==0);
value[0]|=(value[718]>2);
if(value[0]){
set('V',710,'c',10,NULL,NULL);
}else{
set('V',710,'c',0,NULL,NULL);
}
set('V',711,'c',10,NULL,NULL);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(bitest(evar(677),5));
if(value[0]){
value[0]=(getloc(value[677])==141);
value[0]|=(getloc(value[677])==243);
if(value[0]){
value[710] += 15;
}else{
value[0]=(bitest(evar(677),4));
if(value[0]){
value[710] += 7;
}}
value[711] += 15;
}}value[0]=(
getloc(95)==141);
if(value[0]){
value[710] -= 8;
}else{
value[0]=(getloc(95)==140);
if(value[0]){
value[710] += 8;
}}value[0]=(
bitest(80,5));
if(!value[0]){
value[711] += 15;
}value[0]=(
bitest(97,4));
if(value[0]){
value[710] += 1;
value[0]=(getloc(97)==141);
if(value[0]){
value[710] += 1;
}}
value[0]=!(getloc(108)==236);
if(value[0]){
value[710] += 1;
}
value[711] += 1;
value[0]=(bitest(147,4));
value[0]|=(bitest(163,4));
if(value[0]){
value[710] += 20;
value[0]=(bitest(324,4));
if(value[0]){
value[710] += 10;
}value[0]=(
bitest(377,4));
if(value[0]){
value[710] += 10;
}value[0]=(
bitest(253,4));
if(value[0]){
value[710] += 10;
}value[0]=(
bitest(290,4));
if(value[0]){
value[710] += 15;
}value[0]=(
bitest(27,13));
if(value[0]){
value[710] += 8;
}}
value[711] += 73;
set('V',701,'v',718,NULL,NULL);
value[701] *= 16;
value[710] += value[701];
if (*bitword(701*VARSIZE)==-1) *bitword(710*VARSIZE)= -1;
value[711] += 96;
set('V',704,'v',708,NULL,NULL);
value[704] *= 10;
value[710] -= value[704];
value[710] -= value[709];
value[0]=(value[710]<0);
if(value[0]){
set('V',710,'c',0,NULL,NULL);
}
return 0;
}
int p190(void)
{
p237();
if (loop) return (0);
say(13,1032,710);
say(13,1034,711);
say(13,1035,713);
say(0,777,0);
set('V',702,'v',710,NULL,NULL);
value[0]=(value[710]<30);
if(value[0]){
say(0,1040,0);
value[702] -= 30;
}else{
value[0]=(value[710]<100);
if(value[0]){
say(0,1041,0);
value[702] -= 100;
}else{
value[0]=(value[710]<200);
if(value[0]){
say(0,1042,0);
value[702] -= 200;
}else{
value[0]=(value[710]<300);
if(value[0]){
say(0,1043,0);
value[702] -= 300;
}else{
value[0]=(value[710]<400);
if(value[0]){
say(0,1044,0);
value[702] -= 400;
}else{
value[0]=(value[710]<500);
if(value[0]){
say(0,1045,0);
value[702] -= 500;
}else{
value[0]=(value[710]<600);
if(value[0]){
say(0,1046,0);
value[702] -= 600;
}else{
value[0]=(value[710]<650);
if(value[0]){
say(0,1047,0);
value[702] -= 650;
}else{
value[0]=(value[710]<660);
if(value[0]){
say(0,1048,0);
value[702] -= 660;
}else{
say(0,1049,0);
set('V',702,'c',0,NULL,NULL);
}}}}}}}}}
say(0,777,0);
value[702] *= -1;
value[0]=(value[702]>0);
if(value[0]){
value[0]=(value[702]==1);
if(value[0]){
say(0,1050,0);
}else{
say(13,1051,702);
}}
say(0,777,0);
finita();
loop=1; return(0);
return 0;
}
int p442(void)
{
value[0]=(value[14]<8);
if(value[0]){
value[14] = irand(8);
}
value[15] = irand(8);
value[0]=(isnear(99,0,1));
if(value[0]){
apport(15,484);
}else{
apport(15,256);
value[0]=(value[HERE]==256);
value[0]&=!(bitest(evar(699),0));
if(value[0]){
say(0,15,0);
}}
return 0;
}
int p93(void)
{
set('V',719,'c',0,NULL,NULL);
flush_command();
say(0,777,0);
bitmod('c',evar(712),2);
bitmod('c',evar(712),0);
bitmod('c',95,13);
value[0]=(value[95]==2);
if(value[0]){
set('E',95,'c',3,NULL,NULL);
}value[0]=(
value[HERE]==432);
value[0]|=(value[HERE]==256);
if(value[0]){
move(485,-1);
}
set('E',17,'c',0,NULL,NULL);
apport(17,484);
value[0]=(getloc(121)==485);
if(!value[0]){
apport(121,484);
}value[0]=(
value[25]==1);
if(value[0]){
set('E',25,'c',0,NULL,NULL);
}else{
value[0]=(value[25]==3);
if(value[0]){
set('E',25,'c',2,NULL,NULL);
}}
apport(14,255);
set('E',14,'c',8,NULL,NULL);
value[0]=(value[43]>1);
if(value[0]){
set('E',43,'c',1,NULL,NULL);
}value[0]=(
value[35]==1);
if(value[0]){
set('E',35,'c',0,NULL,NULL);
set('V',685,'c',0,NULL,NULL);
bitmod('c',35,13);
}
value[708] += 1;
value[0]=(value[718]>1);
if(value[0]){
value[0]=(value[718]==2);
if(value[0]){
say(0,936,0);
}else{
value[708] -= 1;
}
p190();
if (loop) return (0);
}
value[679]=886;*bitword(679)= -1;
value[679] += value[708];
if (*bitword(708*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
value[679] += value[708];
if (*bitword(708*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
value[0]=(query(value[679])); if (loop>1) return (0);
if(value[0]){
value[679] += 1;
say(2,679,0);
say(0,777,0);
value[702]=895;*bitword(702)= -1;
value[0]=(value[679]<value[702]);
if(value[0]){
value[0]=(have(53,-1,-1));
if(value[0]){
apport(53,485);
apport(54,INHAND);
}value[0]=(
have(111,-1,-1));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}value[0]=(
have(114,-1,-1));
value[0]&=!(value[114]==1);
if(value[0]){
set('E',114,'c',2,NULL,NULL);
}
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
if(value[0]){
apport(value[677],HERE);
}}
set('V',707,'c',0,NULL,NULL);
set('E',99,'c',0,NULL,NULL);
apport(14,255);
set('E',14,'c',8,NULL,NULL);
p442();
if (loop) return (0);
move(141,-1);
set('V',674,'c',0,NULL,NULL);
apport(99,136);
value[0]=(value[700]==0);
if(value[0]){
value[0]=(bitest(324,4));
if(!value[0]){
value[678]=getloc(39);*bitword(678)= -1;
value[0]=(value[39]==3);
value[0]|=!(bitest(evar(678),7));
if(value[0]){
apport(99,485);
}}}
apport(134,484);
set('E',134,'c',0,NULL,NULL);
bitmod('c',19,13);
loop=1; return(0);
}}
p190();
if (loop) return (0);
return 0;
}
int p527(void)
{
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
say(0,1601,0);
p93();
if (loop) return (0);
}
return 0;
}
int p246(void)
{
value[0]=(isnear(121,-1,-1));
if(value[0]){
value[0]=(value[121]==6);
if(value[0]){
p93();
if (loop) return (0);
}
value[121] += 1;
}
return 0;
}
int p253(void)
{
set('V',703,'c',0,NULL,NULL);
set('V',702,'c',1,NULL,NULL);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(ishere(value[677],-1,-1));
if(value[0]){
bitmod('s',evar(677),4);
value[0]=(value[702]==1);
value[0]&=!(bitest(evar(677),10));
if(value[0]){
set('V',702,'c',0,NULL,NULL);
say(0,777,0);
}value[0]=(
value[703]==20);
if(value[0]){
set('V',703,'c',0,NULL,NULL);
value[676]=134;*bitword(676)= -1;
value[0]=(value[677]<value[676]);
if(value[0]){
value[0]=(query(1596)); if (loop>1) return (0);
if(!value[0]){
value[677]=134;*bitword(677)= -1;
}}
value[677] -= 1;
}else{
value[703] += 1;
bitmod('s',evar(677),4);
say(2,677,0);
}}}
return 0;
}
int p120(void)
{
say(2,673,0);
value[0]=(437<=value[673] && value[673]<=460);
if(value[0]){
say(0,862,0);
}value[0]=(
value[702]>0);
if(value[0]){
value[0]=(value[HERE]==463);
if(value[0]){
say(2,686,0);
}else{
value[0]=(value[HERE]==150);
value[0]&=(value[151]==0);
if(value[0]){
say(0,1231,0);
}}}value[0]=(
bitest(evar(673),15));
if(value[0]){
say(0,877,0);
}
p253();
if (loop) return (0);
value[0]=(have(38,-1,-1));
if(value[0]){
say(0,940,0);
}
p246();
if (loop) return (0);
return 0;
}
int p231(void)
{
if (!isnear(101,-1,-1)) return 0;
value[0]=(have(101,-1,-1));
if(value[0]){
say(76,814,101);
 if (loop) return(0);}value[0]=(
value[101]==1);
if(value[0]){
apport(100,INHAND);
bitmod('s',100,4);
apport(101,INHAND);
say(12,1496,671);
say(64,1521,0);
 if (loop) return(0);}value[0]=(
have(100,-1,-1));
if(value[0]){
value[0]=(have(102,-1,-1));
if(value[0]){
say(64,816,0);
 if (loop) return(0);}
bitmod('c',149,8);
apport(101,INHAND);
set('E',101,'c',1,NULL,NULL);
say(64,1502,0);
 if (loop) return(0);}
say(64,817,0);
 if (loop) return(0);return 0;
}
int p232(void)
{
if (!isnear(100,-1,-1)) return 0;
value[0]=(have(100,-1,-1));
if(value[0]){
say(76,814,100);
 if (loop) return(0);}
apport(100,INHAND);
say(12,1496,671);
value[0]=(value[101]==1);
if(value[0]){
apport(101,INHAND);
bitmod('s',101,4);
say(64,1521,0);
 if (loop) return(0);}
say(76,1497,672);
 if (loop) return(0);return 0;
}
int p233(void)
{
value[0]=(ishere(134,-1,-1));
if(value[0]){
say(64,922,0);
 if (loop) return(0);}
return 0;
}
int p235(void)
{
value[0]=(ishere(78,-1,-1));
if(value[0]){
value[0]=(value[707]<value[722]);
if(value[0]){
apport(78,INHAND);
apport(79,485);
say(12,1496,671);
say(76,1497,672);
 if (loop) return(0);}
flush_command();
say(64,898,0);
 if (loop) return(0);}
return 0;
}
int p238(void)
{
value[0]=(isnear(95,-1,-1));
value[0]&=(value[707]<value[722]);
value[0]&=!(have(95,-1,-1));
if(value[0]){
value[0]=(value[95]==0);
if(value[0]){
set('E',95,'c',1,NULL,NULL);
apport(95,INHAND);
say(64,1359,0);
 if (loop) return(0);}value[0]=(
getloc(95)==value[673]);
if(value[0]){
value[0]=(bitest(95,13));
if(value[0]){
say(64,1361,0);
 if (loop) return(0);}
bitmod('s',95,13);
say(64,1360,0);
 if (loop) return(0);}}
return 0;
}
int p138(void)
{
value[0]=(have(95,-1,-1));
if(value[0]){
apport(95,HERE);
bitmod('c',95,13);
p308();
if (loop) return (0);
value[0]=(value[95]>1);
value[0]|=(value[HERE]==324);
if(value[0]){
value[0]=(value[HERE]==324);
if(value[0]){
say(0,1461,0);
}else{
say(0,1462,0);
}
set('E',95,'c',3,NULL,NULL);
value[0]=(isnear(85,-1,-1));
if(value[0]){
p433();
if (loop) return (0);
}value[0]=(
value[701]==1);
if(value[0]){
say(0,777,0);
p120();
if (loop) return (0);
}
}else{
value[0]=(value[701]==0);
if(value[0]){
say(0,1362,0);
}else{
say(0,1363,0);
}}value[0]=(
value[HERE]==393);
if(value[0]){
p649();
if (loop) return (0);
}value[0]=(
value[HERE]==204);
if(value[0]){
apport(95,203);
}
loop=1; return(0);
}
return 0;
}
int p136(void)
{
if (!have(97,-1,-1)) return 0;
if (!isnear(34,-1,-1)) return 0;
apport(97,HERE);
say(64,1392,0);
 if (loop) return(0);return 0;
}
int p133(void)
{
if (!have(101,-1,-1)) return 0;
apport(101,HERE);
set('E',101,'c',0,NULL,NULL);
value[0]=(isnear(7,-1,-1));
if(value[0]){
apport(7,485);
bitmod('c',155,8);
say(64,820,0);
 if (loop) return(0);}value[0]=(
isnear(20,-1,-1));
if(value[0]){
value[0]=(value[20]==0);
if(value[0]){
p653();
if (loop) return (0);
say(64,958,0);
 if (loop) return(0);}
say(76,1499,672);
 if (loop) return(0);}value[0]=(
isnear(22,-1,-1));
if(value[0]){
say(64,959,0);
 if (loop) return(0);}value[0]=(
isnear(24,-1,-1));
if(value[0]){
p653();
if (loop) return (0);
say(64,960,0);
 if (loop) return(0);}value[0]=(
isnear(38,-1,-1));
if(value[0]){
value[0]=(value[38]==0);
if(value[0]){
p653();
if (loop) return (0);
say(64,961,0);
 if (loop) return(0);}
say(64,962,0);
 if (loop) return(0);}value[0]=(
isnear(25,-1,-1));
value[0]&=(value[25]<2);
if(value[0]){
say(0,963,0);
p653();
if (loop) return (0);
p93();
if (loop) return (0);
}value[0]=(
isnear(134,-1,-1));
if(value[0]){
p653();
if (loop) return (0);
say(13,964,134);
loop=1; return(0);
}value[0]=(
isnear(87,-1,-1));
if(value[0]){
p653();
if (loop) return (0);
say(64,965,0);
 if (loop) return(0);}value[0]=(
value[HERE]==379);
if(value[0]){
value[0]=(getloc(104)==485);
if(!value[0]){
say(0,1391,0);
say(0,1379,0);
p93();
if (loop) return (0);
}}
say(12,1499,672);
value[0]=(value[HERE]==393);
if(value[0]){
p649();
if (loop) return (0);
}
loop=1; return(0);
return 0;
}
int p135(void)
{
if (!have(100,-1,-1)) return 0;
apport(100,HERE);
say(12,1496,671);
value[0]=(have(101,-1,-1));
if(value[0]){
apport(101,HERE);
say(0,1521,0);
value[0]=(value[HERE]==204);
if(value[0]){
say(0,777,0);
say(0,1550,0);
apport(101,203);
}
}else{
say(12,1497,672);
}value[0]=(
value[HERE]==204);
if(value[0]){
apport(100,203);
}value[0]=(
value[HERE]==393);
if(value[0]){
p649();
if (loop) return (0);
}
loop=1; return(0);
return 0;
}
int p139(void)
{
if (!have(53,-1,-1)) return 0;
apport(53,HERE);
value[0]=(value[HERE]==225);
if(value[0]){
say(12,1496,671);
say(12,1497,672);
}else{
value[0]=(value[HERE]==204);
if(value[0]){
say(0,1222,0);
apport(53,485);
apport(54,203);
}else{
value[0]=(have(105,-1,-1));
value[0]|=!(isnear(105,-1,-1));
if(value[0]){
set('E',53,'c',2,NULL,NULL);
say(0,53,0);
apport(53,485);
apport(54,value[673]);
}else{
set('E',53,'c',1,NULL,NULL);
say(0,53,0);
set('E',53,'c',0,NULL,NULL);
}}}value[0]=(
value[HERE]==393);
if(value[0]){
p649();
if (loop) return (0);
}
loop=1; return(0);
return 0;
}
int p137(void)
{
value[0]=(value[672]==value[112]);
if(value[0]){
set('V',702,'c',3,NULL,NULL);
}else{
set('V',702,'c',4,NULL,NULL);
}value[0]=(
have(111,0,value[702]));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
value[0]=(isnear(134,-1,-1));
value[0]&=(KEY(556));
if(value[0]){
bitmod('s',134,14);
value[0]=(value[134]==1);
if(value[0]){
say(76,1074,672);
 if (loop) return(0);}
say(76,1075,672);
 if (loop) return(0);}
say(76,876,672);
 if (loop) return(0);}value[0]=(
have(114,0,value[702]));
if(value[0]){
set('E',114,'c',2,NULL,NULL);
say(64,1374,0);
 if (loop) return(0);}
return 0;
}
int p134(void)
{
if (!have(111,-1,-1)) return 0;
set('V',701,'e',111,NULL,NULL);
apport(111,HERE);
set('E',111,'c',2,NULL,NULL);
value[0]=(value[HERE]==393);
if(value[0]){
say(12,1551,672);
say(9,1608,0);
apport(111,419);
p649();
if (loop) return (0);
}value[0]=(
value[HERE]==415);
if(value[0]){
apport(111,419);
say(12,1551,672);
say(72,1269,0);
 if (loop) return(0);}value[0]=(
value[HERE]==204);
value[0]|=(value[HERE]==418);
value[0]|=(value[HERE]==417);
if(value[0]){
say(12,1551,672);
value[0]=(value[HERE]==204);
if(value[0]){
apport(111,203);
bitmod('s',203,15);
say(72,1083,0);
 if (loop) return(0);}
apport(111,419);
say(72,1301,0);
 if (loop) return(0);}
say(12,1496,671);
say(12,1498,672);
value[0]=(value[701]==3);
if(value[0]){
bitmod('s',evar(673),15);
say(76,1500,112);
 if (loop) return(0);}value[0]=(
value[701]==4);
if(value[0]){
bitmod('s',evar(673),15);
say(76,1500,113);
 if (loop) return(0);}
say(64,777,0);
 if (loop) return(0);return 0;
}
int p234(void)
{
value[0]=(ishere(113,-1,-1));
if(value[0]){
value[0]=(have(111,-1,-1));
value[0]|=(have(114,-1,-1));
if(!value[0]){
say(76,910,113);
 if (loop) return(0);}value[0]=(
have(111,0,2));
if(value[0]){
set('E',111,'c',4,NULL,NULL);
say(76,913,113);
 if (loop) return(0);}value[0]=(
have(114,0,2));
if(value[0]){
set('E',114,'c',4,NULL,NULL);
say(76,914,113);
 if (loop) return(0);}value[0]=(
have(111,-1,-1));
value[0]&=(have(114,-1,-1));
if(value[0]){
say(64,1375,0);
 if (loop) return(0);}value[0]=(
have(111,-1,-1));
if(value[0]){
say(64,911,0);
 if (loop) return(0);}
say(64,1376,0);
 if (loop) return(0);}
return 0;
}
int p241(void)
{
value[0]=(bitest(evar(673),9));
if(!value[0]){
return 0;
}value[0]=(
have(81,-1,-1));
if(value[0]){
value[0]=(value[HERE]==412);
if(value[0]){
value[0]=(have(111,-1,-1));
value[0]|=(have(114,-1,-1));
if(!value[0]){
say(64,1274,0);
 if (loop) return(0);}
}else{
bitmod('s',81,13);
bitmod('s',81,14);
say(64,1275,0);
 if (loop) return(0);}}value[0]=(
have(111,-1,-1));
value[0]|=(have(114,-1,-1));
if(!value[0]){
say(76,910,112);
 if (loop) return(0);}value[0]=(
have(111,0,2));
if(value[0]){
set('E',111,'c',3,NULL,NULL);
say(76,913,112);
 if (loop) return(0);}value[0]=(
have(114,0,2));
if(value[0]){
set('E',114,'c',3,NULL,NULL);
say(76,914,112);
 if (loop) return(0);}value[0]=(
have(114,-1,-1));
value[0]&=(have(111,-1,-1));
if(value[0]){
say(64,1375,0);
 if (loop) return(0);}value[0]=(
have(111,-1,-1));
if(value[0]){
say(64,911,0);
 if (loop) return(0);}
say(64,1376,0);
 if (loop) return(0);return 0;
}
int p316(void)
{
value[0]=(value[20]>0);
if(value[0]){
say(64,977,0);
 if (loop) return(0);}value[0]=(
query(837)); if (loop>1) return (0);
if(value[0]){
set('E',20,'c',1,NULL,NULL);
apport(60,247);
apport(117,247);
bitmod('c',20,6);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(ishere(value[677],-1,-1));
if(value[0]){
apport(value[677],247);
}}
move(247,-1453); if (loop) return(0);
}
say(12,1228,671);
say(76,1229,672);
 if (loop) return(0);return 0;
}
int p315(void)
{
value[0]=(value[718]<3);
if(value[0]){
p653();
if (loop) return (0);
say(64,832,0);
 if (loop) return(0);}
say(64,1406,0);
 if (loop) return(0);return 0;
}
int p317(void)
{
value[0]=(query(837)); if (loop>1) return (0);
if(value[0]){
set('V',701,'v',722,NULL,NULL);
value[701] -= value[707];
value[701] += 2;
value[701] *= 10;
value[0]=(irand(100)<value[701]);
if(value[0]){
set('V',706,'c',1,NULL,NULL);
say(0,834,0);
p319();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
irand(100)<value[701]);
if(value[0]){
say(64,835,0);
 if (loop) return(0);}
say(0,836,0);
p93();
if (loop) return (0);
}
say(12,1228,671);
say(76,1229,672);
 if (loop) return(0);return 0;
}
int p318(void)
{
value[0]=(query(837)); if (loop>1) return (0);
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
say(64,1124,0);
 if (loop) return(0);}
say(0,1125,0);
p93();
if (loop) return (0);
}
say(12,1228,671);
say(76,1229,672);
 if (loop) return(0);return 0;
}
int p230(void)
{
if (!isnear(38,-1,-1)) return 0;
value[0]=(have(38,-1,-1));
if(value[0]){
say(64,1702,0);
 if (loop) return(0);}value[0]=(
value[HERE]==299);
value[0]&=(value[38]<2);
if(value[0]){
say(64,979,0);
 if (loop) return(0);}
apport(38,INHAND);
say(64,1701,0);
 if (loop) return(0);return 0;
}
int p240(void)
{
if (!isnear(116,-1,-1)) return 0;
value[0]=(value[116]==0);
value[0]&=(value[707]<value[722]);
if(value[0]){
value[0]=(value[119]==2);
if(value[0]){
set('V',734,'c',5,NULL,NULL);
value[116] += 1;
apport(116,INHAND);
bitmod('c',434,8);
say(64,1095,0);
 if (loop) return(0);}
say(64,1096,0);
 if (loop) return(0);}
return 0;
}
int p236(void)
{
if (!isnear(56,-1,-1)) return 0;
value[0]=(value[56]==0);
value[0]&=(value[707]<value[722]);
if(value[0]){
apport(56,INHAND);
set('E',56,'c',1,NULL,NULL);
value[701] = irand(4);
value[0]=(value[701]==0);
if(value[0]){
value[685]=511;*bitword(685)= -1;
}else{
value[0]=(value[701]==1);
if(value[0]){
value[685]=506;*bitword(685)= -1;
}else{
value[0]=(value[701]==2);
if(value[0]){
value[685]=505;*bitword(685)= -1;
}else{
value[685]=499;*bitword(685)= -1;
}}}
apport(37,485);
value[0]=(value[35]==0);
if(value[0]){
say(76,1138,685);
 if (loop) return(0);}
say(64,1139,0);
 if (loop) return(0);}
return 0;
}
int p543(void)
{
value[679]=1017;*bitword(679)= -1;
value[679] += value[708];
if (*bitword(708*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
say(2,679,0);
p93();
if (loop) return (0);
return 0;
}
int p126(void)
{
bitmod('s',evar(712),1);
p7();
if (loop) return (0);
value[0]=(value[703]==1);
if(value[0]){
return 0;
}
bitmod('s',evar(699),7);
say(0,777,0);
value[679] = irand(1205-1196+1)+1196;
*bitword(679)= -1;
say(2,679,0);
return 0;
}
int p90(void)
{
say(0,937,0);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
bitmod('c',evar(677),4);
value[0]=(value[677]==27);
if(!value[0]){
bitmod('c',evar(677),13);
bitmod('c',evar(677),14);
}value[0]=(
bitest(evar(677),3));
if(value[0]){
value[0]=(have(value[677],-1,-1));
if(value[0]){
apport(value[677],HERE);
}}}
apport(10,379);
bitmod('s',111,13);
set('E',111,'c',2,NULL,NULL);
bitmod('s',11,13);
bitmod('s',107,13);
set('E',107,'c',0,NULL,NULL);
bitmod('s',107,7);
bitmod('c',107,15);
bitmod('s',102,13);
bitmod('s',99,13);
set('E',99,'c',0,NULL,NULL);
set('V',700,'c',0,NULL,NULL);
bitmod('s',134,13);
set('E',134,'c',0,NULL,NULL);
bitmod('s',104,14);
bitmod('s',7,14);
bitmod('s',100,14);
set('E',100,'c',1,NULL,NULL);
bitmod('s',101,14);
set('E',101,'c',1,NULL,NULL);
bitmod('s',105,14);
set('E',120,'c',2,NULL,NULL);
*bitword(678)= -1; value[678]=FLOC-1; while (++value[678]<=LLOC) {
value[0]=(bitest(evar(678),7));
if(value[0]){
bitmod('c',evar(678),4);
}}
bitmod('s',461,4);
bitmod('s',243,4);
set('V',718,'c',3,NULL,NULL);
set('E',143,'c',1,NULL,NULL);
set('V',717,'c',999,NULL,NULL);
set('V',674,'c',0,NULL,NULL);
move(378,-2); if (loop) return(0);
return 0;
}
int p261(void)
{
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(bitest(evar(677),5));
if(value[0]){
value[0]=(getloc(value[677])==141);
value[0]|=(getloc(value[677])==485);
if(!value[0]){
return 0;
}}}value[0]=(
getloc(95)==141);
value[0]|=(getloc(95)==140);
value[0]|=(getloc(95)==485);
if(!value[0]){
return 0;
}value[0]=(
have(64,-1,-1));
value[0]|=(getloc(64)==141);
value[0]|=(getloc(64)==485);
if(!value[0]){
return 0;
}value[0]=(
getloc(80)==141);
value[0]|=(getloc(80)==485);
if(value[0]){
set('V',718,'c',1,NULL,NULL);
}
return 0;
}
int p165(void)
{
bitmod('s',evar(699),7);
value[0]=(value[43]>1);
if(value[0]){
return 0;
}value[0]=(
value[718]==0);
if(value[0]){
bitmod('c',95,5);
bitmod('c',64,5);
p261();
if (loop) return (0);
bitmod('s',64,5);
bitmod('s',95,5);
value[0]=(value[718]==1);
if(value[0]){
set('V',717,'c',35,NULL,NULL);
}else{
value[717] = irand(39-30+1)+30;
}value[0]=(
value[66]>0);
if(value[0]){
value[66] = irand(16 - 1) + 1;
}value[0]=(
value[116]>0);
if(value[0]){
value[116] = irand(14 - 1) + 1;
}value[0]=(
value[20]==1);
if(value[0]){
value[724] -= value[725];
value[0]=(value[724]<0);
if(value[0]){
set('E',20,'c',2,NULL,NULL);
}}value[0]=(
getloc(82)==484);
value[0]|=(isnear(82,-1,-1));
if(!value[0]){
apport(82,485);
}value[0]=(
bitest(27,13));
value[0]&=!(bitest(27,14));
value[0]&=!(isnear(134,-1,-1));
if(value[0]){
bitmod('s',27,14);
say(0,1176,0);
set('V',717,'c',5,NULL,NULL);
set('V',725,'v',717,NULL,NULL);
return 0;
}value[0]=(
value[119]==2);
if(value[0]){
value[723] -= value[725];
value[0]=(value[723]<0);
if(value[0]){
set('E',119,'c',3,NULL,NULL);
set('V',723,'c',40,NULL,NULL);
say(0,119,0);
set('E',119,'c',0,NULL,NULL);
set('V',722,'c',7,NULL,NULL);
set('V',717,'c',8,NULL,NULL);
set('V',725,'v',717,NULL,NULL);
return 0;
}}value[0]=(
bitest(151,4));
value[0]|=(bitest(163,4));
if(value[0]){
value[0]=(bitest(evar(673),7));
value[0]|=(bitest(evar(673),5));
if(value[0]){
value[717] = irand(17-8+1)+8;
}else{
p308();
if (loop) return (0);
value[0]=(value[701]==0);
value[0]&=(value[714]>150);
value[0]&=!(bitest(19,4));
value[0]|=(bitest(19,13));
value[0]|=(irand(100)<10);
value[0]&=!(bitest(49,4));
value[0]&=!(isnear(134,-1,-1));
value[0]&=!(bitest(evar(673),3));
value[0]&=!(bitest(43,12));
if(value[0]){
bitmod('c',19,13);
set('V',702,'c',0,NULL,NULL);
value[0]=(value[64]==2);
if(value[0]){
bitmod('c',64,5);
}value[0]=(
bitest(62,12));
if(value[0]){
bitmod('c',62,5);
}value[0]=(
bitest(67,12));
if(value[0]){
bitmod('c',67,5);
}
value[678]=getloc(95);*bitword(678)= -1;
value[0]=(value[678]==value[673]);
if(value[0]){
bitmod('c',95,5);
}
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(isnear(value[677],1,5));
if(value[0]){
apport(value[677],192);
set('V',702,'c',1,NULL,NULL);
}}
bitmod('s',64,5);
bitmod('s',95,5);
bitmod('s',62,5);
bitmod('s',67,5);
value[0]=(value[702]==0);
if(value[0]){
value[0]=(bitest(19,4));
value[0]|=(value[714]<150);
if(value[0]){
say(0,931,0);
bitmod('s',19,13);
value[717] = irand(13-4+1)+4;
}else{
apport(49,192);
apport(29,311);
bitmod('s',19,4);
say(0,998,0);
}
}else{
value[0]=(bitest(19,4));
if(value[0]){
say(0,933,0);
}else{
say(0,932,0);
bitmod('s',19,4);
apport(49,192);
apport(29,311);
}}
}else{
set('V',701,'v',716,NULL,NULL);
value[701] += 2;
value[701] *= 10;
value[0]=(isnear(134,-1,-1));
value[0]|=(irand(100)<value[701]);
value[0]&=(value[134]<value[716]);
if(value[0]){
value[0]=(bitest(115,4));
if(value[0]){
apport(134,value[673]);
value[134] += 1;
value[0]=(value[134]==1);
if(value[0]){
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
say(0,134,0);
}
bitmod('s',134,13);
bitmod('c',134,14);
}value[0]=(
value[688]==0);
if(value[0]){
p85();
if (loop) return (0);
}
}else{
value[0]=(bitest(evar(673),11));
if(!value[0]){
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
apport(115,value[673]);
bitmod('s',115,4);
say(0,784,0);
}}}}}}}
}else{
value[0]=(value[718]==1);
if(value[0]){
set('V',718,'c',2,NULL,NULL);
set('E',4,'c',0,NULL,NULL);
say(0,934,0);
value[0]=(isnear(134,-1,-1));
if(value[0]){
say(13,1038,134);
}
apport(134,484);
set('E',134,'c',0,NULL,NULL);
set('V',716,'c',0,NULL,NULL);
set('V',761,'c',0,NULL,NULL);
set('V',688,'c',0,NULL,NULL);
set('E',8,'c',0,NULL,NULL);
set('E',30,'c',0,NULL,NULL);
apport(22,485);
apport(20,485);
set('E',22,'c',4,NULL,NULL);
apport(23,244);
bitmod('s',8,10);
bitmod('s',30,10);
apport(82,485);
set('V',717,'c',30,NULL,NULL);
}else{
value[0]=(bitest(evar(712),3));
if(value[0]){
bitmod('c',evar(712),3);
set('V',717,'c',15,NULL,NULL);
}else{
p90();
if (loop) return (0);
}}}
set('V',725,'v',717,NULL,NULL);
return 0;
}
int p18(void)
{
value[0]=(KEY(113));
if(value[0]){
p250();
if (loop) return (0);
}value[0]=(
KEY(112));
if(value[0]){
p251();
if (loop) return (0);
}value[0]=(
value[699]==1);
if(value[0]){
value[0]=(532<=value[671] && value[671]<=569);
value[0]|=(KEY(493));
if(value[0]){
bitmod('s',evar(699),5);
say(76,1037,671);
 if (loop) return(0);}value[0]=(
611<=value[671] && value[671]<=625);
if(value[0]){
say(0,938,0);
}else{
value[0]=(626<=value[671] && value[671]<=655);
if(value[0]){
say(12,819,671);
}else{
say(0,1510,0);
}}
}else{
value[0]=(value[699]==2);
value[0]&=(bitest(evar(672),0));
if(value[0]){
value[0]=(KEY(112));
value[0]&=(bitest(evar(673),9));
if(value[0]){
say(64,1595,0);
 if (loop) return(0);}value[0]=(
isnear(value[672],-1,-1));
if(value[0]){
say(12,1511,671);
say(12,1512,672);
}else{
say(12,819,672);
}
}else{
value[0]=(626<=value[672] && value[672]<=659);
if(value[0]){
value[0]=(KEY(549));
if(value[0]){
say(0,916,0);
}else{
value[0]=(626<=value[672] && value[672]<=655);
if(value[0]){
say(12,819,672);
}else{
say(12,1511,671);
say(12,1512,672);
}}
}else{
value[701] = irand(1009-1005+1)+1005;
*bitword(701)= -1;
say(2,701,0);
}}}
loop=1; return(0);
return 0;
}
int p323(void)
{
value[0]=(value[700]>0);
if(value[0]){
value[0]=(value[39]==3);
if(value[0]){
say(0,1001,0);
}else{
value[0]=(isnear(39,-1,-1));
if(value[0]){
say(0,1000,0);
set('E',39,'c',3,NULL,NULL);
set('V',700,'c',400,NULL,NULL);
}else{
value[0]=(bitest(39,4));
if(value[0]){
say(0,999,0);
}else{
value[0]=(168<=value[673] && value[673]<=193);
if(value[0]){
say(9,995,1);
}else{
value[0]=(300<=value[673] && value[673]<=311);
if(value[0]){
say(9,995,2);
}else{
say(9,995,0);
}}}}}
}else{
value[0]=(value[718]==2);
if(value[0]){
p90();
if (loop) return (0);
}else{
value[0]=(isnear(39,0,1));
if(value[0]){
say(0,1000,0);
set('E',39,'c',3,NULL,NULL);
value[700] += 400;
}else{
say(0,996,0);
set('E',99,'c',0,NULL,NULL);
bitmod('s',evar(712),5);
p442();
if (loop) return (0);
}}}
return 0;
}
int p646(void)
{
bitmod('s',evar(673),14);
value[0]=(328<=value[673] && value[673]<=364);
if(value[0]){
*bitword(678)= -1;value[678]=328-1; while(++value[678]<=364) {
bitmod('s',evar(678),14);
}}value[0]=(
271<=value[673] && value[673]<=289);
if(value[0]){
*bitword(678)= -1;value[678]=271-1; while(++value[678]<=289) {
bitmod('s',evar(678),14);
}}value[0]=(
385<=value[673] && value[673]<=390);
if(value[0]){
*bitword(678)= -1;value[678]=385-1; while(++value[678]<=390) {
bitmod('s',evar(678),14);
}}value[0]=(
168<=value[673] && value[673]<=193);
if(value[0]){
*bitword(678)= -1;value[678]=168-1; while(++value[678]<=193) {
bitmod('s',evar(678),14);
}}value[0]=(
300<=value[673] && value[673]<=311);
if(value[0]){
*bitword(678)= -1;value[678]=300-1; while(++value[678]<=311) {
bitmod('s',evar(678),14);
}}
return 0;
}
int p257(void)
{
value[0]=(bitest(evar(673),14));
value[0]&=!(KEY(577));
value[0]|=!(bitest(evar(673),8));
if(value[0]){
return 0;
}
set('V',679,'c',0,NULL,NULL);
value[0]=(value[HERE]==434);
value[0]&=(KEY(577));
if(value[0]){
set('V',721,'c',0,NULL,NULL);
say(64,1481,0);
 if (loop) return(0);}value[0]=(
value[HERE]==144);
value[0]&=(value[4]==0);
value[0]&=!(have(98,-1,-1));
if(value[0]){
value[679]=860;*bitword(679)= -1;
}value[0]=(
value[HERE]==149);
value[0]&=(isnear(101,0,0));
value[0]&=(have(102,-1,-1));
if(value[0]){
value[679]=807;*bitword(679)= -1;
}value[0]=(
value[HERE]==155);
value[0]&=(isnear(7,-1,-1));
if(value[0]){
value[0]=(bitest(evar(736),0));
if(value[0]){
value[0]=(KEY(577));
value[0]&=!(bitest(evar(673),14));
if(!value[0]){
value[679]=1586;*bitword(679)= -1;
}
}else{
value[0]=(getloc(101)==485);
if(value[0]){
value[679]=811;*bitword(679)= -1;
}else{
value[679]=809;*bitword(679)= -1;
}}}value[0]=(
value[HERE]==236);
if(value[0]){
value[679]=992;*bitword(679)= -1;
}value[0]=(
value[HERE]==228);
value[0]|=(value[HERE]==227);
value[0]|=(value[HERE]==229);
value[0]&=!(bitest(229,4));
if(value[0]){
value[679]=990;*bitword(679)= -1;
}value[0]=(
value[HERE]==256);
if(value[0]){
value[679]=1146;*bitword(679)= -1;
}value[0]=(
168<=value[673] && value[673]<=193);
value[0]|=(300<=value[673] && value[673]<=311);
value[0]|=(385<=value[673] && value[673]<=390);
if(value[0]){
value[679]=988;*bitword(679)= -1;
}value[0]=(
328<=value[673] && value[673]<=364);
if(value[0]){
value[679]=1223;*bitword(679)= -1;
}value[0]=(
271<=value[673] && value[673]<=289);
if(value[0]){
value[0]=(KEY(577));
value[0]&=!(bitest(evar(673),14));
if(value[0]){
return 0;
}else{
value[679]=1326;*bitword(679)= -1;
}}value[0]=(
value[HERE]==427);
value[0]|=(value[HERE]==428);
if(value[0]){
value[679]=1661;*bitword(679)= -1;
}value[0]=(
value[HERE]==239);
if(value[0]){
value[679]=1324;*bitword(679)= -1;
}value[0]=(
isnear(16,-1,-1));
if(value[0]){
value[679]=1438;*bitword(679)= -1;
}value[0]=(
value[HERE]==252);
if(value[0]){
value[679]=1443;*bitword(679)= -1;
}value[0]=(
value[HERE]==264);
if(value[0]){
value[679]=1524;*bitword(679)= -1;
}value[0]=(
value[HERE]==254);
if(value[0]){
value[0]=(bitest(evar(736),1));
if(value[0]){
value[0]=(KEY(577));
value[0]&=!(bitest(evar(673),14));
if(!value[0]){
value[679]=1519;*bitword(679)= -1;
}
}else{
value[679]=1445;*bitword(679)= -1;
bitmod('s',evar(736),1);
}}value[0]=(
isnear(24,-1,-1));
if(value[0]){
value[679]=1448;*bitword(679)= -1;
}value[0]=(
isnear(35,-1,-1));
if(value[0]){
value[679]=1450;*bitword(679)= -1;
}value[0]=(
value[HERE]==259);
if(value[0]){
value[0]=(KEY(577));
if(value[0]){
value[679]=1459;*bitword(679)= -1;
}else{
return 0;
}}value[0]=(
value[679]==0);
if(value[0]){
return 0;
}
set('V',721,'c',0,NULL,NULL);
value[0]=(query(value[679])); if (loop>1) return (0);
if(value[0]){
say(9,986,20);
value[0]=(query(987)); if (loop>1) return (0);
if(value[0]){
value[679] += 1;
say(2,679,0);
value[709] += 20;
bitmod('c',evar(673),8);
bitmod('c',evar(673),14);
value[679] -= 1;
value[0]=(value[HERE]==427);
if(value[0]){
bitmod('c',428,8);
}value[0]=(
value[HERE]==428);
if(value[0]){
bitmod('c',427,8);
}value[0]=(
isnear(7,-1,-1));
if(value[0]){
value[0]=(value[679]==1586);
if(value[0]){
bitmod('c',evar(736),0);
}else{
bitmod('s',evar(673),8);
bitmod('s',evar(736),0);
}}value[0]=(
value[HERE]==254);
if(value[0]){
value[0]=(value[679]==1519);
if(value[0]){
bitmod('c',evar(736),1);
}else{
bitmod('s',evar(673),8);
bitmod('s',evar(736),1);
}}value[0]=(
value[HERE]==228);
value[0]|=(value[HERE]==227);
value[0]|=(value[HERE]==229);
if(value[0]){
bitmod('c',228,8);
bitmod('c',229,8);
bitmod('c',227,8);
}value[0]=(
value[HERE]==254);
value[0]&=(bitest(197,4));
value[0]&=(bitest(evar(736),1));
if(value[0]){
say(0,1447,0);
}value[0]=(
isnear(35,-1,-1));
if(value[0]){
bitmod('c',314,8);
bitmod('c',315,8);
}value[0]=(
value[HERE]==236);
if(value[0]){
bitmod('s',evar(736),2);
}value[0]=(
328<=value[673] && value[673]<=364);
if(value[0]){
*bitword(678)= -1;value[678]=328-1; while(++value[678]<=364) {
bitmod('c',evar(678),8);
}}value[0]=(
271<=value[673] && value[673]<=289);
if(value[0]){
*bitword(678)= -1;value[678]=271-1; while(++value[678]<=289) {
bitmod('c',evar(678),8);
}}value[0]=(
385<=value[673] && value[673]<=390);
if(value[0]){
*bitword(678)= -1;value[678]=385-1; while(++value[678]<=390) {
bitmod('c',evar(678),8);
}}value[0]=(
168<=value[673] && value[673]<=193);
if(value[0]){
*bitword(678)= -1;value[678]=168-1; while(++value[678]<=193) {
bitmod('c',evar(678),8);
}}value[0]=(
300<=value[673] && value[673]<=311);
if(value[0]){
*bitword(678)= -1;value[678]=300-1; while(++value[678]<=311) {
bitmod('c',evar(678),8);
}}value[0]=(
isnear(16,-1,-1));
if(value[0]){
bitmod('c',165,8);
bitmod('c',166,8);
}
}else{
p646();
if (loop) return (0);
}
}else{
p646();
if (loop) return (0);
}
return 0;
}
int p404(void)
{
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
value[0]=(KEY(527));
if(value[0]){
set('V',676,'v',672,NULL,NULL);
}else{
set('V',676,'v',671,NULL,NULL);
}
value[679] = irand(798-793+1)+793;
*bitword(679)= -1;
say(78,679,676);
 if (loop) return(0);}value[0]=(
irand(100)<25);
value[0]&=!(value[HERE]==393);
value[0]&=(value[718]<3);
if(value[0]){
say(0,813,0);
p93();
if (loop) return (0);
}
say(64,799,0);
 if (loop) return(0);return 0;
}
int p45(void)
{
apport(118,485);
say(0,1098,0);
value[679] = irand(1105-1099+1)+1099;
*bitword(679)= -1;
say(2,679,0);
value[0]=(isnear(33,-1,-1));
if(value[0]){
set('E',118,'c',8,NULL,NULL);
apport(33,485);
}else{
value[0]=(isnear(134,-1,-1));
if(value[0]){
say(0,777,0);
value[0]=(value[134]==1);
if(value[0]){
set('E',118,'c',1,NULL,NULL);
}else{
set('E',118,'c',2,NULL,NULL);
}
set('V',706,'e',134,NULL,NULL);
say(0,118,0);
p319();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
isnear(22,-1,-1));
if(value[0]){
set('E',118,'c',3,NULL,NULL);
}value[0]=(
isnear(38,-1,-1));
if(value[0]){
set('E',118,'c',4,NULL,NULL);
value[0]=(value[38]>0);
if(value[0]){
set('E',118,'c',5,NULL,NULL);
}}value[0]=(
isnear(7,-1,-1));
if(value[0]){
set('E',118,'c',6,NULL,NULL);
}value[0]=(
isnear(101,-1,-1));
if(value[0]){
set('E',118,'c',7,NULL,NULL);
}value[0]=(
isnear(20,-1,-1));
value[0]&=(value[20]==0);
if(value[0]){
set('E',118,'c',9,NULL,NULL);
}value[0]=(
isnear(27,-1,-1));
if(value[0]){
set('E',118,'c',10,NULL,NULL);
}value[0]=(
isnear(25,-1,-1));
value[0]&=(value[25]<2);
if(value[0]){
set('E',118,'c',11,NULL,NULL);
}value[0]=(
isnear(121,-1,-1));
if(value[0]){
set('E',118,'c',12,NULL,NULL);
apport(121,484);
}value[0]=(
isnear(87,-1,-1));
if(value[0]){
set('E',118,'c',13,NULL,NULL);
}value[0]=(
isnear(24,-1,-1));
if(value[0]){
set('E',118,'c',14,NULL,NULL);
}}value[0]=(
value[118]>0);
if(value[0]){
say(0,118,0);
}
loop=1; return(0);
return 0;
}
int p140(void)
{
value[0]=(have(118,-1,-1));
value[0]&=(irand(100)<10);
if(value[0]){
say(0,1097,0);
apport(118,485);
p93();
if (loop) return (0);
}
return 0;
}
int p145(void)
{
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
set('V',702,'e',134,NULL,NULL);
value[702] += 2;
value[702] = irand(value[702]);
value[702] -= 1;
value[0]=(value[702]<1);
if(value[0]){
return 0;
}
say(13,785,702);
set('V',703,'v',722,NULL,NULL);
value[703] -= value[707];
value[703] *= 5;
value[0]=(bitest(134,14));
if(value[0]){
value[703] -= 20;
}
value[729] += value[703];
if (*bitword(703*VARSIZE)==-1) *bitword(729*VARSIZE)= -1;
value[729] += 35;
value[729] /= value[702];
value[0]=(value[64]==2);
if(value[0]){
value[703] = irand(5);
value[0]=(value[703]>0);
if(value[0]){
value[679]=787;*bitword(679)= -1;
value[679] += value[703];
if (*bitword(703*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
say(15,679,702);
return 0;
}}value[0]=(
irand(100)<value[729]);
value[0]|=(bitest(134,13));
if(value[0]){
say(13,786,702);
bitmod('c',134,13);
}else{
say(13,787,702);
p93();
if (loop) return (0);
}}
return 0;
}
int p625(void)
{
value[0]=(have(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(556));
if(value[0]){
apport(value[672],HERE);
}value[0]=(
isnear(134,-1,-1));
if(value[0]){
set('V',701,'v',722,NULL,NULL);
value[701] -= value[707];
value[701] *= 5;
set('V',702,'e',134,NULL,NULL);
value[702] += 2;
value[702] *= 15;
value[701] += value[702];
if (*bitword(702*VARSIZE)==-1) *bitword(701*VARSIZE)= -1;
value[0]=(KEY(115));
value[0]|=(KEY(546));
if(value[0]){
value[0]=(KEY(115));
value[0]&=(KEY(546));
if(!value[0]){
value[701] += 15;
}}value[0]=(
irand(100)<value[701]);
if(value[0]){
value[0]=(irand(100)<5);
value[0]&=!(bitest(evar(712),7));
if(value[0]){
say(12,953,672);
bitmod('s',evar(712),7);
}else{
say(0,952,0);
}
set('V',706,'c',1,NULL,NULL);
p319();
if (loop) return (0);
value[0]=(value[134]>0);
if(value[0]){
set('V',729,'c',0,NULL,NULL);
set('V',728,'c',100,NULL,NULL);
}
}else{
say(0,835,0);
set('V',729,'c',0,NULL,NULL);
set('V',728,'c',100,NULL,NULL);
}
}else{
value[0]=(isnear(7,-1,-1));
if(value[0]){
value[0]=(KEY(556));
if(value[0]){
apport(value[672],INHAND);
}
say(64,833,0);
 if (loop) return(0);}value[0]=(
isnear(20,-1,-1));
if(value[0]){
say(76,956,672);
 if (loop) return(0);}value[0]=(
isnear(38,-1,-1));
if(value[0]){
value[0]=(value[38]==0);
if(value[0]){
value[0]=(KEY(556));
if(value[0]){
value[0]=(KEY(115));
if(value[0]){
set('E',115,'c',1,NULL,NULL);
say(64,975,0);
 if (loop) return(0);}
say(64,1108,0);
 if (loop) return(0);}value[0]=(
irand(100)<50);
if(value[0]){
say(64,1109,0);
 if (loop) return(0);}
say(0,1110,0);
p93();
if (loop) return (0);
}
say(76,976,38);
 if (loop) return(0);}value[0]=(
isnear(22,-1,-1));
if(value[0]){
say(64,968,0);
 if (loop) return(0);}value[0]=(
isnear(24,-1,-1));
if(value[0]){
value[0]=(KEY(546));
if(value[0]){
say(12,1121,672);
p93();
if (loop) return (0);
}value[0]=(
KEY(115));
if(value[0]){
say(12,1122,672);
p93();
if (loop) return (0);
}
bitmod('c',323,8);
apport(24,485);
apport(116,HERE);
set('V',734,'c',6,NULL,NULL);
bitmod('c',323,5);
say(64,1123,0);
 if (loop) return(0);}value[0]=(
isnear(17,-1,-1));
if(value[0]){
say(76,1143,672);
 if (loop) return(0);}value[0]=(
isnear(25,-1,-1));
if(value[0]){
value[0]=(value[25]>1);
if(value[0]){
value[0]=(KEY(556));
if(value[0]){
apport(value[672],INHAND);
}
say(64,977,0);
 if (loop) return(0);}
say(12,1152,672);
p93();
if (loop) return (0);
}value[0]=(
isnear(27,-1,-1));
if(value[0]){
say(76,1153,672);
 if (loop) return(0);}value[0]=(
isnear(121,-1,-1));
if(value[0]){
say(12,1154,672);
p93();
if (loop) return (0);
}value[0]=(
isnear(135,-1,-1));
if(value[0]){
say(12,1320,672);
p93();
if (loop) return (0);
}value[0]=(
KEY(556));
if(value[0]){
apport(value[672],INHAND);
return 0;
}
say(12,1496,671);
say(76,1497,672);
 if (loop) return(0);}
}else{
say(76,1026,672);
 if (loop) return(0);}
loop=1; return(0);
return 0;
}
int p436(void)
{
value[0]=(isnear(35,-1,-1));
if(value[0]){
bitmod('c',314,8);
bitmod('c',315,8);
value[0]=(value[35]==0);
if(value[0]){
value[0]=(value[699]==2);
value[0]&=(value[672]==value[685]);
value[0]|=(value[671]==value[685]);
if(value[0]){
set('E',35,'c',1,NULL,NULL);
bitmod('s',35,13);
value[0]=(bitest(462,4));
if(!value[0]){
set('V',734,'c',11,NULL,NULL);
}
say(64,1137,0);
 if (loop) return(0);}
value[0]=!(bitest(35,13));
if(value[0]){
set('E',35,'c',2,NULL,NULL);
set('E',17,'c',1,NULL,NULL);
bitmod('s',evar(712),0);
bitmod('s',evar(712),2);
set('E',4,'c',0,NULL,NULL);
say(64,1140,0);
 if (loop) return(0);}}}
say(64,829,0);
 if (loop) return(0);return 0;
}
int p582(void)
{
value[0]=(value[17]>0);
if(value[0]){
value[17] += 1;
value[0]=(value[17]>15);
if(value[0]){
apport(17,value[673]);
}
value[679]=1528;*bitword(679)= -1;
value[679] += value[17];
value[679] -= 2;
say(2,679,0);
value[0]=(value[17]==18);
if(value[0]){
p93();
if (loop) return (0);
}}
return 0;
}
int p462(void)
{
value[0]=(value[699]>1);
if(value[0]){
say(12,1039,672);
}
return 0;
}
int p460(void)
{
move(485,-1);
value[0]=(value[99]==1);
if(value[0]){
set('V',700,'c',0,NULL,NULL);
value[0]=(have(99,-1,-1));
if(value[0]){
say(0,1164,0);
}else{
say(0,1165,0);
}
}else{
say(0,1165,0);
}
p93();
if (loop) return (0);
return 0;
}
int p603(void)
{
value[0]=(KEY(101));
value[0]|=(KEY(38));
if(value[0]){
return 0;
}
set('V',679,'c',0,NULL,NULL);
value[0]=(value[HERE]==140);
if(value[0]){
value[679]=1206;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==150);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=151;*bitword(678)= -1;
}value[0]=(
value[HERE]==152);
if(value[0]){
value[679]=1208;*bitword(679)= -1;
value[678]=224;*bitword(678)= -1;
}value[0]=(
value[HERE]==153);
if(value[0]){
value[679]=1208;*bitword(679)= -1;
value[678]=224;*bitword(678)= -1;
}value[0]=(
value[HERE]==156);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=159;*bitword(678)= -1;
}value[0]=(
value[HERE]==157);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=158;*bitword(678)= -1;
}value[0]=(
value[HERE]==160);
if(value[0]){
value[679]=1209;*bitword(679)= -1;
value[678]=198;*bitword(678)= -1;
}value[0]=(
value[HERE]==165);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=237;*bitword(678)= -1;
}value[0]=(
value[HERE]==166);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=237;*bitword(678)= -1;
}value[0]=(
value[HERE]==199);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=200;*bitword(678)= -1;
}value[0]=(
value[HERE]==201);
if(value[0]){
value[679]=1209;*bitword(679)= -1;
value[678]=202;*bitword(678)= -1;
}value[0]=(
value[HERE]==193);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=149;*bitword(678)= -1;
}value[0]=(
value[HERE]==210);
if(value[0]){
value[679]=1210;*bitword(679)= -1;
value[678]=209;*bitword(678)= -1;
}value[0]=(
value[HERE]==211);
if(value[0]){
value[679]=1210;*bitword(679)= -1;
value[678]=207;*bitword(678)= -1;
}value[0]=(
value[HERE]==215);
if(value[0]){
value[679]=1211;*bitword(679)= -1;
value[678]=216;*bitword(678)= -1;
}value[0]=(
value[HERE]==221);
if(value[0]){
value[679]=1210;*bitword(679)= -1;
value[678]=213;*bitword(678)= -1;
}value[0]=(
value[HERE]==224);
if(value[0]){
value[679]=1212;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==230);
if(value[0]){
value[679]=1213;*bitword(679)= -1;
value[678]=224;*bitword(678)= -1;
}value[0]=(
value[HERE]==238);
if(value[0]){
value[679]=1210;*bitword(679)= -1;
value[678]=176;*bitword(678)= -1;
}value[0]=(
value[HERE]==239);
value[0]|=(value[HERE]==240);
if(value[0]){
value[679]=1221;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==242);
if(value[0]){
value[679]=1210;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==244);
if(value[0]){
value[679]=1214;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==245);
if(value[0]){
value[679]=1214;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==252);
if(value[0]){
value[679]=1215;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==253);
if(value[0]){
value[679]=1215;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==261);
if(value[0]){
value[679]=1216;*bitword(679)= -1;
value[678]=260;*bitword(678)= -1;
}value[0]=(
value[HERE]==463);
if(value[0]){
value[679]=1220;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==262);
if(value[0]){
value[679]=1217;*bitword(679)= -1;
value[678]=255;*bitword(678)= -1;
}value[0]=(
value[HERE]==265);
if(value[0]){
value[679]=1218;*bitword(679)= -1;
value[678]=266;*bitword(678)= -1;
}value[0]=(
value[HERE]==266);
if(value[0]){
value[679]=1218;*bitword(679)= -1;
value[678]=267;*bitword(678)= -1;
}value[0]=(
value[HERE]==267);
if(value[0]){
value[679]=1218;*bitword(679)= -1;
value[678]=268;*bitword(678)= -1;
}value[0]=(
value[HERE]==325);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==326);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==327);
if(value[0]){
value[679]=1219;*bitword(679)= -1;
value[678]=328;*bitword(678)= -1;
}value[0]=(
value[HERE]==365);
if(value[0]){
value[679]=1207;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==376);
if(value[0]){
value[679]=1220;*bitword(679)= -1;
value[678]=377;*bitword(678)= -1;
}value[0]=(
value[HERE]==382);
if(value[0]){
value[679]=1215;*bitword(679)= -1;
value[678]=485;*bitword(678)= -1;
}value[0]=(
value[HERE]==204);
if(value[0]){
value[679]=1210;*bitword(679)= -1;
value[678]=203;*bitword(678)= -1;
}value[0]=(
value[HERE]==395);
if(value[0]){
value[679]=955;*bitword(679)= -1;
value[678]=394;*bitword(678)= -1;
}value[0]=(
value[HERE]==408);
if(value[0]){
value[679]=955;*bitword(679)= -1;
value[678]=203;*bitword(678)= -1;
}value[0]=(
399<=value[673] && value[673]<=404);
if(value[0]){
value[679]=955;*bitword(679)= -1;
set('V',678,'v',673,NULL,NULL);
value[678] -= 1;
}value[0]=(
value[HERE]==394);
if(value[0]){
set('V',679,'c',1,NULL,NULL);
}value[0]=(
value[HERE]==151);
if(value[0]){
value[679]=1685;*bitword(679)= -1;
value[0]=(bitest(evar(672),11));
if(value[0]){
value[679] += 1;
}
value[678]=150;*bitword(678)= -1;
}value[0]=(
value[679]==0);
if(value[0]){
return 0;
}value[0]=(
value[HERE]==394);
if(value[0]){
p580();
if (loop) return (0);
}else{
say(14,679,672);
apport(value[672],value[678]);
}value[0]=(
KEY(53));
if(value[0]){
apport(53,485);
apport(54,value[678]);
value[0]=(getloc(54)==485);
if(!value[0]){
say(64,1222,0);
 if (loop) return(0);}}value[0]=(
value[HERE]==151);
if(value[0]){
value[0]=(KEY(44));
if(value[0]){
set('E',154,'c',1,NULL,NULL);
set('E',7,'c',1,NULL,NULL);
say(64,1687,0);
 if (loop) return(0);}}value[0]=(
KEY(111));
if(value[0]){
value[0]=(value[111]==3);
value[0]|=(value[111]==4);
if(value[0]){
bitmod('s',evar(678),15);
}}value[0]=(
KEY(113));
value[0]|=(KEY(112));
if(value[0]){
set('V',702,'c',3,NULL,NULL);
value[0]=(KEY(113));
if(value[0]){
set('V',702,'c',4,NULL,NULL);
}value[0]=(
have(111,0,value[702]));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}else{
value[0]=(have(114,0,value[702]));
if(value[0]){
set('E',114,'c',2,NULL,NULL);
}}}value[0]=(
KEY(100));
value[0]&=(have(101,-1,-1));
if(value[0]){
apport(101,value[678]);
value[0]=(getloc(101)==485);
if(!value[0]){
value[0]=(value[HERE]==151);
if(value[0]){
set('V',702,'c',1,NULL,NULL);
}else{
set('V',702,'c',0,NULL,NULL);
say(0,777,0);
}
say(12,1550,702);
}
loop=1; return(0);
}value[0]=(
KEY(99));
value[0]&=(value[99]==1);
if(value[0]){
p308();
if (loop) return (0);
value[0]=(value[701]>0);
if(value[0]){
say(64,806,0);
 if (loop) return(0);}}value[0]=(
value[HERE]==151);
if(value[0]){
say(0,777,0);
}
loop=1; return(0);
return 0;
}
int p248(void)
{
value[0]=(value[699]==1);
if(value[0]){
say(64,1225,0);
 if (loop) return(0);}value[0]=(
bitest(evar(672),0));
if(value[0]){
value[0]=(have(value[672],-1,-1));
value[0]|=!(bitest(evar(672),3));
if(value[0]){
return 0;
}
set('V',701,'v',707,NULL,NULL);
value[701] -= value[722];
value[701] *= 5;
value[701] += 60;
value[0]=(irand(100)<value[701]);
if(value[0]){
say(12,1226,672);
p93();
if (loop) return (0);
}
say(76,1227,672);
 if (loop) return(0);}
return 0;
}
int p463(void)
{
value[733] += 1;
value[0]=(irand(100)<40);
if(value[0]){
value[733] -= 2;
value[0]=(value[733]<0);
if(value[0]){
set('V',733,'c',1,NULL,NULL);
}}
value[679]=1243;*bitword(679)= -1;
value[679] += value[733];
if (*bitword(733*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
say(0,777,0);
value[0]=(value[733]==7);
if(value[0]){
value[0]=(value[HERE]==420);
value[0]|=(value[HERE]==421);
if(value[0]){
value[679] += 1;
}value[0]=(
have(110,-1,-1));
if(value[0]){
apport(110,484);
}
move(419,-1);
say(2,679,0);
p93();
if (loop) return (0);
}
say(2,679,0);
return 0;
}
int p243(void)
{
value[0]=(value[135]>3);
if(value[0]){
value[0]=(have(95,-1,-1));
value[0]|=(bitest(135,14));
value[0]&=(bitest(27,14));
if(value[0]){
say(0,1372,0);
apport(135,485);
set('E',135,'c',0,NULL,NULL);
}else{
value[0]=(irand(100)<20);
value[0]|=!(bitest(27,14));
if(value[0]){
bitmod('s',135,14);
}
set('E',135,'c',5,NULL,NULL);
move(139,-1368); if (loop) return(0);
}
return 0;
}value[0]=(
have(88,-1,-1));
if(value[0]){
say(0,1312,0);
value[0]=(bitest(27,14));
if(!value[0]){
say(0,1313,0);
}
apport(83,value[673]);
apport(135,140);
set('E',135,'c',4,NULL,NULL);
apport(88,485);
return 0;
}value[0]=(
have(50,-1,-1));
if(value[0]){
set('E',135,'c',2,NULL,NULL);
value[758] = irand(60-40+1)+40;
say(8,1311,0);
say(0,1314,0);
apport(50,484);
value[0]=(bitest(385,4));
if(!value[0]){
set('V',734,'c',3,NULL,NULL);
}
}else{
value[0]=(value[135]==0);
if(value[0]){
value[0]=(bitest(135,4));
value[0]&=!(have(110,-1,-1));
value[0]&=!(bitest(135,13));
value[0]&=(irand(100)<10);
if(value[0]){
say(0,1321,0);
bitmod('s',135,13);
return 0;
}else{
set('V',703,'c',0,NULL,NULL);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
value[0]&=!(value[677]==99);
if(value[0]){
value[0]=(bitest(evar(677),16));
if(value[0]){
value[702]=value[value[677]];*bitword(702)= *bitword(677);value[0]=(value[702]<2);
if(value[0]){
apport(value[677],HERE);
value[703] += 1;
}
}else{
apport(value[677],HERE);
value[703] += 1;
}}}value[0]=(
isnear(110,-1,-1));
if(value[0]){
value[703] -= 1;
}
say(12,1311,703);
value[0]=(isnear(110,-1,-1));
if(value[0]){
apport(110,484);
move(412,-1315); if (loop) return(0);
}else{
set('E',135,'c',1,NULL,NULL);
value[758] = irand(10-6+1)+6;
value[759] = irand(10-8+1)+8;
move(384,-1316); if (loop) return(0);
}}}value[0]=(
value[135]==1);
if(value[0]){
say(8,1311,0);
value[0]=(value[758]>0);
if(value[0]){
move(384,-1316); if (loop) return(0);
}
say(0,1317,0);
set('E',135,'c',0,NULL,NULL);
p93();
if (loop) return (0);
}}
return 0;
}
int p143(void)
{
value[0]=(value[135]==1);
if(value[0]){
p308();
if (loop) return (0);
value[0]=(value[701]==0);
value[0]&=(value[759]==0);
if(value[0]){
say(0,1268,0);
}value[0]=(
value[758]<1);
if(value[0]){
set('V',701,'c',9,NULL,NULL);
value[701] += value[758];
if (*bitword(758*VARSIZE)==-1) *bitword(701*VARSIZE)= -1;
value[0]=(value[701]<1);
if(value[0]){
set('V',701,'c',1,NULL,NULL);
}
value[701] *= 10;
value[0]=(irand(100)<value[701]);
if(value[0]){
say(0,1318,0);
}else{
say(0,1319,0);
say(0,1317,0);
move(383,-1);
set('E',135,'c',0,NULL,NULL);
p93();
if (loop) return (0);
}}}
return 0;
}
int p571(void)
{
value[760] -= value[707];
value[0]=(value[760]<155);
value[0]&=(value[760]>135);
if(value[0]){
say(0,1348,0);
value[760] = irand(135-110+1)+110;
}else{
value[0]=(value[760]<60);
value[0]&=(value[760]>40);
if(value[0]){
say(0,1349,0);
value[760] = irand(39-25+1)+25;
}else{
value[0]=(value[760]<0);
if(value[0]){
say(0,1350,0);
value[760] = irand(650-550+1)+550;
p93();
if (loop) return (0);
}}}
return 0;
}
int p518(void)
{
value[0]=(bitest(16,14));
if(value[0]){
bitmod('s',16,13);
bitmod('c',16,14);
}
return 0;
}
int p199(void)
{
value[0]=(value[715]>-1);
if(value[0]){
say(0,1079,0);
}else{
say(0,829,0);
}
set('V',715,'c',0,NULL,NULL);
return 0;
}
int p447(void)
{
value[0]=(bitest(evar(672),16));
if(value[0]){
value[702]=value[value[672]];*bitword(702)= *bitword(672);value[0]=(value[702]>1);
if(value[0]){
value[value[672]]=1;
value[0]=(value[672]==64);
if(!value[0]){
bitmod('c',evar(672),12);
}}}value[0]=(
bitest(evar(672),7));
if(value[0]){
value[702]=value[value[672]];*bitword(702)= *bitword(672);value[0]=(value[702]==0);
if(value[0]){
value[0]=(value[672]==111);
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}else{
value[value[672]]=1;
}}}
return 0;
}
int p197(void)
{
value[0]=(value[680]==value[674]);
if(value[0]){
move(value[681],-2); if (loop) return(0);
}value[0]=(
value[681]==value[674]);
if(value[0]){
move(value[680],-2); if (loop) return(0);
}
say(64,800,0);
 if (loop) return(0);return 0;
}
int p457(void)
{
value[0]=(bitest(8,13));
if(value[0]){
say(0,1495,0);
}else{
say(0,1494,0);
bitmod('s',8,13);
}
return 0;
}
int p467(void)
{
value[0]=(value[91]>1);
if(value[0]){
value[0]=(KEY(553));
if(value[0]){
say(64,868,0);
 if (loop) return(0);}value[0]=(
KEY(558));
if(value[0]){
say(64,1509,0);
 if (loop) return(0);}
say(64,1508,0);
 if (loop) return(0);}
say(76,819,91);
 if (loop) return(0);return 0;
}
int p541(void)
{
set('E',117,'c',0,NULL,NULL);
value[0]=(KEY(556));
value[0]&=!(value[HERE]==393);
value[0]&=!(isnear(121,-1,-1));
if(value[0]){
p603();
if (loop) return (0);
}
say(0,1517,0);
apport(117,HERE);
value[0]=(value[HERE]==393);
if(value[0]){
p649();
if (loop) return (0);
}value[0]=(
isnear(121,-1,-1));
if(value[0]){
apport(117,485);
apport(121,485);
say(64,1157,0);
 if (loop) return(0);}
return 0;
}
int p342(void)
{
value[0]=(isnear(8,-1,-1));
if(value[0]){
say(64,1545,0);
 if (loop) return(0);}value[0]=(
isnear(30,-1,-1));
if(value[0]){
say(64,1556,0);
 if (loop) return(0);}value[0]=(
isnear(21,-1,-1));
if(value[0]){
value[0]=(value[21]==0);
if(value[0]){
say(64,1546,0);
 if (loop) return(0);}
say(64,1547,0);
 if (loop) return(0);}
return 0;
}
int p312(void)
{
if (*bitword(702) == -1) *bitword(701)= -1;value[701]=value[702]-1; while(++value[701]<=value[703]) {
say(2,701,0);
value[0]=(value[701]<value[703]);
if(value[0]){
value[0]=(query(945)); if (loop>1) return (0);
if(!value[0]){
say(2,720,0);
return 0;
}}}
return 0;
}
int p623(void)
{
value[679]=1598;*bitword(679)= -1;
value[679] += value[11];
say(2,679,0);
value[11] += 1;
value[0]=(value[11]>2);
if(value[0]){
set('E',11,'c',0,NULL,NULL);
bitmod('s',12,10);
}else{
bitmod('c',12,10);
}
say(0,777,0);
say(0,11,0);
set('E',12,'e',11,NULL,NULL);
return 0;
}
int p169(void)
{
value[0]=(KEY(120));
value[0]|=(KEY(125));
if(value[0]){
value[0]=(KEY(570));
value[0]|=(KEY(561));
if(value[0]){
say(64,1404,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p172(void)
{
value[0]=(KEY(526));
if(value[0]){
say(64,1558,0);
 if (loop) return(0);}value[0]=(
KEY(637));
if(!value[0]){
return 0;
}value[0]=(
KEY(570));
if(value[0]){
say(64,1643,0);
 if (loop) return(0);}value[0]=(
KEY(555));
value[0]|=(KEY(563));
value[0]|=(KEY(535));
value[0]|=(KEY(537));
if(value[0]){
say(76,1644,671);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p170(void)
{
value[0]=(KEY(209));
if(value[0]){
value[0]=(KEY(570));
if(value[0]){
say(64,1647,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p171(void)
{
value[0]=(KEY(5));
if(value[0]){
say(76,1553,5);
 if (loop) return(0);}
return 0;
}
int p8(void)
{
value[0]=(value[718]==5);
if(value[0]){
value[0]=(bitest(evar(718),0));
if(value[0]){
say(0,1669,0);
}else{
bitmod('s',evar(718),0);
say(0,1668,0);
}
move(value[681],-2); if (loop) return(0);
}
move(value[680],-2); if (loop) return(0);
return 0;
}
int p598(void)
{
value[0]=(getloc(82)==379);
if(value[0]){
value[0]=(bitest(82,14));
if(value[0]){
value[0]=(bitest(82,13));
if(value[0]){
bitmod('c',82,13);
say(64,1589,0);
 if (loop) return(0);}else{
apport(82,485);
say(64,1591,0);
 if (loop) return(0);}
}else{
say(0,1589,0);
value[0]=(bitest(82,13));
if(value[0]){
bitmod('s',82,14);
say(64,1590,0);
 if (loop) return(0);}
bitmod('s',82,13);
loop=1; return(0);
}}
say(64,1589,0);
 if (loop) return(0);return 0;
}
int p429(void)
{
if (!isnear(value[672],-1,-1)) return 0;
value[0]=(have(value[672],-1,-1));
if(value[0]){
say(76,926,672);
 if (loop) return(0);}value[0]=(
have(51,-1,-1));
if(value[0]){
value[0]=(KEY(106));
if(value[0]){
apport(106,485);
apport(107,value[673]);
apport(59,234);
say(64,928,0);
 if (loop) return(0);}
say(64,929,0);
 if (loop) return(0);}
say(76,927,672);
 if (loop) return(0);return 0;
}
int p82(void)
{
value[0]=(KEY(533));
value[0]|=(KEY(534));
if(value[0]){
return 0;
}
value[702]=3;*bitword(702)= -1;
value[0]=(value[702]==value[671]);
if(value[0]){
say(64,1492,0);
 if (loop) return(0);}value[0]=(
KEY(598));
if(value[0]){
say(64,1684,0);
 if (loop) return(0);}
say(12,1490,671);
say(76,1491,672);
 if (loop) return(0);return 0;
}
int p124(void)
{
value[0]=(have(114,0,4));
value[0]|=(have(111,0,4));
if(value[0]){
value[0]=(have(111,0,4));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}else{
set('E',114,'c',2,NULL,NULL);
}
}else{
say(76,1026,113);
 if (loop) return(0);}
return 0;
}
int p503(void)
{
value[0]=(isnear(66,-1,-1));
if(!value[0]){
say(76,819,672);
 if (loop) return(0);}
set('V',701,'v',672,NULL,NULL);
value[704]=638;*bitword(704)= -1;
value[701] -= value[704];
value[0]=(value[66]==0);
value[0]&=(value[701]==1);
value[0]|=(value[66]==value[701]);
if(value[0]){
value[704]=66;*bitword(704)= -1;
special(10,&value[704]);
}else{
say(76,819,672);
 if (loop) return(0);}
if (!anyof(639,640,641,642,643,644,645,646,
   647,648,649,650,651,652,653,-1)) return 0;
say(64,777,0);
 if (loop) return(0);return 0;
}
int p418(void)
{
value[0]=(660<=value[671] && value[671]<=666);
if(value[0]){
value[701]=660;*bitword(701)= -1;
value[701] += 1;
value[701] += value[727];
if (*bitword(727*VARSIZE)==-1) *bitword(701*VARSIZE)= -1;
value[0]=(value[701]==value[671]);
if(value[0]){
value[701] += 1;
value[0]=(value[701]==666);
if(value[0]){
set('V',727,'c',0,NULL,NULL);
bitmod('s',evar(712),4);
say(64,1523,0);
 if (loop) return(0);}
value[727] += 1;
}else{
set('V',727,'c',0,NULL,NULL);
}
set('V',671,'c',-2,NULL,NULL);
}else{
value[0]=(value[727]>0);
if(value[0]){
set('V',727,'c',0,NULL,NULL);
}}
return 0;
}
int p482(void)
{
value[0]=(KEY(570));
if(value[0]){
value[0]=(KEY(141));
if(value[0]){
say(64,1646,0);
 if (loop) return(0);}
return 0;
}
move(137,0,136,520,-523); if (loop) return(0);
move(141,0,525,141,-516); if (loop) return(0);
move(142,0,597,622,518,-524); if (loop) return(0);
move(440,0,514,-618); if (loop) return(0);
move(458,0,-515); if (loop) return(0);
move(460,0,-517); if (loop) return(0);
move(450,0,-519); if (loop) return(0);
move(457,0,521,-457); if (loop) return(0);
move(138,0,-138); if (loop) return(0);
value[0]=(KEY(144));
value[0]|=(KEY(615));
if(value[0]){
value[681]=461;*bitword(681)= -1;
value[0]=(KEY(144));
if(value[0]){
value[680]=144;*bitword(680)= -1;
}else{
value[680]=140;*bitword(680)= -1;
}
p8();
if (loop) return (0);
}value[0]=(
KEY(6));
if(value[0]){
say(72,1465,525);
 if (loop) return(0);}
return 0;
}
int p255(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(136,0,136,141,-516); if (loop) return(0);
move(457,0,-514); if (loop) return(0);
move(440,0,515,-618); if (loop) return(0);
move(142,0,-517); if (loop) return(0);
move(450,0,-518); if (loop) return(0);
move(461,0,-519); if (loop) return(0);
move(438,0,-521); if (loop) return(0);
if (!anyof(520,528,595,-1)) return 0;
value[0]=(query(1270)); if (loop>1) return (0);
if(value[0]){
say(0,1271,0);
finita();
loop=1; return(0);
}
say(64,1259,0);
 if (loop) return(0);return 0;
}
int p256(void)
{
p169();
if (loop) return (0);
if (!keyword(537,93,-1)) return 0;
say(76,1135,672);
 if (loop) return(0);return 0;
}
int p49(void)
{
value[0]=(KEY(6));
if(value[0]){
say(72,1465,526);
 if (loop) return(0);}value[0]=(
KEY(635));
if(value[0]){
say(64,1593,0);
 if (loop) return(0);}value[0]=(
KEY(634));
if(value[0]){
say(64,1592,0);
 if (loop) return(0);}
if (!keyword(570,141,-1)) return 0;
say(64,1646,0);
 if (loop) return(0);return 0;
}
int p50(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(136,0,136,526,-520); if (loop) return(0);
value[0]=(value[718]<2);
if(value[0]){
move(147,value[687],-497); if (loop) return(0);
move(163,value[687],-501); if (loop) return(0);
}
if (!anyof(622,597,-1)) return 0;
say(64,1025,0);
 if (loop) return(0);return 0;
}
int p606(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(136,0,596,514,515,-521); if (loop) return(0);
move(460,0,-516); if (loop) return(0);
move(454,0,-517); if (loop) return(0);
move(451,0,-519); if (loop) return(0);
move(450,0,520,-618); if (loop) return(0);
value[0]=(KEY(523));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(450,-2); if (loop) return(0);
}
move(460,-2); if (loop) return(0);
}value[0]=(
KEY(141));
if(value[0]){
value[681]=143;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}value[0]=(
KEY(144));
if(value[0]){
value[681]=440;*bitword(681)= -1;
value[680]=144;*bitword(680)= -1;
p8();
if (loop) return (0);
}
if (!keyword(527,622,-1)) return 0;
value[680]=136;*bitword(680)= -1;
value[681]=143;*bitword(681)= -1;
p197();
if (loop) return (0);
return 0;
}
int p607(void)
{
if (!anyof(518,524,597,-1)) return 0;
move(143,-2); if (loop) return(0);
return 0;
}
int p524(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}value[0]=(
value[143]>0);
if(value[0]){
value[0]=(KEY(141));
if(value[0]){
value[681]=436;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}value[0]=(
KEY(144));
if(value[0]){
value[681]=142;*bitword(681)= -1;
value[680]=144;*bitword(680)= -1;
p8();
if (loop) return (0);
}
}else{
move(136,0,-141); if (loop) return(0);
move(144,0,-144); if (loop) return(0);
}
move(142,0,596,-514); if (loop) return(0);
move(144,0,597,636,518,-144); if (loop) return(0);
move(460,0,-515); if (loop) return(0);
move(454,0,-516); if (loop) return(0);
move(455,0,-517); if (loop) return(0);
move(452,0,-519); if (loop) return(0);
move(451,0,520,-618); if (loop) return(0);
move(450,0,-521); if (loop) return(0);
value[0]=(KEY(523));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(451,-2); if (loop) return(0);
}
move(454,-2); if (loop) return(0);
}
if (!anyof(143,622,524,525,-1)) return 0;
value[0]=(value[143]==0);
if(value[0]){
say(64,902,0);
 if (loop) return(0);}
set('V',718,'c',6,NULL,NULL);
p237();
if (loop) return (0);
say(0,1397,0);
value[0]=(query(1398)); if (loop>1) return (0);
if(value[0]){
say(0,1399,0);
value[0]=(query(1398)); if (loop>1) return (0);
if(value[0]){
value[0]=(value[710]>658);
if(value[0]){
say(0,1401,0);
}else{
say(0,1400,0);
}}}
set('V',719,'c',0,NULL,NULL);
p190();
if (loop) return (0);
return 0;
}
int p119(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(454,0,-515); if (loop) return(0);
move(455,0,-516); if (loop) return(0);
move(456,0,-517); if (loop) return(0);
move(436,0,-518); if (loop) return(0);
move(453,0,-519); if (loop) return(0);
move(452,0,520,-618); if (loop) return(0);
move(451,0,-521); if (loop) return(0);
value[0]=(KEY(141));
if(value[0]){
value[681]=436;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}value[0]=(
KEY(523));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(455,-2); if (loop) return(0);
}
move(452,-2); if (loop) return(0);
}
move(143,0,596,514,-143); if (loop) return(0);
if (!anyof(525,524,525,-1)) return 0;
value[0]=(value[4]==1);
if(value[0]){
move(145,-2); if (loop) return(0);
}
say(64,900,0);
 if (loop) return(0);return 0;
}
int p301(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(146,0,623,146,525,-520); if (loop) return(0);
move(150,0,-150); if (loop) return(0);
move(147,0,-147); if (loop) return(0);
if (!anyof(523,526,526,616,-1)) return 0;
value[0]=(value[4]==1);
if(value[0]){
move(144,-2); if (loop) return(0);
}
say(64,900,0);
 if (loop) return(0);return 0;
}
int p91(void)
{
move(145,0,526,616,619,-516); if (loop) return(0);
move(147,0,525,229,520,-147); if (loop) return(0);
move(150,0,-150); if (loop) return(0);
return 0;
}
int p116(void)
{
move(145,0,-619); if (loop) return(0);
move(146,0,623,146,621,213,-516); if (loop) return(0);
move(148,0,148,525,523,-520); if (loop) return(0);
move(150,0,-150); if (loop) return(0);
value[0]=(value[4]==1);
if(value[0]){
move(144,0,-144); if (loop) return(0);
}else{
move(145,900,-144); if (loop) return(0);
}value[0]=(
KEY(497));
if(value[0]){
value[0]=(value[718]>1);
value[0]|=(bitest(evar(712),2));
if(value[0]){
bitmod('s',evar(712),3);
say(64,935,0);
 if (loop) return(0);}
move(141,-value[687]); if (loop) return(0);
}
if (!keyword(125,-1)) return 0;
value[0]=(KEY(570));
value[0]|=(KEY(561));
if(value[0]){
say(64,1621,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p52(void)
{
move(145,0,-619); if (loop) return(0);
move(147,0,524,516,-147); if (loop) return(0);
move(149,0,525,523,-520); if (loop) return(0);
move(150,0,-150); if (loop) return(0);
value[0]=(value[4]==1);
if(value[0]){
move(144,0,-144); if (loop) return(0);
}else{
move(145,900,-144); if (loop) return(0);
}
return 0;
}
int p31(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(145,0,-619); if (loop) return(0);
move(147,0,-147); if (loop) return(0);
move(148,0,148,-516); if (loop) return(0);
move(150,0,621,150,-520); if (loop) return(0);
value[0]=(value[4]==1);
if(value[0]){
move(144,0,-144); if (loop) return(0);
}else{
move(145,900,-144); if (loop) return(0);
}
return 0;
}
int p449(void)
{
value[0]=(value[154]==0);
if(value[0]){
value[0]=(have(44,-1,-1));
if(value[0]){
apport(5,484);
}else{
apport(5,150);
}}value[0]=(
KEY(570));
if(value[0]){
return 0;
}
move(145,0,-619); if (loop) return(0);
move(147,0,-147); if (loop) return(0);
move(149,0,621,-516); if (loop) return(0);
move(151,0,-524); if (loop) return(0);
value[0]=(value[4]==1);
if(value[0]){
move(144,0,-144); if (loop) return(0);
}else{
move(145,900,-144); if (loop) return(0);
}
if (!anyof(524,150,5,-1)) return 0;
value[0]=(ishere(5,-1,-1));
if(value[0]){
move(151,-2); if (loop) return(0);
}
say(0,1013,0);
move(151,-1);
p93();
if (loop) return (0);
return 0;
}
int p450(void)
{
if (!anyof(520,612,-1)) return 0;
value[0]=(KEY(570));
if(value[0]){
say(64,1024,0);
 if (loop) return(0);}
say(64,1023,0);
 if (loop) return(0);return 0;
}
int p451(void)
{
if (!keyword(530,-1)) return 0;
move(151,-1);
p543();
if (loop) return (0);
return 0;
}
int p392(void)
{
value[0]=(value[154]==0);
if(value[0]){
value[0]=(have(44,-1,-1));
if(value[0]){
apport(5,484);
set('E',151,'c',1,NULL,NULL);
}else{
apport(5,150);
set('E',151,'c',0,NULL,NULL);
}}value[0]=(
KEY(570));
if(value[0]){
return 0;
}
move(154,0,-518); if (loop) return(0);
move(312,0,-516); if (loop) return(0);
move(152,0,528,624,-520); if (loop) return(0);
move(155,0,524,-514); if (loop) return(0);
move(164,0,-163); if (loop) return(0);
if (!anyof(595,523,150,614,621,5,-1)) return 0;
value[0]=(ishere(5,-1,-1));
if(value[0]){
move(150,-2); if (loop) return(0);
}
say(64,1014,0);
 if (loop) return(0);return 0;
}
int p149(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(151,0,516,-624); if (loop) return(0);
if (!anyof(528,531,520,-1)) return 0;
value[0]=(value[8]==0);
if(value[0]){
say(64,904,0);
 if (loop) return(0);}
p457();
if (loop) return (0);
move(153,-2); if (loop) return(0);
return 0;
}
int p150(void)
{
value[0]=(KEY(530));
if(value[0]){
value[0]=(value[8]==1);
if(value[0]){
say(64,903,0);
 if (loop) return(0);}
move(224,-1);
p543();
if (loop) return (0);
}value[0]=(
KEY(9));
value[0]&=(value[8]==1);
if(value[0]){
apport(9,value[673]);
}
return 0;
}
int p416(void)
{
move(151,0,514,526,-624); if (loop) return(0);
if (!keyword(125,-1)) return 0;
value[0]=(KEY(570));
value[0]|=(KEY(561));
if(value[0]){
say(76,1622,154);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p396(void)
{
set('V',678,'c',0,NULL,NULL);
value[0]=(KEY(570));
if(value[0]){
return 0;
}
if (!anyof(5,523,516,-1)) return 0;
value[678]=151;*bitword(678)= -1;
return 0;
}
int p397(void)
{
value[0]=(KEY(514));
if(value[0]){
value[678]=160;*bitword(678)= -1;
}else{
value[0]=(KEY(519));
value[0]|=(KEY(620));
if(value[0]){
value[678]=215;*bitword(678)= -1;
}else{
value[0]=(KEY(518));
if(value[0]){
value[678]=161;*bitword(678)= -1;
}else{
value[0]=(KEY(520));
if(value[0]){
value[678]=162;*bitword(678)= -1;
}else{
value[0]=(KEY(524));
if(value[0]){
value[678]=314;*bitword(678)= -1;
}else{
value[0]=(KEY(515));
if(value[0]){
value[678]=313;*bitword(678)= -1;
}else{
value[0]=(KEY(521));
if(value[0]){
value[678]=319;*bitword(678)= -1;
}else{
value[0]=(KEY(517));
if(value[0]){
value[678]=316;*bitword(678)= -1;
}}}}}}}}value[0]=(
value[678]==0);
if(value[0]){
return 0;
}value[0]=(
isnear(7,-1,-1));
value[0]&=!(value[678]==value[683]);
if(value[0]){
p527();
if (loop) return (0);
value[0]=(value[7]==1);
if(value[0]){
say(0,1433,0);
set('E',7,'c',0,NULL,NULL);
}else{
say(64,1015,0);
 if (loop) return(0);}}value[0]=(
KEY(519));
value[0]&=(irand(100)<66);
if(value[0]){
set('V',683,'v',678,NULL,NULL);
say(64,844,0);
 if (loop) return(0);}
move(value[678],-2); if (loop) return(0);
return 0;
}
int p640(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(157,0,516,-531); if (loop) return(0);
move(209,0,520,-209); if (loop) return(0);
move(159,0,524,-150); if (loop) return(0);
if (!keyword(617,-1)) return 0;
say(64,951,0);
 if (loop) return(0);return 0;
}
int p151(void)
{
move(157,0,523,-526); if (loop) return(0);
return 0;
}
int p639(void)
{
move(156,0,523,-526); if (loop) return(0);
value[0]=(KEY(595));
if(value[0]){
value[0]=(value[11]==0);
if(value[0]){
say(64,1027,0);
 if (loop) return(0);}value[0]=(
value[11]==1);
if(value[0]){
move(156,-1028); if (loop) return(0);
}value[0]=(
bitest(220,4));
if(!value[0]){
set('V',734,'c',2,NULL,NULL);
}
move(220,-1029); if (loop) return(0);
}
if (!keyword(533,11,-1)) return 0;
say(64,921,0);
 if (loop) return(0);return 0;
}
int p638(void)
{
move(167,0,-520); if (loop) return(0);
value[0]=(KEY(530));
if(value[0]){
value[0]=(value[8]==1);
if(value[0]){
say(64,903,0);
 if (loop) return(0);}
move(224,-1);
p543();
if (loop) return (0);
}value[0]=(
KEY(514));
if(value[0]){
move(167,-1016); if (loop) return(0);
}value[0]=(
KEY(9));
value[0]&=(value[8]==1);
if(value[0]){
apport(9,value[673]);
}
if (!anyof(528,531,516,-1)) return 0;
value[0]=(value[8]==0);
if(value[0]){
say(64,904,0);
 if (loop) return(0);}
p457();
if (loop) return (0);
move(152,-2); if (loop) return(0);
return 0;
}
int p347(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(155,0,518,526,-624); if (loop) return(0);
move(163,0,514,-163); if (loop) return(0);
move(198,0,524,-617); if (loop) return(0);
move(198,0,-617); if (loop) return(0);
return 0;
}
int p537(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(155,0,514,526,-624); if (loop) return(0);
move(395,0,524,-5); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p641(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(155,0,516,526,-624); if (loop) return(0);
move(196,0,520,-523); if (loop) return(0);
return 0;
}
int p651(void)
{
move(160,0,-518); if (loop) return(0);
move(164,0,516,-164); if (loop) return(0);
move(165,0,-520); if (loop) return(0);
if (!anyof(501,228,-1)) return 0;
value[0]=(value[699]==2);
value[0]&=!(KEY(493));
if(value[0]){
return 0;
}value[0]=(
KEY(228));
if(value[0]){
bitmod('c',228,8);
bitmod('c',227,8);
bitmod('c',229,8);
move(228,-2); if (loop) return(0);
}value[0]=(
value[718]>1);
value[0]|=(bitest(evar(712),2));
if(value[0]){
bitmod('s',evar(712),3);
say(64,935,0);
 if (loop) return(0);}
move(141,-value[687]); if (loop) return(0);
return 0;
}
int p310(void)
{
move(163,0,524,-163); if (loop) return(0);
move(151,0,-523); if (loop) return(0);
return 0;
}
int p153(void)
{
move(163,0,516,-163); if (loop) return(0);
p172();
if (loop) return (0);
if (!keyword(530,-1)) return 0;
say(0,1013,0);
move(237,-1);
p93();
if (loop) return (0);
return 0;
}
int p123(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(199,0,516,-623); if (loop) return(0);
move(160,0,523,-617); if (loop) return(0);
move(201,0,-520); if (loop) return(0);
move(207,0,-207); if (loop) return(0);
return 0;
}
int p47(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(198,0,520,-623); if (loop) return(0);
move(200,0,524,595,-150); if (loop) return(0);
return 0;
}
int p554(void)
{
move(199,0,523,526,-595); if (loop) return(0);
if (!anyof(143,622,524,596,597,-1)) return 0;
say(64,902,0);
 if (loop) return(0);return 0;
}
int p144(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(198,0,516,-621); if (loop) return(0);
move(202,0,524,-617); if (loop) return(0);
move(207,0,-207); if (loop) return(0);
return 0;
}
int p637(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(168,0,518,523,621,-595); if (loop) return(0);
move(153,0,-516); if (loop) return(0);
move(194,0,520,-623); if (loop) return(0);
if (!keyword(514,-1)) return 0;
move(153,-1016); if (loop) return(0);
return 0;
}
int p352(void)
{
move(167,0,-523); if (loop) return(0);
move(168,0,-514); if (loop) return(0);
move(169,0,-516); if (loop) return(0);
move(171,0,-518); if (loop) return(0);
move(178,0,-520); if (loop) return(0);
return 0;
}
int p353(void)
{
move(168,0,-520); if (loop) return(0);
move(170,0,-518); if (loop) return(0);
move(171,0,-516); if (loop) return(0);
return 0;
}
int p354(void)
{
move(169,0,-516); if (loop) return(0);
move(189,0,-524); if (loop) return(0);
move(173,0,-518); if (loop) return(0);
move(191,0,-514); if (loop) return(0);
return 0;
}
int p355(void)
{
move(168,0,-520); if (loop) return(0);
move(169,0,-514); if (loop) return(0);
move(182,0,-516); if (loop) return(0);
move(185,0,-518); if (loop) return(0);
move(181,0,523,-524); if (loop) return(0);
move(181,0,-524); if (loop) return(0);
return 0;
}
int p356(void)
{
move(171,0,526,-520); if (loop) return(0);
return 0;
}
int p357(void)
{
move(171,0,526,-516); if (loop) return(0);
return 0;
}
int p358(void)
{
move(170,0,526,-523); if (loop) return(0);
return 0;
}
int p359(void)
{
move(173,0,-516); if (loop) return(0);
move(174,0,-520); if (loop) return(0);
return 0;
}
int p360(void)
{
move(170,0,-516); if (loop) return(0);
move(172,0,-520); if (loop) return(0);
move(174,0,-524); if (loop) return(0);
move(175,0,-518); if (loop) return(0);
return 0;
}
int p361(void)
{
move(172,0,-520); if (loop) return(0);
move(173,0,-523); if (loop) return(0);
move(175,0,-516); if (loop) return(0);
move(176,0,-518); if (loop) return(0);
return 0;
}
int p362(void)
{
move(173,0,-520); if (loop) return(0);
move(174,0,-516); if (loop) return(0);
move(175,0,-518); if (loop) return(0);
move(176,0,-523); if (loop) return(0);
move(177,0,-514); if (loop) return(0);
move(188,0,-524); if (loop) return(0);
return 0;
}
int p363(void)
{
move(174,0,-520); if (loop) return(0);
move(175,0,-514); if (loop) return(0);
move(183,0,-518); if (loop) return(0);
return 0;
}
int p364(void)
{
move(176,0,526,-520); if (loop) return(0);
return 0;
}
int p365(void)
{
move(175,0,-520); if (loop) return(0);
move(177,0,-514); if (loop) return(0);
move(190,0,-524); if (loop) return(0);
move(193,0,-516); if (loop) return(0);
return 0;
}
int p366(void)
{
move(177,0,526,-523); if (loop) return(0);
return 0;
}
int p376(void)
{
move(149,0,524,-595); if (loop) return(0);
move(177,0,-520); if (loop) return(0);
move(186,0,-518); if (loop) return(0);
move(179,0,-514); if (loop) return(0);
move(180,0,-516); if (loop) return(0);
return 0;
}
int p367(void)
{
move(193,0,526,-516); if (loop) return(0);
return 0;
}
int p330(void)
{
move(167,0,516,523,-623); if (loop) return(0);
move(195,0,-520); if (loop) return(0);
move(196,0,514,524,-617); if (loop) return(0);
return 0;
}
int p331(void)
{
move(194,0,-516); if (loop) return(0);
move(196,0,-514); if (loop) return(0);
move(300,0,-518); if (loop) return(0);
return 0;
}
int p99(void)
{
move(194,0,-520); if (loop) return(0);
move(197,0,-514); if (loop) return(0);
move(162,0,-516); if (loop) return(0);
move(195,0,-518); if (loop) return(0);
return 0;
}
int p113(void)
{
move(196,0,526,-518); if (loop) return(0);
return 0;
}
int p92(void)
{
move(201,0,523,-595); if (loop) return(0);
move(207,0,520,-207); if (loop) return(0);
move(232,0,514,-232); if (loop) return(0);
move(235,0,-516); if (loop) return(0);
return 0;
}
int p30(void)
{
move(202,0,-516); if (loop) return(0);
move(208,0,-520); if (loop) return(0);
move(209,0,-209); if (loop) return(0);
value[0]=(KEY(521));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
say(64,844,0);
 if (loop) return(0);}
move(226,-2); if (loop) return(0);
}
if (!anyof(514,518,523,524,-1)) return 0;
value[0]=(irand(100)<65);
if(value[0]){
say(64,930,0);
 if (loop) return(0);}
move(235,0,-524); if (loop) return(0);
value[0]=(irand(100)<75);
if(value[0]){
move(213,0,-514); if (loop) return(0);
move(201,0,-523); if (loop) return(0);
move(209,0,-518); if (loop) return(0);
}else{
move(212,0,-514); if (loop) return(0);
move(211,0,-523); if (loop) return(0);
move(218,0,-518); if (loop) return(0);
}
return 0;
}
int p566(void)
{
move(207,0,-515); if (loop) return(0);
move(157,0,-520); if (loop) return(0);
move(218,0,-148); if (loop) return(0);
move(225,0,516,-225); if (loop) return(0);
move(226,0,-226); if (loop) return(0);
if (!anyof(521,518,-1)) return 0;
value[0]=(irand(100)<33);
if(value[0]){
move(218,0,-518); if (loop) return(0);
move(226,0,-521); if (loop) return(0);
}else{
say(64,844,0);
 if (loop) return(0);}
return 0;
}
int p152(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(208,0,-516); if (loop) return(0);
move(156,0,520,-531); if (loop) return(0);
move(158,0,524,-150); if (loop) return(0);
return 0;
}
int p521(void)
{
move(156,0,-518); if (loop) return(0);
move(210,0,523,-595); if (loop) return(0);
move(207,0,514,-207); if (loop) return(0);
move(207,0,-207); if (loop) return(0);
p170();
if (loop) return (0);
return 0;
}
int p510(void)
{
move(209,0,524,-209); if (loop) return(0);
move(237,0,514,-10); if (loop) return(0);
move(239,0,-239); if (loop) return(0);
move(247,0,-518); if (loop) return(0);
return 0;
}
int p511(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(212,0,-514); if (loop) return(0);
move(207,0,524,-621); if (loop) return(0);
move(238,0,-518); if (loop) return(0);
return 0;
}
int p507(void)
{
move(207,0,-517); if (loop) return(0);
move(211,0,-518); if (loop) return(0);
move(166,0,-514); if (loop) return(0);
return 0;
}
int p346(void)
{
move(207,0,-207); if (loop) return(0);
move(246,0,-519); if (loop) return(0);
move(214,0,-514); if (loop) return(0);
move(226,0,517,-226); if (loop) return(0);
return 0;
}
int p114(void)
{
move(213,0,518,526,-623); if (loop) return(0);
return 0;
}
int p506(void)
{
move(155,0,-516); if (loop) return(0);
move(216,0,-524); if (loop) return(0);
if (!keyword(520,-1)) return 0;
value[0]=(value[20]==0);
if(value[0]){
move(248,-2); if (loop) return(0);
}
move(247,-2); if (loop) return(0);
return 0;
}
int p644(void)
{
move(217,0,-518); if (loop) return(0);
move(218,0,-514); if (loop) return(0);
return 0;
}
int p583(void)
{
move(216,0,-514); if (loop) return(0);
return 0;
}
int p568(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(216,0,-516); if (loop) return(0);
move(219,0,-520); if (loop) return(0);
move(208,0,514,-623); if (loop) return(0);
move(483,0,518,-525); if (loop) return(0);
return 0;
}
int p608(void)
{
value[0]=(KEY(125));
if(value[0]){
value[0]=(KEY(570));
value[0]|=(KEY(561));
if(value[0]){
say(64,1623,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
move(218,0,514,-526); if (loop) return(0);
value[0]=(KEY(525));
if(value[0]){
say(64,1582,0);
 if (loop) return(0);}value[0]=(
KEY(534));
value[0]&=(value[699]==2);
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(!value[0]){
say(76,1026,672);
 if (loop) return(0);}value[0]=(
KEY(80));
if(value[0]){
say(0,1572,0);
}else{
value[0]=(KEY(100));
value[0]&=(have(101,-1,-1));
if(value[0]){
say(0,1573,0);
}else{
value[0]=(KEY(101));
if(value[0]){
apport(101,HERE);
set('E',101,'c',0,NULL,NULL);
say(64,1574,0);
 if (loop) return(0);}value[0]=(
KEY(95));
if(value[0]){
value[0]=(value[95]==1);
if(value[0]){
say(0,1576,0);
}else{
say(0,1577,0);
}
}else{
value[0]=(KEY(53));
if(value[0]){
say(0,1575,0);
}else{
say(12,1571,672);
}}}}value[0]=(
bitest(evar(712),6));
if(value[0]){
say(0,1579,0);
}else{
say(0,1578,0);
bitmod('s',evar(712),6);
}value[0]=(
KEY(100));
value[0]&=(have(101,-1,-1));
if(value[0]){
say(0,1581,0);
apport(101,149);
set('E',101,'c',0,NULL,NULL);
}
say(0,1580,0);
value[0]=(KEY(80));
if(value[0]){
bitmod('s',80,5);
}value[0]=(
KEY(97));
value[0]|=(KEY(108));
if(value[0]){
say(0,1568,0);
}else{
value[0]=(KEY(95));
if(value[0]){
say(0,1570,0);
}else{
value[0]=(KEY(53));
if(value[0]){
say(0,1569,0);
}else{
value[0]=(bitest(evar(672),5));
if(value[0]){
say(0,1566,0);
}else{
say(0,1567,0);
}}}}
apport(value[672],485);
value[0]=(KEY(111));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}value[0]=(
KEY(114));
if(value[0]){
set('E',114,'c',2,NULL,NULL);
}
loop=1; return(0);
}value[0]=(
KEY(633));
if(value[0]){
value[0]=(KEY(570));
if(value[0]){
say(64,1583,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p115(void)
{
move(218,0,-518); if (loop) return(0);
return 0;
}
int p368(void)
{
move(168,0,-514); if (loop) return(0);
move(178,0,520,-518); if (loop) return(0);
move(184,0,-516); if (loop) return(0);
return 0;
}
int p369(void)
{
move(178,0,520,-526); if (loop) return(0);
return 0;
}
int p370(void)
{
move(170,0,526,-518); if (loop) return(0);
return 0;
}
int p371(void)
{
move(193,0,-518); if (loop) return(0);
move(180,0,-516); if (loop) return(0);
move(187,0,-520); if (loop) return(0);
return 0;
}
int p372(void)
{
move(193,0,-514); if (loop) return(0);
move(179,0,-520); if (loop) return(0);
move(192,0,-521); if (loop) return(0);
return 0;
}
int p373(void)
{
move(179,0,526,-516); if (loop) return(0);
return 0;
}
int p374(void)
{
move(175,0,526,-523); if (loop) return(0);
return 0;
}
int p375(void)
{
move(171,0,523,-524); if (loop) return(0);
return 0;
}
int p400(void)
{
move(159,0,516,524,-595); if (loop) return(0);
move(222,0,-520); if (loop) return(0);
return 0;
}
int p401(void)
{
if (!keyword(530,-1)) return 0;
move(159,-1);
p543();
if (loop) return (0);
return 0;
}
int p303(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(224,0,514,224,-621); if (loop) return(0);
move(213,1118,524,-595); if (loop) return(0);
return 0;
}
int p242(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(220,0,518,-249); if (loop) return(0);
move(322,0,-516); if (loop) return(0);
move(223,0,-514); if (loop) return(0);
return 0;
}
int p299(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(222,0,518,-621); if (loop) return(0);
move(222,0,-621); if (loop) return(0);
if (!anyof(514,525,224,-1)) return 0;
value[0]=(value[6]==0);
if(value[0]){
say(64,917,0);
 if (loop) return(0);}
move(224,-2); if (loop) return(0);
return 0;
}
int p76(void)
{
move(223,0,518,-526); if (loop) return(0);
move(221,0,520,-221); if (loop) return(0);
if (!anyof(524,530,-1)) return 0;
value[0]=(query(1172)); if (loop>1) return (0);
if(value[0]){
bitmod('c',239,8);
set('V',703,'c',0,NULL,NULL);
value[0]=(have(101,-1,-1));
if(value[0]){
apport(101,485);
}
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
value[0]&=!(bitest(evar(677),12));
value[0]&=!(value[677]==99);
if(value[0]){
apport(value[677],485);
value[703] += 1;
}}value[0]=(
have(99,-1,-1));
if(value[0]){
set('V',707,'c',1,NULL,NULL);
set('V',703,'c',1,NULL,NULL);
}else{
set('V',707,'c',0,NULL,NULL);
}
say(12,1173,707);
value[0]=(value[703]>0);
if(value[0]){
say(12,1174,707);
}else{
say(0,777,0);
}
move(240,-1175); if (loop) return(0);
}
say(66,720,0);
 if (loop) return(0);return 0;
}
int p530(void)
{
move(429,0,-516); if (loop) return(0);
move(392,0,-517); if (loop) return(0);
move(208,0,520,526,-208); if (loop) return(0);
if (!anyof(514,518,519,521,515,-1)) return 0;
say(64,1254,0);
 if (loop) return(0);return 0;
}
int p531(void)
{
value[0]=(KEY(570));
if(value[0]){
value[0]=(KEY(657));
if(value[0]){
say(64,1603,0);
 if (loop) return(0);}value[0]=(
KEY(40));
if(value[0]){
say(76,1553,672);
 if (loop) return(0);}}
return 0;
}
int p430(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(208,0,-517); if (loop) return(0);
move(213,0,520,-623); if (loop) return(0);
move(213,0,-623); if (loop) return(0);
move(230,0,514,523,-224); if (loop) return(0);
return 0;
}
int p393(void)
{
move(226,0,518,-226); if (loop) return(0);
move(227,0,-520); if (loop) return(0);
if (!keyword(530,-1)) return 0;
move(224,-1);
p543();
if (loop) return (0);
return 0;
}
int p6(void)
{
move(230,0,521,-224); if (loop) return(0);
if (!anyof(516,621,228,-1)) return 0;
set('V',701,'v',707,NULL,NULL);
value[0]=(have(55,-1,-1));
if(value[0]){
value[701] -= 1;
}value[0]=(
have(58,-1,-1));
if(value[0]){
value[701] -= 1;
}value[0]=(
value[701]==0);
if(value[0]){
move(228,-2); if (loop) return(0);
}
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
value[0]&=!(bitest(evar(677),12));
if(value[0]){
say(64,923,0);
 if (loop) return(0);}}
move(228,-2); if (loop) return(0);
return 0;
}
int p458(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(229,0,515,229,-525); if (loop) return(0);
value[0]=(KEY(228));
if(value[0]){
value[0]=(have(55,-1,-1));
if(value[0]){
apport(55,HERE);
say(0,1588,0);
}
move(163,-2); if (loop) return(0);
}
if (!anyof(520,621,227,-1)) return 0;
set('V',701,'v',707,NULL,NULL);
value[0]=(have(55,-1,-1));
if(value[0]){
value[701] -= 1;
}value[0]=(
have(58,-1,-1));
if(value[0]){
value[701] -= 1;
}value[0]=(
value[701]==0);
if(value[0]){
move(227,-2); if (loop) return(0);
}
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
value[0]&=!(bitest(evar(677),12));
if(value[0]){
say(64,923,0);
 if (loop) return(0);}}
move(227,-2); if (loop) return(0);
return 0;
}
int p109(void)
{
move(228,0,518,526,-228); if (loop) return(0);
if (!keyword(124,-1)) return 0;
value[0]=(KEY(561));
value[0]|=(KEY(570));
if(value[0]){
say(64,1010,0);
 if (loop) return(0);}
say(76,1553,124);
 if (loop) return(0);return 0;
}
int p11(void)
{
move(232,0,524,-232); if (loop) return(0);
move(370,0,516,-523); if (loop) return(0);
return 0;
}
int p520(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(231,0,523,520,-624); if (loop) return(0);
move(233,0,524,-249); if (loop) return(0);
if (!anyof(518,202,-1)) return 0;
value[0]=(have(106,-1,-1));
if(value[0]){
say(76,924,106);
 if (loop) return(0);}value[0]=(
have(107,-1,-1));
if(value[0]){
say(76,924,107);
 if (loop) return(0);}
move(202,-2); if (loop) return(0);
return 0;
}
int p466(void)
{
move(232,0,523,-232); if (loop) return(0);
move(234,0,-524); if (loop) return(0);
return 0;
}
int p102(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(233,0,523,526,-249); if (loop) return(0);
move(232,0,-232); if (loop) return(0);
return 0;
}
int p9(void)
{
move(413,0,-515); if (loop) return(0);
move(203,0,-518); if (loop) return(0);
move(202,0,-523); if (loop) return(0);
move(207,0,-520); if (loop) return(0);
value[0]=(KEY(516));
if(value[0]){
bitmod('c',evar(736),2);
move(236,-2); if (loop) return(0);
}
p169();
if (loop) return (0);
return 0;
}
int p377(void)
{
move(302,0,-518); if (loop) return(0);
move(303,0,-519); if (loop) return(0);
move(304,0,-515); if (loop) return(0);
move(305,0,-517); if (loop) return(0);
move(306,0,-523); if (loop) return(0);
move(307,0,-521); if (loop) return(0);
move(308,0,-516); if (loop) return(0);
move(309,0,-520); if (loop) return(0);
move(310,0,-514); if (loop) return(0);
move(195,0,-524); if (loop) return(0);
return 0;
}
int p647(void)
{
value[0]=(KEY(520));
if(value[0]){
bitmod('c',evar(736),2);
move(236,-2); if (loop) return(0);
}
if (!anyof(514,518,516,523,524,515,521,517,519,-1)) return 0;
value[0]=(bitest(evar(736),2));
if(value[0]){
set('V',702,'c',50,NULL,NULL);
}else{
set('V',702,'c',95,NULL,NULL);
}value[0]=(
irand(100)<value[702]);
if(value[0]){
move(236,-2); if (loop) return(0);
}
move(235,-2); if (loop) return(0);
return 0;
}
int p390(void)
{
move(210,0,-518); if (loop) return(0);
move(239,0,514,-239); if (loop) return(0);
if (!anyof(10,637,-1)) return 0;
say(64,951,0);
 if (loop) return(0);return 0;
}
int p642(void)
{
move(212,0,-520); if (loop) return(0);
p172();
if (loop) return (0);
if (!keyword(530,-1)) return 0;
say(0,1013,0);
move(237,-1);
p93();
if (loop) return (0);
return 0;
}
int p586(void)
{
move(211,0,-514); if (loop) return(0);
value[0]=(KEY(524));
value[0]&=(irand(100)<67);
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(176,-2); if (loop) return(0);
}
move(171,-2); if (loop) return(0);
}
if (!anyof(530,595,524,-1)) return 0;
move(173,-2); if (loop) return(0);
return 0;
}
int p378(void)
{
move(302,0,-519); if (loop) return(0);
move(303,0,-514); if (loop) return(0);
move(304,0,-516); if (loop) return(0);
move(305,0,-521); if (loop) return(0);
move(306,0,-517); if (loop) return(0);
move(307,0,-515); if (loop) return(0);
move(308,0,-520); if (loop) return(0);
move(309,0,-524); if (loop) return(0);
move(310,0,-523); if (loop) return(0);
move(311,0,-518); if (loop) return(0);
return 0;
}
int p477(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(237,0,518,-10); if (loop) return(0);
move(397,0,-520); if (loop) return(0);
if (!anyof(514,531,-1)) return 0;
say(64,1168,0);
 if (loop) return(0);return 0;
}
int p478(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(241,0,514,241,-621); if (loop) return(0);
move(242,0,-242); if (loop) return(0);
if (!anyof(518,531,-1)) return 0;
value[0]=(isnear(28,-1,-1));
if(value[0]){
apport(28,484);
move(239,-1169); if (loop) return(0);
}
say(64,1168,0);
 if (loop) return(0);return 0;
}
int p616(void)
{
move(240,0,518,-239); if (loop) return(0);
move(242,0,515,-242); if (loop) return(0);
return 0;
}
int p19(void)
{
move(241,0,520,526,-241); if (loop) return(0);
move(240,0,-239); if (loop) return(0);
if (!keyword(530,-1)) return 0;
move(485,-1);
p543();
if (loop) return (0);
return 0;
}
int p351(void)
{
move(180,0,-517); if (loop) return(0);
return 0;
}
int p556(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(246,0,519,524,246,-249); if (loop) return(0);
value[0]=(KEY(556));
if(value[0]){
value[0]=(value[699]==1);
value[0]|=(isnear(23,-1,-1));
if(value[0]){
return 0;
}value[0]=(
KEY(115));
value[0]|=(KEY(116));
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(value[0]){
apport(value[672],HERE);
say(76,969,672);
 if (loop) return(0);}
return 0;
}value[0]=(
bitest(evar(672),0));
if(value[0]){
value[0]=(bitest(evar(672),5));
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(50));
if(value[0]){
value[0]=(bitest(22,13));
if(value[0]){
set('E',22,'c',0,NULL,NULL);
apport(50,485);
bitmod('s',50,14);
say(64,1456,0);
 if (loop) return(0);}
bitmod('s',50,13);
}
set('E',22,'c',1,NULL,NULL);
apport(22,484);
apport(23,244);
apport(value[672],485);
say(76,970,672);
 if (loop) return(0);}}}}value[0]=(
KEY(530));
if(value[0]){
value[0]=(value[21]==0);
if(value[0]){
say(64,903,0);
 if (loop) return(0);}
move(485,-1);
p543();
if (loop) return (0);
}value[0]=(
KEY(9));
if(value[0]){
apport(9,value[673]);
}value[0]=(
KEY(120));
if(value[0]){
value[0]=(KEY(561));
value[0]|=(KEY(570));
if(value[0]){
say(64,1404,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
if (!anyof(531,515,-1)) return 0;
value[0]=(value[21]>0);
if(value[0]){
say(64,972,0);
 if (loop) return(0);}value[0]=(
value[22]==0);
if(value[0]){
say(64,971,0);
 if (loop) return(0);}value[0]=(
value[22]==2);
if(value[0]){
set('E',22,'c',0,NULL,NULL);
apport(22,244);
apport(23,484);
say(64,1454,0);
 if (loop) return(0);}value[0]=(
value[22]==1);
if(value[0]){
set('E',22,'c',2,NULL,NULL);
}
move(245,-2); if (loop) return(0);
return 0;
}
int p525(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(213,0,524,519,526,-213); if (loop) return(0);
move(244,0,523,515,-21); if (loop) return(0);
return 0;
}
int p508(void)
{
move(210,0,514,-526); if (loop) return(0);
value[0]=(KEY(60));
if(value[0]){
value[0]=(value[20]==0);
if(value[0]){
say(64,1031,0);
 if (loop) return(0);}}
if (!anyof(528,516,-1)) return 0;
value[0]=(value[20]==0);
if(value[0]){
p527();
if (loop) return (0);
say(64,957,0);
 if (loop) return(0);}
move(215,-2); if (loop) return(0);
return 0;
}
int p509(void)
{
move(215,0,516,-526); if (loop) return(0);
value[0]=(KEY(60));
if(value[0]){
value[0]=(value[20]==0);
if(value[0]){
say(64,1031,0);
 if (loop) return(0);}}
if (!anyof(528,514,-1)) return 0;
value[0]=(value[20]==0);
if(value[0]){
p527();
if (loop) return (0);
say(64,957,0);
 if (loop) return(0);}
move(210,-2); if (loop) return(0);
return 0;
}
int p402(void)
{
value[0]=(KEY(534));
value[0]|=(KEY(556));
value[0]&=(KEY(38));
if(value[0]){
value[0]=(have(38,-1,-1));
value[0]&=(isnear(22,-1,-1));
if(value[0]){
set('E',22,'c',3,NULL,NULL);
apport(22,485);
apport(23,244);
apport(38,HERE);
say(0,1703,0);
say(64,974,0);
 if (loop) return(0);}}value[0]=(
KEY(556));
if(value[0]){
value[0]=(value[699]==1);
value[0]|=(isnear(23,-1,-1));
if(value[0]){
return 0;
}value[0]=(
KEY(115));
value[0]|=(KEY(116));
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(value[0]){
apport(value[672],HERE);
say(76,969,672);
 if (loop) return(0);}
return 0;
}value[0]=(
bitest(evar(672),0));
if(value[0]){
value[0]=(bitest(evar(672),5));
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(50));
if(value[0]){
value[0]=(bitest(22,13));
if(value[0]){
set('E',22,'c',0,NULL,NULL);
apport(50,485);
bitmod('s',50,14);
say(64,1456,0);
 if (loop) return(0);}
bitmod('s',50,13);
}
set('E',22,'c',1,NULL,NULL);
apport(22,484);
apport(23,244);
apport(value[672],485);
say(76,970,672);
 if (loop) return(0);}}}}value[0]=(
KEY(530));
if(value[0]){
value[0]=(value[21]==0);
if(value[0]){
say(64,903,0);
 if (loop) return(0);}
move(485,-1);
p543();
if (loop) return (0);
}value[0]=(
KEY(570));
if(value[0]){
return 0;
}
move(249,0,515,-249); if (loop) return(0);
move(250,0,-250); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
move(298,0,-298); if (loop) return(0);
value[0]=(KEY(9));
if(value[0]){
apport(9,value[673]);
}
if (!anyof(531,519,-1)) return 0;
value[0]=(value[21]>0);
if(value[0]){
say(64,972,0);
 if (loop) return(0);}value[0]=(
value[22]==0);
if(value[0]){
say(64,971,0);
 if (loop) return(0);}value[0]=(
value[22]==2);
if(value[0]){
set('E',22,'c',0,NULL,NULL);
apport(22,244);
apport(23,484);
say(64,1454,0);
 if (loop) return(0);}value[0]=(
value[22]==1);
if(value[0]){
set('E',22,'c',2,NULL,NULL);
}value[0]=(
have(38,-1,-1));
if(value[0]){
say(0,973,0);
set('E',21,'c',1,NULL,NULL);
apport(23,484);
move(485,-1);
p93();
if (loop) return (0);
}value[0]=(
value[22]==3);
value[0]&=(bitest(22,13));
if(value[0]){
value[0]=(isnear(38,-1,-1));
if(value[0]){
apport(38,INHAND);
set('E',21,'c',1,NULL,NULL);
say(0,1106,0);
}else{
set('E',21,'c',2,NULL,NULL);
say(0,1107,0);
}
apport(23,484);
move(485,-1);
p93();
if (loop) return (0);
}
move(244,-2); if (loop) return(0);
return 0;
}
int p94(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(245,0,520,-21); if (loop) return(0);
move(250,0,516,-250); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
move(298,0,-298); if (loop) return(0);
return 0;
}
int p221(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(245,0,-21); if (loop) return(0);
move(249,0,520,-249); if (loop) return(0);
move(251,0,-515); if (loop) return(0);
move(297,0,517,-524); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
move(298,0,-298); if (loop) return(0);
return 0;
}
int p617(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(250,0,518,-250); if (loop) return(0);
move(252,0,514,-615); if (loop) return(0);
move(296,0,516,-623); if (loop) return(0);
return 0;
}
int p46(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(251,0,518,526,-621); if (loop) return(0);
move(250,0,-250); if (loop) return(0);
value[0]=(KEY(530));
if(value[0]){
move(485,-1);
p543();
if (loop) return (0);
}value[0]=(
KEY(9));
value[0]&=(value[30]==2);
if(value[0]){
apport(9,value[673]);
}
if (!anyof(142,531,514,-1)) return 0;
value[0]=(value[30]==0);
if(value[0]){
say(64,1126,0);
 if (loop) return(0);}value[0]=(
have(64,0,2));
if(value[0]){
value[0]=(have(38,-1,-1));
if(value[0]){
say(0,1130,0);
apport(38,485);
}else{
value[0]=!(bitest(253,4));
if(value[0]){
bitmod('c',252,8);
say(0,1128,0);
}}value[0]=(
bitest(253,4));
if(!value[0]){
set('V',734,'c',8,NULL,NULL);
}
move(253,-2); if (loop) return(0);
}value[0]=(
have(38,-1,-1));
if(value[0]){
say(0,1129,0);
}else{
say(0,1127,0);
}value[0]=(
have(64,-1,-1));
if(value[0]){
say(0,1431,0);
}
move(485,-1);
p93();
if (loop) return (0);
return 0;
}
int p40(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(251,0,520,526,-623); if (loop) return(0);
move(250,0,-250); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
return 0;
}
int p328(void)
{
move(250,0,514,523,-250); if (loop) return(0);
move(298,0,518,524,-298); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
return 0;
}
int p21(void)
{
move(297,0,520,-523); if (loop) return(0);
move(250,0,-250); if (loop) return(0);
move(299,0,516,525,-298); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
p169();
if (loop) return (0);
return 0;
}
int p29(void)
{
move(298,0,520,-526); if (loop) return(0);
move(250,0,-250); if (loop) return(0);
move(252,0,-615); if (loop) return(0);
if (!keyword(533,115,-1)) return 0;
if (!isnear(115,-1,-1)) return 0;
value[0]=(have(115,-1,-1));
value[0]|=(value[115]==0);
if(!value[0]){
say(76,1030,672);
 if (loop) return(0);}
return 0;
}
int p379(void)
{
move(300,0,-520); if (loop) return(0);
move(303,0,-517); if (loop) return(0);
move(304,0,-521); if (loop) return(0);
move(305,0,-519); if (loop) return(0);
move(306,0,-515); if (loop) return(0);
move(307,0,-523); if (loop) return(0);
move(308,0,-524); if (loop) return(0);
move(309,0,-514); if (loop) return(0);
move(310,0,-518); if (loop) return(0);
move(301,0,-516); if (loop) return(0);
return 0;
}
int p380(void)
{
move(300,0,-521); if (loop) return(0);
move(302,0,-523); if (loop) return(0);
move(304,0,-514); if (loop) return(0);
move(305,0,-518); if (loop) return(0);
move(306,0,-520); if (loop) return(0);
move(307,0,-519); if (loop) return(0);
move(308,0,-515); if (loop) return(0);
move(309,0,-516); if (loop) return(0);
move(310,0,-524); if (loop) return(0);
move(301,0,-517); if (loop) return(0);
return 0;
}
int p381(void)
{
move(300,0,-523); if (loop) return(0);
move(302,0,-524); if (loop) return(0);
move(303,0,-520); if (loop) return(0);
move(305,0,-515); if (loop) return(0);
move(306,0,-519); if (loop) return(0);
move(307,0,-516); if (loop) return(0);
move(308,0,-514); if (loop) return(0);
move(309,0,-521); if (loop) return(0);
move(310,0,-517); if (loop) return(0);
move(301,0,-518); if (loop) return(0);
return 0;
}
int p382(void)
{
move(300,0,-515); if (loop) return(0);
move(302,0,-514); if (loop) return(0);
move(303,0,-521); if (loop) return(0);
move(304,0,-517); if (loop) return(0);
move(306,0,-516); if (loop) return(0);
move(307,0,-524); if (loop) return(0);
move(308,0,-518); if (loop) return(0);
move(309,0,-523); if (loop) return(0);
move(310,0,-520); if (loop) return(0);
move(301,0,-519); if (loop) return(0);
return 0;
}
int p383(void)
{
move(300,0,-514); if (loop) return(0);
move(302,0,-517); if (loop) return(0);
move(303,0,-524); if (loop) return(0);
move(304,0,-518); if (loop) return(0);
move(305,0,-516); if (loop) return(0);
move(307,0,-520); if (loop) return(0);
move(308,0,-519); if (loop) return(0);
move(309,0,-515); if (loop) return(0);
move(310,0,-521); if (loop) return(0);
move(301,0,-523); if (loop) return(0);
return 0;
}
int p384(void)
{
move(300,0,-516); if (loop) return(0);
move(302,0,-520); if (loop) return(0);
move(303,0,-523); if (loop) return(0);
move(304,0,-519); if (loop) return(0);
move(305,0,-524); if (loop) return(0);
move(306,0,-518); if (loop) return(0);
move(308,0,-521); if (loop) return(0);
move(309,0,-517); if (loop) return(0);
move(310,0,-515); if (loop) return(0);
move(301,0,-514); if (loop) return(0);
return 0;
}
int p385(void)
{
move(300,0,-517); if (loop) return(0);
move(302,0,-515); if (loop) return(0);
move(303,0,-518); if (loop) return(0);
move(304,0,-524); if (loop) return(0);
move(305,0,-523); if (loop) return(0);
move(306,0,-521); if (loop) return(0);
move(307,0,-514); if (loop) return(0);
move(309,0,-519); if (loop) return(0);
move(310,0,-516); if (loop) return(0);
move(301,0,-520); if (loop) return(0);
return 0;
}
int p386(void)
{
move(300,0,-524); if (loop) return(0);
move(302,0,-516); if (loop) return(0);
move(303,0,-515); if (loop) return(0);
move(304,0,-523); if (loop) return(0);
move(305,0,-520); if (loop) return(0);
move(306,0,-514); if (loop) return(0);
move(307,0,-518); if (loop) return(0);
move(308,0,-517); if (loop) return(0);
move(310,0,-519); if (loop) return(0);
move(301,0,-521); if (loop) return(0);
return 0;
}
int p387(void)
{
move(300,0,-519); if (loop) return(0);
move(302,0,-521); if (loop) return(0);
move(303,0,-516); if (loop) return(0);
move(304,0,-520); if (loop) return(0);
move(305,0,-514); if (loop) return(0);
move(306,0,-524); if (loop) return(0);
move(307,0,-517); if (loop) return(0);
move(308,0,-523); if (loop) return(0);
move(309,0,-518); if (loop) return(0);
move(301,0,-515); if (loop) return(0);
return 0;
}
int p388(void)
{
move(301,0,514,-526); if (loop) return(0);
if (!keyword(534,48,-1)) return 0;
if (!have(48,-1,-1)) return 0;
apport(48,485);
apport(39,value[673]);
bitmod('s',39,4);
set('E',34,'c',1,NULL,NULL);
say(64,1501,0);
 if (loop) return(0);return 0;
}
int p327(void)
{
move(141,0,-526); if (loop) return(0);
return 0;
}
int p494(void)
{
move(320,0,-524); if (loop) return(0);
p171();
if (loop) return (0);
move(151,0,520,526,-151); if (loop) return(0);
return 0;
}
int p394(void)
{
move(155,0,518,-526); if (loop) return(0);
return 0;
}
int p609(void)
{
value[0]=(KEY(525));
if(value[0]){
value[0]=(value[35]==1);
if(value[0]){
set('V',682,'v',673,NULL,NULL);
move(462,-2); if (loop) return(0);
}
say(64,1166,0);
 if (loop) return(0);}
if (!anyof(523,526,514,-1)) return 0;
value[0]=(value[35]==1);
if(value[0]){
say(64,1141,0);
 if (loop) return(0);}
move(155,-2); if (loop) return(0);
return 0;
}
int p305(void)
{
move(value[682],0,-526); if (loop) return(0);
if (!keyword(35,-1)) return 0;
value[0]=(value[699]==1);
if(value[0]){
bitmod('s',evar(699),5);
say(76,1022,35);
 if (loop) return(0);}value[0]=(
KEY(537));
if(value[0]){
say(64,1167,0);
 if (loop) return(0);}value[0]=(
KEY(535));
if(value[0]){
say(76,1133,672);
 if (loop) return(0);}value[0]=(
KEY(570));
if(value[0]){
say(64,1610,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p645(void)
{
move(155,0,-518); if (loop) return(0);
move(317,0,-514); if (loop) return(0);
return 0;
}
int p643(void)
{
move(316,0,518,521,-520); if (loop) return(0);
move(318,0,514,515,-516); if (loop) return(0);
move(318,0,-516); if (loop) return(0);
return 0;
}
int p584(void)
{
move(317,0,526,-518); if (loop) return(0);
return 0;
}
int p125(void)
{
move(155,0,-518); if (loop) return(0);
move(321,0,-514); if (loop) return(0);
move(320,0,-516); if (loop) return(0);
return 0;
}
int p101(void)
{
move(312,0,523,-516); if (loop) return(0);
p171();
if (loop) return (0);
move(319,0,518,249,-526); if (loop) return(0);
return 0;
}
int p542(void)
{
move(319,0,526,514,-249); if (loop) return(0);
return 0;
}
int p307(void)
{
move(222,0,-518); if (loop) return(0);
move(323,0,-514); if (loop) return(0);
return 0;
}
int p244(void)
{
move(322,0,-518); if (loop) return(0);
if (!anyof(514,324,-1)) return 0;
value[0]=(isnear(24,-1,-1));
if(value[0]){
p527();
if (loop) return (0);
say(64,1120,0);
 if (loop) return(0);}
move(324,-2); if (loop) return(0);
return 0;
}
int p322(void)
{
move(325,0,-516); if (loop) return(0);
move(323,0,-520); if (loop) return(0);
return 0;
}
int p37(void)
{
move(324,0,-514); if (loop) return(0);
move(326,0,-520); if (loop) return(0);
move(365,0,-516); if (loop) return(0);
move(324,0,-514); if (loop) return(0);
if (!keyword(530,-1)) return 0;
p460();
if (loop) return (0);
return 0;
}
int p39(void)
{
move(325,0,-514); if (loop) return(0);
move(327,0,-517); if (loop) return(0);
if (!keyword(530,-1)) return 0;
p460();
if (loop) return (0);
return 0;
}
int p262(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(326,0,-521); if (loop) return(0);
move(328,1118,524,516,-328); if (loop) return(0);
return 0;
}
int p523(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(332,0,-518); if (loop) return(0);
move(335,0,-521); if (loop) return(0);
if (!anyof(328,523,514,595,-1)) return 0;
say(64,1119,0);
 if (loop) return(0);return 0;
}
int p263(void)
{
move(331,0,-520); if (loop) return(0);
move(330,0,-514); if (loop) return(0);
return 0;
}
int p276(void)
{
move(329,0,-518); if (loop) return(0);
return 0;
}
int p277(void)
{
move(329,0,-516); if (loop) return(0);
move(333,0,-520); if (loop) return(0);
move(332,0,-514); if (loop) return(0);
return 0;
}
int p289(void)
{
move(328,0,-514); if (loop) return(0);
move(331,0,-518); if (loop) return(0);
return 0;
}
int p290(void)
{
move(331,0,-516); if (loop) return(0);
move(334,0,-514); if (loop) return(0);
return 0;
}
int p292(void)
{
move(333,0,-518); if (loop) return(0);
return 0;
}
int p293(void)
{
move(328,0,-516); if (loop) return(0);
move(336,0,-520); if (loop) return(0);
return 0;
}
int p294(void)
{
move(335,0,-515); if (loop) return(0);
move(337,0,-518); if (loop) return(0);
return 0;
}
int p295(void)
{
move(336,0,-514); if (loop) return(0);
move(338,0,-518); if (loop) return(0);
move(340,0,-520); if (loop) return(0);
return 0;
}
int p296(void)
{
move(337,0,-514); if (loop) return(0);
return 0;
}
int p297(void)
{
move(340,0,-514); if (loop) return(0);
return 0;
}
int p298(void)
{
move(337,0,-516); if (loop) return(0);
move(339,0,-518); if (loop) return(0);
move(341,0,-514); if (loop) return(0);
return 0;
}
int p264(void)
{
move(340,0,-518); if (loop) return(0);
move(342,0,-521); if (loop) return(0);
return 0;
}
int p265(void)
{
move(341,0,-516); if (loop) return(0);
move(343,0,-520); if (loop) return(0);
return 0;
}
int p266(void)
{
move(342,0,-515); if (loop) return(0);
move(344,0,-518); if (loop) return(0);
move(347,0,-520); if (loop) return(0);
return 0;
}
int p267(void)
{
move(343,0,-514); if (loop) return(0);
move(345,0,-518); if (loop) return(0);
return 0;
}
int p268(void)
{
move(344,0,-514); if (loop) return(0);
return 0;
}
int p269(void)
{
move(348,0,-514); if (loop) return(0);
return 0;
}
int p270(void)
{
move(343,0,-516); if (loop) return(0);
move(348,0,-518); if (loop) return(0);
move(349,0,-521); if (loop) return(0);
return 0;
}
int p271(void)
{
move(346,0,-518); if (loop) return(0);
move(347,0,-514); if (loop) return(0);
return 0;
}
int p272(void)
{
move(347,0,-516); if (loop) return(0);
move(350,0,-520); if (loop) return(0);
return 0;
}
int p273(void)
{
move(349,0,-515); if (loop) return(0);
move(351,0,-518); if (loop) return(0);
return 0;
}
int p274(void)
{
move(350,0,-514); if (loop) return(0);
move(352,0,-518); if (loop) return(0);
move(354,0,-520); if (loop) return(0);
move(355,0,-521); if (loop) return(0);
return 0;
}
int p275(void)
{
move(351,0,-514); if (loop) return(0);
move(353,0,-520); if (loop) return(0);
return 0;
}
int p278(void)
{
move(352,0,-516); if (loop) return(0);
move(354,0,-514); if (loop) return(0);
return 0;
}
int p279(void)
{
move(351,0,-516); if (loop) return(0);
move(353,0,-518); if (loop) return(0);
return 0;
}
int p280(void)
{
move(351,0,-517); if (loop) return(0);
move(356,0,-521); if (loop) return(0);
return 0;
}
int p281(void)
{
move(355,0,-516); if (loop) return(0);
move(357,0,-520); if (loop) return(0);
return 0;
}
int p282(void)
{
move(356,0,-515); if (loop) return(0);
move(358,0,-518); if (loop) return(0);
move(363,0,-520); if (loop) return(0);
return 0;
}
int p283(void)
{
move(357,0,-514); if (loop) return(0);
move(359,0,-518); if (loop) return(0);
move(361,0,-520); if (loop) return(0);
move(362,0,-521); if (loop) return(0);
return 0;
}
int p284(void)
{
move(358,0,-514); if (loop) return(0);
move(360,0,-521); if (loop) return(0);
return 0;
}
int p285(void)
{
move(359,0,-517); if (loop) return(0);
move(361,0,-514); if (loop) return(0);
return 0;
}
int p286(void)
{
move(358,0,-516); if (loop) return(0);
move(360,0,-518); if (loop) return(0);
return 0;
}
int p287(void)
{
move(358,0,-517); if (loop) return(0);
move(363,0,-514); if (loop) return(0);
return 0;
}
int p288(void)
{
move(357,0,-516); if (loop) return(0);
move(362,0,-518); if (loop) return(0);
move(364,0,-521); if (loop) return(0);
return 0;
}
int p291(void)
{
move(363,0,-516); if (loop) return(0);
move(327,value[687],-498); if (loop) return(0);
value[0]=(KEY(120));
if(value[0]){
say(76,1652,672);
 if (loop) return(0);}
return 0;
}
int p38(void)
{
move(325,0,-514); if (loop) return(0);
move(366,0,515,-612); if (loop) return(0);
if (!keyword(530,-1)) return 0;
p460();
if (loop) return (0);
return 0;
}
int p95(void)
{
move(365,0,-519); if (loop) return(0);
move(367,0,-517); if (loop) return(0);
return 0;
}
int p96(void)
{
move(366,0,-520); if (loop) return(0);
if (!keyword(518,-1)) return 0;
value[0]=(isnear(33,-1,-1));
if(value[0]){
say(0,1090,0);
p93();
if (loop) return (0);
}
move(368,-2); if (loop) return(0);
return 0;
}
int p97(void)
{
move(367,0,-514); if (loop) return(0);
move(369,0,518,-623); if (loop) return(0);
return 0;
}
int p526(void)
{
move(368,0,514,526,-623); if (loop) return(0);
return 0;
}
int p12(void)
{
move(231,0,-520); if (loop) return(0);
if (!anyof(516,531,-1)) return 0;
value[0]=(value[32]==0);
value[0]|=(have(106,-1,-1));
value[0]|=(have(107,-1,-1));
if(value[0]){
set('E',32,'c',0,NULL,NULL);
value[0]=(bitest(32,13));
if(value[0]){
say(0,1089,0);
}else{
say(0,1088,0);
bitmod('s',32,13);
}
move(485,-1);
p93();
if (loop) return (0);
}
set('E',32,'c',0,NULL,NULL);
move(371,-2); if (loop) return(0);
return 0;
}
int p13(void)
{
move(372,0,-514); if (loop) return(0);
if (!anyof(520,531,-1)) return 0;
value[0]=(value[32]==0);
if(value[0]){
value[0]=(bitest(32,13));
if(value[0]){
say(0,1089,0);
}else{
say(0,1088,0);
bitmod('s',32,13);
}
move(485,-1);
p93();
if (loop) return (0);
}
set('E',32,'c',0,NULL,NULL);
move(370,-2); if (loop) return(0);
return 0;
}
int p14(void)
{
move(371,0,-518); if (loop) return(0);
move(375,0,514,-375); if (loop) return(0);
move(373,0,516,-373); if (loop) return(0);
return 0;
}
int p222(void)
{
move(372,0,-521); if (loop) return(0);
move(376,0,-519); if (loop) return(0);
return 0;
}
int p483(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(375,0,-520); if (loop) return(0);
move(377,0,524,-5); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p26(void)
{
set('E',41,'c',1,NULL,NULL);
if (!anyof(523,376,5,520,-1)) return 0;
move(376,-2); if (loop) return(0);
return 0;
}
int p27(void)
{
p171();
if (loop) return (0);
if (!anyof(112,559,554,-1)) return 0;
value[0]=(KEY(112));
value[0]|=(KEY(554));
if(value[0]){
value[0]=(have(114,0,3));
value[0]|=(have(111,0,3));
if(value[0]){
return 0;
}}
say(64,871,0);
 if (loop) return(0);return 0;
}
int p309(void)
{
move(374,0,-518); if (loop) return(0);
move(372,0,-520); if (loop) return(0);
return 0;
}
int p302(void)
{
move(373,0,514,-526); if (loop) return(0);
value[0]=(KEY(13));
if(value[0]){
say(64,1476,0);
 if (loop) return(0);}
return 0;
}
int p167(void)
{
move(254,0,-514); if (loop) return(0);
value[0]=(KEY(9));
value[0]&=(value[30]==2);
if(value[0]){
apport(9,value[673]);
}
if (!anyof(531,30,518,-1)) return 0;
value[0]=(value[30]==0);
if(value[0]){
say(64,1126,0);
 if (loop) return(0);}value[0]=(
have(64,-1,-1));
if(value[0]){
value[0]=(have(56,-1,-1));
if(value[0]){
say(0,1131,0);
move(485,-1);
set('E',30,'c',0,NULL,NULL);
p93();
if (loop) return (0);
}
move(252,-2); if (loop) return(0);
}
say(0,1127,0);
move(485,-1);
p93();
if (loop) return (0);
return 0;
}
int p51(void)
{
value[0]=(value[31]==1);
if(value[0]){
move(255,0,-521); if (loop) return(0);
move(263,0,-514); if (loop) return(0);
move(292,0,-515); if (loop) return(0);
}
move(253,0,-518); if (loop) return(0);
return 0;
}
int p453(void)
{
move(254,0,-518); if (loop) return(0);
if (!keyword(514,-1)) return 0;
set('E',14,'c',0,NULL,NULL);
apport(14,256);
move(256,-2); if (loop) return(0);
return 0;
}
int p454(void)
{
if (!anyof(514,516,518,520,515,521,517,519,-1)) return 0;
value[678]=485;*bitword(678)= -1;
p499();
if (loop) return (0);
value[701]=514;*bitword(701)= -1;
value[701] += value[15];
value[0]=(value[701]==value[672]);
value[0]&=(value[699]==2);
value[0]|=(value[701]==value[671]);
value[0]&=(isnear(15,-1,-1));
if(value[0]){
set('E',14,'c',8,NULL,NULL);
value[686] = irand(1358-1355+1)+1355;
*bitword(686)= -1;
move(257,-2); if (loop) return(0);
}
move(256,-2); if (loop) return(0);
return 0;
}
int p455(void)
{
value[0]=(KEY(524));
if(value[0]){
apport(14,255);
move(258,-2); if (loop) return(0);
}
if (!anyof(514,516,518,520,515,521,517,519,-1)) return 0;
set('E',14,'c',0,NULL,NULL);
move(256,-2); if (loop) return(0);
return 0;
}
int p405(void)
{
move(259,0,514,-259); if (loop) return(0);
if (!anyof(523,518,-1)) return 0;
apport(14,256);
move(257,-2); if (loop) return(0);
return 0;
}
int p440(void)
{
if (!keyword(534,114,-1)) return 0;
if (!have(114,-1,-1)) return 0;
value[0]=(value[114]==1);
if(value[0]){
apport(114,HERE);
set('E',114,'c',0,NULL,NULL);
say(64,1195,0);
 if (loop) return(0);}
return 0;
}
int p441(void)
{
move(258,0,520,526,-258); if (loop) return(0);
move(260,0,514,612,-260); if (loop) return(0);
if (!keyword(570,259,-1)) return 0;
say(64,1518,0);
 if (loop) return(0);return 0;
}
int p84(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(261,0,523,595,-261); if (loop) return(0);
move(259,0,518,612,-259); if (loop) return(0);
return 0;
}
int p596(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(260,0,524,595,-260); if (loop) return(0);
move(262,0,518,328,-261); if (loop) return(0);
move(463,0,-521); if (loop) return(0);
return 0;
}
int p597(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(255,1118,518,524,-328); if (loop) return(0);
move(261,0,-514); if (loop) return(0);
move(260,0,260,-261); if (loop) return(0);
return 0;
}
int p409(void)
{
move(254,0,-518); if (loop) return(0);
value[0]=(KEY(514));
if(value[0]){
value[25] += 1;
value[0]=(value[25]==1);
if(value[0]){
say(0,1148,0);
}
move(264,-2); if (loop) return(0);
}
return 0;
}
int p24(void)
{
move(265,0,-514); if (loop) return(0);
value[0]=(KEY(518));
if(value[0]){
value[25] -= 1;
value[0]=(value[25]==0);
if(value[0]){
p527();
if (loop) return (0);
value[0]=(have(103,0,2));
if(value[0]){
say(0,1149,0);
set('E',25,'c',2,NULL,NULL);
bitmod('c',264,8);
}else{
say(0,1150,0);
p93();
if (loop) return (0);
}}
move(263,-2); if (loop) return(0);
}
return 0;
}
int p535(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(315,0,514,-315); if (loop) return(0);
move(264,0,-518); if (loop) return(0);
move(266,0,524,-5); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p438(void)
{
value[0]=(KEY(525));
if(value[0]){
value[0]=(value[35]==1);
if(value[0]){
set('V',682,'v',673,NULL,NULL);
move(462,-2); if (loop) return(0);
}
say(64,1166,0);
 if (loop) return(0);}
if (!anyof(518,526,250,-1)) return 0;
value[0]=(value[35]==1);
if(value[0]){
say(64,1141,0);
 if (loop) return(0);}
move(265,-2); if (loop) return(0);
return 0;
}
int p419(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(265,0,-523); if (loop) return(0);
move(267,0,524,-5); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p544(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(266,0,-523); if (loop) return(0);
move(268,0,524,-5); if (loop) return(0);
move(269,0,514,-526); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p546(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(267,0,523,-5); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p173(void)
{
move(267,0,-518); if (loop) return(0);
move(271,0,-520); if (loop) return(0);
move(270,0,-516); if (loop) return(0);
if (!anyof(501,228,-1)) return 0;
value[0]=(bitest(evar(712),2));
if(value[0]){
say(64,829,0);
 if (loop) return(0);}
move(382,-value[687]); if (loop) return(0);
return 0;
}
int p168(void)
{
move(269,0,520,-524); if (loop) return(0);
move(271,0,-523); if (loop) return(0);
return 0;
}
int p15(void)
{
move(291,0,-516); if (loop) return(0);
move(281,0,-520); if (loop) return(0);
return 0;
}
int p16(void)
{
move(290,0,-520); if (loop) return(0);
return 0;
}
int p343(void)
{
move(254,0,-521); if (loop) return(0);
move(293,0,-514); if (loop) return(0);
return 0;
}
int p247(void)
{
move(294,0,515,-294); if (loop) return(0);
move(295,0,521,-295); if (loop) return(0);
move(292,0,-518); if (loop) return(0);
return 0;
}
int p10(void)
{
move(293,0,518,-526); if (loop) return(0);
return 0;
}
int p595(void)
{
if (!anyof(526,516,529,-1)) return 0;
value[0]=(getloc(121)==484);
if(value[0]){
apport(121,value[673]);
set('E',121,'c',0,NULL,NULL);
}
move(293,-2); if (loop) return(0);
return 0;
}
int p456(void)
{
move(269,value[687],501,-228); if (loop) return(0);
if (!anyof(524,595,514,518,516,520,515,
   521,517,519,530,-1)) return 0;
say(0,1142,0);
move(485,-1);
p93();
if (loop) return (0);
return 0;
}
int p415(void)
{
value[0]=(KEY(657));
if(value[0]){
say(64,1606,0);
 if (loop) return(0);}
move(243,0,514,515,516,517,518,519,520,521,523,-524); if (loop) return(0);
if (!anyof(496,497,498,499,500,501,502,503,
   504,505,506,507,508,509,510,511,-1)) return 0;
value[0]=(KEY(493));
if(value[0]){
set('V',701,'v',672,NULL,NULL);
}else{
value[0]=(value[699]==1);
if(value[0]){
set('V',701,'v',671,NULL,NULL);
}else{
say(64,805,0);
 if (loop) return(0);}}
set('V',703,'v',701,NULL,NULL);
value[702]=495;*bitword(702)= -1;
value[702] += 1;
value[703] -= value[702];
value[0]=(value[703]==value[726]);
if(value[0]){
value[702]=512;*bitword(702)= -1;
value[702] -= 1;
value[0]=(value[701]==value[702]);
if(value[0]){
set('V',718,'c',5,NULL,NULL);
value[0]=(getloc(95)==140);
if(value[0]){
apport(95,243);
}else{
apport(95,485);
}
bitmod('c',95,3);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(bitest(evar(677),3));
if(value[0]){
value[0]=(getloc(value[677])==141);
if(value[0]){
apport(value[677],243);
}else{
apport(value[677],485);
}}}
move(136,-1402); if (loop) return(0);
}else{
say(0,778,0);
value[726] += 1;
}
}else{
value[703] -= 1;
value[0]=(value[703]==value[726]);
if(value[0]){
say(0,1565,0);
}else{
value[0]=(value[726]>2);
if(value[0]){
value[0]=(irand(100)<20);
value[0]|=!(bitest(evar(736),4));
if(value[0]){
say(0,1457,0);
bitmod('s',evar(736),4);
}else{
say(0,803,0);
}
}else{
say(0,829,0);
}}
set('V',726,'c',0,NULL,NULL);
}
loop=1; return(0);
return 0;
}
int p54(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',737,NULL,NULL);
value[680]=272;*bitword(680)= -1;
value[681]=271;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',737,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p64(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',738,NULL,NULL);
value[680]=273;*bitword(680)= -1;
value[681]=271;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',738,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p65(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',739,NULL,NULL);
value[680]=274;*bitword(680)= -1;
value[681]=272;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',739,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p66(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',740,NULL,NULL);
value[680]=275;*bitword(680)= -1;
value[681]=273;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',740,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p67(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',741,NULL,NULL);
value[680]=276;*bitword(680)= -1;
value[681]=274;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',741,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p68(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',742,NULL,NULL);
value[680]=277;*bitword(680)= -1;
value[681]=275;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',742,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p69(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',743,NULL,NULL);
value[680]=278;*bitword(680)= -1;
value[681]=276;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',743,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p70(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',744,NULL,NULL);
value[680]=279;*bitword(680)= -1;
value[681]=277;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',744,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p71(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',745,NULL,NULL);
value[680]=280;*bitword(680)= -1;
value[681]=278;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',745,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p55(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',746,NULL,NULL);
value[680]=281;*bitword(680)= -1;
value[681]=279;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',746,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p56(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',747,NULL,NULL);
value[680]=282;*bitword(680)= -1;
value[681]=289;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',747,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p57(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',748,NULL,NULL);
value[680]=283;*bitword(680)= -1;
value[681]=281;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',748,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p58(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',749,NULL,NULL);
value[680]=284;*bitword(680)= -1;
value[681]=282;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',749,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p59(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',750,NULL,NULL);
value[680]=285;*bitword(680)= -1;
value[681]=283;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',750,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p60(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',751,NULL,NULL);
value[680]=286;*bitword(680)= -1;
value[681]=284;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',751,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p61(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',752,NULL,NULL);
value[680]=287;*bitword(680)= -1;
value[681]=285;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',752,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p62(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',753,NULL,NULL);
value[680]=288;*bitword(680)= -1;
value[681]=287;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',753,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p63(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',754,NULL,NULL);
value[680]=269;*bitword(680)= -1;
value[681]=287;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',754,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p72(void)
{
value[0]=(KEY(516));
if(value[0]){
value[0]=(bitest(290,4));
if(!value[0]){
set('V',734,'c',10,NULL,NULL);
}
move(290,-2); if (loop) return(0);
}
if (!anyof(514,515,517,518,519,520,521,523,524,-1)) return 0;
set('V',703,'v',755,NULL,NULL);
value[680]=280;*bitword(680)= -1;
value[681]=281;*bitword(681)= -1;
p53();
if (loop) return (0);
set('V',755,'v',704,NULL,NULL);
loop=1; return(0);
return 0;
}
int p249(void)
{
move(161,0,-523); if (loop) return(0);
move(394,0,-524); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p78(void)
{
value[0]=(KEY(595));
if(value[0]){
say(64,1667,0);
 if (loop) return(0);}value[0]=(
KEY(77));
if(value[0]){
say(64,1666,0);
 if (loop) return(0);}value[0]=(
KEY(78));
value[0]|=(KEY(601));
value[0]&=(getloc(78)==393);
if(value[0]){
say(64,1233,0);
 if (loop) return(0);}value[0]=(
KEY(72));
if(value[0]){
value[0]=(KEY(113));
value[0]|=(KEY(570));
if(!value[0]){
say(64,1232,0);
 if (loop) return(0);}}
move(395,0,523,-5); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p80(void)
{
move(204,0,-595); if (loop) return(0);
move(235,0,-514); if (loop) return(0);
move(205,0,-520); if (loop) return(0);
move(408,0,-523); if (loop) return(0);
move(407,0,-524); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p612(void)
{
move(203,0,516,-526); if (loop) return(0);
return 0;
}
int p464(void)
{
move(384,0,-514); if (loop) return(0);
move(385,0,-518); if (loop) return(0);
move(412,0,-516); if (loop) return(0);
if (!keyword(630,-1)) return 0;
say(72,1553,630);
 if (loop) return(0);return 0;
}
int p28(void)
{
move(203,0,526,-524); if (loop) return(0);
return 0;
}
int p141(void)
{
move(383,0,-518); if (loop) return(0);
value[0]=(KEY(74));
if(value[0]){
value[0]=(KEY(560));
if(value[0]){
set('V',674,'c',0,NULL,NULL);
move(405,-1241); if (loop) return(0);
}value[0]=(
KEY(570));
if(value[0]){
say(64,1624,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p627(void)
{
move(412,0,-412); if (loop) return(0);
move(386,0,-516); if (loop) return(0);
move(389,0,-520); if (loop) return(0);
move(390,0,-523); if (loop) return(0);
move(387,0,-524); if (loop) return(0);
move(383,0,-514); if (loop) return(0);
return 0;
}
int p628(void)
{
move(385,0,-514); if (loop) return(0);
move(386,0,516,-524); if (loop) return(0);
move(389,0,-520); if (loop) return(0);
move(387,0,-518); if (loop) return(0);
move(388,0,-523); if (loop) return(0);
return 0;
}
int p629(void)
{
move(386,0,-514); if (loop) return(0);
move(387,0,516,-524); if (loop) return(0);
move(385,0,-520); if (loop) return(0);
move(388,0,-518); if (loop) return(0);
move(389,0,-523); if (loop) return(0);
return 0;
}
int p631(void)
{
if (!keyword(556,115,-1)) return 0;
if (!have(115,-1,-1)) return 0;
if (!isnear(87,-1,-1)) return 0;
set('E',115,'c',2,NULL,NULL);
apport(115,HERE);
say(64,1330,0);
 if (loop) return(0);return 0;
}
int p632(void)
{
value[0]=(KEY(533));
if(value[0]){
default_to(0,value[HERE],3);
value[0]=(KEY(115));
value[0]&=(value[115]==2);
if(value[0]){
say(64,1310,0);
 if (loop) return(0);}}value[0]=(
KEY(88));
value[0]&=(value[86]==1);
if(value[0]){
say(64,1337,0);
 if (loop) return(0);}
move(386,0,-514); if (loop) return(0);
move(390,0,-520); if (loop) return(0);
move(387,0,-516); if (loop) return(0);
move(389,0,-523); if (loop) return(0);
value[0]=(KEY(518));
if(value[0]){
p527();
if (loop) return (0);
value[0]=(value[86]==0);
if(value[0]){
say(64,1310,0);
 if (loop) return(0);}value[0]=(
value[86]==1);
if(value[0]){
say(64,1336,0);
 if (loop) return(0);}
move(433,-2); if (loop) return(0);
}value[0]=(
KEY(525));
value[0]&=(value[86]==2);
if(value[0]){
move(433,-2); if (loop) return(0);
}
return 0;
}
int p633(void)
{
move(390,0,-514); if (loop) return(0);
move(385,0,-516); if (loop) return(0);
move(389,0,520,-523); if (loop) return(0);
move(388,0,-518); if (loop) return(0);
move(387,0,-524); if (loop) return(0);
return 0;
}
int p634(void)
{
move(389,0,520,-514); if (loop) return(0);
move(385,0,-516); if (loop) return(0);
move(388,0,-518); if (loop) return(0);
move(389,0,-523); if (loop) return(0);
move(386,0,-524); if (loop) return(0);
return 0;
}
int p23(void)
{
move(392,0,523,-5); if (loop) return(0);
p169();
if (loop) return (0);
p170();
if (loop) return (0);
p171();
if (loop) return (0);
move(393,1239,525,-518); if (loop) return(0);
return 0;
}
int p77(void)
{
value[0]=(KEY(72));
if(value[0]){
value[0]=(KEY(113));
value[0]|=(KEY(570));
if(!value[0]){
say(64,1232,0);
 if (loop) return(0);}}
p170();
if (loop) return (0);
p171();
if (loop) return (0);
value[0]=(KEY(77));
value[0]&=(KEY(527));
value[0]|=(KEY(524));
if(value[0]){
say(64,1371,0);
 if (loop) return(0);}
return 0;
}
int p105(void)
{
move(391,0,-524); if (loop) return(0);
move(412,0,-523); if (loop) return(0);
p171();
if (loop) return (0);
move(225,0,521,525,-225); if (loop) return(0);
return 0;
}
int p74(void)
{
move(397,0,516,-531); if (loop) return(0);
move(432,0,520,514,518,521,-519); if (loop) return(0);
value[0]=(KEY(524));
if(value[0]){
say(0,1253,0);
move(485,-1);
p93();
if (loop) return (0);
}value[0]=(
KEY(636));
if(value[0]){
say(64,1557,0);
 if (loop) return(0);}
return 0;
}
int p73(void)
{
move(396,0,520,-531); if (loop) return(0);
move(239,0,518,517,516,-239); if (loop) return(0);
value[0]=(KEY(524));
if(value[0]){
say(0,1253,0);
move(485,-1);
p93();
if (loop) return (0);
}value[0]=(
KEY(636));
if(value[0]){
say(64,1557,0);
 if (loop) return(0);}
return 0;
}
int p587(void)
{
move(408,0,514,-526); if (loop) return(0);
move(399,0,523,-595); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p590(void)
{
move(398,0,-524); if (loop) return(0);
move(400,0,523,-595); if (loop) return(0);
p171();
if (loop) return (0);
return 0;
}
int p589(void)
{
move(399,0,-524); if (loop) return(0);
move(401,0,523,-595); if (loop) return(0);
p172();
if (loop) return (0);
p171();
if (loop) return (0);
return 0;
}
int p588(void)
{
value[0]=(KEY(524));
if(value[0]){
value[0]=(irand(100)<80);
if(value[0]){
move(402,-2); if (loop) return(0);
}
move(400,-2); if (loop) return(0);
}
p171();
if (loop) return (0);
if (!anyof(523,595,-1)) return 0;
value[0]=(irand(100)<80);
if(value[0]){
move(400,-2); if (loop) return(0);
}
move(402,-2); if (loop) return(0);
return 0;
}
int p593(void)
{
move(403,0,523,-595); if (loop) return(0);
move(401,0,-524); if (loop) return(0);
p172();
if (loop) return (0);
p171();
if (loop) return (0);
return 0;
}
int p592(void)
{
move(404,0,523,-595); if (loop) return(0);
move(402,0,-524); if (loop) return(0);
p169();
if (loop) return (0);
value[0]=(KEY(6));
if(value[0]){
value[0]=(KEY(535));
value[0]|=(KEY(563));
if(value[0]){
say(64,1427,0);
 if (loop) return(0);}value[0]=(
KEY(537));
if(value[0]){
say(76,1135,672);
 if (loop) return(0);}value[0]=(
KEY(113));
if(value[0]){
value[0]=(have(114,0,4));
value[0]|=(have(111,0,4));
if(value[0]){
value[0]=(have(111,0,4));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}else{
set('E',114,'c',2,NULL,NULL);
}
say(12,1496,113);
say(76,1497,6);
 if (loop) return(0);}
say(76,1026,671);
 if (loop) return(0);}value[0]=(
KEY(570));
if(value[0]){
say(64,1611,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}value[0]=(
KEY(120));
if(value[0]){
value[0]=(KEY(561));
value[0]|=(KEY(570));
if(value[0]){
say(64,1612,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
p171();
if (loop) return (0);
return 0;
}
int p591(void)
{
move(403,0,-524); if (loop) return(0);
p172();
if (loop) return (0);
p171();
if (loop) return (0);
if (!anyof(523,595,-1)) return 0;
value[679]=1255;*bitword(679)= -1;
value[679] += value[731];
if (*bitword(731*VARSIZE)==-1) *bitword(679*VARSIZE)= -1;
value[0]=(value[731]==2);
if(value[0]){
value[0]=(query(value[679])); if (loop>1) return (0);
if(value[0]){
value[679] += 1;
}else{
say(64,1259,0);
 if (loop) return(0);}
}else{
value[731] += 1;
}
say(2,679,0);
value[0]=(have(99,-1,-1));
if(value[0]){
apport(99,HERE);
}
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
if(value[0]){
apport(value[677],485);
}}
p93();
if (loop) return (0);
return 0;
}
int p142(void)
{
move(406,0,524,-518); if (loop) return(0);
value[0]=(KEY(523));
if(value[0]){
set('V',674,'c',0,NULL,NULL);
move(384,-1585); if (loop) return(0);
}
return 0;
}
int p635(void)
{
move(405,0,514,-523); if (loop) return(0);
value[0]=(KEY(595));
if(value[0]){
value[0]=(value[718]<2);
if(value[0]){
set('V',674,'c',0,NULL,NULL);
move(141,-1242); if (loop) return(0);
}
say(64,1584,0);
 if (loop) return(0);}
if (!keyword(634,-1)) return 0;
say(64,939,0);
 if (loop) return(0);return 0;
}
int p100(void)
{
move(203,0,523,526,-5); if (loop) return(0);
p171();
if (loop) return (0);
if (!keyword(627,-1)) return 0;
say(72,1553,627);
 if (loop) return(0);return 0;
}
int p585(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(203,0,524,-5); if (loop) return(0);
move(409,0,-514); if (loop) return(0);
move(398,0,-518); if (loop) return(0);
move(412,0,520,-412); if (loop) return(0);
value[0]=(KEY(18));
if(value[0]){
say(64,1386,0);
 if (loop) return(0);}
p171();
if (loop) return (0);
return 0;
}
int p534(void)
{
move(408,0,-518); if (loop) return(0);
move(410,0,-514); if (loop) return(0);
move(412,0,519,-412); if (loop) return(0);
return 0;
}
int p408(void)
{
move(409,0,-518); if (loop) return(0);
move(411,0,514,-525); if (loop) return(0);
return 0;
}
int p118(void)
{
move(410,0,-518); if (loop) return(0);
move(409,0,-526); if (loop) return(0);
value[0]=(KEY(628));
if(value[0]){
say(64,1552,0);
 if (loop) return(0);}
if (!keyword(629,-1)) return 0;
say(72,1553,629);
 if (loop) return(0);return 0;
}
int p435(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(408,0,-516); if (loop) return(0);
move(409,0,-515); if (loop) return(0);
move(392,0,514,524,-5); if (loop) return(0);
move(383,0,-520); if (loop) return(0);
return 0;
}
int p601(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(235,0,-518); if (loop) return(0);
move(414,0,-514); if (loop) return(0);
if (!anyof(520,612,617,143,-1)) return 0;
p414();
if (loop) return (0);
move(415,-2); if (loop) return(0);
return 0;
}
int p611(void)
{
move(413,0,-518); if (loop) return(0);
return 0;
}
int p349(void)
{
move(416,0,-520); if (loop) return(0);
move(416,0,-596); if (loop) return(0);
value[0]=(KEY(524));
if(value[0]){
value[0]=(bitest(91,13));
if(value[0]){
move(418,-2); if (loop) return(0);
}
move(417,-2); if (loop) return(0);
}
if (!anyof(523,516,612,617,143,-1)) return 0;
p414();
if (loop) return (0);
move(413,-2); if (loop) return(0);
return 0;
}
int p36(void)
{
value[0]=(KEY(94));
if(value[0]){
say(64,1263,0);
 if (loop) return(0);}
move(415,0,516,524,-526); if (loop) return(0);
return 0;
}
int p444(void)
{
if (!anyof(523,526,529,-1)) return 0;
bitmod('s',91,13);
set('E',91,'c',0,NULL,NULL);
value[733] = irand(4);
move(415,-2); if (loop) return(0);
return 0;
}
int p445(void)
{
if (!anyof(524,530,-1)) return 0;
say(0,1252,0);
say(0,1251,0);
move(419,-1);
p93();
if (loop) return (0);
return 0;
}
int p519(void)
{
move(415,0,523,-526); if (loop) return(0);
if (!anyof(530,524,-1)) return 0;
bitmod('c',91,13);
bitmod('s',91,14);
move(420,-1252); if (loop) return(0);
return 0;
}
int p103(void)
{
move(419,0,-514); if (loop) return(0);
move(421,0,-518); if (loop) return(0);
return 0;
}
int p504(void)
{
move(420,0,518,-526); if (loop) return(0);
value[0]=(KEY(76));
if(value[0]){
say(64,1302,0);
 if (loop) return(0);}
return 0;
}
int p104(void)
{
value[0]=(KEY(91));
value[0]&=(value[699]>1);
if(value[0]){
p467();
if (loop) return (0);
}
move(420,0,-514); if (loop) return(0);
move(425,0,-519); if (loop) return(0);
move(426,0,520,-521); if (loop) return(0);
move(422,0,516,518,-517); if (loop) return(0);
return 0;
}
int p513(void)
{
move(421,0,-514); if (loop) return(0);
move(423,0,517,-519); if (loop) return(0);
return 0;
}
int p514(void)
{
move(422,0,-514); if (loop) return(0);
move(424,0,518,-517); if (loop) return(0);
return 0;
}
int p515(void)
{
move(423,0,514,-526); if (loop) return(0);
return 0;
}
int p516(void)
{
move(421,0,-515); if (loop) return(0);
if (!anyof(523,525,623,-1)) return 0;
set('V',721,'c',1,NULL,NULL);
move(427,-2); if (loop) return(0);
return 0;
}
int p517(void)
{
move(421,0,516,-526); if (loop) return(0);
return 0;
}
int p552(void)
{
if (!anyof(524,597,-1)) return 0;
bitmod('s',91,14);
move(425,-2); if (loop) return(0);
return 0;
}
int p553(void)
{
move(425,0,524,-597); if (loop) return(0);
value[0]=(KEY(622));
if(value[0]){
say(64,925,0);
 if (loop) return(0);}
if (!anyof(514,515,516,517,518,519,520,521,523,596,-1)) return 0;
set('V',732,'c',0,NULL,NULL);
move(428,-2); if (loop) return(0);
return 0;
}
int p549(void)
{
value[0]=(KEY(523));
value[0]&=!(bitest(91,14));
if(value[0]){
value[0]=(value[732]<2);
if(value[0]){
value[678]=419;*bitword(678)= -1;
p499();
if (loop) return (0);
value[732] += 1;
move(428,-2); if (loop) return(0);
}
set('V',732,'c',0,NULL,NULL);
bitmod('c',91,14);
move(164,-2); if (loop) return(0);
}value[0]=(
KEY(622));
if(value[0]){
say(64,925,0);
 if (loop) return(0);}
if (!anyof(524,597,-1)) return 0;
value[0]=(irand(100)<10);
if(value[0]){
move(427,-2); if (loop) return(0);
}
return 0;
}
int p550(void)
{
if (!anyof(514,518,516,520,515,517,521,519,596,524,597,523,-1)) return 0;
value[678]=419;*bitword(678)= -1;
p499();
if (loop) return (0);
value[732] += 1;
move(428,-2); if (loop) return(0);
return 0;
}
int p20(void)
{
move(225,0,-520); if (loop) return(0);
move(430,0,-514); if (loop) return(0);
return 0;
}
int p348(void)
{
move(429,0,-518); if (loop) return(0);
move(431,0,-514); if (loop) return(0);
return 0;
}
int p324(void)
{
move(430,0,-519); if (loop) return(0);
move(432,0,-514); if (loop) return(0);
return 0;
}
int p562(void)
{
if (!anyof(514,515,516,517,518,519,520,521,-1)) return 0;
value[678]=485;*bitword(678)= -1;
p499();
if (loop) return (0);
return 0;
}
int p563(void)
{
if (!anyof(516,520,-1)) return 0;
p345();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p564(void)
{
if (!anyof(518,517,519,-1)) return 0;
value[0]=(irand(100)<33);
if(value[0]){
move(431,-2); if (loop) return(0);
}
p345();
if (loop) return (0);
return 0;
}
int p565(void)
{
if (!anyof(514,515,521,-1)) return 0;
value[0]=(irand(100)<33);
if(value[0]){
move(396,-2); if (loop) return(0);
}
p345();
if (loop) return (0);
return 0;
}
int p108(void)
{
if (!anyof(514,529,526,-1)) return 0;
move(388,-2); if (loop) return(0);
return 0;
}
int p567(void)
{
if (!anyof(514,515,516,517,518,519,520,521,529,-1)) return 0;
move(432,-2); if (loop) return(0);
return 0;
}
int p547(void)
{
move(403,0,-526); if (loop) return(0);
return 0;
}
int p555(void)
{
move(139,0,138,457,526,-517); if (loop) return(0);
value[0]=(KEY(141));
if(value[0]){
value[681]=139;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}value[0]=(
KEY(530));
if(value[0]){
move(485,-1);
p543();
if (loop) return (0);
}
if (!anyof(631,632,-1)) return 0;
value[0]=(KEY(570));
if(value[0]){
value[0]=(KEY(631));
if(value[0]){
say(64,1554,0);
 if (loop) return(0);}
say(64,1648,0);
 if (loop) return(0);}value[0]=(
KEY(631));
if(value[0]){
say(72,1553,631);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p468(void)
{
move(139,0,-520); if (loop) return(0);
move(457,0,-517); if (loop) return(0);
value[0]=(KEY(141));
if(value[0]){
value[681]=140;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}
if (!keyword(527,457,-1)) return 0;
value[680]=139;*bitword(680)= -1;
value[681]=457;*bitword(681)= -1;
p197();
if (loop) return (0);
return 0;
}
int p469(void)
{
move(138,0,-516); if (loop) return(0);
move(140,0,-521); if (loop) return(0);
value[0]=(KEY(141));
if(value[0]){
value[681]=140;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}
if (!keyword(527,457,-1)) return 0;
value[680]=138;*bitword(680)= -1;
value[681]=140;*bitword(681)= -1;
p197();
if (loop) return (0);
return 0;
}
int p437(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(138,0,521,-138); if (loop) return(0);
move(437,0,514,-618); if (loop) return(0);
move(441,0,-515); if (loop) return(0);
move(440,0,-516); if (loop) return(0);
move(136,0,-517); if (loop) return(0);
move(137,0,-518); if (loop) return(0);
move(438,0,519,-520); if (loop) return(0);
value[0]=(KEY(141));
if(value[0]){
value[681]=140;*bitword(681)= -1;
value[680]=136;*bitword(680)= -1;
p8();
if (loop) return (0);
}
if (!keyword(527,457,-1)) return 0;
value[680]=136;*bitword(680)= -1;
value[681]=138;*bitword(681)= -1;
p197();
if (loop) return (0);
return 0;
}
int p410(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(442,0,514,-618); if (loop) return(0);
move(443,0,-515); if (loop) return(0);
move(444,0,-516); if (loop) return(0);
move(459,0,-517); if (loop) return(0);
move(136,0,519,-141); if (loop) return(0);
move(440,0,-520); if (loop) return(0);
move(441,0,-521); if (loop) return(0);
value[0]=(KEY(518));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(459,-2); if (loop) return(0);
}
move(136,-2); if (loop) return(0);
}
return 0;
}
int p147(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(444,0,-514); if (loop) return(0);
move(445,0,-515); if (loop) return(0);
move(446,0,516,-618); if (loop) return(0);
move(448,0,-517); if (loop) return(0);
move(447,0,-518); if (loop) return(0);
move(460,0,-519); if (loop) return(0);
move(458,0,-521); if (loop) return(0);
move(136,0,-141); if (loop) return(0);
value[0]=(KEY(520));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(458,-2); if (loop) return(0);
}
move(460,-2); if (loop) return(0);
}
return 0;
}
int p536(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(459,0,-515); if (loop) return(0);
move(447,0,-516); if (loop) return(0);
move(449,0,-517); if (loop) return(0);
move(454,0,518,-618); if (loop) return(0);
move(143,0,-519); if (loop) return(0);
move(142,0,520,-524); if (loop) return(0);
move(136,0,521,-141); if (loop) return(0);
value[0]=(KEY(514));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(459,-2); if (loop) return(0);
}
move(136,-2); if (loop) return(0);
}
return 0;
}
int p163(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(144,0,514,-144); if (loop) return(0);
move(136,0,-141); if (loop) return(0);
move(143,0,-143); if (loop) return(0);
move(142,0,-142); if (loop) return(0);
move(455,0,-515); if (loop) return(0);
move(456,0,-516); if (loop) return(0);
move(453,0,520,-618); if (loop) return(0);
move(452,0,-521); if (loop) return(0);
value[0]=(KEY(595));
if(value[0]){
say(64,1230,0);
 if (loop) return(0);}
return 0;
}
int p201(void)
{
move(461,0,514,515,520,-521); if (loop) return(0);
move(441,0,-516); if (loop) return(0);
move(440,0,-517); if (loop) return(0);
move(457,0,518,-519); if (loop) return(0);
return 0;
}
int p212(void)
{
move(457,0,515,-516); if (loop) return(0);
move(137,0,-517); if (loop) return(0);
move(461,0,518,519,-520); if (loop) return(0);
move(439,0,-521); if (loop) return(0);
return 0;
}
int p214(void)
{
move(461,0,514,517,518,519,520,-521); if (loop) return(0);
move(438,0,515,-516); if (loop) return(0);
return 0;
}
int p215(void)
{
move(441,0,-514); if (loop) return(0);
move(442,0,-515); if (loop) return(0);
move(458,0,-516); if (loop) return(0);
move(136,0,517,518,519,-141); if (loop) return(0);
move(457,0,-520); if (loop) return(0);
move(437,0,-521); if (loop) return(0);
return 0;
}
int p216(void)
{
move(461,0,514,515,-521); if (loop) return(0);
move(442,0,-516); if (loop) return(0);
move(458,0,-517); if (loop) return(0);
move(440,0,-518); if (loop) return(0);
move(457,0,-519); if (loop) return(0);
move(437,0,-520); if (loop) return(0);
return 0;
}
int p217(void)
{
move(461,0,514,515,-521); if (loop) return(0);
move(443,0,-516); if (loop) return(0);
move(444,0,-517); if (loop) return(0);
move(458,0,-518); if (loop) return(0);
move(440,0,-519); if (loop) return(0);
move(441,0,-520); if (loop) return(0);
return 0;
}
int p218(void)
{
move(461,0,514,515,516,-521); if (loop) return(0);
move(445,0,-517); if (loop) return(0);
move(444,0,-518); if (loop) return(0);
move(458,0,-519); if (loop) return(0);
move(442,0,-520); if (loop) return(0);
return 0;
}
int p219(void)
{
move(443,0,-514); if (loop) return(0);
move(461,0,-515); if (loop) return(0);
move(445,0,-516); if (loop) return(0);
move(446,0,-517); if (loop) return(0);
move(459,0,-518); if (loop) return(0);
move(458,0,-520); if (loop) return(0);
move(442,0,-521); if (loop) return(0);
value[0]=(KEY(519));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(458,-2); if (loop) return(0);
}
move(459,-2); if (loop) return(0);
}
return 0;
}
int p220(void)
{
move(461,0,514,515,516,-517); if (loop) return(0);
move(446,0,-518); if (loop) return(0);
move(459,0,-519); if (loop) return(0);
move(444,0,-520); if (loop) return(0);
move(443,0,-521); if (loop) return(0);
return 0;
}
int p202(void)
{
move(445,0,-514); if (loop) return(0);
move(461,0,515,516,-517); if (loop) return(0);
move(448,0,-518); if (loop) return(0);
move(447,0,-519); if (loop) return(0);
move(459,0,-520); if (loop) return(0);
move(444,0,-521); if (loop) return(0);
return 0;
}
int p203(void)
{
move(459,0,-514); if (loop) return(0);
move(446,0,-515); if (loop) return(0);
move(448,0,-516); if (loop) return(0);
move(461,0,-517); if (loop) return(0);
move(449,0,-518); if (loop) return(0);
move(454,0,-519); if (loop) return(0);
move(460,0,-520); if (loop) return(0);
value[0]=(KEY(521));
if(value[0]){
value[0]=(irand(100)<50);
if(value[0]){
move(459,-2); if (loop) return(0);
}
move(460,-2); if (loop) return(0);
}
return 0;
}
int p204(void)
{
move(446,0,-514); if (loop) return(0);
move(461,0,515,516,517,-518); if (loop) return(0);
move(449,0,-519); if (loop) return(0);
move(447,0,-520); if (loop) return(0);
move(459,0,-521); if (loop) return(0);
return 0;
}
int p205(void)
{
move(447,0,-514); if (loop) return(0);
move(448,0,-515); if (loop) return(0);
move(461,0,516,517,-518); if (loop) return(0);
move(455,0,-519); if (loop) return(0);
move(454,0,-520); if (loop) return(0);
move(460,0,-521); if (loop) return(0);
return 0;
}
int p206(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(136,0,514,-515); if (loop) return(0);
move(142,0,524,516,-142); if (loop) return(0);
move(143,0,-517); if (loop) return(0);
move(451,0,-518); if (loop) return(0);
move(461,0,519,520,-521); if (loop) return(0);
return 0;
}
int p207(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(450,0,-514); if (loop) return(0);
move(142,0,-515); if (loop) return(0);
move(143,0,516,524,-142); if (loop) return(0);
move(144,0,-517); if (loop) return(0);
move(452,0,-518); if (loop) return(0);
move(461,0,519,520,-521); if (loop) return(0);
return 0;
}
int p208(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(451,0,-514); if (loop) return(0);
move(143,0,-515); if (loop) return(0);
move(144,0,516,524,-142); if (loop) return(0);
move(436,0,-517); if (loop) return(0);
move(453,0,-518); if (loop) return(0);
move(461,0,519,520,-521); if (loop) return(0);
return 0;
}
int p209(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(452,0,-514); if (loop) return(0);
move(144,0,-515); if (loop) return(0);
move(436,0,516,524,-142); if (loop) return(0);
move(461,0,517,518,519,520,-521); if (loop) return(0);
return 0;
}
int p210(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(460,0,-514); if (loop) return(0);
move(447,0,-515); if (loop) return(0);
move(449,0,-516); if (loop) return(0);
move(461,0,-517); if (loop) return(0);
move(455,0,-518); if (loop) return(0);
move(144,0,-519); if (loop) return(0);
move(143,0,520,524,-142); if (loop) return(0);
move(142,0,-521); if (loop) return(0);
return 0;
}
int p211(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(454,0,-514); if (loop) return(0);
move(449,0,-515); if (loop) return(0);
move(461,0,516,-517); if (loop) return(0);
move(456,0,-518); if (loop) return(0);
move(436,0,-519); if (loop) return(0);
move(144,0,520,524,-142); if (loop) return(0);
move(143,0,-521); if (loop) return(0);
return 0;
}
int p213(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(455,0,-514); if (loop) return(0);
move(461,0,515,516,517,518,-519); if (loop) return(0);
move(436,0,520,524,-142); if (loop) return(0);
move(144,0,-521); if (loop) return(0);
return 0;
}
int p110(void)
{
if (!anyof(514,515,516,517,518,519,520,521,-1)) return 0;
value[702]=516;*bitword(702)= -1;
set('V',701,'v',671,NULL,NULL);
value[0]=(value[699]>1);
if(value[0]){
set('V',701,'v',672,NULL,NULL);
}
value[701] -= value[702];
value[0]=(value[763]<0);
value[0]&=(irand(100)<20);
if(value[0]){
set('V',763,'v',701,NULL,NULL);
}value[0]=(
value[763]==value[701]);
if(value[0]){
say(64,1560,0);
 if (loop) return(0);}
value[678]=485;*bitword(678)= -1;
p499();
if (loop) return (0);
set('V',763,'c',-1,NULL,NULL);
set('V',701,'v',764,NULL,NULL);
value[0]=(value[701]>100);
if(value[0]){
set('V',701,'c',100,NULL,NULL);
}value[0]=(
irand(100)<value[701]);
if(value[0]){
value[764] -= 10;
value[0]=(value[764]<1);
if(value[0]){
set('V',764,'c',1,NULL,NULL);
}
move(461,-2); if (loop) return(0);
}
value[764] = irand(160-120+1)+120;
value[678] = irand(456-437+1)+437;
*bitword(678)= -1;
move(value[678],-2); if (loop) return(0);
return 0;
}
int p22(void)
{
value[0]=(KEY(570));
if(value[0]){
return 0;
}
move(261,0,516,-261); if (loop) return(0);
if (!keyword(530,-1)) return 0;
move(485,-1);
p543();
if (loop) return (0);
return 0;
}
int p403(void)
{
move(379,0,-519); if (loop) return(0);
p169();
if (loop) return (0);
if (!keyword(533,11,-1)) return 0;
say(64,921,0);
 if (loop) return(0);return 0;
}
int p33(void)
{
move(379,0,-519); if (loop) return(0);
move(381,0,-515); if (loop) return(0);
return 0;
}
int p557(void)
{
value[0]=(KEY(515));
if(value[0]){
value[0]=(bitest(381,4));
if(value[0]){
move(380,-2); if (loop) return(0);
}
move(378,-2); if (loop) return(0);
}
return 0;
}
int p111(void)
{
move(380,0,-519); if (loop) return(0);
move(482,0,515,523,518,520,-514); if (loop) return(0);
if (!anyof(516,517,524,521,-1)) return 0;
p598();
if (loop) return (0);
return 0;
}
int p112(void)
{
if (!anyof(514,515,516,517,518,519,520,521,523,524,-1)) return 0;
value[0]=(irand(100)<25);
if(value[0]){
move(381,-2); if (loop) return(0);
}value[0]=(
irand(100)<40);
if(value[0]){
move(482,-2); if (loop) return(0);
}
p598();
if (loop) return (0);
return 0;
}
int p325(void)
{
value[0]=(KEY(4));
if(value[0]){
value[0]=(isnear(4,-1,-1));
if(value[0]){
value[0]=(value[4]==1);
if(value[0]){
say(76,1133,4);
 if (loop) return(0);}
say(76,1704,671);
 if (loop) return(0);}
return 0;
}
(*procs[533])();
if (loop) return (0);
return 0;
}
int p227(void)
{
if (!anyof(554,591,-1)) return 0;
set('V',699,'c',1,NULL,NULL);
if (value[672]<LPROC && value[672]>= 0) (*procs[value[672]])(); else pcall(value[672]);
if (loop) return (0);
return 0;
}
int p228(void)
{
value[0]=(KEY(3));
if(value[0]){
default_to(1,value[673],3);
}value[0]=(
value[718]==3);
value[0]&=(value[699]>1);
value[0]&=(bitest(evar(672),3));
value[0]&=!(bitest(evar(672),4));
if(value[0]){
set('V',702,'c',0,NULL,NULL);
value[0]=(value[HERE]==378);
value[0]&=(bitest(evar(672),13));
if(value[0]){
set('V',702,'c',1,NULL,NULL);
}value[0]=(
value[HERE]==379);
value[0]&=(bitest(evar(672),14));
if(value[0]){
set('V',702,'c',1,NULL,NULL);
}value[0]=(
value[702]==1);
if(value[0]){
apport(value[672],value[673]);
bitmod('s',evar(672),4);
}}
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
value[0]=(KEY(85));
value[0]&=(isnear(85,-1,-1));
if(value[0]){
say(64,1309,0);
 if (loop) return(0);}
p248();
if (loop) return (0);
}
set('V',703,'c',0,NULL,NULL);
value[0]=(value[699]==1);
if(value[0]){
set('V',703,'c',1,NULL,NULL);
}
default_to(0,value[HERE],3);
if (!anyof(101,100,113,112,63,38,106,
   116,56,109,78,95,82,-1)) return 0;
value[0]=(KEY(101));
if(value[0]){
p231();
if (loop) return (0);
}else{
value[0]=(KEY(100));
if(value[0]){
p232();
if (loop) return (0);
}else{
value[0]=(KEY(113));
if(value[0]){
p234();
if (loop) return (0);
}else{
value[0]=(KEY(112));
if(value[0]){
p241();
if (loop) return (0);
}else{
value[0]=(KEY(63));
value[0]&=(value[HERE]==299);
value[0]&=(ishere(63,-1,-1));
value[0]&=!(value[63]==1);
if(value[0]){
say(64,980,0);
 if (loop) return(0);}value[0]=(
KEY(38));
if(value[0]){
p230();
if (loop) return (0);
}else{
value[0]=(KEY(116));
if(value[0]){
p240();
if (loop) return (0);
}else{
value[0]=(KEY(56));
if(value[0]){
p236();
if (loop) return (0);
}else{
value[0]=(KEY(109));
if(value[0]){
p233();
if (loop) return (0);
}else{
value[0]=(KEY(78));
if(value[0]){
p235();
if (loop) return (0);
}else{
value[0]=(KEY(95));
if(value[0]){
p238();
if (loop) return (0);
}else{
value[0]=(KEY(82));
value[0]&=(ishere(82,-1,-1));
if(value[0]){
say(64,1672,0);
 if (loop) return(0);}}}}}}}}}}}
return 0;
}
int p229(void)
{
value[0]=(value[699]==1);
if(value[0]){
return 0;
}value[0]=(
bitest(evar(672),0));
if(!value[0]){
return 0;
}value[0]=(
have(value[672],-1,-1));
if(value[0]){
say(76,814,672);
 if (loop) return(0);}value[0]=(
isnear(value[672],-1,-1));
if(!value[0]){
say(76,819,672);
 if (loop) return(0);}value[0]=(
KEY(84));
if(value[0]){
(*procs[570])();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
bitest(evar(672),3));
if(!value[0]){
say(64,815,0);
 if (loop) return(0);}value[0]=(
KEY(60));
value[0]&=(value[60]==0);
if(value[0]){
value[0]=(bitest(evar(736),6));
if(value[0]){
set('V',701,'c',0,NULL,NULL);
}else{
bitmod('s',evar(736),6);
set('V',701,'c',1,NULL,NULL);
}
say(76,1697,701);
 if (loop) return(0);}value[0]=(
value[707]<value[722]);
if(value[0]){
apport(value[672],INHAND);
say(12,1496,671);
value[0]=(KEY(104));
value[0]&=(value[703]>0);
if(value[0]){
say(12,1497,102);
}else{
say(12,1497,672);
}value[0]=(
KEY(107));
value[0]&=(value[718]==3);
value[0]&=(value[107]==0);
if(value[0]){
say(0,1393,0);
set('E',107,'c',1,NULL,NULL);
}
p447();
if (loop) return (0);
}else{
say(0,898,0);
flush_command();
}
loop=1; return(0);
return 0;
}
int p131(void)
{
default_to(0,486,-1);
value[0]=(KEY(3));
if(value[0]){
default_to(1,486,-1);
}
if (!anyof(101,100,111,113,112,53,118,27,38,
   97,95,-1)) return 0;
value[0]=(KEY(101));
if(value[0]){
p133();
if (loop) return (0);
}else{
value[0]=(KEY(100));
if(value[0]){
p135();
if (loop) return (0);
}else{
value[0]=(KEY(111));
if(value[0]){
p134();
if (loop) return (0);
}else{
value[0]=(KEY(113));
value[0]|=(KEY(112));
if(value[0]){
p137();
if (loop) return (0);
}else{
value[0]=(KEY(53));
if(value[0]){
p139();
if (loop) return (0);
}else{
value[0]=(KEY(118));
if(value[0]){
p140();
if (loop) return (0);
}else{
value[0]=(KEY(27));
value[0]&=(isnear(27,-1,-1));
if(value[0]){
apport(27,485);
bitmod('s',27,13);
value[717] = irand(12-4+1)+4;
set('V',725,'v',717,NULL,NULL);
say(64,1160,0);
 if (loop) return(0);}value[0]=(
KEY(38));
if(value[0]){
value[0]=(have(38,-1,-1));
if(value[0]){
apport(38,HERE);
say(0,1703,0);
say(64,777,0);
 if (loop) return(0);}
}else{
value[0]=(KEY(95));
if(value[0]){
p138();
if (loop) return (0);
}else{
value[0]=(KEY(97));
if(value[0]){
p136();
if (loop) return (0);
}}}}}}}}}
return 0;
}
int p132(void)
{
value[0]=(value[699]==1);
if(value[0]){
return 0;
}value[0]=(
bitest(evar(672),0));
if(!value[0]){
return 0;
}value[0]=(
have(value[672],-1,-1));
if(value[0]){
p83();
if (loop) return (0);
value[0]=(value[HERE]==393);
value[0]&=!(bitest(evar(672),15));
value[0]|=(value[HERE]==415);
value[0]|=(value[HERE]==418);
value[0]|=(value[HERE]==417);
value[0]|=(bitest(evar(673),13));
if(value[0]){
apport(value[672],419);
value[0]=(KEY(110));
if(value[0]){
apport(110,484);
value[0]=(isnear(91,-1,-1));
if(value[0]){
say(0,1299,0);
value[0]=(value[91]>1);
if(value[0]){
say(0,1300,0);
}
set('E',91,'c',0,NULL,NULL);
loop=1; return(0);
}}
say(12,1551,672);
value[0]=(value[HERE]==393);
value[0]|=(value[HERE]==415);
if(value[0]){
value[0]=(value[HERE]==393);
if(value[0]){
value[703]=1608;*bitword(703)= -1;
}else{
value[703]=1269;*bitword(703)= -1;
}
}else{
value[703]=1301;*bitword(703)= -1;
}value[0]=(
bitest(evar(672),11));
if(value[0]){
say(11,703,1);
}else{
say(11,703,0);
}
loop=1; return(0);
}value[0]=(
value[HERE]==204);
if(value[0]){
apport(value[672],203);
say(12,1551,672);
value[0]=(bitest(evar(672),11));
if(value[0]){
say(9,1083,1);
}else{
say(9,1083,0);
}
loop=1; return(0);
}
apport(value[672],HERE);
value[0]=(KEY(89));
value[0]&=(value[89]==0);
if(value[0]){
bitmod('s',evar(699),4);
say(0,89,0);
bitmod('c',evar(699),4);
set('E',89,'c',1,NULL,NULL);
}else{
value[0]=(KEY(80));
value[0]&=(value[HERE]==141);
value[0]&=!(bitest(80,5));
if(value[0]){
say(12,1273,672);
bitmod('s',80,5);
}else{
value[0]=(KEY(114));
if(value[0]){
say(12,1496,671);
say(12,1498,114);
value[0]=(value[114]==4);
if(value[0]){
say(12,1500,113);
}else{
value[0]=(value[114]==3);
if(value[0]){
say(12,1500,112);
}else{
say(0,777,0);
}}value[0]=(
value[114]>2);
if(value[0]){
set('E',114,'c',2,NULL,NULL);
}
}else{
say(12,1496,671);
say(12,1497,672);
}}}
}else{
say(12,818,672);
}
loop=1; return(0);
return 0;
}
int p245(void)
{
value[0]=(value[699]==1);
if(value[0]){
say(76,887,671);
 if (loop) return(0);}value[0]=(
513<=value[671] && value[671]<=522);
value[0]|=(513<=value[672] && value[672]<=522);
value[0]|=(611<=value[672] && value[672]<=625);
value[0]|=(KEY(523));
value[0]|=(KEY(524));
value[0]|=(KEY(529));
value[0]|=(KEY(613));
value[0]|=(KEY(526));
value[0]|=(KEY(525));
if(value[0]){
if (value[672]<LPROC && value[672]>= 0) (*procs[value[672]])(); else pcall(value[672]);
if (loop) return (0);
}else{
value[0]=(bitest(evar(672),1));
if(value[0]){
value[0]=(value[HERE]==value[672]);
if(value[0]){
say(64,846,0);
 if (loop) return(0);}
say(64,938,0);
 if (loop) return(0);}value[0]=(
KEY(19));
value[0]|=(KEY(134));
value[0]|=(KEY(82));
value[0]|=(KEY(16));
value[0]|=(KEY(38));
value[0]|=(KEY(85));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
say(76,1594,672);
 if (loop) return(0);}
say(76,819,672);
 if (loop) return(0);}
say(64,1523,0);
 if (loop) return(0);}
return 0;
}
int p122(void)
{
say(64,864,0);
 if (loop) return(0);return 0;
}
int p75(void)
{
value[0]=(value[HERE]==142);
value[0]|=(value[HERE]==136);
value[0]|=(value[HERE]==143);
value[0]|=(value[HERE]==144);
value[0]|=(value[HERE]==141);
if(value[0]){
say(64,845,0);
 if (loop) return(0);}
say(64,846,0);
 if (loop) return(0);return 0;
}
int p417(void)
{
value[0]=(isnear(111,0,4));
value[0]|=(isnear(114,0,4));
value[0]|=(isnear(113,-1,-1));
if(!value[0]){
value[0]=(have(118,-1,-1));
if(value[0]){
say(64,1428,0);
 if (loop) return(0);}
say(76,1026,113);
 if (loop) return(0);}value[0]=(
value[699]>1);
if(value[0]){
value[0]=(value[671]==113);
value[0]&=!(isnear(value[672],-1,-1));
if(value[0]){
say(76,819,672);
 if (loop) return(0);}else{
value[0]=(value[672]==113);
value[0]&=!(isnear(value[671],-1,-1));
if(value[0]){
say(76,819,671);
 if (loop) return(0);}}}value[0]=(
KEY(6));
if(value[0]){
p124();
if (loop) return (0);
value[0]=(value[6]==0);
if(value[0]){
set('E',6,'c',1,NULL,NULL);
say(64,920,0);
 if (loop) return(0);}
say(12,1496,113);
say(76,1497,6);
 if (loop) return(0);}value[0]=(
KEY(11));
if(value[0]){
p124();
if (loop) return (0);
say(64,918,0);
 if (loop) return(0);}value[0]=(
KEY(72));
if(value[0]){
p124();
if (loop) return (0);
set('E',72,'c',1,NULL,NULL);
say(64,1698,0);
 if (loop) return(0);}value[0]=(
KEY(115));
if(value[0]){
say(64,1663,0);
 if (loop) return(0);}value[0]=(
KEY(116));
if(value[0]){
value[703]=1709;*bitword(703)= -1;
value[0]=(value[116]==0);
if(value[0]){
value[703] += 1;
}
say(66,703,0);
 if (loop) return(0);}
return 0;
}
int p620(void)
{
if (!KEY(11)) return 0;
value[0]=(isnear(11,-1,-1));
if(value[0]){
value[0]=(have(114,0,3));
value[0]|=(have(111,0,3));
if(value[0]){
value[0]=(have(111,0,3));
if(!value[0]){
set('E',114,'c',2,NULL,NULL);
say(64,1373,0);
 if (loop) return(0);}
set('E',111,'c',2,NULL,NULL);
p623();
if (loop) return (0);
}else{
say(12,1026,671);
}
}else{
say(12,819,672);
}
loop=1; return(0);
return 0;
}
int p621(void)
{
if (!KEY(6)) return 0;
value[0]=(isnear(6,-1,-1));
if(value[0]){
value[0]=(have(114,0,3));
value[0]|=(have(111,0,3));
if(value[0]){
value[0]=(have(111,0,3));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
}else{
set('E',114,'c',2,NULL,NULL);
}
set('E',6,'c',0,NULL,NULL);
say(64,919,0);
 if (loop) return(0);}
say(76,1026,112);
 if (loop) return(0);}
say(76,819,6);
 if (loop) return(0);return 0;
}
int p573(void)
{
if (!anyof(115,116,-1)) return 0;
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
p625();
if (loop) return (0);
}
return 0;
}
int p574(void)
{
if (!KEY(112)) return 0;
value[0]=(have(114,0,3));
value[0]|=(have(111,0,3));
if(value[0]){
p137();
if (loop) return (0);
loop=1; return(0);
}
say(76,1026,672);
 if (loop) return(0);return 0;
}
int p575(void)
{
if (!KEY(113)) return 0;
value[0]=(have(114,0,4));
value[0]|=(have(111,0,4));
if(value[0]){
p137();
if (loop) return (0);
loop=1; return(0);
}
say(76,1026,672);
 if (loop) return(0);return 0;
}
int p576(void)
{
if (!KEY(110)) return 0;
if (!have(110,-1,-1)) return 0;
value[0]=(isnear(22,-1,-1));
if(value[0]){
say(64,994,0);
 if (loop) return(0);}value[0]=(
isnear(134,-1,-1));
if(value[0]){
apport(110,HERE);
bitmod('s',134,14);
say(64,909,0);
 if (loop) return(0);}value[0]=(
isnear(38,-1,-1));
if(value[0]){
apport(110,484);
set('E',38,'c',1,NULL,NULL);
value[0]=(value[115]==1);
if(value[0]){
set('E',115,'c',0,NULL,NULL);
}
say(64,978,0);
 if (loop) return(0);}value[0]=(
isnear(91,-1,-1));
if(value[0]){
say(0,1299,0);
apport(110,484);
value[0]=(value[91]>1);
if(value[0]){
say(64,1300,0);
 if (loop) return(0);}
loop=1; return(0);
}
return 0;
}
int p577(void)
{
if (!KEY(117)) return 0;
if (!have(117,-1,-1)) return 0;
p541();
if (loop) return (0);
return 0;
}
int p578(void)
{
if (!KEY(118)) return 0;
if (!have(118,-1,-1)) return 0;
p45();
if (loop) return (0);
return 0;
}
int p579(void)
{
value[0]=(value[699]==2);
if(value[0]){
value[0]=(bitest(evar(672),0));
if(value[0]){
value[0]=(have(value[672],-1,-1));
value[0]&=(bitest(evar(673),12));
if(value[0]){
p83();
if (loop) return (0);
p603();
if (loop) return (0);
}}}
(*procs[534])();
if (loop) return (0);
return 0;
}
int p540(void)
{
value[0]=(value[699]>1);
if(value[0]){
value[0]=(KEY(117));
if(value[0]){
value[0]=(have(117,-1,-1));
if(value[0]){
p541();
if (loop) return (0);
}
say(76,1026,672);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p624(void)
{
bitmod('s',16,13);
value[0]=(value[699]>1);
if(value[0]){
value[0]=(bitest(evar(672),0));
if(!value[0]){
return 0;
}value[0]=(
bitest(evar(672),3));
if(!value[0]){
say(64,815,0);
 if (loop) return(0);}value[0]=(
have(value[672],-1,-1));
if(!value[0]){
say(76,818,672);
 if (loop) return(0);}
}else{
say(0,1650,0);
value[0]=(isnear(16,-1,-1));
if(value[0]){
say(64,777,0);
 if (loop) return(0);}
say(64,1651,0);
 if (loop) return(0);}value[0]=(
KEY(102));
if(value[0]){
value[0]=(isnear(8,-1,-1));
if(value[0]){
value[0]=(value[718]<2);
if(value[0]){
value[8] += 1;
value[0]=(value[8]==2);
if(value[0]){
set('E',8,'c',0,NULL,NULL);
bitmod('s',8,10);
apport(9,484);
}else{
bitmod('c',8,10);
}
say(12,1493,8);
}else{
say(0,829,0);
}
}else{
value[0]=(isnear(32,-1,-1));
if(value[0]){
say(0,1087,0);
set('E',32,'c',1,NULL,NULL);
}else{
value[0]=(isnear(30,-1,-1));
value[0]&=(value[718]<2);
if(value[0]){
value[30] += 1;
say(0,30,0);
value[30] += 1;
value[0]=(value[30]==4);
if(value[0]){
set('E',30,'c',0,NULL,NULL);
bitmod('s',30,10);
apport(9,484);
}else{
bitmod('c',30,10);
}
}else{
say(0,829,0);
}}}
}else{
value[0]=(KEY(115));
value[0]|=(KEY(116));
if(value[0]){
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
say(76,1649,672);
 if (loop) return(0);}
p625();
if (loop) return (0);
}else{
say(0,829,0);
}}
loop=1; return(0);
return 0;
}
int p407(void)
{
p404();
if (loop) return (0);
return 0;
}
int p533(void)
{
p404();
if (loop) return (0);
return 0;
}
int p602(void)
{
p404();
if (loop) return (0);
return 0;
}
int p127(void)
{
p404();
if (loop) return (0);
return 0;
}
int p146(void)
{
p404();
if (loop) return (0);
return 0;
}
int p636(void)
{
p404();
if (loop) return (0);
return 0;
}
int p411(void)
{
p404();
if (loop) return (0);
return 0;
}
int p412(void)
{
p404();
if (loop) return (0);
return 0;
}
int p538(void)
{
p404();
if (loop) return (0);
return 0;
}
int p539(void)
{
p404();
if (loop) return (0);
return 0;
}
int p306(void)
{
set('V',702,'c',0,NULL,NULL);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
if(value[0]){
value[0]=(value[702]==0);
if(value[0]){
say(0,905,0);
set('V',702,'c',1,NULL,NULL);
}
say(2,677,0);
}}value[0]=(
value[702]==0);
if(value[0]){
say(0,899,0);
}
loop=1; return(0);
return 0;
}
int p333(void)
{
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
say(64,825,0);
 if (loop) return(0);}value[0]=(
value[699]==1);
if(value[0]){
set('V',702,'c',0,NULL,NULL);
value[0]=(bitest(evar(699),2));
if(!value[0]){
bitmod('s',evar(699),3);
set('V',702,'c',1,NULL,NULL);
}
p120();
if (loop) return (0);
bitmod('c',evar(699),3);
bitmod('s',evar(673),4);
loop=1; return(0);
}
if (!anyof(581,73,18,14,602,-1)) return 0;
if (value[672]<LPROC && value[672]>= 0) (*procs[value[672]])(); else pcall(value[672]);
if (loop) return (0);
return 0;
}
int p334(void)
{
if (!anyof(120,88,71,108,70,-1)) return 0;
(*procs[561])();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p335(void)
{
if (!KEY(113)) return 0;
value[0]=(isnear(111,0,4));
value[0]|=(isnear(113,-1,-1));
if(value[0]){
say(64,1458,0);
 if (loop) return(0);}value[0]=(
isnear(118,-1,-1));
if(value[0]){
say(64,1428,0);
 if (loop) return(0);}
return 0;
}
int p336(void)
{
if (!KEY(11)) return 0;
value[0]=(isnear(12,-1,-1));
value[0]&=(value[12]>0);
value[0]|=(isnear(11,-1,-1));
if(value[0]){
say(76,1597,11);
 if (loop) return(0);}
say(76,819,672);
 if (loop) return(0);return 0;
}
int p337(void)
{
if (!KEY(112)) return 0;
value[0]=(isnear(111,0,3));
value[0]|=(bitest(evar(673),9));
if(value[0]){
say(64,1595,0);
 if (loop) return(0);}
return 0;
}
int p338(void)
{
value[0]=(bitest(evar(672),0));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(!value[0]){
say(76,819,672);
 if (loop) return(0);}
}else{
value[0]=(bitest(evar(672),1));
if(value[0]){
value[0]=(value[672]==143);
if(value[0]){
value[0]=(value[HERE]==value[672]);
if(!value[0]){
say(64,1681,0);
 if (loop) return(0);}
}else{
say(64,966,0);
 if (loop) return(0);}}}
if (!anyof(9,78,-1)) return 0;
value[0]=(KEY(9));
if(value[0]){
p342();
if (loop) return (0);
}else{
value[0]=(KEY(78));
if(value[0]){
(*procs[601])();
if (loop) return (0);
}}
loop=1; return(0);
return 0;
}
int p339(void)
{
if (!KEY(114)) return 0;
value[0]=(value[114]<2);
if(value[0]){
say(64,1607,0);
 if (loop) return(0);}
return 0;
}
int p340(void)
{
if (!KEY(107)) return 0;
value[0]=(value[718]==3);
value[0]&=(value[107]==0);
value[0]&=(have(107,-1,-1));
if(value[0]){
set('E',107,'c',1,NULL,NULL);
say(64,1393,0);
 if (loop) return(0);}
return 0;
}
int p341(void)
{
value[0]=(KEY(43));
value[0]&=(value[43]>1);
if(value[0]){
say(76,1613,43);
 if (loop) return(0);}value[0]=(
bitest(evar(672),0));
value[0]|=(bitest(evar(672),1));
if(value[0]){
bitmod('s',evar(699),4);
say(2,672,0);
bitmod('c',evar(699),4);
value[0]=(KEY(89));
value[0]&=(value[89]==0);
if(value[0]){
set('E',89,'c',1,NULL,NULL);
}
loop=1; return(0);
}value[0]=(
bitest(evar(672),2));
if(value[0]){
value[0]=(611<=value[672] && value[672]<=625);
if(value[0]){
say(64,966,0);
 if (loop) return(0);}value[0]=(
626<=value[672] && value[672]<=659);
if(value[0]){
value[0]=(KEY(657));
if(value[0]){
value[0]=(bitest(evar(673),7));
if(value[0]){
say(64,916,0);
 if (loop) return(0);}
say(76,1548,672);
 if (loop) return(0);}value[0]=(
KEY(656));
if(value[0]){
say(64,966,0);
 if (loop) return(0);}
say(76,819,672);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p502(void)
{
value[0]=(value[699]==1);
value[0]|=(KEY(78));
if(!value[0]){
say(64,815,0);
 if (loop) return(0);}
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
say(64,1422,0);
 if (loop) return(0);}value[0]=(
isnear(78,-1,-1));
if(value[0]){
bitmod('s',evar(756),4);
p613();
if (loop) return (0);
value[702]=1410;*bitword(702)= -1;
value[702] += value[701];
if (*bitword(701*VARSIZE)==-1) *bitword(702*VARSIZE)= -1;
say(0,1409,0);
say(2,702,0);
say(64,1421,0);
 if (loop) return(0);}value[0]=(
value[HERE]==394);
value[0]&=(getloc(78)==393);
if(value[0]){
say(64,1233,0);
 if (loop) return(0);}
say(64,1423,0);
 if (loop) return(0);return 0;
}
int p326(void)
{
value[0]=(KEY(545));
if(value[0]){
(*procs[545])();
if (loop) return (0);
}value[0]=(
KEY(99));
value[0]|=(KEY(544));
value[0]|=(value[699]==1);
if(value[0]){
value[0]=(isnear(99,-1,-1));
if(value[0]){
value[0]=(value[99]==1);
if(value[0]){
say(72,827,1);
 if (loop) return(0);}value[0]=(
value[700]<40);
if(value[0]){
value[0]=(isnear(39,0,1));
if(value[0]){
say(0,1052,0);
set('E',39,'c',3,NULL,NULL);
value[700] += 400;
}}value[0]=(
value[700]>0);
if(value[0]){
set('E',99,'c',1,NULL,NULL);
say(8,826,1);
value[0]=(bitest(evar(673),3));
if(!value[0]){
value[0]=(isnear(85,-1,-1));
if(value[0]){
p433();
if (loop) return (0);
}
set('V',699,'c',1,NULL,NULL);
(*procs[570])();
if (loop) return (0);
}
p442();
if (loop) return (0);
}else{
say(0,996,0);
}
}else{
value[0]=(KEY(99));
if(value[0]){
say(76,819,672);
 if (loop) return(0);}
say(64,825,0);
 if (loop) return(0);}
loop=1; return(0);
}
return 0;
}
int p166(void)
{
value[0]=(KEY(99));
value[0]|=(KEY(545));
value[0]|=(value[699]==1);
if(value[0]){
value[0]=(isnear(99,-1,-1));
if(value[0]){
value[0]=(value[99]==0);
if(value[0]){
say(72,827,0);
 if (loop) return(0);}
set('E',99,'c',0,NULL,NULL);
say(8,826,0);
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
say(0,806,0);
}
p442();
if (loop) return (0);
}else{
value[0]=(KEY(99));
if(value[0]){
say(76,819,672);
 if (loop) return(0);}
return 0;
}
}else{
say(64,950,0);
 if (loop) return(0);}
loop=1; return(0);
return 0;
}
int p599(void)
{
default_to(0,value[HERE],9);
value[0]=(KEY(4));
value[0]&=(value[4]==0);
value[0]&=!(have(98,-1,-1));
if(value[0]){
say(64,821,0);
 if (loop) return(0);}
(*procs[535])();
if (loop) return (0);
return 0;
}
int p421(void)
{
if (!KEY(259)) return 0;
value[0]=(value[HERE]==259);
if(value[0]){
value[0]=(isnear(27,-1,-1));
if(value[0]){
apport(27,485);
bitmod('s',27,13);
say(64,1160,0);
 if (loop) return(0);}
say(64,1162,0);
 if (loop) return(0);}
say(76,819,672);
 if (loop) return(0);return 0;
}
int p422(void)
{
value[0]=(KEY(576));
if(value[0]){
say(64,838,0);
 if (loop) return(0);}
default_to(0,value[HERE],9);
value[0]=(value[699]==2);
value[0]&=!(isnear(value[672],-1,-1));
if(value[0]){
say(76,819,672);
 if (loop) return(0);}
if (!anyof(111,42,49,100,106,107,118,98,93,-1)) return 0;
value[0]=(KEY(111));
if(value[0]){
say(64,1472,0);
 if (loop) return(0);}value[0]=(
KEY(42));
if(value[0]){
say(64,1471,0);
 if (loop) return(0);}value[0]=(
KEY(49));
if(value[0]){
say(64,1390,0);
 if (loop) return(0);}value[0]=(
KEY(100));
if(value[0]){
say(66,720,0);
 if (loop) return(0);}value[0]=(
KEY(106));
value[0]|=(KEY(107));
if(value[0]){
p429();
if (loop) return (0);
}value[0]=(
KEY(118));
if(value[0]){
p45();
if (loop) return (0);
}value[0]=(
KEY(98));
if(value[0]){
say(64,843,0);
 if (loop) return(0);}value[0]=(
KEY(73));
if(value[0]){
value[0]=(value[HERE]==225);
if(value[0]){
say(64,1604,0);
 if (loop) return(0);}
say(64,1605,0);
 if (loop) return(0);}value[0]=(
KEY(93));
if(value[0]){
say(64,1260,0);
 if (loop) return(0);}
return 0;
}
int p423(void)
{
if (!anyof(4,63,35,-1)) return 0;
value[701]=value[value[672]];*bitword(701)= *bitword(672);value[0]=(value[701]==1);
if(value[0]){
say(76,1133,672);
 if (loop) return(0);}value[0]=(
KEY(35));
value[0]|=(have(98,-1,-1));
if(!value[0]){
value[0]=(KEY(4));
if(value[0]){
say(76,1704,671);
 if (loop) return(0);}
say(64,821,0);
 if (loop) return(0);}
return 0;
}
int p424(void)
{
if (!KEY(4)) return 0;
value[0]=(value[718]>1);
value[0]|=(bitest(evar(712),2));
if(value[0]){
say(0,824,0);
value[0]=(value[718]>1);
if(value[0]){
bitmod('s',evar(712),3);
value[0]=(bitest(4,13));
if(!value[0]){
bitmod('s',4,13);
say(64,935,0);
 if (loop) return(0);}}
loop=1; return(0);
}
set('E',4,'c',1,NULL,NULL);
bitmod('c',144,8);
set('V',701,'c',1,NULL,NULL);
value[0]=(KEY(535));
if(value[0]){
value[701] += 1;
}
say(76,823,701);
 if (loop) return(0);return 0;
}
int p425(void)
{
if (!KEY(63)) return 0;
value[0]=(value[38]==0);
if(value[0]){
say(64,828,0);
 if (loop) return(0);}
set('E',63,'c',1,NULL,NULL);
set('E',38,'c',2,NULL,NULL);
say(64,981,0);
 if (loop) return(0);return 0;
}
int p426(void)
{
if (!KEY(6)) return 0;
if (!isnear(6,-1,-1)) return 0;
value[0]=(value[6]==0);
if(value[0]){
say(64,917,0);
 if (loop) return(0);}
say(66,720,0);
 if (loop) return(0);return 0;
}
int p427(void)
{
if (!KEY(35)) return 0;
if (!isnear(35,-1,-1)) return 0;
value[0]=(value[35]==0);
if(value[0]){
say(64,1132,0);
 if (loop) return(0);}value[0]=(
value[35]==1);
if(value[0]){
say(76,1133,672);
 if (loop) return(0);}
say(64,1134,0);
 if (loop) return(0);return 0;
}
int p428(void)
{
if (!KEY(114)) return 0;
if (!isnear(114,-1,-1)) return 0;
value[0]=(value[114]<2);
if(value[0]){
set('E',114,'c',2,NULL,NULL);
value[0]=(getloc(114)==259);
if(value[0]){
apport(27,259);
say(64,1158,0);
 if (loop) return(0);}
say(64,1159,0);
 if (loop) return(0);}
say(76,1133,672);
 if (loop) return(0);return 0;
}
int p88(void)
{
value[0]=(KEY(613));
value[0]&=(bitest(evar(712),4));
if(value[0]){
p90();
if (loop) return (0);
}
default_to(0,value[HERE],9);
value[0]=(value[699]==2);
value[0]&=!(isnear(value[672],-1,-1));
if(value[0]){
say(76,819,672);
 if (loop) return(0);}
if (!anyof(111,6,100,114,-1)) return 0;
value[0]=(KEY(111));
if(value[0]){
say(64,1472,0);
 if (loop) return(0);}value[0]=(
KEY(6));
if(value[0]){
say(64,822,0);
 if (loop) return(0);}value[0]=(
KEY(100));
if(value[0]){
say(66,720,0);
 if (loop) return(0);}value[0]=(
KEY(114));
if(value[0]){
value[0]=(value[114]<2);
if(value[0]){
say(76,1135,672);
 if (loop) return(0);}
say(64,1609,0);
 if (loop) return(0);}
return 0;
}
int p89(void)
{
if (!anyof(4,63,35,-1)) return 0;
value[701]=value[value[672]];*bitword(701)= *bitword(672);value[0]=!(value[701]==1);
if(value[0]){
say(76,1135,672);
 if (loop) return(0);}
value[value[672]]=0;
value[0]=(KEY(4));
if(value[0]){
say(72,823,0);
 if (loop) return(0);}value[0]=(
KEY(35));
if(value[0]){
say(64,1136,0);
 if (loop) return(0);}value[0]=(
value[HERE]==299);
if(value[0]){
value[0]=(have(63,-1,-1));
if(value[0]){
apport(63,HERE);
}value[0]=(
isnear(38,-1,-1));
if(value[0]){
apport(38,HERE);
set('E',38,'c',1,NULL,NULL);
say(64,983,0);
 if (loop) return(0);}
set('E',63,'c',2,NULL,NULL);
say(64,982,0);
 if (loop) return(0);}
say(64,984,0);
 if (loop) return(0);return 0;
}
int p252(void)
{
value[0]=(value[HERE]==461);
if(value[0]){
say(9,1463,20);
value[0]=(query(1464)); if (loop>1) return (0);
if(value[0]){
value[709] += 20;
move(136,-value[687]); if (loop) return(0);
}
loop=1; return(0);
}value[0]=(
168<=value[673] && value[673]<=193);
value[0]|=(300<=value[673] && value[673]<=311);
value[0]|=(328<=value[673] && value[673]<=364);
value[0]&=!(bitest(evar(673),8));
if(value[0]){
p237();
if (loop) return (0);
value[0]=(value[710]>99);
if(value[0]){
say(64,1655,0);
 if (loop) return(0);}
say(9,1654,20);
value[0]=(query(1464)); if (loop>1) return (0);
if(value[0]){
value[709] += 20;
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],1,5));
if(value[0]){
apport(value[677],HERE);
}}value[0]=(
have(80,-1,-1));
if(value[0]){
apport(80,HERE);
}
say(2,687,0);
value[0]=(168<=value[673] && value[673]<=193);
if(value[0]){
move(167,-2); if (loop) return(0);
}value[0]=(
300<=value[673] && value[673]<=311);
if(value[0]){
move(195,-2); if (loop) return(0);
}value[0]=(
328<=value[673] && value[673]<=364);
if(value[0]){
move(327,-2); if (loop) return(0);
}}
loop=1; return(0);
}value[0]=(
value[721]==0);
if(value[0]){
set('V',721,'c',1,NULL,NULL);
}value[0]=(
value[HERE]==427);
value[0]|=(value[HERE]==428);
value[0]&=!(bitest(evar(673),14));
if(value[0]){
say(64,842,0);
 if (loop) return(0);}
p257();
if (loop) return (0);
value[0]=(value[721]>0);
if(value[0]){
value[0]=(isnear(134,-1,-1));
value[0]|=(isnear(20,-1,-1));
value[0]|=(isnear(22,-1,-1));
value[0]|=(isnear(24,-1,-1));
value[0]|=(value[17]>0);
value[0]|=(isnear(32,-1,-1));
value[0]|=(value[121]>0);
value[0]|=(isnear(25,-1,-1));
value[0]|=(isnear(87,-1,-1));
value[0]|=(isnear(35,-1,-1));
value[0]|=(isnear(71,-1,-1));
if(value[0]){
say(0,841,0);
}else{
value[0]=(bitest(evar(736),3));
if(value[0]){
say(0,842,0);
}else{
bitmod('s',evar(736),3);
value[702]=839;*bitword(702)= -1;
value[703]=840;*bitword(703)= -1;
p312();
if (loop) return (0);
}}}
loop=1; return(0);
return 0;
}
int p304(void)
{
value[702]=941;*bitword(702)= -1;
value[703]=944;*bitword(703)= -1;
p312();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p465(void)
{
value[0]=(query(946)); if (loop>1) return (0);
if(value[0]){
set('V',719,'c',1,NULL,NULL);
p190();
if (loop) return (0);
}
say(66,720,0);
 if (loop) return(0);return 0;
}
int p406(void)
{
bitmod('s',evar(699),1);
bitmod('c',evar(699),2);
value[720]=778;*bitword(720)= -1;
say(64,967,0);
 if (loop) return(0);return 0;
}
int p648(void)
{
value[0]=(bitest(evar(712),4));
if(value[0]){
bitmod('c',evar(712),4);
say(66,720,0);
 if (loop) return(0);}value[0]=(
query(1084)); if (loop>1) return (0);
if(value[0]){
input(1085);
if (loop) return(0);
value[709] += 10;
say(64,1086,0);
 if (loop) return(0);}
say(66,720,0);
 if (loop) return(0);return 0;
}
int p194(void)
{
value[0]=(bitest(evar(712),4));
if(value[0]){
value[0]=(value[699]==1);
if(value[0]){
bitmod('s',evar(699),5);
say(76,830,671);
 if (loop) return(0);}value[0]=(
bitest(evar(672),1));
if(value[0]){
move(value[672],-value[687]); if (loop) return(0);
}
say(64,916,0);
 if (loop) return(0);}value[0]=(
value[699]==1);
if(value[0]){
say(64,1331,0);
 if (loop) return(0);}value[0]=(
isnear(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(60));
if(value[0]){
say(64,1559,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p259(void)
{
value[0]=(value[699]==2);
if(value[0]){
value[0]=(bitest(evar(672),8));
if(value[0]){
(*procs[548])();
if (loop) return (0);
}else{
value[0]=(KEY(26));
value[0]&=(isnear(26,-1,-1));
if(value[0]){
value[0]=(isnear(28,-1,-1));
if(value[0]){
say(64,1170,0);
 if (loop) return(0);}
apport(28,value[673]);
say(64,1171,0);
 if (loop) return(0);}value[0]=(
KEY(10));
if(value[0]){
(*procs[563])();
if (loop) return (0);
}}}
return 0;
}
int p314(void)
{
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
value[0]=(value[699]==1);
value[0]&=(isnear(85,-1,-1));
value[0]|=(KEY(85));
if(value[0]){
say(64,1309,0);
 if (loop) return(0);}
say(64,1082,0);
 if (loop) return(0);}value[0]=(
KEY(134));
value[0]&=(value[718]==3);
value[0]&=(isnear(134,-1,-1));
if(value[0]){
say(0,1388,0);
p93();
if (loop) return (0);
}value[0]=(
KEY(34));
if(value[0]){
(*procs[550])();
if (loop) return (0);
}
default_to(0,value[HERE],8);
value[0]=(value[699]==1);
if(value[0]){
value[0]=(value[672]==-2);
if(value[0]){
say(64,831,0);
 if (loop) return(0);}
return 0;
}
if (!anyof(22,134,20,7,101,38,107,106,24,17,
   27,121,25,135,82,87,91,28,-1)) return 0;
if (!isnear(value[672],-1,-1)) return 0;
value[0]=(KEY(22));
if(value[0]){
say(64,968,0);
 if (loop) return(0);}value[0]=(
KEY(134));
if(value[0]){
p317();
if (loop) return (0);
}else{
value[0]=(KEY(20));
if(value[0]){
p316();
if (loop) return (0);
}else{
value[0]=(KEY(7));
if(value[0]){
say(64,833,0);
 if (loop) return(0);}value[0]=(
KEY(17));
if(value[0]){
say(64,1144,0);
 if (loop) return(0);}value[0]=(
KEY(38));
if(value[0]){
say(76,976,38);
 if (loop) return(0);}value[0]=(
KEY(106));
value[0]|=(KEY(107));
if(value[0]){
say(64,954,0);
 if (loop) return(0);}value[0]=(
KEY(24));
if(value[0]){
p318();
if (loop) return (0);
}else{
value[0]=(KEY(135));
if(value[0]){
value[0]=(query(837)); if (loop>1) return (0);
if(value[0]){
say(64,1323,0);
 if (loop) return(0);}
say(12,1228,671);
say(76,1229,672);
 if (loop) return(0);}value[0]=(
KEY(27));
if(value[0]){
say(64,1155,0);
 if (loop) return(0);}value[0]=(
KEY(121));
if(value[0]){
say(0,1161,0);
p93();
if (loop) return (0);
}value[0]=(
KEY(25));
if(value[0]){
value[0]=(value[25]<2);
if(value[0]){
say(0,1151,0);
p93();
if (loop) return (0);
}
say(64,977,0);
 if (loop) return(0);}value[0]=(
KEY(28));
if(value[0]){
say(64,1653,0);
 if (loop) return(0);}value[0]=(
KEY(82));
if(value[0]){
say(64,1283,0);
 if (loop) return(0);}value[0]=(
KEY(87));
if(value[0]){
say(64,1310,0);
 if (loop) return(0);}value[0]=(
KEY(91));
if(value[0]){
p467();
if (loop) return (0);
}else{
value[0]=(KEY(101));
if(value[0]){
p315();
if (loop) return (0);
}}}}}
loop=1; return(0);
return 0;
}
int p570(void)
{
bitmod('c',evar(699),1);
bitmod('s',evar(699),2);
say(2,720,0);
value[720]=776;*bitword(720)= -1;
loop=1; return(0);
return 0;
}
int p610(void)
{
bitmod('c',evar(699),1);
bitmod('c',evar(699),2);
value[720]=778;*bitword(720)= -1;
say(2,720,0);
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(!value[0]){
say(0,777,0);
bitmod('s',evar(699),3);
p120();
if (loop) return (0);
bitmod('c',evar(699),3);
}
loop=1; return(0);
return 0;
}
int p178(void)
{
if (!anyof(38,22,101,7,134,20,25,
   121,82,-1)) return 0;
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(38));
if(value[0]){
value[0]=(isnear(110,-1,-1));
if(value[0]){
set('E',38,'c',1,NULL,NULL);
value[0]=(have(110,-1,-1));
if(value[0]){
}
apport(110,484);
value[0]=(value[115]==1);
if(value[0]){
set('E',115,'c',0,NULL,NULL);
}
say(64,978,0);
 if (loop) return(0);}
say(64,908,0);
 if (loop) return(0);}value[0]=(
KEY(22));
if(value[0]){
say(64,994,0);
 if (loop) return(0);}value[0]=(
KEY(7));
if(value[0]){
value[0]=(have(101,-1,-1));
if(value[0]){
p653();
if (loop) return (0);
say(64,907,0);
 if (loop) return(0);}
say(64,908,0);
 if (loop) return(0);}value[0]=(
KEY(134));
if(value[0]){
value[0]=(value[718]>1);
if(value[0]){
say(64,916,0);
 if (loop) return(0);}
bitmod('s',134,14);
say(64,909,0);
 if (loop) return(0);}value[0]=(
KEY(101));
if(value[0]){
say(64,906,0);
 if (loop) return(0);}value[0]=(
KEY(20));
if(value[0]){
value[0]=(value[20]==0);
if(value[0]){
say(64,908,0);
 if (loop) return(0);}
say(64,977,0);
 if (loop) return(0);}value[0]=(
KEY(25));
if(value[0]){
value[0]=(value[25]<2);
if(value[0]){
say(64,908,0);
 if (loop) return(0);}
say(64,977,0);
 if (loop) return(0);}value[0]=(
KEY(121));
if(value[0]){
say(64,1156,0);
 if (loop) return(0);}value[0]=(
KEY(82));
if(value[0]){
value[0]=(isnear(110,-1,-1));
if(value[0]){
apport(110,484);
apport(84,value[673]);
say(64,1285,0);
 if (loop) return(0);}
say(64,1284,0);
 if (loop) return(0);}}
say(76,819,672);
 if (loop) return(0);return 0;
}
int p179(void)
{
value[0]=(value[699]==1);
if(!value[0]){
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p500(void)
{
set('V',719,'c',1,NULL,NULL);
p237();
if (loop) return (0);
say(13,1033,710);
say(13,1034,711);
say(13,1035,713);
loop=1; return(0);
return 0;
}
int p311(void)
{
say(64,1036,0);
 if (loop) return(0);return 0;
}
int p300(void)
{
say(64,801,0);
 if (loop) return(0);return 0;
}
int p431(void)
{
say(64,801,0);
 if (loop) return(0);return 0;
}
int p223(void)
{
say(64,1482,0);
 if (loop) return(0);return 0;
}
int p512(void)
{
say(64,838,0);
 if (loop) return(0);return 0;
}
int p175(void)
{
value[0]=(value[718]>1);
if(value[0]){
say(64,829,0);
 if (loop) return(0);}
if (!anyof(509,508,507,578,-1)) return 0;
set('V',715,'c',0,NULL,NULL);
say(64,805,0);
 if (loop) return(0);return 0;
}
int p176(void)
{
set('V',715,'c',1,NULL,NULL);
say(66,720,0);
 if (loop) return(0);return 0;
}
int p180(void)
{
value[0]=(value[715]==0);
if(value[0]){
set('V',715,'c',2,NULL,NULL);
say(66,720,0);
 if (loop) return(0);}
p199();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p195(void)
{
value[0]=(value[715]==1);
if(value[0]){
set('V',715,'c',3,NULL,NULL);
say(66,720,0);
 if (loop) return(0);}
p199();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p198(void)
{
value[0]=(value[699]==2);
value[0]&=!(KEY(493));
if(value[0]){
say(64,859,0);
 if (loop) return(0);}value[0]=(
value[715]==2);
if(value[0]){
value[0]=(getloc(50)==222);
value[0]|=(bitest(50,14));
if(value[0]){
say(0,829,0);
value[0]=(getloc(50)==222);
if(value[0]){
say(0,1080,0);
}
}else{
value[0]=(isnear(50,-1,-1));
if(value[0]){
set('E',50,'c',1,NULL,NULL);
}else{
value[0]=(value[HERE]==222);
if(value[0]){
set('E',50,'c',0,NULL,NULL);
}else{
set('E',50,'c',2,NULL,NULL);
}}value[0]=(
have(50,-1,-1));
if(value[0]){
apport(50,HERE);
}
say(0,50,0);
set('E',50,'c',0,NULL,NULL);
value[0]=(bitest(50,13));
if(value[0]){
bitmod('c',50,13);
bitmod('s',22,13);
value[0]=(value[22]==1);
value[0]|=(value[22]==2);
if(value[0]){
value[0]=(isnear(23,-1,-1));
if(value[0]){
say(0,1455,0);
}
set('E',22,'c',0,NULL,NULL);
apport(22,244);
apport(23,484);
}}
apport(50,222);
value[0]=(value[135]==2);
if(value[0]){
set('E',135,'c',3,NULL,NULL);
value[758] = irand(5-3+1)+3;
}}
set('V',715,'c',0,NULL,NULL);
}else{
p199();
if (loop) return (0);
}
loop=1; return(0);
return 0;
}
int p225(void)
{
say(64,1475,0);
 if (loop) return(0);return 0;
}
int p650(void)
{
say(64,829,0);
 if (loop) return(0);return 0;
}
int p459(void)
{
say(64,829,0);
 if (loop) return(0);return 0;
}
int p505(void)
{
say(64,1473,0);
 if (loop) return(0);return 0;
}
int p188(void)
{
if (!KEY(613)) return 0;
value[0]=(bitest(evar(673),7));
value[0]&=!(bitest(145,4));
if(value[0]){
say(64,845,0);
 if (loop) return(0);}
say(64,846,0);
 if (loop) return(0);return 0;
}
int p189(void)
{
value[0]=(value[699]>1);
if(value[0]){
value[0]=(bitest(evar(672),0));
if(value[0]){
value[0]=(bitest(evar(712),4));
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(value[0]){
say(76,814,672);
 if (loop) return(0);}
value[678]=getloc(value[672]);*bitword(678)= -1;
move(value[678],-2); if (loop) return(0);
}value[0]=(
isnear(value[672],-1,-1));
if(value[0]){
value[0]=(have(value[672],-1,-1));
value[0]|=(KEY(16));
if(value[0]){
say(64,901,0);
 if (loop) return(0);}
say(64,1407,0);
 if (loop) return(0);}
say(64,847,0);
 if (loop) return(0);}value[0]=(
bitest(evar(672),1));
if(value[0]){
value[0]=(value[HERE]==value[672]);
if(value[0]){
say(64,939,0);
 if (loop) return(0);}
say(64,847,0);
 if (loop) return(0);}
say(64,859,0);
 if (loop) return(0);}
return 0;
}
int p560(void)
{
say(64,950,0);
 if (loop) return(0);return 0;
}
int p42(void)
{
if (!KEY(53)) return 0;
if (!isnear(53,-1,-1)) return 0;
apport(53,485);
apport(54,value[673]);
say(64,1011,0);
 if (loop) return(0);return 0;
}
int p43(void)
{
if (!KEY(118)) return 0;
if (!isnear(118,-1,-1)) return 0;
p45();
if (loop) return (0);
return 0;
}
int p44(void)
{
if (!KEY(10)) return 0;
if (!isnear(10,-1,-1)) return 0;
value[0]=(value[718]==3);
if(value[0]){
say(0,1378,0);
p93();
if (loop) return (0);
}
say(64,1386,0);
 if (loop) return(0);return 0;
}
int p192(void)
{
if (!KEY(53)) return 0;
if (!isnear(54,-1,-1)) return 0;
say(64,949,0);
 if (loop) return(0);return 0;
}
int p193(void)
{
if (!KEY(41)) return 0;
value[0]=(isnear(41,-1,-1));
if(value[0]){
say(64,949,0);
 if (loop) return(0);}
return 0;
}
int p182(void)
{
if (!KEY(53)) return 0;
value[0]=(isnear(53,-1,-1));
if(value[0]){
value[0]=(bitest(evar(673),9));
value[0]|=(value[HERE]==158);
if(value[0]){
value[0]=(have(53,-1,-1));
if(value[0]){
value[707] -= 1;
}
apport(53,485);
apport(54,value[673]);
say(64,948,0);
 if (loop) return(0);}
say(64,947,0);
 if (loop) return(0);}
return 0;
}
int p183(void)
{
if (!KEY(114)) return 0;
value[0]=(have(114,-1,-1));
if(value[0]){
value[0]=(value[114]<2);
if(value[0]){
say(64,1377,0);
 if (loop) return(0);}value[0]=(
value[114]==2);
if(value[0]){
value[0]=(bitest(evar(673),9));
if(value[0]){
set('E',114,'c',3,NULL,NULL);
say(76,914,112);
 if (loop) return(0);}value[0]=(
value[HERE]==158);
if(value[0]){
set('E',114,'c',4,NULL,NULL);
say(76,914,113);
 if (loop) return(0);}value[0]=(
bitest(evar(673),13));
if(value[0]){
say(64,875,0);
 if (loop) return(0);}
say(76,912,114);
 if (loop) return(0);}
say(64,1376,0);
 if (loop) return(0);}
say(76,1026,672);
 if (loop) return(0);return 0;
}
int p184(void)
{
if (!KEY(111)) return 0;
value[0]=(have(111,-1,-1));
if(value[0]){
value[0]=(value[111]==2);
if(value[0]){
value[0]=(bitest(evar(673),9));
if(value[0]){
set('E',111,'c',3,NULL,NULL);
say(76,913,112);
 if (loop) return(0);}value[0]=(
value[HERE]==158);
if(value[0]){
set('E',111,'c',4,NULL,NULL);
say(76,913,113);
 if (loop) return(0);}
say(76,912,111);
 if (loop) return(0);}
say(64,911,0);
 if (loop) return(0);}
say(76,1026,672);
 if (loop) return(0);return 0;
}
int p185(void)
{
if (!KEY(81)) return 0;
value[0]=(have(81,-1,-1));
if(value[0]){
value[0]=(bitest(evar(673),9));
if(value[0]){
value[0]=(value[HERE]==412);
if(value[0]){
say(64,1274,0);
 if (loop) return(0);}
bitmod('s',81,13);
bitmod('s',81,14);
say(64,1275,0);
 if (loop) return(0);}
say(76,912,81);
 if (loop) return(0);}
say(76,1026,672);
 if (loop) return(0);return 0;
}
int p186(void)
{
value[0]=(value[699]>1);
value[0]&=(isnear(value[672],-1,-1));
if(value[0]){
say(64,915,0);
 if (loop) return(0);}
return 0;
}
int p158(void)
{
if (!anyof(111,114,81,118,-1)) return 0;
value[0]=(have(value[672],-1,-1));
if(!value[0]){
say(76,1026,672);
 if (loop) return(0);}
return 0;
}
int p159(void)
{
if (!KEY(81)) return 0;
value[0]=(bitest(81,13));
if(value[0]){
say(64,1328,0);
 if (loop) return(0);}
say(64,1276,0);
 if (loop) return(0);return 0;
}
int p160(void)
{
if (!KEY(111)) return 0;
value[0]=(isnear(111,0,4));
value[0]|=(isnear(111,0,3));
if(value[0]){
value[672]=112;*bitword(672)= -1;
value[0]=(isnear(111,0,4));
if(value[0]){
value[672]=113;*bitword(672)= -1;
}
(*procs[552])();
if (loop) return (0);
}else{
say(0,1276,0);
}
loop=1; return(0);
return 0;
}
int p161(void)
{
if (!KEY(114)) return 0;
value[0]=(value[114]<2);
if(value[0]){
(*procs[535])();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
value[114]==2);
if(value[0]){
say(64,1276,0);
 if (loop) return(0);}value[0]=(
value[114]==3);
if(value[0]){
value[672]=112;*bitword(672)= -1;
}else{
value[672]=113;*bitword(672)= -1;
}value[0]=(
have(value[672],-1,-1));
if(value[0]){
apport(value[672],435);
}
(*procs[552])();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p162(void)
{
value[0]=(KEY(118));
if(value[0]){
say(64,802,0);
 if (loop) return(0);}value[0]=(
value[699]==2);
if(value[0]){
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p461(void)
{
value[0]=(value[672]==112);
if(value[0]){
value[0]=(isnear(111,0,3));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
value[0]=(isnear(11,-1,-1));
if(value[0]){
p623();
if (loop) return (0);
}else{
value[0]=(isnear(6,-1,-1));
if(value[0]){
say(0,919,0);
set('E',6,'c',0,NULL,NULL);
}else{
say(12,876,112);
}
bitmod('s',evar(673),15);
}
}else{
value[0]=(isnear(114,0,3));
if(value[0]){
set('E',114,'c',2,NULL,NULL);
say(0,1374,0);
}else{
say(12,1026,112);
}}
loop=1; return(0);
}value[0]=(
value[672]==113);
if(value[0]){
value[0]=(isnear(114,0,4));
value[0]|=(isnear(111,0,4));
if(!value[0]){
say(76,1026,113);
 if (loop) return(0);}value[0]=(
isnear(111,0,4));
if(value[0]){
set('E',111,'c',2,NULL,NULL);
set('V',702,'c',1,NULL,NULL);
}else{
set('E',114,'c',2,NULL,NULL);
set('V',702,'c',2,NULL,NULL);
}value[0]=(
isnear(11,-1,-1));
if(value[0]){
say(0,918,0);
}else{
value[0]=(isnear(6,-1,-1));
if(value[0]){
say(0,920,0);
set('E',6,'c',1,NULL,NULL);
}else{
value[0]=(value[702]==1);
if(value[0]){
say(12,876,112);
bitmod('s',evar(673),15);
}else{
say(0,1374,0);
}}}
loop=1; return(0);
}value[0]=(
value[699]>1);
if(value[0]){
say(64,878,0);
 if (loop) return(0);}
return 0;
}
int p452(void)
{
if (!anyof(134,7,101,20,22,38,19,24,25,
   121,82,-1)) return 0;
if (!isnear(value[672],-1,-1)) return 0;
say(64,804,0);
 if (loop) return(0);return 0;
}
int p155(void)
{
value[0]=(isnear(110,-1,-1));
if(value[0]){
value[0]=(KEY(110));
value[0]|=(value[699]==1);
if(value[0]){
apport(110,484);
say(64,869,0);
 if (loop) return(0);}}value[0]=(
isnear(119,-1,-1));
if(value[0]){
value[0]=(KEY(119));
value[0]|=(value[699]==1);
if(value[0]){
set('V',723,'c',30,NULL,NULL);
value[723] += value[725];
if (*bitword(725*VARSIZE)==-1) *bitword(723*VARSIZE)= -1;
set('V',722,'c',12,NULL,NULL);
bitmod('c',434,8);
set('E',119,'c',2,NULL,NULL);
apport(119,485);
say(64,119,0);
 if (loop) return(0);}}value[0]=(
value[699]==1);
if(value[0]){
say(64,985,0);
 if (loop) return(0);}
if (!anyof(134,20,101,7,38,22,11,24,
   25,121,-1)) return 0;
say(64,868,0);
 if (loop) return(0);return 0;
}
int p156(void)
{
value[0]=(bitest(evar(672),0));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
say(64,875,0);
 if (loop) return(0);}}
return 0;
}
int p487(void)
{
if (!KEY(99)) return 0;
if (!isnear(99,-1,-1)) return 0;
say(76,873,672);
 if (loop) return(0);return 0;
}
int p488(void)
{
if (!KEY(89)) return 0;
if (!isnear(89,-1,-1)) return 0;
say(76,1699,89);
 if (loop) return(0);return 0;
}
int p489(void)
{
if (!KEY(103)) return 0;
value[0]=(isnear(103,-1,-1));
value[0]&=(value[103]<2);
if(value[0]){
value[0]=(have(123,-1,-1));
if(value[0]){
set('E',103,'c',2,NULL,NULL);
set('E',123,'c',2,NULL,NULL);
say(76,1369,671);
 if (loop) return(0);}
say(76,1555,671);
 if (loop) return(0);}
return 0;
}
int p490(void)
{
if (!KEY(259)) return 0;
value[0]=(value[HERE]==259);
if(value[0]){
say(64,874,0);
 if (loop) return(0);}
say(76,819,259);
 if (loop) return(0);return 0;
}
int p491(void)
{
value[0]=(value[699]>1);
if(value[0]){
value[0]=(bitest(evar(672),0));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
value[0]=(bitest(evar(672),8));
value[0]&=!(KEY(106));
value[0]&=!(KEY(107));
value[0]&=!(KEY(82));
if(value[0]){
say(64,916,0);
 if (loop) return(0);}
say(64,874,0);
 if (loop) return(0);}}}
return 0;
}
int p17(void)
{
value[0]=(bitest(evar(673),6));
value[0]|=(bitest(evar(674),6));
value[0]|=(value[674]==0);
if(value[0]){
say(64,897,0);
 if (loop) return(0);}
move(value[674],-2); if (loop) return(0);
return 0;
}
int p391(void)
{
value[0]=(value[HERE]==243);
if(value[0]){
say(64,1602,0);
 if (loop) return(0);}value[0]=(
value[HERE]==151);
value[0]|=(value[HERE]==150);
value[0]|=(value[HERE]==152);
value[0]|=(value[HERE]==165);
value[0]|=(value[HERE]==166);
value[0]|=(value[HERE]==230);
value[0]|=(value[HERE]==237);
value[0]|=(value[HERE]==239);
value[0]|=(value[HERE]==244);
value[0]|=(value[HERE]==431);
value[0]|=(value[HERE]==432);
value[0]|=(value[HERE]==434);
value[0]|=(isnear(21,-1,-1));
if(value[0]){
say(64,867,0);
 if (loop) return(0);}
say(72,819,581);
 if (loop) return(0);return 0;
}
int p106(void)
{
value[0]=(value[HERE]==225);
value[0]|=(value[HERE]==392);
value[0]|=(value[HERE]==429);
if(value[0]){
say(64,1240,0);
 if (loop) return(0);}
say(76,819,73);
 if (loop) return(0);return 0;
}
int p128(void)
{
value[0]=(value[HERE]==408);
if(value[0]){
say(64,1442,0);
 if (loop) return(0);}value[0]=(
isnear(18,-1,-1));
if(value[0]){
say(64,781,0);
 if (loop) return(0);}
say(76,819,18);
 if (loop) return(0);return 0;
}
int p196(void)
{
value[0]=(value[HERE]==255);
value[0]|=(value[HERE]==256);
value[0]|=(value[HERE]==257);
if(value[0]){
say(64,1477,0);
 if (loop) return(0);}
say(76,819,14);
 if (loop) return(0);return 0;
}
int p497(void)
{
if (!anyof(501,497,498,504,503,500,
   505,496,506,499,511,502,576,
   510,509,508,507,578,-1)) return 0;
if (value[672]<LPROC && value[672]>= 0) (*procs[value[672]])(); else pcall(value[672]);
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p498(void)
{
value[0]=(value[699]==2);
if(value[0]){
loop=1; return(0);
}
return 0;
}
int p495(void)
{
say(9,1054,10);
say(9,1055,5);
value[0]=(query(1012)); if (loop>1) return (0);
if(!value[0]){
say(64,1069,0);
 if (loop) return(0);}value[0]=(
value[701]==0);
if(value[0]){
}
special(1,&value[701]);
value[0]=(value[701]==0);
if(value[0]){
value[0]=(query(1060)); if (loop>1) return (0);
if(value[0]){
say(66,720,0);
 if (loop) return(0);}
say(0,777,0);
value[679] = irand(1068-1063+1)+1063;
*bitword(679)= -1;
say(2,679,0);
say(0,777,0);
finita();
loop=1; return(0);
}else{
say(64,1056,0);
 if (loop) return(0);}
return 0;
}
int p480(void)
{
value[0]=(bitest(evar(712),4));
if(value[0]){
set('V',701,'c',1,NULL,NULL);
}
special(6,&value[701]);
special(2,&value[701]);
value[0]=(value[701]==0);
if(value[0]){
special(7,&value[702]);
value[0]=(value[702]==1);
if(value[0]){
bitmod('s',evar(712),4);
}else{
bitmod('c',evar(712),4);
}
special(8,&value[701]);
value[0]=(value[701]<10);
if(value[0]){
say(9,1061,10);
say(9,1062,5);
value[709] += 10;
}value[0]=(
value[99]==1);
if(value[0]){
value[700] += 1;
}value[0]=(
query(1059)); if (loop>1) return (0);
if(value[0]){
say(2,720,0);
}else{
value[679] = irand(1073-1070+1)+1070;
*bitword(679)= -1;
say(2,679,0);
special(3,&value[701]);
}
move(value[673],-1);
say(0,777,0);
}else{
value[0]=(value[701]==1);
if(value[0]){
say(0,1057,0);
value[713] -= 1;
}else{
value[0]=(query(1058)); if (loop>1) return (0);
if(value[0]){
special(3,&value[701]);
}
say(2,720,0);
}}
loop=1; return(0);
return 0;
}
int p129(void)
{
value[0]=(KEY(81));
value[0]&=(isnear(81,-1,-1));
if(value[0]){
value[0]=(bitest(81,13));
if(value[0]){
p79();
if (loop) return (0);
}
say(64,1276,0);
 if (loop) return(0);}value[0]=(
value[699]==1);
value[0]&=(isnear(81,1,13));
if(value[0]){
p79();
if (loop) return (0);
}value[0]=(
KEY(113));
if(value[0]){
say(64,815,0);
 if (loop) return(0);}value[0]=(
value[699]==1);
value[0]|=(KEY(112));
value[0]|=(KEY(111));
value[0]|=(KEY(114));
if(value[0]){
value[0]=(bitest(evar(673),9));
value[0]&=!(KEY(111));
value[0]&=!(KEY(114));
if(value[0]){
value[701]=870;*bitword(701)= -1;
set('V',702,'c',900,NULL,NULL);
p622();
if (loop) return (0);
}value[0]=(
KEY(111));
value[0]|=(KEY(114));
if(value[0]){
value[0]=(have(value[672],-1,-1));
if(!value[0]){
value[0]=(ishere(value[672],-1,-1));
if(value[0]){
say(76,1705,672);
 if (loop) return(0);}
say(76,819,672);
 if (loop) return(0);}
value[701]=value[value[672]];*bitword(701)= *bitword(672);value[0]=(value[701]==4);
if(value[0]){
say(64,1707,0);
 if (loop) return(0);}value[0]=(
KEY(114));
value[0]&=(value[114]<2);
if(value[0]){
say(64,1708,0);
 if (loop) return(0);}value[0]=(
value[701]==2);
if(value[0]){
say(76,1706,672);
 if (loop) return(0);}}value[0]=(
isnear(111,0,3));
value[0]&=!(KEY(114));
if(value[0]){
value[701]=872;*bitword(701)= -1;
set('E',111,'c',2,NULL,NULL);
set('V',702,'c',600,NULL,NULL);
p622();
if (loop) return (0);
}value[0]=(
isnear(114,0,3));
if(value[0]){
value[701]=1374;*bitword(701)= -1;
set('E',114,'c',2,NULL,NULL);
set('V',702,'c',400,NULL,NULL);
p622();
if (loop) return (0);
}value[0]=(
KEY(112));
if(value[0]){
say(76,819,672);
 if (loop) return(0);}
say(64,1076,0);
 if (loop) return(0);}value[0]=(
bitest(evar(672),0));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(118));
if(value[0]){
say(64,1429,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}
say(76,819,672);
 if (loop) return(0);}
say(64,815,0);
 if (loop) return(0);return 0;
}
int p258(void)
{
say(64,780,0);
 if (loop) return(0);return 0;
}
int p164(void)
{
value[0]=(isnear(89,-1,-1));
if(value[0]){
set('E',89,'c',2,NULL,NULL);
say(64,1262,0);
 if (loop) return(0);}
say(64,1452,0);
 if (loop) return(0);return 0;
}
int p471(void)
{
if (!KEY(120)) return 0;
value[0]=(value[HERE]==137);
value[0]|=(value[HERE]==391);
value[0]|=(value[HERE]==403);
if(value[0]){
say(64,1466,0);
 if (loop) return(0);}value[0]=(
value[HERE]==140);
if(value[0]){
bitmod('s',evar(699),4);
say(0,120,0);
bitmod('c',evar(699),4);
value[0]=(value[120]==0);
if(value[0]){
set('E',120,'c',1,NULL,NULL);
}
loop=1; return(0);
}
return 0;
}
int p472(void)
{
value[0]=(value[699]>1);
if(value[0]){
value[0]=(KEY(88));
if(value[0]){
value[0]=(isnear(86,0,1));
if(value[0]){
say(64,1337,0);
 if (loop) return(0);}value[0]=(
isnear(88,-1,-1));
if(value[0]){
say(64,1322,0);
 if (loop) return(0);}}value[0]=(
KEY(71));
value[0]&=(have(89,-1,-1));
value[0]&=!(isnear(71,-1,-1));
if(value[0]){
say(64,1261,0);
 if (loop) return(0);}value[0]=(
KEY(656));
value[0]&=(isnear(71,-1,-1));
if(value[0]){
p473();
if (loop) return (0);
}value[0]=(
KEY(124));
if(value[0]){
value[0]=(value[HERE]==229);
if(value[0]){
say(64,1010,0);
 if (loop) return(0);}}value[0]=(
bitest(evar(672),0));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(108));
if(value[0]){
say(64,1002,0);
 if (loop) return(0);}value[0]=(
KEY(29));
if(value[0]){
say(64,1003,0);
 if (loop) return(0);}value[0]=(
KEY(89));
if(value[0]){
say(64,1261,0);
 if (loop) return(0);}value[0]=(
KEY(656));
value[0]&=(value[HERE]==434);
value[0]|=(KEY(71));
if(value[0]){
p473();
if (loop) return (0);
}value[0]=(
KEY(70));
if(value[0]){
say(64,1351,0);
 if (loop) return(0);}value[0]=(
KEY(107));
if(value[0]){
p434();
if (loop) return (0);
}value[0]=(
KEY(34));
if(value[0]){
say(64,1004,0);
 if (loop) return(0);}}
}else{
say(64,916,0);
 if (loop) return(0);}}
return 0;
}
int p86(void)
{
value[0]=(value[HERE]==461);
if(value[0]){
say(64,1353,0);
 if (loop) return(0);}value[0]=(
437<=value[673] && value[673]<=461);
if(value[0]){
say(64,1354,0);
 if (loop) return(0);}
say(64,1021,0);
 if (loop) return(0);return 0;
}
int p344(void)
{
say(64,866,0);
 if (loop) return(0);return 0;
}
int p389(void)
{
value[0]=(isnear(31,0,0));
if(value[0]){
set('E',31,'c',1,NULL,NULL);
bitmod('c',254,8);
set('V',734,'c',9,NULL,NULL);
value[0]=(value[135]<4);
if(value[0]){
apport(135,485);
set('E',135,'c',0,NULL,NULL);
}
say(64,1145,0);
 if (loop) return(0);}
say(64,829,0);
 if (loop) return(0);return 0;
}
int p413(void)
{
value[0]=(KEY(500));
if(value[0]){
say(64,805,0);
 if (loop) return(0);}value[0]=(
bitest(evar(712),8));
if(value[0]){
bitmod('c',evar(712),8);
say(64,829,0);
 if (loop) return(0);}value[0]=(
value[718]>2);
value[0]|=!(bitest(324,4));
value[0]|=!(isnear(99,-1,-1));
if(value[0]){
say(64,829,0);
 if (loop) return(0);}
bitmod('s',evar(712),8);
say(66,720,0);
 if (loop) return(0);return 0;
}
int p493(void)
{
value[0]=(bitest(evar(712),8));
if(value[0]){
value[0]=(have(99,-1,-1));
if(value[0]){
say(0,1091,0);
p93();
if (loop) return (0);
}value[0]=(
value[700]>40);
if(value[0]){
apport(99,485);
set('E',39,'c',3,NULL,NULL);
set('E',99,'c',0,NULL,NULL);
set('V',700,'c',0,NULL,NULL);
value[0]=(irand(100)<50);
if(value[0]){
say(0,1092,0);
p308();
if (loop) return (0);
value[0]=(value[701]==1);
if(value[0]){
say(0,806,0);
}
}else{
say(0,1093,0);
p93();
if (loop) return (0);
}
}else{
bitmod('s',evar(756),8);
say(0,1094,0);
value[700] += 300;
set('E',99,'c',1,NULL,NULL);
}
}else{
say(0,829,0);
}
loop=1; return(0);
return 0;
}
int p581(void)
{
say(64,829,0);
 if (loop) return(0);return 0;
}
int p654(void)
{
value[0]=(value[116]==0);
value[0]|=!(isnear(116,-1,-1));
if(value[0]){
say(0,829,0);
}else{
value[0]=(have(116,-1,-1));
if(value[0]){
say(0,1340,0);
}else{
say(0,1341,0);
}value[0]=(
bitest(evar(712),9));
if(value[0]){
say(0,1343,0);
}
apport(116,485);
apport(64,value[673]);
bitmod('s',64,4);
set('V',734,'c',7,NULL,NULL);
}
loop=1; return(0);
return 0;
}
int p321(void)
{
p436();
if (loop) return (0);
return 0;
}
int p320(void)
{
p436();
if (loop) return (0);
return 0;
}
int p528(void)
{
p436();
if (loop) return (0);
return 0;
}
int p34(void)
{
p436();
if (loop) return (0);
return 0;
}
int p481(void)
{
value[0]=(KEY(60));
if(value[0]){
(*procs[568])();
if (loop) return (0);
}value[0]=(
value[699]==1);
value[0]|=(KEY(28));
value[0]&=(isnear(28,-1,-1));
if(value[0]){
apport(28,484);
move(239,-1169); if (loop) return(0);
}value[0]=(
value[699]==1);
value[0]|=(KEY(82));
value[0]&=(isnear(82,-1,-1));
if(value[0]){
value[0]=(value[718]==3);
if(value[0]){
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
if(value[0]){
apport(value[677],485);
}}
set('V',718,'c',4,NULL,NULL);
bitmod('s',243,4);
move(243,-1387); if (loop) return(0);
}
say(0,1280,0);
finita();
loop=1; return(0);
}
return 0;
}
int p446(void)
{
bitmod('s',27,14);
value[0]=(bitest(evar(673),7));
value[0]|=(value[718]>2);
if(value[0]){
say(0,829,0);
}else{
value[0]=(isnear(111,0,3));
value[0]|=(bitest(evar(673),9));
if(value[0]){
value[0]=(irand(100)<85);
if(value[0]){
say(0,829,0);
}else{
value[0]=(irand(100)<95);
if(value[0]){
say(0,1177,0);
p93();
if (loop) return (0);
}
say(0,1178,0);
value[0]=(query(1179)); if (loop>1) return (0);
if(value[0]){
say(0,1180,0);
}
finita();
loop=1; return(0);
}
}else{
value[0]=(isnear(115,-1,-1));
value[0]|=(isnear(116,-1,-1));
if(value[0]){
value[0]=(isnear(115,-1,-1));
if(value[0]){
say(0,1181,0);
apport(115,485);
}value[0]=(
isnear(116,-1,-1));
if(value[0]){
say(0,1182,0);
apport(116,485);
}
}else{
value[701] = irand(3);
value[701] *= 2;
value[0]=(value[134]>1);
if(value[0]){
value[701] += 1;
}value[0]=(
isnear(134,-1,-1));
if(value[0]){
value[0]=(irand(100)<70);
if(value[0]){
value[702]=1183;*bitword(702)= -1;
value[701] += value[702];
if (*bitword(702*VARSIZE)==-1) *bitword(701*VARSIZE)= -1;
say(2,701,0);
set('V',706,'e',134,NULL,NULL);
p319();
if (loop) return (0);
}else{
value[702]=1189;*bitword(702)= -1;
value[701] += value[702];
if (*bitword(702*VARSIZE)==-1) *bitword(701*VARSIZE)= -1;
say(2,701,0);
p93();
if (loop) return (0);
}
}else{
say(0,829,0);
}}}}
loop=1; return(0);
return 0;
}
int p492(void)
{
value[0]=(bitest(evar(712),4));
if(value[0]){
set('V',717,'c',0,NULL,NULL);
move(value[673],-778); if (loop) return(0);
}
say(64,859,0);
 if (loop) return(0);return 0;
}
int p501(void)
{
say(0,1264,0);
say(64,1680,0);
 if (loop) return(0);return 0;
}
int p479(void)
{
value[0]=(value[699]>1);
if(value[0]){
(*procs[490])();
if (loop) return (0);
}value[0]=(
value[HERE]==141);
value[0]|=(value[HERE]==205);
if(value[0]){
say(64,1564,0);
 if (loop) return(0);}
say(64,1265,0);
 if (loop) return(0);return 0;
}
int p522(void)
{
say(64,1266,0);
 if (loop) return(0);return 0;
}
int p260(void)
{
value[0]=(value[699]==1);
if(value[0]){
value[0]=(getloc(85)==485);
if(value[0]){
say(64,1527,0);
 if (loop) return(0);}value[0]=(
isnear(85,-1,-1));
if(value[0]){
say(64,1308,0);
 if (loop) return(0);}value[0]=(
bitest(evar(673),3));
value[0]|=(bitest(evar(673),5));
value[0]|=(bitest(evar(673),7));
if(value[0]){
value[0]=(isnear(87,-1,-1));
value[0]|=(value[HERE]==205);
value[0]|=(isnear(20,-1,-1));
if(value[0]){
set('V',703,'c',0,NULL,NULL);
set('V',702,'c',30,NULL,NULL);
p432();
if (loop) return (0);
}else{
say(0,1527,0);
}
loop=1; return(0);
}
value[703]=getloc(85);*bitword(703)= -1;
value[703] -= value[673];
value[0]=(value[703]<0);
if(value[0]){
value[703] *= -1;
}
value[703] -= 1;
value[703] *= 3;
set('V',702,'v',703,NULL,NULL);
value[702] += value[703];
if (*bitword(703*VARSIZE)==-1) *bitword(702*VARSIZE)= -1;
p432();
if (loop) return (0);
}else{
say(0,859,0);
}
loop=1; return(0);
return 0;
}
int p107(void)
{
value[0]=(value[699]==1);
if(value[0]){
return 0;
}value[0]=(
have(116,-1,-1));
value[0]|=(have(115,-1,-1));
if(!value[0]){
say(64,1332,0);
 if (loop) return(0);}value[0]=(
bitest(evar(672),0));
value[0]|=(KEY(122));
if(!value[0]){
say(64,916,0);
 if (loop) return(0);}value[0]=(
KEY(122));
if(value[0]){
say(64,1352,0);
 if (loop) return(0);}value[0]=(
KEY(75));
value[0]&=(isnear(75,-1,-1));
if(value[0]){
say(64,1333,0);
 if (loop) return(0);}value[0]=(
KEY(11));
if(value[0]){
value[0]=(value[HERE]==159);
if(value[0]){
say(64,1334,0);
 if (loop) return(0);}value[0]=(
value[HERE]==156);
value[0]&=(value[11]>0);
if(value[0]){
say(64,1334,0);
 if (loop) return(0);}}value[0]=(
KEY(86));
if(value[0]){
value[0]=(value[HERE]==388);
if(value[0]){
value[0]=(value[86]==0);
if(value[0]){
say(64,1310,0);
 if (loop) return(0);}value[0]=(
value[86]==1);
if(value[0]){
apport(88,value[673]);
set('E',86,'c',2,NULL,NULL);
say(64,1329,0);
 if (loop) return(0);}
say(64,1335,0);
 if (loop) return(0);}}
return 0;
}
int p35(void)
{
if (!KEY(90)) return 0;
value[0]=(have(90,-1,-1));
if(value[0]){
value[0]=(bitest(evar(673),7));
if(value[0]){
say(64,1345,0);
 if (loop) return(0);}value[0]=(
399<=value[673] && value[673]<=404);
if(value[0]){
set('V',702,'v',673,NULL,NULL);
value[703]=398;*bitword(703)= -1;
value[702] -= value[703];
value[716] += value[702];
if (*bitword(702*VARSIZE)==-1) *bitword(716*VARSIZE)= -1;
value[0]=(value[716]>9);
if(value[0]){
set('V',716,'c',9,NULL,NULL);
}value[0]=(
value[731]==0);
if(value[0]){
set('V',731,'c',1,NULL,NULL);
}}value[0]=(
bitest(evar(673),5));
value[0]|=(value[716]==0);
if(value[0]){
say(64,1347,0);
 if (loop) return(0);}value[0]=(
value[134]<value[716]);
if(value[0]){
set('V',702,'v',716,NULL,NULL);
value[702] -= value[134];
say(13,1346,702);
apport(134,value[673]);
set('E',134,'v',716,NULL,NULL);
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
bitmod('s',134,14);
say(0,134,0);
}
}else{
say(13,1370,134);
bitmod('s',134,14);
set('V',729,'c',0,NULL,NULL);
set('V',728,'c',100,NULL,NULL);
}
}else{
say(12,1026,672);
}
loop=1; return(0);
return 0;
}
int p313(void)
{
value[0]=(KEY(34));
value[0]|=(value[699]==1);
value[0]&=(isnear(34,-1,-1));
if(value[0]){
value[0]=(bitest(39,4));
value[0]&=!(bitest(97,4));
if(value[0]){
bitmod('s',97,4);
apport(97,value[673]);
say(64,1364,0);
 if (loop) return(0);}
say(64,829,0);
 if (loop) return(0);}value[0]=(
value[699]==2);
if(!value[0]){
return 0;
}value[0]=(
bitest(evar(672),2));
if(value[0]){
say(64,916,0);
 if (loop) return(0);}value[0]=(
isnear(value[672],-1,-1));
if(value[0]){
value[0]=(KEY(134));
value[0]&=(isnear(134,-1,-1));
if(value[0]){
value[0]=(value[718]==3);
if(value[0]){
say(0,1380,0);
}else{
say(0,836,0);
}
p93();
if (loop) return (0);
}
say(64,916,0);
 if (loop) return(0);}
return 0;
}
int p476(void)
{
if (!KEY(39)) return 0;
value[0]=(isnear(39,-1,-1));
value[0]&=(value[39]<3);
if(value[0]){
set('V',700,'c',400,NULL,NULL);
set('E',39,'c',3,NULL,NULL);
say(64,1366,0);
 if (loop) return(0);}
say(64,1365,0);
 if (loop) return(0);return 0;
}
int p224(void)
{
value[679] = irand(886-879+1)+879;
*bitword(679)= -1;
say(66,679,0);
 if (loop) return(0);return 0;
}
int p615(void)
{
value[0]=(value[699]==2);
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(value[0]){
value[0]=(value[718]==3);
value[0]&=(KEY(134));
if(value[0]){
say(0,1380,0);
p93();
if (loop) return (0);
}
say(64,916,0);
 if (loop) return(0);}}
return 0;
}
int p32(void)
{
value[0]=(value[718]==3);
value[0]&=(bitest(104,4));
if(value[0]){
value[0]=(getloc(104)==485);
if(value[0]){
say(64,1408,0);
 if (loop) return(0);}value[0]=(
isnear(104,-1,-1));
if(value[0]){
say(0,1381,0);
p93();
if (loop) return (0);
}value[0]=(
value[HERE]==378);
if(value[0]){
say(0,1382,0);
p93();
if (loop) return (0);
}
bitmod('s',381,4);
value[674]=380;*bitword(674)= -1;
apport(104,485);
apport(134,485);
apport(82,value[673]);
say(64,1383,0);
 if (loop) return(0);}
say(64,865,0);
 if (loop) return(0);return 0;
}
int p569(void)
{
say(64,1389,0);
 if (loop) return(0);return 0;
}
int p626(void)
{
value[0]=(value[699]==1);
value[0]|=!(bitest(evar(672),0));
if(value[0]){
return 0;
}value[0]=(
isnear(value[672],-1,-1));
if(value[0]){
value[0]=(bitest(evar(672),16));
if(!value[0]){
say(64,815,0);
 if (loop) return(0);}value[0]=(
have(value[672],-1,-1));
if(value[0]){
value[702]=value[value[672]];*bitword(702)= *bitword(672);value[0]=(value[702]>1);
if(value[0]){
say(76,1425,672);
 if (loop) return(0);}}value[0]=(
KEY(52));
if(value[0]){
say(64,1424,0);
 if (loop) return(0);}value[0]=(
have(value[672],-1,-1));
if(!value[0]){
value[0]=(value[707]<value[722]);
value[0]|=(bitest(evar(672),12));
if(value[0]){
apport(value[672],INHAND);
p447();
if (loop) return (0);
}else{
say(64,898,0);
 if (loop) return(0);}}value[0]=(
KEY(47));
if(value[0]){
say(64,1430,0);
 if (loop) return(0);}
value[value[672]]=2;
bitmod('s',evar(672),12);
value[0]=(KEY(43));
if(value[0]){
value[0]=(bitest(43,14));
if(value[0]){
say(12,1496,671);
say(12,1497,672);
}else{
bitmod('s',43,14);
say(0,1432,0);
}
}else{
say(12,1496,671);
say(12,1497,672);
}
loop=1; return(0);
}
return 0;
}
int p475(void)
{
value[0]=(KEY(101));
value[0]&=(have(101,-1,-1));
if(value[0]){
p133();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
value[699]==1);
if(value[0]){
return 0;
}value[0]=(
bitest(evar(672),16));
if(!value[0]){
say(64,916,0);
 if (loop) return(0);}
value[702]=value[value[672]];*bitword(702)= *bitword(672);value[0]=(value[702]<2);
value[0]|=!(have(value[672],-1,-1));
if(value[0]){
say(64,1426,0);
 if (loop) return(0);}value[0]=(
value[707]<value[722]);
if(!value[0]){
say(64,898,0);
 if (loop) return(0);}value[0]=(
KEY(64));
if(!value[0]){
bitmod('c',evar(672),12);
}value[0]=(
KEY(43));
if(value[0]){
set('V',701,'e',43,NULL,NULL);
value[0]=(value[701]>1);
if(value[0]){
set('E',43,'c',1,NULL,NULL);
value[0]=(value[701]<9);
if(value[0]){
value[0]=(value[701]==3);
value[0]|=(value[HERE]==252);
value[0]|=(value[HERE]==253);
value[0]|=(value[HERE]==251);
if(!value[0]){
say(64,1614,0);
 if (loop) return(0);}
}else{
value[0]=(value[701]<12);
if(value[0]){
say(64,1615,0);
 if (loop) return(0);}value[0]=(
value[701]<17);
if(value[0]){
say(64,1616,0);
 if (loop) return(0);}value[0]=(
value[701]<19);
if(value[0]){
say(64,1617,0);
 if (loop) return(0);}
say(64,1618,0);
 if (loop) return(0);}}}
value[value[672]]=1;
say(12,1496,671);
say(76,1497,672);
 if (loop) return(0);return 0;
}
int p604(void)
{
say(64,1434,0);
 if (loop) return(0);return 0;
}
int p474(void)
{
value[0]=(value[699]==1);
if(value[0]){
value[0]=(isnear(16,-1,-1));
if(value[0]){
value[0]=(value[HERE]==165);
if(value[0]){
move(166,-1);
}else{
move(165,-1);
}
bitmod('c',evar(699),0);
bitmod('c',16,14);
say(64,1087,0);
 if (loop) return(0);}value[0]=(
isnear(10,-1,-1));
if(value[0]){
say(64,1437,0);
 if (loop) return(0);}
say(64,1436,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p594(void)
{
value[0]=(value[699]==2);
if(value[0]){
value[0]=(KEY(70));
value[0]|=(KEY(108));
value[0]|=(KEY(134));
value[0]|=(KEY(88));
if(!value[0]){
say(64,916,0);
 if (loop) return(0);}}value[0]=(
isnear(88,-1,-1));
if(value[0]){
value[0]=(value[699]==1);
value[0]|=(KEY(88));
if(value[0]){
say(64,1053,0);
 if (loop) return(0);}}value[0]=(
isnear(70,-1,-1));
value[0]|=(isnear(108,-1,-1));
value[0]|=(isnear(134,-1,-1));
if(value[0]){
say(64,1440,0);
 if (loop) return(0);}
say(76,1441,671);
 if (loop) return(0);return 0;
}
int p652(void)
{
value[0]=(irand(100)<30);
if(value[0]){
say(64,1513,0);
 if (loop) return(0);}value[0]=(
irand(100)<50);
if(value[0]){
say(64,1514,0);
 if (loop) return(0);}
say(64,1515,0);
 if (loop) return(0);return 0;
}
int p614(void)
{
value[702]=1478;*bitword(702)= -1;
value[703]=1480;*bitword(703)= -1;
p312();
if (loop) return (0);
loop=1; return(0);
return 0;
}
int p98(void)
{
value[0]=(value[HERE]==136);
value[0]|=(value[HERE]==142);
value[0]|=(value[HERE]==143);
value[0]|=(value[HERE]==200);
value[0]|=(value[HERE]==425);
value[0]|=(value[HERE]==427);
value[0]|=(value[HERE]==428);
if(value[0]){
say(64,1483,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);return 0;
}
int p329(void)
{
value[0]=(value[HERE]==252);
value[0]|=(value[HERE]==253);
value[0]|=(value[HERE]==251);
if(value[0]){
say(64,1487,0);
 if (loop) return(0);}value[0]=(
have(43,-1,-1));
value[0]&=(value[43]>2);
if(value[0]){
value[0]=(bitest(43,13));
if(!value[0]){
value[43] += 1;
}value[0]=(
value[43]<9);
if(value[0]){
say(64,1619,0);
 if (loop) return(0);}
say(64,1620,0);
 if (loop) return(0);}value[0]=(
isnear(134,-1,-1));
value[0]|=(isnear(121,-1,-1));
value[0]|=(value[17]>0);
if(value[0]){
say(64,1488,0);
 if (loop) return(0);}value[0]=(
ishere(101,-1,-1));
if(value[0]){
say(76,1485,101);
 if (loop) return(0);}value[0]=(
isnear(116,-1,-1));
if(value[0]){
say(76,1485,116);
 if (loop) return(0);}value[0]=(
bitest(evar(673),7));
value[0]&=!(value[HERE]==141);
if(value[0]){
say(64,1489,0);
 if (loop) return(0);}value[0]=(
value[HERE]==432);
value[0]|=(value[HERE]==250);
value[0]|=(value[HERE]==249);
value[0]|=(value[HERE]==296);
value[0]|=(value[HERE]==463);
value[0]|=(value[HERE]==324);
if(value[0]){
say(64,1486,0);
 if (loop) return(0);}value[0]=(
value[HERE]==230);
value[0]|=(value[HERE]==239);
if(value[0]){
say(64,1645,0);
 if (loop) return(0);}value[0]=(
bitest(19,13));
value[0]&=!(bitest(evar(673),7));
value[0]&=!(bitest(evar(673),3));
if(value[0]){
say(64,931,0);
 if (loop) return(0);}
say(64,1484,0);
 if (loop) return(0);return 0;
}
int p532(void)
{
say(64,1526,0);
 if (loop) return(0);return 0;
}
int p618(void)
{
value[0]=(value[699]>1);
if(value[0]){
value[0]=(bitest(evar(672),3));
if(value[0]){
value[0]=(isnear(value[672],-1,-1));
if(!value[0]){
say(76,819,672);
 if (loop) return(0);}value[0]=(
bitest(evar(673),9));
if(value[0]){
value[0]=(value[HERE]==428);
value[0]|=(value[HERE]==427);
value[0]&=(bitest(91,14));
if(value[0]){
say(64,1665,0);
 if (loop) return(0);}value[0]=(
KEY(123));
value[0]&=(value[123]==2);
if(value[0]){
say(64,1664,0);
 if (loop) return(0);}
say(64,1663,0);
 if (loop) return(0);}
say(64,1660,0);
 if (loop) return(0);}
say(64,916,0);
 if (loop) return(0);}value[0]=(
value[HERE]==427);
value[0]|=(value[HERE]==428);
value[0]&=(bitest(91,14));
if(value[0]){
value[0]=(bitest(91,12));
if(value[0]){
say(0,1670,0);
}else{
say(0,1656,0);
bitmod('s',91,12);
}
bitmod('c',91,14);
bitmod('c',428,8);
bitmod('c',427,8);
loop=1; return(0);
}value[0]=(
419<=value[673] && value[673]<=426);
if(value[0]){
say(64,1657,0);
 if (loop) return(0);}value[0]=(
bitest(evar(673),9));
if(value[0]){
say(64,1658,0);
 if (loop) return(0);}value[0]=(
have(114,0,3));
value[0]|=(have(111,0,3));
if(value[0]){
say(64,1659,0);
 if (loop) return(0);}
say(64,1660,0);
 if (loop) return(0);return 0;
}
int p559(void)
{
say(0,778,0);
say(0,777,0);
value[701] = irand(1679-1673+1)+1673;
*bitword(701)= -1;
say(2,701,0);
say(64,1680,0);
 if (loop) return(0);return 0;
}
int p200(void)
{
value[0]=(437<=value[673] && value[673]<=461);
if(value[0]){
say(64,939,0);
 if (loop) return(0);}value[0]=(
bitest(evar(673),7));
if(value[0]){
say(76,1682,671);
 if (loop) return(0);}
say(76,1683,671);
 if (loop) return(0);return 0;
}
int p121(void)
{
value[0]=(bitest(evar(673),7));
if(value[0]){
say(64,1688,0);
 if (loop) return(0);}value[0]=(
bitest(evar(736),5));
if(value[0]){
say(64,1691,0);
 if (loop) return(0);}value[0]=(
query(1689)); if (loop>1) return (0);
if(value[0]){
bitmod('s',evar(736),5);
say(64,1690,0);
 if (loop) return(0);}
say(64,1692,0);
 if (loop) return(0);return 0;
}
int p485(void)
{
if (!KEY(60)) return 0;
value[0]=(isnear(60,-1,-1));
if(value[0]){
value[0]=(value[60]==1);
if(value[0]){
say(72,1693,1);
 if (loop) return(0);}
set('E',60,'c',1,NULL,NULL);
say(72,1694,1);
 if (loop) return(0);}
return 0;
}
int p600(void)
{
if (!KEY(60)) return 0;
value[0]=(isnear(60,-1,-1));
if(value[0]){
value[0]=(value[60]==0);
if(value[0]){
say(72,1693,0);
 if (loop) return(0);}value[0]=(
have(60,-1,-1));
if(value[0]){
value[0]=(bitest(evar(736),7));
if(value[0]){
say(64,1696,0);
 if (loop) return(0);}
bitmod('s',evar(736),7);
say(64,1695,0);
 if (loop) return(0);}
set('E',60,'c',0,NULL,NULL);
say(72,1694,0);
 if (loop) return(0);}
return 0;
}
int p1(void)
{
say(0,777,0);
bitmod('s',98,3);
bitmod('s',99,3);
bitmod('s',100,3);
bitmod('s',102,3);
bitmod('s',104,3);
bitmod('s',101,3);
bitmod('s',105,3);
bitmod('s',106,3);
bitmod('s',107,3);
bitmod('s',108,3);
bitmod('s',109,3);
bitmod('s',110,3);
bitmod('s',111,3);
bitmod('s',115,3);
bitmod('s',38,3);
bitmod('s',39,3);
bitmod('s',44,3);
bitmod('s',45,3);
bitmod('s',46,3);
bitmod('s',47,3);
bitmod('s',48,3);
bitmod('s',49,3);
bitmod('s',50,3);
bitmod('s',51,3);
bitmod('s',53,3);
bitmod('s',54,3);
bitmod('s',55,3);
bitmod('s',58,3);
bitmod('s',59,3);
bitmod('s',60,3);
bitmod('s',61,3);
bitmod('s',63,3);
bitmod('s',64,3);
bitmod('s',65,3);
bitmod('s',42,3);
bitmod('s',52,3);
bitmod('s',117,3);
bitmod('s',118,3);
bitmod('s',119,3);
bitmod('s',66,3);
bitmod('s',67,3);
bitmod('s',68,3);
bitmod('s',116,3);
bitmod('s',43,3);
bitmod('s',56,3);
bitmod('s',57,3);
bitmod('s',114,3);
bitmod('s',103,3);
bitmod('s',62,3);
bitmod('s',78,3);
bitmod('s',92,3);
bitmod('s',89,3);
bitmod('s',80,3);
bitmod('s',85,3);
bitmod('s',81,3);
bitmod('s',88,3);
bitmod('s',90,3);
bitmod('s',83,3);
bitmod('s',84,3);
bitmod('s',95,3);
bitmod('s',97,3);
bitmod('s',123,3);
bitmod('s',101,12);
bitmod('s',38,12);
bitmod('s',97,12);
bitmod('s',64,12);
bitmod('s',59,12);
bitmod('s',136,3);
bitmod('s',137,3);
bitmod('s',437,3);
bitmod('s',438,3);
bitmod('s',439,3);
bitmod('s',440,3);
bitmod('s',441,3);
bitmod('s',442,3);
bitmod('s',443,3);
bitmod('s',444,3);
bitmod('s',445,3);
bitmod('s',446,3);
bitmod('s',447,3);
bitmod('s',448,3);
bitmod('s',449,3);
bitmod('s',450,3);
bitmod('s',451,3);
bitmod('s',452,3);
bitmod('s',453,3);
bitmod('s',454,3);
bitmod('s',455,3);
bitmod('s',456,3);
bitmod('s',461,3);
bitmod('s',460,3);
bitmod('s',459,3);
bitmod('s',458,3);
bitmod('s',138,3);
bitmod('s',139,3);
bitmod('s',140,3);
bitmod('s',457,3);
bitmod('s',141,3);
bitmod('s',142,3);
bitmod('s',143,3);
bitmod('s',144,3);
bitmod('s',436,3);
bitmod('s',145,3);
bitmod('s',146,3);
bitmod('s',228,3);
bitmod('s',252,3);
bitmod('s',324,3);
bitmod('s',376,3);
bitmod('s',377,3);
bitmod('s',256,3);
bitmod('s',257,3);
bitmod('s',463,3);
bitmod('s',253,3);
bitmod('s',254,3);
bitmod('s',290,3);
bitmod('s',291,3);
bitmod('s',295,3);
bitmod('s',382,3);
bitmod('s',243,3);
bitmod('s',242,3);
bitmod('s',419,3);
bitmod('s',399,3);
bitmod('s',400,3);
bitmod('s',401,3);
bitmod('s',402,3);
bitmod('s',403,3);
bitmod('s',404,3);
bitmod('s',383,3);
bitmod('s',483,3);
bitmod('s',378,3);
bitmod('s',380,3);
bitmod('s',379,3);
bitmod('s',145,5);
bitmod('s',146,5);
bitmod('s',227,5);
bitmod('s',244,5);
bitmod('s',245,5);
bitmod('s',249,5);
bitmod('s',228,5);
bitmod('s',229,5);
bitmod('s',250,5);
bitmod('s',251,5);
bitmod('s',252,5);
bitmod('s',296,5);
bitmod('s',297,5);
bitmod('s',298,5);
bitmod('s',299,5);
bitmod('s',192,5);
bitmod('s',158,5);
bitmod('s',159,5);
bitmod('s',200,5);
bitmod('s',377,5);
bitmod('s',376,5);
bitmod('s',253,5);
bitmod('s',254,5);
bitmod('s',255,5);
bitmod('s',256,5);
bitmod('s',257,5);
bitmod('s',463,5);
bitmod('s',258,5);
bitmod('s',259,5);
bitmod('s',260,5);
bitmod('s',261,5);
bitmod('s',262,5);
bitmod('s',263,5);
bitmod('s',264,5);
bitmod('s',265,5);
bitmod('s',315,5);
bitmod('s',266,5);
bitmod('s',267,5);
bitmod('s',268,5);
bitmod('s',269,5);
bitmod('s',270,5);
bitmod('s',271,5);
bitmod('s',272,5);
bitmod('s',273,5);
bitmod('s',274,5);
bitmod('s',275,5);
bitmod('s',276,5);
bitmod('s',277,5);
bitmod('s',278,5);
bitmod('s',279,5);
bitmod('s',280,5);
bitmod('s',281,5);
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
bitmod('s',240,5);
bitmod('s',241,5);
bitmod('s',242,5);
bitmod('s',462,5);
bitmod('s',323,5);
bitmod('s',393,5);
bitmod('s',388,5);
bitmod('s',433,5);
bitmod('s',383,5);
bitmod('s',384,5);
bitmod('s',405,5);
bitmod('s',406,5);
bitmod('s',432,5);
bitmod('s',420,5);
bitmod('s',421,5);
bitmod('s',428,5);
bitmod('s',419,5);
bitmod('s',422,5);
bitmod('s',423,5);
bitmod('s',424,5);
bitmod('s',425,5);
bitmod('s',426,5);
bitmod('s',418,5);
bitmod('s',417,5);
bitmod('s',416,5);
bitmod('s',415,5);
bitmod('s',205,5);
bitmod('s',434,5);
bitmod('s',484,5);
bitmod('s',485,5);
bitmod('s',147,5);
bitmod('s',148,5);
bitmod('s',149,5);
bitmod('s',150,5);
bitmod('s',411,5);
bitmod('s',204,5);
bitmod('s',382,5);
bitmod('s',435,5);
bitmod('s',136,7);
bitmod('s',137,7);
bitmod('s',437,7);
bitmod('s',438,7);
bitmod('s',439,7);
bitmod('s',440,7);
bitmod('s',441,7);
bitmod('s',442,7);
bitmod('s',443,7);
bitmod('s',444,7);
bitmod('s',445,7);
bitmod('s',446,7);
bitmod('s',447,7);
bitmod('s',448,7);
bitmod('s',449,7);
bitmod('s',450,7);
bitmod('s',451,7);
bitmod('s',452,7);
bitmod('s',453,7);
bitmod('s',454,7);
bitmod('s',455,7);
bitmod('s',456,7);
bitmod('s',141,7);
bitmod('s',461,7);
bitmod('s',458,7);
bitmod('s',459,7);
bitmod('s',460,7);
bitmod('s',457,7);
bitmod('s',138,7);
bitmod('s',139,7);
bitmod('s',140,7);
bitmod('s',142,7);
bitmod('s',143,7);
bitmod('s',144,7);
bitmod('s',436,7);
bitmod('s',169,6);
bitmod('s',170,6);
bitmod('s',171,6);
bitmod('s',182,6);
bitmod('s',185,6);
bitmod('s',189,6);
bitmod('s',172,6);
bitmod('s',173,6);
bitmod('s',174,6);
bitmod('s',175,6);
bitmod('s',176,6);
bitmod('s',183,6);
bitmod('s',177,6);
bitmod('s',190,6);
bitmod('s',193,6);
bitmod('s',186,6);
bitmod('s',178,6);
bitmod('s',184,6);
bitmod('s',191,6);
bitmod('s',179,6);
bitmod('s',180,6);
bitmod('s',187,6);
bitmod('s',188,6);
bitmod('s',181,6);
bitmod('s',301,6);
bitmod('s',192,6);
bitmod('s',302,6);
bitmod('s',303,6);
bitmod('s',304,6);
bitmod('s',305,6);
bitmod('s',306,6);
bitmod('s',307,6);
bitmod('s',308,6);
bitmod('s',309,6);
bitmod('s',310,6);
bitmod('s',311,6);
bitmod('s',232,6);
bitmod('s',370,6);
bitmod('s',371,6);
bitmod('s',240,6);
bitmod('s',221,6);
bitmod('s',238,6);
bitmod('s',244,6);
bitmod('s',245,6);
bitmod('s',236,6);
bitmod('s',227,6);
bitmod('s',228,6);
bitmod('s',229,6);
bitmod('s',216,6);
bitmod('s',329,6);
bitmod('s',331,6);
bitmod('s',333,6);
bitmod('s',335,6);
bitmod('s',336,6);
bitmod('s',337,6);
bitmod('s',338,6);
bitmod('s',339,6);
bitmod('s',340,6);
bitmod('s',341,6);
bitmod('s',342,6);
bitmod('s',343,6);
bitmod('s',345,6);
bitmod('s',346,6);
bitmod('s',347,6);
bitmod('s',349,6);
bitmod('s',350,6);
bitmod('s',351,6);
bitmod('s',352,6);
bitmod('s',353,6);
bitmod('s',354,6);
bitmod('s',328,6);
bitmod('s',327,6);
bitmod('s',271,6);
bitmod('s',272,6);
bitmod('s',273,6);
bitmod('s',274,6);
bitmod('s',275,6);
bitmod('s',276,6);
bitmod('s',277,6);
bitmod('s',278,6);
bitmod('s',279,6);
bitmod('s',280,6);
bitmod('s',281,6);
bitmod('s',282,6);
bitmod('s',283,6);
bitmod('s',284,6);
bitmod('s',285,6);
bitmod('s',286,6);
bitmod('s',287,6);
bitmod('s',288,6);
bitmod('s',289,6);
bitmod('s',253,6);
bitmod('s',255,6);
bitmod('s',256,6);
bitmod('s',263,6);
bitmod('s',243,6);
bitmod('s',382,6);
bitmod('s',385,6);
bitmod('s',386,6);
bitmod('s',387,6);
bitmod('s',388,6);
bitmod('s',389,6);
bitmod('s',390,6);
bitmod('s',428,6);
bitmod('s',393,6);
bitmod('s',432,6);
bitmod('s',164,6);
bitmod('s',270,6);
bitmod('s',461,6);
bitmod('s',168,10);
bitmod('s',169,10);
bitmod('s',170,10);
bitmod('s',171,10);
bitmod('s',182,10);
bitmod('s',185,10);
bitmod('s',189,10);
bitmod('s',172,10);
bitmod('s',173,10);
bitmod('s',174,10);
bitmod('s',175,10);
bitmod('s',176,10);
bitmod('s',183,10);
bitmod('s',177,10);
bitmod('s',190,10);
bitmod('s',193,10);
bitmod('s',186,10);
bitmod('s',178,10);
bitmod('s',184,10);
bitmod('s',191,10);
bitmod('s',179,10);
bitmod('s',180,10);
bitmod('s',187,10);
bitmod('s',188,10);
bitmod('s',181,10);
bitmod('s',300,10);
bitmod('s',301,10);
bitmod('s',192,10);
bitmod('s',302,10);
bitmod('s',303,10);
bitmod('s',304,10);
bitmod('s',305,10);
bitmod('s',306,10);
bitmod('s',307,10);
bitmod('s',308,10);
bitmod('s',309,10);
bitmod('s',310,10);
bitmod('s',311,10);
bitmod('s',385,10);
bitmod('s',386,10);
bitmod('s',387,10);
bitmod('s',388,10);
bitmod('s',389,10);
bitmod('s',390,10);
bitmod('s',329,10);
bitmod('s',330,10);
bitmod('s',331,10);
bitmod('s',332,10);
bitmod('s',333,10);
bitmod('s',334,10);
bitmod('s',335,10);
bitmod('s',336,10);
bitmod('s',337,10);
bitmod('s',338,10);
bitmod('s',339,10);
bitmod('s',340,10);
bitmod('s',341,10);
bitmod('s',342,10);
bitmod('s',343,10);
bitmod('s',344,10);
bitmod('s',345,10);
bitmod('s',346,10);
bitmod('s',347,10);
bitmod('s',348,10);
bitmod('s',349,10);
bitmod('s',350,10);
bitmod('s',351,10);
bitmod('s',352,10);
bitmod('s',353,10);
bitmod('s',354,10);
bitmod('s',355,10);
bitmod('s',356,10);
bitmod('s',357,10);
bitmod('s',358,10);
bitmod('s',359,10);
bitmod('s',360,10);
bitmod('s',361,10);
bitmod('s',362,10);
bitmod('s',363,10);
bitmod('s',364,10);
bitmod('s',271,10);
bitmod('s',272,10);
bitmod('s',273,10);
bitmod('s',274,10);
bitmod('s',275,10);
bitmod('s',276,10);
bitmod('s',277,10);
bitmod('s',278,10);
bitmod('s',279,10);
bitmod('s',280,10);
bitmod('s',281,10);
bitmod('s',282,10);
bitmod('s',283,10);
bitmod('s',284,10);
bitmod('s',285,10);
bitmod('s',286,10);
bitmod('s',287,10);
bitmod('s',288,10);
bitmod('s',289,10);
bitmod('s',154,11);
bitmod('s',394,11);
bitmod('s',165,11);
bitmod('s',166,11);
bitmod('s',182,11);
bitmod('s',185,11);
bitmod('s',189,11);
bitmod('s',172,11);
bitmod('s',183,11);
bitmod('s',190,11);
bitmod('s',186,11);
bitmod('s',197,11);
bitmod('s',214,11);
bitmod('s',219,11);
bitmod('s',184,11);
bitmod('s',191,11);
bitmod('s',187,11);
bitmod('s',188,11);
bitmod('s',234,11);
bitmod('s',242,11);
bitmod('s',311,11);
bitmod('s',462,11);
bitmod('s',318,11);
bitmod('s',321,11);
bitmod('s',330,11);
bitmod('s',334,11);
bitmod('s',338,11);
bitmod('s',339,11);
bitmod('s',345,11);
bitmod('s',346,11);
bitmod('s',369,11);
bitmod('s',377,11);
bitmod('s',411,11);
bitmod('s',407,11);
bitmod('s',205,11);
bitmod('s',433,11);
bitmod('s',483,11);
bitmod('s',4,9);
bitmod('s',6,9);
bitmod('s',106,9);
bitmod('s',107,9);
bitmod('s',114,9);
bitmod('s',63,9);
bitmod('s',35,9);
bitmod('s',118,9);
bitmod('s',49,9);
bitmod('s',73,9);
bitmod('s',93,9);
bitmod('s',113,10);
bitmod('s',112,10);
bitmod('s',5,10);
bitmod('s',73,10);
bitmod('s',8,10);
bitmod('s',10,10);
bitmod('s',16,10);
bitmod('s',13,10);
bitmod('s',18,10);
bitmod('s',19,10);
bitmod('s',31,10);
bitmod('s',30,10);
bitmod('s',32,10);
bitmod('s',36,10);
bitmod('s',40,10);
bitmod('s',134,10);
bitmod('s',109,10);
bitmod('s',14,10);
bitmod('s',12,10);
bitmod('s',135,10);
bitmod('s',75,10);
bitmod('s',70,10);
bitmod('s',85,10);
bitmod('s',91,10);
bitmod('s',122,10);
bitmod('s',17,10);
bitmod('s',9,10);
bitmod('s',93,10);
bitmod('s',8,10);
bitmod('s',168,8);
bitmod('s',169,8);
bitmod('s',170,8);
bitmod('s',171,8);
bitmod('s',182,8);
bitmod('s',185,8);
bitmod('s',189,8);
bitmod('s',172,8);
bitmod('s',173,8);
bitmod('s',174,8);
bitmod('s',175,8);
bitmod('s',176,8);
bitmod('s',183,8);
bitmod('s',177,8);
bitmod('s',190,8);
bitmod('s',193,8);
bitmod('s',186,8);
bitmod('s',178,8);
bitmod('s',184,8);
bitmod('s',191,8);
bitmod('s',179,8);
bitmod('s',180,8);
bitmod('s',187,8);
bitmod('s',188,8);
bitmod('s',181,8);
bitmod('s',300,8);
bitmod('s',301,8);
bitmod('s',192,8);
bitmod('s',302,8);
bitmod('s',303,8);
bitmod('s',304,8);
bitmod('s',305,8);
bitmod('s',306,8);
bitmod('s',307,8);
bitmod('s',308,8);
bitmod('s',309,8);
bitmod('s',310,8);
bitmod('s',311,8);
bitmod('s',149,8);
bitmod('s',144,8);
bitmod('s',155,8);
bitmod('s',165,8);
bitmod('s',166,8);
bitmod('s',236,8);
bitmod('s',228,8);
bitmod('s',227,8);
bitmod('s',229,8);
bitmod('s',328,8);
bitmod('s',329,8);
bitmod('s',330,8);
bitmod('s',331,8);
bitmod('s',332,8);
bitmod('s',333,8);
bitmod('s',334,8);
bitmod('s',335,8);
bitmod('s',336,8);
bitmod('s',337,8);
bitmod('s',338,8);
bitmod('s',339,8);
bitmod('s',340,8);
bitmod('s',341,8);
bitmod('s',342,8);
bitmod('s',343,8);
bitmod('s',344,8);
bitmod('s',345,8);
bitmod('s',346,8);
bitmod('s',347,8);
bitmod('s',348,8);
bitmod('s',349,8);
bitmod('s',350,8);
bitmod('s',351,8);
bitmod('s',352,8);
bitmod('s',353,8);
bitmod('s',354,8);
bitmod('s',355,8);
bitmod('s',356,8);
bitmod('s',357,8);
bitmod('s',358,8);
bitmod('s',359,8);
bitmod('s',360,8);
bitmod('s',361,8);
bitmod('s',362,8);
bitmod('s',363,8);
bitmod('s',364,8);
bitmod('s',256,8);
bitmod('s',239,8);
bitmod('s',434,8);
bitmod('s',252,8);
bitmod('s',254,8);
bitmod('s',323,8);
bitmod('s',259,8);
bitmod('s',314,8);
bitmod('s',315,8);
bitmod('s',264,8);
bitmod('s',427,8);
bitmod('s',428,8);
bitmod('s',141,9);
bitmod('s',136,9);
bitmod('s',142,9);
bitmod('s',143,9);
bitmod('s',200,9);
bitmod('s',224,9);
bitmod('s',239,9);
bitmod('s',240,9);
bitmod('s',412,9);
bitmod('s',427,9);
bitmod('s',428,9);
bitmod('s',98,11);
bitmod('s',48,11);
bitmod('s',108,11);
bitmod('s',39,11);
bitmod('s',45,11);
bitmod('s',61,11);
bitmod('s',62,11);
bitmod('s',88,11);
bitmod('s',123,11);
bitmod('s',117,11);
bitmod('s',140,12);
bitmod('s',150,12);
bitmod('s',152,12);
bitmod('s',153,12);
bitmod('s',156,12);
bitmod('s',157,12);
bitmod('s',160,12);
bitmod('s',165,12);
bitmod('s',166,12);
bitmod('s',199,12);
bitmod('s',201,12);
bitmod('s',193,12);
bitmod('s',210,12);
bitmod('s',211,12);
bitmod('s',215,12);
bitmod('s',221,12);
bitmod('s',224,12);
bitmod('s',230,12);
bitmod('s',239,12);
bitmod('s',240,12);
bitmod('s',238,12);
bitmod('s',242,12);
bitmod('s',244,12);
bitmod('s',245,12);
bitmod('s',252,12);
bitmod('s',253,12);
bitmod('s',261,12);
bitmod('s',463,12);
bitmod('s',262,12);
bitmod('s',265,12);
bitmod('s',266,12);
bitmod('s',267,12);
bitmod('s',325,12);
bitmod('s',326,12);
bitmod('s',327,12);
bitmod('s',365,12);
bitmod('s',204,12);
bitmod('s',376,12);
bitmod('s',382,12);
bitmod('s',395,12);
bitmod('s',394,12);
bitmod('s',408,12);
bitmod('s',399,12);
bitmod('s',400,12);
bitmod('s',401,12);
bitmod('s',402,12);
bitmod('s',403,12);
bitmod('s',404,12);
bitmod('s',151,12);
bitmod('s',44,5);
bitmod('s',45,5);
bitmod('s',46,5);
bitmod('s',47,5);
bitmod('s',48,5);
bitmod('s',49,5);
bitmod('s',50,5);
bitmod('s',51,5);
bitmod('s',53,5);
bitmod('s',55,5);
bitmod('s',58,5);
bitmod('s',59,5);
bitmod('s',60,5);
bitmod('s',61,5);
bitmod('s',63,5);
bitmod('s',68,5);
bitmod('s',66,5);
bitmod('s',67,5);
bitmod('s',64,5);
bitmod('s',65,5);
bitmod('s',42,5);
bitmod('s',52,5);
bitmod('s',43,5);
bitmod('s',56,5);
bitmod('s',57,5);
bitmod('s',62,5);
bitmod('s',78,5);
bitmod('s',92,5);
bitmod('s',83,5);
bitmod('s',81,5);
bitmod('s',95,5);
bitmod('s',49,7);
bitmod('s',66,7);
bitmod('s',100,7);
bitmod('s',68,7);
bitmod('s',117,7);
bitmod('s',119,7);
bitmod('s',42,7);
bitmod('s',43,7);
bitmod('s',57,7);
bitmod('s',103,7);
bitmod('s',62,7);
bitmod('s',92,7);
bitmod('s',111,7);
bitmod('s',95,7);
bitmod('s',39,7);
bitmod('s',123,7);
bitmod('s',97,7);
bitmod('s',22,8);
bitmod('s',134,8);
bitmod('s',20,8);
bitmod('s',7,8);
bitmod('s',17,8);
bitmod('s',38,8);
bitmod('s',106,8);
bitmod('s',107,8);
bitmod('s',24,8);
bitmod('s',101,8);
bitmod('s',27,8);
bitmod('s',28,8);
bitmod('s',121,8);
bitmod('s',25,8);
bitmod('s',135,8);
bitmod('s',82,8);
bitmod('s',87,8);
bitmod('s',91,8);
bitmod('s',43,16);
bitmod('s',62,16);
bitmod('s',67,16);
bitmod('s',64,16);
bitmod('s',47,16);
bitmod('s',52,16);
bitmod('s',38,15);
bitmod('s',63,15);
bitmod('s',85,15);
bitmod('s',84,15);
bitmod('s',88,15);
bitmod('s',101,15);
bitmod('s',106,15);
bitmod('s',107,15);
bitmod('s',100,15);
bitmod('s',105,15);
bitmod('s',49,15);
bitmod('s',50,15);
bitmod('s',51,15);
bitmod('s',53,15);
bitmod('s',60,15);
bitmod('s',52,15);
bitmod('s',68,15);
bitmod('s',116,15);
bitmod('s',43,15);
bitmod('s',56,15);
bitmod('s',57,15);
bitmod('s',103,15);
bitmod('s',78,15);
bitmod('s',81,15);
bitmod('s',80,15);
bitmod('s',461,4);
bitmod('s',243,4);
bitmod('s',420,13);
bitmod('s',421,13);
bitmod('s',422,13);
bitmod('s',423,13);
bitmod('s',424,13);
bitmod('s',425,13);
bitmod('s',426,13);
value[673]=484;*bitword(673)= -1;
set('V',761,'c',1,NULL,NULL);
value[684]=203;*bitword(684)= -1;
value[757] = irand(300-150+1)+150;
set('V',734,'c',1,NULL,NULL);
value[762] = irand(30-15+1)+15;
value[760] = irand(800-650+1)+650;
value[764] = irand(160-120+1)+120;
set('V',763,'c',-1,NULL,NULL);
value[686]=1355;*bitword(686)= -1;
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
apport(value[677],484);
value[702] = irand(100);
}
apport(98,141);
apport(99,141);
apport(4,144);
apport(100,146);
apport(102,147);
apport(5,150);
apport(101,149);
apport(6,223);
apport(105,225);
apport(7,155);
apport(8,152);
apport(106,232);
apport(108,236);
apport(110,484);
apport(111,140);
apport(11,159);
apport(12,156);
apport(13,238);
apport(16,165);
apport(18,226);
apport(20,247);
apport(21,244);
apport(22,244);
apport(38,299);
apport(30,252);
apport(34,311);
apport(40,225);
apport(44,154);
apport(45,153);
apport(46,160);
apport(47,161);
apport(48,162);
apport(50,222);
apport(51,224);
apport(53,226);
apport(55,228);
apport(58,229);
apport(124,229);
apport(61,296);
apport(63,299);
apport(65,374);
apport(42,377);
apport(41,377);
apport(52,313);
apport(118,321);
apport(119,433);
apport(66,346);
apport(68,369);
apport(35,314);
apport(32,370);
apport(33,367);
apport(116,434);
apport(24,323);
apport(36,291);
apport(37,291);
apport(56,291);
apport(43,462);
apport(31,254);
apport(14,256);
apport(57,239);
apport(67,295);
apport(114,294);
apport(25,263);
apport(103,268);
apport(62,242);
apport(26,240);
apport(78,393);
apport(79,394);
apport(86,388);
apport(87,388);
apport(92,411);
apport(89,419);
apport(91,421);
apport(135,383);
apport(80,416);
apport(85,205);
apport(81,204);
apport(75,203);
apport(76,419);
apport(77,393);
apport(70,312);
apport(120,140);
apport(71,434);
apport(90,435);
apport(95,463);
apport(72,393);
apport(120,140);
apport(93,137);
apport(123,318);
apport(121,484);
apport(113,158);
bitmod('s',4,6);
bitmod('s',5,6);
bitmod('s',8,6);
bitmod('s',12,6);
bitmod('s',16,6);
bitmod('s',20,6);
bitmod('s',21,6);
bitmod('s',22,6);
bitmod('s',23,6);
bitmod('s',32,6);
bitmod('s',35,6);
bitmod('s',14,6);
bitmod('s',30,6);
bitmod('s',25,6);
bitmod('s',75,6);
bitmod('s',95,6);
bitmod('s',72,6);
set('E',114,'c',1,NULL,NULL);
set('V',728,'c',35,NULL,NULL);
set('V',729,'c',25,NULL,NULL);
bitmod('s',evar(699),1);
set('V',726,'c',0,NULL,NULL);
value[720]=778;*bitword(720)= -1;
value[717] = irand(25-15+1)+15;
value[700] = irand(460-440+1)+440;
value[716] = irand(8-4+1)+4;
value[0]=(irand(100)<75);
if(value[0]){
bitmod('s',evar(712),1);
}
set('E',63,'c',0,NULL,NULL);
set('E',14,'c',8,NULL,NULL);
set('V',707,'c',0,NULL,NULL);
set('V',722,'c',7,NULL,NULL);
set('V',731,'c',0,NULL,NULL);
say(0,863,0);
value[0]=(query(779)); if (loop>1) return (0);
if(value[0]){
say(0,782,0);
value[0]=(query(1563)); if (loop>1) return (0);
if(value[0]){
value[702]=1561;*bitword(702)= -1;
value[703]=1562;*bitword(703)= -1;
p312();
if (loop) return (0);
}}
move(136,-1);
say(0,777,0);
return 0;
}
int p3(void)
{
value[701] = irand(100);
value[0]=(bitest(evar(699),6));
value[0]&=(value[HERE]==393);
if(value[0]){
value[0]=(KEY(534));
value[0]|=(KEY(556));
if(value[0]){
p649();
if (loop) return (0);
}}value[0]=(
irand(100)<3);
value[0]&=(value[718]==0);
value[0]&=(value[716]<10);
if(value[0]){
p350();
if (loop) return (0);
}value[0]=(
value[718]==3);
if(value[0]){
p545();
if (loop) return (0);
}value[0]=(
bitest(evar(699),0));
if(value[0]){
value[714] += 1;
value[0]=(value[HERE]==155);
if(value[0]){
set('V',683,'v',674,NULL,NULL);
}value[0]=(
437<=value[673] && value[673]<=461);
if(value[0]){
apport(122,value[673]);
}value[0]=(
value[134]>0);
if(value[0]){
value[0]=(bitest(evar(673),7));
value[0]|=(bitest(evar(673),5));
if(value[0]){
value[0]=(irand(100)<15);
if(value[0]){
value[134] -= 1;
value[0]=(value[134]==0);
if(value[0]){
apport(134,484);
}}
}else{
value[0]=(getloc(134)==value[673]);
value[0]|=(getloc(134)==value[674]);
if(!value[0]){
set('E',134,'c',0,NULL,NULL);
apport(134,484);
}}}value[0]=(
bitest(19,13));
value[0]&=(bitest(evar(673),7));
if(value[0]){
bitmod('c',19,13);
}value[0]=(
bitest(evar(674),7));
value[0]&=!(bitest(evar(673),7));
if(value[0]){
bitmod('c',19,13);
}}value[0]=(
bitest(evar(712),0));
if(value[0]){
p582();
if (loop) return (0);
}value[0]=(
bitest(evar(699),0));
if(value[0]){
value[0]=(isnear(95,-1,-1));
value[0]&=(value[95]>1);
if(value[0]){
value[0]=(getloc(95)==value[673]);
if(value[0]){
set('E',95,'c',3,NULL,NULL);
}else{
set('E',95,'c',2,NULL,NULL);
}}value[0]=(
bitest(evar(674),11));
value[0]&=(getloc(134)==value[674]);
value[0]&=!(bitest(evar(712),4));
if(value[0]){
value[0]=(irand(100)<97);
if(value[0]){
move(value[674],-1);
bitmod('c',evar(699),0);
say(0,783,0);
return 0;
}else{
p527();
if (loop) return (0);
}}
}else{
value[0]=(isnear(134,-1,-1));
value[0]&=(irand(100)<value[728]);
if(value[0]){
p145();
if (loop) return (0);
}
return 0;
}value[0]=(
isnear(14,-1,-1));
if(value[0]){
p442();
if (loop) return (0);
}
bitmod('c',evar(699),0);
value[687] = irand(1116-1111+1)+1111;
*bitword(687)= -1;
value[0]=(irand(100)<1);
if(value[0]){
value[687]=1117;*bitword(687)= -1;
}value[0]=(
isnear(99,-1,-1));
if(value[0]){
value[0]=(value[99]==1);
value[0]&=!(value[HERE]==256);
if(value[0]){
value[700] -= 1;
value[0]=(value[700]==40);
value[0]|=(value[700]==0);
if(value[0]){
p323();
if (loop) return (0);
}}}value[0]=(
isnear(85,-1,-1));
value[0]&=(isnear(99,0,1));
if(value[0]){
p433();
if (loop) return (0);
}value[0]=(
getloc(121)==484);
value[0]|=(getloc(121)==485);
if(!value[0]){
apport(121,value[673]);
value[0]=(value[121]>0);
if(value[0]){
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
say(0,1163,0);
}}}value[0]=(
bitest(evar(673),4));
value[0]&=(bitest(evar(699),1));
value[0]|=(bitest(evar(699),2));
if(value[0]){
set('V',702,'c',0,NULL,NULL);
}else{
bitmod('s',evar(699),3);
set('V',702,'c',1,NULL,NULL);
value[0]=(value[135]==5);
if(value[0]){
set('E',135,'c',4,NULL,NULL);
}}
p308();
if (loop) return (0);
value[0]=(value[701]==0);
if(value[0]){
say(2,673,0);
value[0]=(value[702]>0);
if(value[0]){
value[0]=(value[HERE]==463);
if(value[0]){
say(2,686,0);
}else{
value[0]=(value[HERE]==150);
value[0]&=(value[151]==0);
if(value[0]){
say(0,1231,0);
}}}value[0]=(
bitest(evar(673),15));
if(value[0]){
say(0,877,0);
}value[0]=(
bitest(evar(673),4));
if(value[0]){
set('V',703,'c',2,NULL,NULL);
}else{
set('V',703,'c',3,NULL,NULL);
value[0]=(value[HERE]==434);
if(value[0]){
say(0,1342,0);
}}value[0]=(
bitest(evar(673),7));
if(!value[0]){
value[717] -= value[703];
}
bitmod('s',evar(673),4);
value[0]=(value[134]<1);
if(!value[0]){
value[0]=(bitest(evar(673),7));
value[0]|=(bitest(evar(673),5));
if(!value[0]){
apport(134,value[673]);
}}
p253();
if (loop) return (0);
value[0]=(isnear(134,-1,-1));
value[0]&=(value[134]==1);
value[0]&=!(value[688]==0);
value[0]&=(irand(100)<67);
if(value[0]){
say(0,1344,0);
}value[0]=(
have(38,-1,-1));
if(value[0]){
say(0,940,0);
}value[0]=(
irand(100)<1);
value[0]&=(irand(100)<5);
value[0]&=(value[718]<3);
value[0]&=(value[713]>150);
value[0]&=!(bitest(evar(673),3));
if(value[0]){
p126();
if (loop) return (0);
}
}else{
value[0]=(bitest(evar(674),3));
if(!value[0]){
value[0]=(getloc(99)==value[674]);
value[0]&=(value[99]==1);
if(!value[0]){
value[0]=(getloc(95)==value[674]);
value[0]&=!(value[95]==1);
if(!value[0]){
value[0]=(have(58,-1,-1));
value[0]|=(irand(100)<25);
value[0]&=!(bitest(evar(712),5));
value[0]&=(value[718]<3);
if(value[0]){
say(0,813,0);
p93();
if (loop) return (0);
}}}}
say(0,806,0);
}
bitmod('c',evar(699),3);
bitmod('c',evar(712),5);
value[0]=(value[HERE]==163);
if(value[0]){
value[0]=(irand(100)<35);
if(value[0]){
say(0,792,0);
}}
p246();
if (loop) return (0);
value[0]=(bitest(evar(673),7));
value[0]|=(value[718]>2);
if(!value[0]){
value[0]=(value[717]<1);
if(value[0]){
p165();
if (loop) return (0);
}else{
bitmod('c',evar(699),7);
value[762] -= 1;
value[0]=(value[762]==0);
if(value[0]){
value[762] = irand(20-10+1)+10;
p239();
if (loop) return (0);
}}}value[0]=(
isnear(134,-1,-1));
if(value[0]){
set('V',729,'c',50,NULL,NULL);
p145();
if (loop) return (0);
}value[0]=(
getloc(99)==485);
value[0]&=(value[HERE]==136);
value[0]&=(value[718]<4);
if(value[0]){
say(0,997,0);
set('V',719,'c',1,NULL,NULL);
p190();
if (loop) return (0);
}
return 0;
}
int p4(void)
{
value[0]=(bitest(evar(673),8));
if(value[0]){
value[721] += 1;
value[0]=(value[721]>25);
if(value[0]){
p257();
if (loop) return (0);
}
}else{
set('V',721,'c',0,NULL,NULL);
}value[0]=(
isnear(91,-1,-1));
if(value[0]){
value[91] += 1;
value[0]=(value[91]<6);
if(value[0]){
value[0]=(value[91]>1);
if(value[0]){
value[701]=1503;*bitword(701)= -1;
value[701] += value[91];
value[701] -= 2;
say(2,701,0);
}
}else{
say(0,1507,0);
move(419,-1);
value[0]=(have(110,-1,-1));
if(value[0]){
apport(110,484);
}
set('E',91,'c',0,NULL,NULL);
p93();
if (loop) return (0);
}}value[0]=(
bitest(evar(673),13));
if(value[0]){
p463();
if (loop) return (0);
}
value[758] -= 1;
value[0]=(isnear(135,-1,-1));
if(value[0]){
p243();
if (loop) return (0);
}value[0]=(
value[135]>0);
value[0]&=(value[135]<4);
value[0]&=(value[758]<1);
if(value[0]){
value[0]=(value[135]==2);
if(value[0]){
set('E',135,'c',3,NULL,NULL);
bitmod('c',135,4);
value[758] = irand(7-4+1)+4;
}else{
value[0]=(bitest(135,4));
if(value[0]){
set('E',135,'c',0,NULL,NULL);
}}}
p398();
if (loop) return (0);
value[0]=(value[HERE]==412);
value[0]&=!(bitest(96,4));
value[0]&=(irand(100)<3);
if(value[0]){
p7();
if (loop) return (0);
value[0]=(value[703]==0);
if(value[0]){
say(0,1267,0);
bitmod('s',96,4);
}}
value[759] -= 1;
value[0]=(value[HERE]==384);
if(value[0]){
p143();
if (loop) return (0);
}value[0]=(
bitest(evar(699),6));
if(value[0]){
bitmod('c',evar(699),6);
set('V',707,'c',0,NULL,NULL);
*bitword(677)= -1; value[677]=FOBJ-1; while (++value[677]<=LOBJ) {
value[0]=(have(value[677],-1,-1));
value[0]&=!(bitest(evar(677),12));
if(value[0]){
value[707] += 1;
}}}
p571();
if (loop) return (0);
set('V',729,'c',25,NULL,NULL);
set('V',728,'c',35,NULL,NULL);
value[0]=(bitest(81,13));
if(value[0]){
value[0]=(bitest(81,14));
if(value[0]){
bitmod('c',81,14);
}else{
bitmod('c',81,13);
}}value[0]=(
bitest(16,14));
if(value[0]){
value[0]=(isnear(16,-1,-1));
if(value[0]){
say(0,1435,0);
}
bitmod('c',16,14);
}value[0]=(
bitest(16,13));
if(value[0]){
bitmod('s',16,14);
bitmod('c',16,13);
}value[0]=(
bitest(evar(712),8));
value[0]&=!(KEY(503));
if(value[0]){
bitmod('c',evar(712),8);
}value[0]=(
value[713]<1999);
if(!value[0]){
say(0,1081,0);
p190();
if (loop) return (0);
}
bitmod('c',43,13);
value[0]=(value[HERE]==252);
value[0]|=(value[HERE]==253);
value[0]|=(value[HERE]==251);
value[0]|=(value[43]<2);
value[0]|=(value[699]==0);
value[0]|=(bitest(evar(699),5));
if(!value[0]){
value[0]=(have(43,-1,-1));
if(value[0]){
value[0]=(value[43]>2);
if(value[0]){
value[701]=1625;*bitword(701)= -1;
value[701] += value[43];
value[701] -= 2;
say(2,701,0);
}value[0]=(
value[43]<9);
if(!value[0]){
bitmod('s',evar(756),10);
}
bitmod('s',evar(699),7);
value[0]=(irand(100)<40);
value[0]|=(value[43]>9);
value[0]|=(value[43]==2);
value[0]&=!(value[43]==7);
value[0]|=(KEY(540));
if(value[0]){
value[43] += 1;
value[0]=(value[43]>19);
if(value[0]){
p93();
if (loop) return (0);
}
bitmod('s',43,13);
}
}else{
value[0]=(value[43]>1);
if(value[0]){
value[0]=(value[43]<9);
if(!value[0]){
say(0,1625,0);
}
set('E',43,'c',1,NULL,NULL);
}}}
input(0);
if (loop) return(0);
value[715] -= 1;
value[713] += 1;
p418();
if (loop) return (0);
value[0]=(KEY(489));
if(value[0]){
(*procs[489])();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
value[699]==0);
if(value[0]){
loop=1; return(0);
}value[0]=(
value[671]==-2);
value[0]|=(value[671]==-3);
if(value[0]){
value[0]=(value[671]==-2);
if(value[0]){
value[702] = irand(853-848+1)+848;
*bitword(702)= -1;
}else{
value[702] = irand(1470-1467+1)+1467;
*bitword(702)= -1;
}
say(14,702,671);
p518();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
KEY(493));
if(value[0]){
p462();
if (loop) return (0);
}value[0]=(
KEY(3));
if(value[0]){
p82();
if (loop) return (0);
}value[0]=(
value[699]>1);
if(value[0]){
value[0]=(value[672]==-2);
value[0]|=(value[672]==-3);
if(value[0]){
value[0]=(487<=value[671] && value[671]<=494);
if(!value[0]){
value[0]=(value[672]==-3);
if(value[0]){
say(12,1467,672);
}else{
value[702] = irand(858-853+1)+853;
*bitword(702)= -1;
say(14,702,672);
}
p518();
if (loop) return (0);
loop=1; return(0);
}}}value[0]=(
KEY(520));
value[0]&=(KEY(527));
if(value[0]){
value[730] += 1;
value[0]=(value[730]==5);
if(value[0]){
value[702]=527;*bitword(702)= -1;
value[0]=(value[702]==value[671]);
if(value[0]){
say(12,1516,671);
}else{
say(12,1516,672);
}}}value[0]=(
value[718]==5);
if(value[0]){
p558();
if (loop) return (0);
}value[0]=(
KEY(524));
value[0]&=(value[699]>1);
if(value[0]){
value[0]=(bitest(evar(672),0));
value[0]&=(bitest(evar(672),3));
if(value[0]){
say(64,1671,0);
 if (loop) return(0);}}value[0]=(
value[699]>1);
value[0]&=(638<=value[672] && value[672]<=654);
if(value[0]){
p503();
if (loop) return (0);
}value[0]=(
KEY(493));
value[0]&=(value[699]>1);
value[0]&=!(495<=value[672] && value[672]<=512);
if(!value[0]){
if (value[673]<LPROC && value[673]>= 0) (*procs[value[673]])(); else pcall(value[673]);
if (loop) return (0);
}value[0]=(
value[718]==5);
if(value[0]){
p558();
if (loop) return (0);
}value[0]=(
bitest(evar(671),1));
if(value[0]){
value[0]=(value[HERE]==value[671]);
if(value[0]){
say(0,939,0);
}else{
say(0,938,0);
}
}else{
value[0]=(value[718]==3);
if(value[0]){
value[0]=(KEY(658));
if(value[0]){
say(64,1522,0);
 if (loop) return(0);}
p81();
if (loop) return (0);
value[0]=(KEY(102));
if(value[0]){
p484();
if (loop) return (0);
}}value[0]=(
KEY(73));
if(value[0]){
value[0]=(value[HERE]==225);
value[0]|=(value[HERE]==392);
value[0]|=(value[HERE]==429);
if(value[0]){
apport(73,value[673]);
}}value[0]=(
KEY(658));
if(value[0]){
say(64,1523,0);
 if (loop) return(0);}value[0]=(
KEY(598));
if(value[0]){
say(64,800,0);
 if (loop) return(0);}value[0]=(
KEY(147));
if(value[0]){
p117();
if (loop) return (0);
loop=1; return(0);
}value[0]=(
KEY(69));
if(value[0]){
say(64,1281,0);
 if (loop) return(0);}value[0]=(
bitest(evar(673),13));
if(value[0]){
value[0]=(KEY(112));
value[0]|=(KEY(559));
if(value[0]){
say(64,1549,0);
 if (loop) return(0);}}
if (value[671]<LPROC && value[671]>= 0) (*procs[value[671]])(); else pcall(value[671]);
if (loop) return (0);
p518();
if (loop) return (0);
value[0]=(bitest(evar(671),0));
if(value[0]){
value[0]=(KEY(112));
value[0]&=(bitest(evar(673),9));
value[0]|=(isnear(value[671],-1,-1));
if(value[0]){
say(12,1022,671);
bitmod('s',evar(699),5);
}else{
say(12,819,671);
}
}else{
p18();
if (loop) return (0);
}}
loop=1; return(0);
return 0;
}
int p5(void)
{
if (!anyof(661,662,663,664,665,-1)) return 0;
say(64,1523,0);
 if (loop) return(0);return 0;
}
