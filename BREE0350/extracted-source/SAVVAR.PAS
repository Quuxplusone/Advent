{********>>>>>>>><<<<<<<<********

>>	MODULE:	SAVVAR
		17-OCT-80

		This module contains the procedure SAVVAR which is used by
	SAVE to save key variable values required to restart a saved game.  

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	SAVVAR 
	.IDENT /V0/  }

{ ****************** SUSPEND PROCEDURE *********************** }

PROCEDURE DATIME(VAR D:UNSIGNED;VAR T:INTEGER);EXTERNAL;

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

PROCEDURE WRWIZ(J:INTEGER);EXTERNAL;

PROCEDURE WRWIZB(X:BOOLEAN);EXTERNAL;

{  The following procedure is used to save a copy of enough key variables
into ADVWIZ.DTA so that a player may resume his Adventure where he left off. 
Global variable I is a record pointer into ADVWIZ; refer to comment in WRWIZ. }
PROCEDURE SAVVAR(SAV:INTEGER);
VAR J,SAVET:INTEGER;
    SAVED:UNSIGNED;
BEGIN
    I:=253*(SAV-1)+102;
    SEEK(ADVWIZ,I); 
    DATIME(SAVED,SAVET);
    WRWIZ(SAVED);
    WRWIZ(SAVET);
    WRWIZ(LOCATION);
    WRWIZ(HOLDING);
    FOR J:=1 TO MAXTRS DO BEGIN
	WRWIZ(PLACE[J]);
	WRWIZ(FIXED[J]);
	WRWIZ(PROP[J])
    END;
    WRWIZ(TALLY);
    WRWIZ(TALLY2);
    WRWIZ(DFLAG);
    WRWIZ(TURNS);
    WRWIZ(IWEST);

    WRWIZ(KNFLOC);
    WRWIZ(NUMDIE);
    WRWIZ(DKILL);
    WRWIZ(CLOCK1);
    WRWIZ(CLOCK2);
    WRWIZ(LIMIT);
    FOR J:=1 TO 10 DO BEGIN
	WRWIZ(HINTLC[J]);
	WRWIZB(HINTED[J])
    END;
    FOR J:=1 TO 6 DO BEGIN
	WRWIZ(ODLOC[J]);
	WRWIZ(DLOC[J]);
	WRWIZB(DSEEN[J])
    END;
    WRWIZB(WZDARK);
    WRWIZB(LMWARN);
    WRWIZB(CLOSING);
    WRWIZB(PANIC);
    WRWIZB(CLOSED);
    WRWIZ(NEWLOC);
    WRWIZ(OLDLC2);
    WRWIZ(OLDLOC)
END;
