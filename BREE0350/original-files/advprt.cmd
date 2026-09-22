;>>	ADVENTURES IN PASCAL
;	Barry C. Breen
;	P.O. Box 1964
;	Bellevue, Washington  98009
;
;>>	FILE:	ADVPRT.CMD
;		13-NOV-80
;
;>>	History -- This version of the "Adventures" game is written in OMSI
;	PASCAL V1.2 running under RSX11M V3.2 BL26 on a PDP 11/23.  It contains
;	special display text files for the VT-100 which makes use of double
;	width and double height characters for special effect.
;
;>>	PRINT SOURCE LISTINGS COMMAND FILE
;
;
.DISABLE LOWERCASE
.ENABLE SUBSTITUTION
.XQT LBR HDR.TXT;1=ADV.ULB/EX:ADVHDR
.RPT:
.ASKS FILE Tell me which module you want listed (omit extension).
.WAIT PIP
.WAIT LBR
.XQT LBR SOURCE.SRC;1=ADV.ULB/EX:'FILE'
.SETS EXT "PAS"
.IF FILE = "ADVPAS" .SETS EXT "ODL"
.IF FILE = "ADVBLD" .SETS EXT "ODL"
.WAIT LBR
PIP 'FILE'.'EXT';1=HDR.TXT;1,SOURCE.SRC;1
.IFACT QMG... PRINT 'FILE'.'EXT';1/DE
.IFNACT QMG... PIP 'FILE'.'EXT';1/SP
.XQT PIP SOURCE.SRC;1/DE
.ASK DONE Are you quite finished listing things yet??
.IFF DONE .GOTO RPT
.WAIT PIP
.XQT PIP HDR.TXT;1/DE
.WAIT PRT...
.WAIT PIP
.IFNACT QMG... PIP *.PAS;*/DE
.XQT PIP *.ODL;*/DE
.EXIT
