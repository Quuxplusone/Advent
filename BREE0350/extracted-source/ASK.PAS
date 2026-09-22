{********>>>>>>>><<<<<<<<********

>>	MODULE:	ASK
		29-OCT-80

		This module contains the procedures ASK, ASKR, ASKM, and
	WIZARD.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE ASK
	.IDENT /V0/   }

{ ****************** I/O SUBROUTINES ****************** }

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE GETIN(VAR WORD1,WORD1X,WORD2,WORD2X: WORD);EXTERNAL;

{  This function asks question I using output procedure SPK (which could
be RSPEAK or MSPEAK) and returns a value true for a 'yes' and outputs
message J or returns a value false for a 'no' and outputs message K.  }
FUNCTION ASK(I,J,K: INTEGER; PROCEDURE SPK): BOOLEAN;
BEGIN
    REPEAT  { Until question answered, WRD1[1]= 'Y' OR 'N' }
	{ Ask the question }
	SPK(I);
	{ Get the player's response }
	GETIN(WRD1,WRD1X,WRD2,WRD2X);
	CASE WRD1[1] OF
		{ Player responded yes }
		'Y': BEGIN
			SPK(J);
			ASK:=TRUE
		END;
		{ Player responded no }
		'N': BEGIN
			SPK(K);
			ASK:=FALSE
		END;
		{ Avoiding the question is not allowed! }
		ELSE RSPEAK(202)
	END
    UNTIL (WRD1[1]='Y')OR(WRD1[1]='N')
END;

{  Ask a question and respond from random messages.  }
FUNCTION ASKR(I,J,K: INTEGER): BOOLEAN;
BEGIN
	ASKR:=ASK(I,J,K,RSPEAK)
END;

{  Ask a question and respond from magic messages.  }
FUNCTION ASKM(I,J,K: INTEGER): BOOLEAN;
BEGIN
	ASKM:=ASK(I,J,K,MSPEAK)
END;

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

TYPE ARR5=ARRAY[1..5] OF INTEGER;

PROCEDURE GETMAGIC(VAR MAGICNUMBER:ARR5;VAR MAGICWORD:WORD);EXTERNAL;

PROCEDURE WRTMAGIC(VAR MAGICWORD:WORD);EXTERNAL;

FUNCTION CHKMAGIC(WRD,MAGICWORD:WORD;MAGICNUMBER:ARR5):BOOLEAN;EXTERNAL;

{  This function tests for Wizards and returns a value of true if he passes.  }
FUNCTION WIZARD:BOOLEAN;
VAR MAGICWORD:WORD;
    MAGICNUMBER:ARR5;

{  Local procedure to deal with imposters... }
PROCEDURE IMPOSTER;
{  Aha! An imposter!  }
BEGIN
    MSPEAK(20);
    WIZARD:=FALSE
END;

BEGIN { Wizard }
    { Get the current magic word and number. }
    GETMAGIC(MAGICNUMBER,MAGICWORD);
    {  Ask if he's a wizard.  If he says yes, make him prove it.  Return 
    true if he really is a wizard.  }
    IF ASKM(16,0,7) THEN BEGIN
	{  He says he is.  First step: does he know anything magical?  }
	MSPEAK(17);
	GETIN(WRD1,WRD1X,WRD2,WRD2X);
	IF WRD1<>MAGICWORD THEN IMPOSTER
	{  He does.  Give him a random challenge and check his reply. }
	ELSE IF ASKM(18,0,0) THEN IMPOSTER
	ELSE BEGIN
	    {  Get random magic word and write it.  }
	    WRTMAGIC(MAGICWORD);
	    {  Get his reply....  }
	    GETIN(WRD1,WRD1X,WRD2,WRD2X);
	    {  If he replied correctly... }
	    IF CHKMAGIC(WRD1,MAGICWORD,MAGICNUMBER) THEN BEGIN 

		{  By golly, he really *is* a wizard...  }
		MSPEAK(19);
		WIZARD:=TRUE
	    END ELSE IMPOSTER
	END
    END ELSE WIZARD:=FALSE;
    CLOSE(ADVWIZ)
END;

