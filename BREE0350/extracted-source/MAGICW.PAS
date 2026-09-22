{********>>>>>>>><<<<<<<<********

>>	MODULE:	MAGICW
		21-OCT-80

	This module contains the procedures MAGIC, ECHO, and FEEFOO.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	MAGICW
	.IDENT	/V0/    }

{******************* DATA STRUCTURE ROUTINES *************************}

FUNCTION VOCAB(ID:WORD;INIT:INTEGER):INTEGER;EXTERNAL;

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DROP(OBJ,LOC:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

{  *********************  VERB SAY ********************  }

PROCEDURE DUMPWD1;EXTERNAL;

{  This function is used by the SAYIT procedure to test for magic words
if used with the verb say, e.g. "say fee", "say fie", etc.  }

FUNCTION MAGIC:BOOLEAN;
BEGIN 
    { Check for magic words...  }
    KK:=VOCAB(WRD1,-1);
    IF (KK=62)OR(KK=65)OR(KK=71)OR(KK=2025) THEN BEGIN
	VERB:=0;
	MAGIC:=TRUE
    END ELSE BEGIN
	{  Check second word... }
	KK:=VOCAB(WRD2,-1);
	IF (KK=62)OR(KK=65)OR(KK=71)OR(KK=2025) THEN BEGIN
	    VERB:=0;
	    DUMPWD1;
	    MAGIC:=TRUE
	END
	ELSE MAGIC:=FALSE
    END
END;

PROCEDURE SMALL(WRD:WORD);EXTERNAL;

{  Procedure used by say to echo player input.  }
PROCEDURE ECHO;
BEGIN
    WRITELN;
    WRITE('Okay, "');
    IF WRD1<>'SAY  ' THEN BEGIN
	SMALL(WRD1);
	SMALL(WRD1X);

	IF (WRD2<>'     ')AND(WRD2<>'SAY  ') 
	THEN WRITE(' ')
    END;
    IF WRD2<>'SAY  ' THEN BEGIN
	SMALL(WRD2);
	SMALL(WRD2X)
    END;
    WRITELN('".')
END;

{*********************  FEE,FIE,FOE,FOO  *****************************}

{  FEE FIE FOE FOO (and FUM).  Advance to the next state if given in proper 
order (and no intervening input).  Look up WRD1 in section 3 of the vocabulary
to determine which word we've got.  Last word zips the eggs back to the Giant
Room (unless already there).   }

PROCEDURE FEEFOO;
BEGIN
	K:=VOCAB(WRD1,3);
	IF FOOBAR=(1-K) THEN BEGIN
		FOOBAR:=K;
		IF K<>4 THEN RSPEAK(54) ELSE BEGIN
			FOOBAR:=0;
			IF (PLACE[EGGS]=PLACE1[EGGS])
			OR(TOTING(EGGS) AND(LOCATION=PLACE1[EGGS]))
			THEN RSPEAK(42)
			ELSE BEGIN
				{  Bring back the Troll if he steals the eggs
				back before crossing...  }
				IF (PLACE[EGGS]=0)AND(PLACE[TROLL]=0)
				AND(PROP[TROLL]=0) THEN PROP[TROLL]:=1;
				IF LOCATION=PLACE1[EGGS] THEN K:=0
				ELSE IF HERE(EGGS) THEN K:=1
				ELSE K:=2;
				IF TOTING(EGGS) THEN DROP(EGGS,LOCATION);
				MOVE(EGGS,PLACE1[EGGS]);
				PSPEAK(EGGS,K)
			END
		END
	END
	ELSE IF FOOBAR<>0 THEN RSPEAK(151)
	ELSE RSPEAK(42)
END;

