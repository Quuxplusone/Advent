{********>>>>>>>><<<<<<<<********

>>	MODULE:	DATIME
		1-OCT-80

	This module contains the "wizardry" routine DATIME.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE	DATIME
	.IDENT /V0/   }

{ ********************** MAGIC MODE PROCEDURES ****************** }

{  This procedure returns the date and time in D and T.  D is the number of
days since 01-JAN-77, T is minutes past midnight.  Since OMSI hasn't a date
function as yet, and I couldn't get their fortran external call to IDATE to 
work right,  I used the time parameters macro.   }

{$C	.MCALL	GTIM$C
TIMBUF:	.WORD	0,0,0,0,0,0,0,0  }

PROCEDURE DATIME(VAR D:UNSIGNED;VAR T:INTEGER);

VAR I,MON,MO,DAY,YEAR:INTEGER;
    HATH:ARRAY[1..12] OF INTEGER;

BEGIN
    HATH[2]:=28;
    FOR I:=1 TO 4 DO HATH[2*I-1]:=31;
    FOR I:=1 TO 3 DO HATH[2*I+6]:=31;
    FOR I:=1 TO 2 DO HATH[2*I+2]:=30;
    FOR I:=1 TO 2 DO HATH[2*I+7]:=30;
{$C	GTIM$C  TIMBUF
	MOV	TIMBUF+G.TIMO,MO(%6)
	MOV	TIMBUF+G.TIDA,DAY(%6)
	MOV	TIMBUF+G.TIYR,YEAR(%6)    }
    T:=ROUND(TIME*60.0);
    D:=DAY;
    FOR MON:=1 TO 12 DO BEGIN
	IF MO=MON THEN EXIT;
	D:=D+HATH[MON]
    END;
    D:=D+(YEAR-77)*365+YEAR DIV 4;
    IF ((YEAR MOD 4)=0)AND((YEAR MOD 100)<>0)AND(MON>2) THEN D:=D+1
END;
