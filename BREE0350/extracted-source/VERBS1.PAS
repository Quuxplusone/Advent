{********>>>>>>>><<<<<<<<********

>>	MODULE:	VERBS1
		21-OCT-80

		This module contains the procedures LOCKED, LOCKIT, READOBJ,
	 and READIT.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	VERBS1
	.IDENT	/V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DROP(OBJ,LOC:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION DARK:BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

FUNCTION ASKR(I,J,K:INTEGER):BOOLEAN;EXTERNAL;

{***************  VERBS UNLOCK/LOCK (TRANSITIVE) ***************}

{ Lock, unlock an object. Special stuff for opening clam/oyster and for chain. }
PROCEDURE LOCKED;
VAR I:INTEGER;
BEGIN
    CASE OBJ OF
	CLAM,
	OYSTER: BEGIN
	    { I is used to keep track of whether referring to clam or oyster.  }
	    IF OBJ=CLAM THEN I:=0 ELSE I:=1;
	    {  Must put it down before attempts to open it.  }
	    IF TOTING(OBJ) THEN RSPEAK(120+I)
	    {  Trident is the only thing strong enough to open it. }
	    ELSE IF NOT TOTING(TRIDENT) THEN RSPEAK(122+I)
	    {  If trying to lock it.. what? }
	    ELSE IF VERB=LOCK THEN RSPEAK(61)
	    {  Else if clam, open and drop pearl. "..this must be an oyster."  }
	    ELSE IF I<>1 THEN BEGIN
		DESTROY(CLAM);
		MOVE(OYSTER,LOCATION);
		MOVE(PEARL,105);
		RSPEAK(124)
	    END
	    {  Opening oyster gets nothing...  }
	    ELSE RSPEAK(125)
	END;
	{  Door may still be too rusty...  }
	DOOR: IF PROP[DOOR]=1 THEN RSPEAK(54) ELSE RSPEAK(111);
	{  The cage isn't locked...  }
	CAGE: RSPEAK(32);
	{  Trying to lock/unlock the keys???  }

	KEYS: RSPEAK(55);
	{  Need keys for the chain..  }
	CHAIN: IF NOT HERE(KEYS) THEN RSPEAK(31)
	{  Got keys, wants to lock it...  }
	ELSE IF VERB=LOCK THEN
	{  Maybe it's already locked...  }
	IF PROP[CHAIN]<>0 THEN RSPEAK(34)
	{  Or else there's nothing here to lock it to.  }
	ELSE IF LOCATION<>PLACE1[CHAIN] THEN RSPEAK(173)
	{  Okay, lock it back to the wall.  }
	ELSE BEGIN
	    PROP[CHAIN]:=2;
	    IF TOTING(CHAIN) THEN DROP(CHAIN,LOCATION);
	    FIXED[CHAIN]:=-1;
	    RSPEAK(172)
	END
	{ Wants to unlock it but the bear's still dangerous..  }
	ELSE IF PROP[BEAR]=0 THEN RSPEAK(41)
	{ Maybe it's already unlocked..  }
	ELSE IF PROP[CHAIN]=0 THEN RSPEAK(37)
	{  Okay, unlock it.  }
	ELSE BEGIN
	    PROP[CHAIN]:=0;
	    FIXED[CHAIN]:=0;
	    IF PROP[BEAR]<>3 THEN PROP[BEAR]:=2;
	    FIXED[BEAR]:=2-PROP[BEAR];
	    RSPEAK(171)
	END;
	{ Grate.. can't be closing or this exit's closed.  }
	GRATE: IF CLOSING THEN BEGIN
	    IF NOT PANIC THEN CLOCK2:=15;
	    PANIC:=TRUE;
	    RSPEAK(130)
	END
	{  If he has no keys...  }
	ELSE IF NOT HERE(KEYS) THEN RSPEAK(31)
	{  Otherwise lock/unlock the grate.  }
	ELSE BEGIN
	    I:=34+PROP[GRATE];
	    IF VERB=LOCK THEN PROP[GRATE]:=0
	    ELSE PROP[GRATE]:=1;
	    I:=I+2*PROP[GRATE];
	    RSPEAK(I)
	END;
	{ All other objects can't be locked/unlocked...  }
	ELSE RSPEAK(ACTSPK[VERB])
    END
END;

{ ************** VERBS LOCK/UNLOCK (INTRANSITIVE) ****************** }

PROCEDURE VERBHUH;EXTERNAL;

{ Intransitive case of verbs "open","unlock","close","lock",etc.  If he wants 
to lock or unlock something fixed, we see if it's here.  If the chain is here 
too, then "...what?", otherwise if the chain is the only thing here, then go 
use the chain as OBJ.  }

PROCEDURE LOCKIT;
BEGIN
    IF HERE(CLAM) THEN OBJ:=CLAM ELSE IF HERE(OYSTER) THEN OBJ:=OYSTER
    ELSE IF AT(DOOR) THEN OBJ:=DOOR ELSE IF AT(GRATE) THEN OBJ:=GRATE;
    IF (OBJ<>0) AND HERE(CHAIN) THEN VERBHUH ELSE BEGIN
	IF HERE(CHAIN) THEN OBJ:=CHAIN;
	IF OBJ=0 THEN RSPEAK(ACTSPK[VERB]) ELSE LOCKED
    END
END;

{********************  VERB READ (INTRANSITIVE)  *********************}

PROCEDURE READOBJ;FORWARD;

{ "read" , no object given.  READOBJ if only one thing present, else ...what?  }
PROCEDURE READIT;
VAR J:INTEGER;
BEGIN
    J:=0;
    IF HERE(MAGAZINE) THEN BEGIN
	J:=J+1;
	OBJ:=MAGAZINE
    END;
    IF HERE(TABLET) THEN BEGIN
	J:=J+1;
	OBJ:=TABLET
    END;
    IF HERE(MESSAGE) THEN BEGIN
	J:=J+1;
	OBJ:=MESSAGE
    END;
    IF CLOSED AND TOTING(OYSTER) THEN BEGIN
	J:=J+1;
	OBJ:=OYSTER
    END;
    IF (J>1)OR(OBJ=0)OR DARK THEN VERBHUH ELSE READOBJ
END;

{******************  VERB READ (TRANSITIVE)  **********************}

PROCEDURE NOTHERE;EXTERNAL;

{ "read" -- Magazines in dwarvish, message we've seen, and ...oyster?  }
PROCEDURE READOBJ;
BEGIN
    IF DARK THEN NOTHERE ELSE CASE OBJ OF
	MAGAZINE: RSPEAK(190);
	TABLET: RSPEAK(196);
	MESSAGE: RSPEAK(191);
	OYSTER: IF NOT CLOSED OR NOT TOTING(OYSTER) THEN RSPEAK(ACTSPK[VERB])
	ELSE IF HINTED[2] THEN RSPEAK(194) ELSE HINTED[2]:=ASKR(192,193,54);
	ELSE RSPEAK(ACTSPK[VERB])
    END
END;

