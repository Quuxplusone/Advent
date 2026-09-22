{********>>>>>>>><<<<<<<<********

>>	MODULE:	VERBS4
		29-OCT-80

		This module contains the verb procedures ASKSCORE, which 
	responds to the "score" command, SAVE, which responds to the verbs
	"pause", "save","suspend", and three procedures used to write saved
	game info to ADVWIZ.DTA: WRWIZ, WRWRD, and WRWIZB.  

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	VERBS4 
	.IDENT /V0/  }

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

FUNCTION ASKR(I,J,K: INTEGER): BOOLEAN;EXTERNAL;

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

{*********************  ASK SCORE PROCEDURE  *****************************}

PROCEDURE CALSCORE;EXTERNAL;

PROCEDURE FINISH;EXTERNAL;

{  Scoring command comes here....  }
PROCEDURE ASKSCORE;
BEGIN
    SCORING:=TRUE;
    CALSCORE;
    SCORING:=FALSE;
    WRITELN;
    WRITE('If you were to quit now, you would score',SCORE:4);
    WRITELN(' out of a possible',MAXSCOR:4,'.');
    GAVEUP:=ASKR(143,54,54);
    IF GAVEUP THEN FINISH
END;

{ ****************** SUSPEND PROCEDURE *********************** }

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

{ Procedure to write one integer to ADVWIZ.  Global variable I is
presumed to be current record count.  This kluge is because sometimes the
"PUT" function dosen't advance through the file sequentially as it should.

I have yet to figure this bug out, so I throw in redundant seeks.  }
PROCEDURE WRWIZ(J:INTEGER);
BEGIN
    ADVWIZ^:=J;
    PUT(ADVWIZ);
    I:=I+1;
    SEEK(ADVWIZ,I)
END;

{ Procedure to write one boolean value to ADVWIZ. }
PROCEDURE WRWIZB(X:BOOLEAN);
BEGIN
    IF X THEN WRWIZ(1) ELSE WRWIZ(0)
END;

{ Procedure to write four player input words to ADVWIZ.  }
PROCEDURE WRWRD(WRD1,WRD1X,WRD2,WRD2X:WORD);
VAR I:INTEGER;
BEGIN
    FOR I:=1 TO 5 DO WRWIZ(ORD(WRD1[I])-ORD('A'));
    FOR I:=1 TO 5 DO WRWIZ(ORD(WRD1X[I])-ORD('A'));
    FOR I:=1 TO 5 DO WRWIZ(ORD(WRD2[I])-ORD('A'));
    FOR I:=1 TO 5 DO WRWIZ(ORD(WRD2X[I])-ORD('A'))
END;

PROCEDURE SAVNAME(VAR SAV:INTEGER);EXTERNAL;

PROCEDURE SAVVAR(SAV:INTEGER);EXTERNAL;

{ "Suspend" -- Offer to exit, while saving all key variables,  but  requiring  a 
delay before restarting (this was originally  so  he  couldn't  save  the  world 
before trying something risky,  but  in  this  version he only gets to restart a 
saved game  once  anyway.  The latency requirement was left in for compatability
with the original program's behaviour).  
	This procedure uses external procedures SAVNAM and SAVVAR  to  save  the
player's name and current variable values, respectively, in ADVWIZ. }
PROCEDURE SAVE;
VAR SAV:INTEGER;
BEGIN  
    IF DEMO THEN RSPEAK(201) ELSE BEGIN
	RESET(ADVWIZ,'ADVWIZ.DTA/RW/SEEK');
	SEEK(ADVWIZ,40);
	IF ADVWIZ^=3 THEN BEGIN
	    MSPEAK(49);
	    ASKSCORE
	END ELSE BEGIN
	    GET(ADVWIZ);
	    MSPEAK(50);
	    WRITELN('wait at least ',ADVWIZ^:3,' minutes before continuing.');
	    IF ASKR(200,54,54) THEN BEGIN
		SAVNAME(SAV);
		SAVVAR(SAV);
		MSPEAK(51);
		DONE:=TRUE;
		MOVED:=TRUE
	    END

	END;
	CLOSE(ADVWIZ)
    END
END;

