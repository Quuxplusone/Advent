{********>>>>>>>><<<<<<<<********

>>	MODULE:	PRIMET
		1-OCT-80

		This module contains the "wizardry" routine PRIMETIME.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE	PRIMET
	.IDENT /V0/   }

{ ******************* MAGIC MODE PROCEDURES ****************** }

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

{  This procedure is used to extract the latest "prime time" info from the 
magic parameters file.  }
PROCEDURE PRIMETIME;
TYPE	DBL=ARRAY[1..2] OF INTEGER;
VAR I,J:INTEGER;

{  Local procedure to read two words at a time.  }
PROCEDURE READ2(VAR X:DBL);
VAR I:INTEGER;
BEGIN
    FOR I:=1 TO 2 DO RDWIZ(X[I])
END;
BEGIN
    {  Open the magic parameters file.  }
    RESET(ADVWIZ,'ADVWIZ.DTA/SEEK');
    {  Read the latest "prime time" information.  }
    SEEK(ADVWIZ,11);
    READ2(WKDAY);
    READ2(WKEND);
    READ2(HOLID);
    RDWIZ(HBEGIN);
    RDWIZ(HEND);
    FOR I:=1 TO 20 DO BEGIN
	RDWIZ(J);
	HNAME[I]:=CHR(J+ORD('A'))
    END;
    CLOSE(ADVWIZ)
END;

