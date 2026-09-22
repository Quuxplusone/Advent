{********>>>>>>>><<<<<<<<********

>>	MODULE:	WIZMAG
		15-OCT-80

		This module contains the "wizardry" routines GETMAGIC, 
	WRTMAGIC, and CHKMAGIC.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	WIZMAG
	.IDENT /V0/    }

{ *************** DATA STRUCTURE ROUTINES ***************** }

FUNCTION RAN(RANGE:INTEGER):INTEGER;EXTERNAL;

{ ****************** MAGIC MODE PROCEDURES ********************* }

{  All of the data required for the wizardry routines, including saving games,
is stored encoded in one random access file of integers: ADVWIZ.
The current record usage assignments for this file are as follows,
	1..5	  --	MAGICWORD
	6..10	  --	MAGICNUMBER
	11..12	  --	WKDAY
	13..14	  --	WKEND
	15..16	  --	HOLID
	17	  --	HBEGIN
	18	  --	HEND
	19..38	  --	HNAME
	39	  --	SHORT
	40	  --	Count of saved games.
	41	  --	LATENCY
	42..61	  --	Name of saved game 1.
	62..81	  --	Name of saved game 2.
	82..101	  --	Name of saved game 3.
	102..354  --	Saved game 1.
	355..607  -- 	Saved game 2.
	608..860  --  	Saved game 3.
	861..1560 --	Message of the day.      }

PROCEDURE RDWIZ(VAR X:INTEGER);EXTERNAL;

TYPE ARR5=ARRAY[1..5] OF INTEGER;

{  The follwing three procedures are used by the WIZARD function to test for
wizards.  The first extracts the current magic word and number from ADVWIZ.  }
PROCEDURE GETMAGIC(VAR MAGICNUMBER:ARR5;VAR MAGICWORD:WORD);
VAR I:INTEGER;
BEGIN 
    {  Open magic parameters file, seek.  }
    RESET(ADVWIZ,'ADVWIZ.DTA/SEEK');
    {  Get present parameters.  }
    SEEK(ADVWIZ,1);
    FOR I:=1 TO 5 DO BEGIN
	RDWIZ(MAGICNUMBER[I]);
	MAGICWORD[I]:=CHR(MAGICNUMBER[I]+ORD('A'))
    END;
    FOR I:=1 TO 5 DO RDWIZ(MAGICNUMBER[I])
END;

{  This procedure is used to challenge the player with a random magic word. }
PROCEDURE WRTMAGIC(VAR MAGICWORD:WORD);
VAR I:INTEGER;
BEGIN
    FOR I:=1 TO 5 DO MAGICWORD[I]:=CHR(ORD('A')+RAN(25));
    WRITELN;

    WRITELN(MAGICWORD)
END;

{  This function returns true if the player input word is the correct function
of the random magic word, the time of day, and the magic number.  }
FUNCTION CHKMAGIC(WRD,MAGICWORD:WORD;MAGICNUMBER:ARR5):BOOLEAN;
VAR WRDX:WORD;
    I,MAGICTIME:INTEGER;
BEGIN
    MAGICTIME:=TRUNC(TIME);
    FOR I:=1 TO 5 DO WRDX[I]:=CHR(ORD('A')+((ORD(MAGICWORD[I])-ORD('A')+
    MAGICNUMBER[I]+MAGICTIME) MOD 26));
    IF WRD1=WRDX THEN CHKMAGIC:=TRUE ELSE CHKMAGIC:=FALSE
END;
 
