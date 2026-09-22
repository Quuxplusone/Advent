;>>	ADVENTURES IN PASCAL
;	Barry C. Breen
;	P.O. Box 1964
;	Bellevue, Washington  98009
;
;>>	FILE:	ADVEDT
;		26-MAY-81
;
;>>	History -- This version of the "Adventures" game is written in OMSI
;	PASCAL V1.2 running under RSX11M V3.2 BL26 on a PDP 11/23.  It contains
;	special display text files for the VT-100 which makes use of double
;	width and double height characters for special effect.
;
;>>	ADVENTURES MODULE EDITING COMMAND FILE
;	   Includes optional listing, compilation, improvement, assembly, 
;	taskbuild, and library management.
;
.ENABLE SUBSTITUTION
.DISABLE LOWERCASE
.XQT LBR ADVGBL.PAS=ADV.ULB/EX:ADVGBL
.STRT:
.ASKS MODULE Which module do you desire to work with?
.IF MODULE = "ADVGBL" .GOTO L2
.IF MODULE = "ADVBLD" .OR .IF MODULE = "ADVPAS" .GOTO L1
.WAIT LBR
.XQT LBR 'MODULE'.PAS=ADV.ULB/EX:'MODULE'
.GOTO L2
.L1:
.WAIT LBR
.XQT LBR 'MODULE'.ODL=ADV.ULB/EX:'MODULE'
.SETS EXT "ODL"
.GOTO L3
.L2:
.SETS EXT "PAS"
.L3:
.ASK GO1 Do you want to edit this module?
.IFF GO1 .GOTO L5
.L4:
.WAIT LBR
EDT 'MODULE'.'EXT'
.WAIT EDT
.ASK GO2 Replace in source library?
.IFT GO2 .XQT LBR ADV.ULB='MODULE'.'EXT'/RP
.L5:
.IF MODULE = "ADVGBL" .GOTO EXT2
.IF EXT <> "PAS" .GOTO EXT5
.ASK GO3 Should we compile this thing?
.IFF GO3 .GOTO EXT2
.SETF AUX
.SETS SWITCH "/E"
.GOTO PAGE2

; 
; ADVEDT.CMD -- PAGE 2
;
.PAGE2:
.IF MODULE = "PEEK" .SETS SWITCH ""
.IF MODULE = "POOF" .SETS SWITCH ""
.IF MODULE = "ADVFLS" .SETS SWITCH ""
.IF MODULE = "100FLS" .SETS SWITCH ""
.IF SWITCH = "" .SETT AUX
.IF MODULE = "MAIN" .SETS SWITCH ""
.WAIT LBR
PAS 'MODULE'=ADVGBL,'MODULE''SWITCH'
.WAIT PAS
.ASK GO Run the code improver?
.IFT GO RUN $IMP
.WAIT 
.ASK GO Is this worth assembling?
.IFT GO .GOTO MAC
.IFF GO .ASK GO Do you want another edit?
.IFT GO .GOTO L4
.GOTO EXT3
.MAC:
MAC 'MODULE'='MODULE'
.WAIT MAC
.IFT AUX .GOTO BLD2
.ASK GO Replace in object library?
.IFF GO .GOTO EXT4
.XQT LBR ADV='MODULE'/RP
.EXT5:
.ASK GO Are you prepared to build a new task image?
.IFT GO .GOSUB BUILD
.IF EXT <> "PAS" .GOTO EXT2
.EXT4:
.WAIT LBR
.ASK SAVE Save the external object file?
.WAIT PIP
.IFF SAVE .XQT PIP 'MODULE'.OBJ;*/DE
.EXT3:
.ASK SAVE Save the macro file?
.WAIT PIP
.IFF SAVE .XQT PIP 'MODULE'.MAC;*/DE
.EXT2:
.ASK SAVE Save the external source file?
.WAIT LBR
.WAIT PIP
.IFF SAVE .XQT PIP 'MODULE'.'EXT';*/DE
.SETF DEL1
.SETF DEL2
.IFT SAVE .ASK DEL1 Delete from source library?
.IFT DEL1 .ASK DEL1 **Are you sure?!!
.IFT DEL1 LBR ADV.ULB/DE:'MODULE'
.IF EXT <> "PAS" .GOTO EXT1
.GOTO PAGE3

;
;  ADVEDT.CMD -- PAGE 3
;
.PAGE3:
.IF MODULE = "ADVGBL" .GOTO EXT1
.IFT DEL1 .ASK DEL2 Delete from object library?
.IFT DEL2 LBR ADV/DE:'MODULE'
.EXT1:
.ASK PRT Do you want to print source listings?
.IFT PRT @ADVPRT
.ASK DONE Are we quite finished?
.IFF DONE .GOTO STRT
.XQT PIP ADVGBL.PAS;*/DE
.ASK COMP Compress libraries??
.IFT COMP .GOSUB CMP
.ASK PUR  Purge everything?
.IFT PUR .GOSUB PURGE
.EXIT
.CMP:
    .WAIT LBR
    LBR ADV/CO:10=ADV
    LBR ADV.ULB/CO:10=ADV.ULB
    LBR ADV/LE/FU,ADVOLB.DIR/-SP
    LBR ADV.ULB/FU,ADVULB.DIR/-SP
    .WAIT PIP
    PIP ADV.*;*,*.DIR;*/LI
.RETURN
.PURGE:
    .WAIT PIP
    PIP *.*/PU
    PIP *.*/TR
    .XQT PIP/LI
.RETURN
.BUILD:
    .WAIT LBR
    .XQT LBR ADVBLD.ODL=ADV.ULB/EX:ADVBLD
    .SETS MAP ""
    .SETS CRF ""
    .SETF CROSS
    .ASK OPTION Do you want a map?
    .IFT OPTION .SETS MAP ",ADVENTURE/-SP"
    .WAIT LBR
    .XQT LBR ADVPAS.ODL=ADV.ULB/EX:ADVPAS
    .IFT OPTION .ASK CROSS With a cross-reference listing?
    .IFT CROSS .SETS CRF "/CR"
    .WAIT LBR
    TKB ADVENTURE/CP/FP'MAP''CRF'=ADVBLD/MP
    .WAIT TKB
    PIP ADVBLD.ODL;*/DE
    .XQT PIP ADVPAS.ODL;*/DE
    .IFF OPTION .RETURN
    .IFT CROSS .WAIT CRF
    .IFACT QMG... PRINT ADVENTURE.MAP/DE
    .IFACT QMG... .RETURN
.GOTO PAGE4

;
;  ADVEDT.CMD -- PAGE 4
;
.PAGE4:
    PIP ADVENTURE.MAP/SP
    .WAIT PIP
    .WAIT PRT...
    PIP ADVENTURE.MAP;1/DE
    .RETURN
	
.BLD2:
    .SETS LIB2 ",ADV/LB"
    .IF MODULE <> "PEEK" .SETS LIB2 ""
    .ASK GO Are you prepared to build a new task image?
    .IFT GO TKB 'MODULE'/CP/FP='MODULE''LIB2',RUNLIB/LB 
    .WAIT TKB
.GOTO EXT4
