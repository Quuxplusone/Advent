;>>	ADVENTURES IN PASCAL
;	Barry C. Breen
;	P.O. Box 1964
;	Bellevue, Washington  98009
;
;>>	FILE:	ADVBLD
;		3-NOV-80
;
;>>	History -- This version of the "Adventures" game is written in OMSI
;	PASCAL V1.2 running under RSX11M V3.2 BL26 on a PDP 11/23.  It contains
;	special display text files for the VT-100 which makes use of double
;	width and double height characters for special effect.
;
;>>	ADVENTURES TASKBUILD COMMAND FILE
;
.ENABLE SUBSTITUTION
    .WAIT LBR
    .XQT LBR ADVBLD.ODL=ADV.ULB/EX:ADVBLD
    .SETS MAP ""
    .SETS CRF ""
    .SETF CROSS
    .ASK OPTION Do you want a map?
    .IFT OPTION .SETS MAP ",ADVENTURE"
    .WAIT LBR
    .XQT LBR ADVPAS.ODL=ADV.ULB/EX:ADVPAS
    .IFT OPTION .ASK CROSS With a cross-reference listing?
    .IFT CROSS .SETS CRF "/CR"
    .WAIT LBR
    TKB ADVENTURE/CP/FP'MAP''CRF'=ADVBLD/MP
    .WAIT TKB
    PIP ADVBLD.ODL;*/DE
    .XQT PIP ADVPAS.ODL;*/DE
    .IFF OPTION .EXIT
    .IFT CROSS .WAIT CRF...
    .WAIT PRT...
    .WAIT PIP
    .XQT PIP ADVENTURE.MAP;*/DE
.EXIT
