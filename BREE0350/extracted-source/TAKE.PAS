{********>>>>>>>><<<<<<<<********

>>	MODULE:	TAKE
		21-OCT-80

		This module contains the procedures GETOBJ and TAKEIT, which 
	along with procedure GETIT, implement carrying an object.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE TAKE
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION LIQ:INTEGER;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

{ ******************* VERB GET (CARRY, TAKE, etc. )******************* }

{   Carry an object.  Special cases for bird and cage  (if bird in cage, can't
take one without the other.   Liquids also special, since they depend on status
of bottle.  Also various side effects, etc.   }
PROCEDURE FILLIT;EXTERNAL;

PROCEDURE GETIT;EXTERNAL;

PROCEDURE GETOBJ;
BEGIN
    {  "You're already carrying it!"  }
    IF TOTING(OBJ) THEN RSPEAK(ACTSPK[VERB])
    {  "You can't get it....   }
    ELSE IF FIXED[OBJ]<>0 THEN BEGIN
	IF (OBJ=PLANT)AND(PROP[PLANT]<=0) THEN RSPEAK(115)
	ELSE IF (OBJ=BEAR)AND(PROP[BEAR]=1) THEN RSPEAK(169)
	ELSE IF (OBJ=CHAIN)AND(PROP[BEAR]<>0) THEN RSPEAK(170)
	ELSE RSPEAK(25)
    END
    {  Handle liquids..  }
    ELSE IF (OBJ=WATER)OR(OBJ=OIL) THEN BEGIN
	{  If we're not referring to liquid in bottle, then..  }
	IF NOT HERE(BOTTLE) OR (LIQ<>OBJ) THEN BEGIN
	    OBJ:=BOTTLE;
	    {  If toting empty bottle, then fill it.  }
	    IF TOTING(BOTTLE) AND (PROP[BOTTLE]=1) THEN FILLIT
	    {  Otherwise,  }
	    ELSE BEGIN
		{  You either can't carry the liquid, }
		IF NOT TOTING(BOTTLE) THEN RSPEAK(104)
		{  Or your bottle's already full.  }
		ELSE IF PROP[BOTTLE]<>1 THEN RSPEAK(105)
	    END
	END
	{  If the liquid he wants is in the bottle, then get the bottle...   }
	ELSE BEGIN
	    OBJ:=BOTTLE;
	    GETIT
	END
    END
    {  Not carrying it, not fixed, not a liquid, so go get it.  }

    ELSE GETIT
END;

{ ********************* VERB TAKE (INTRANSITIVE) ****************** }

PROCEDURE VERBHUH;EXTERNAL;

{  Intransitive verb "take" ("get",etc.).  Take what's here only if one thing 
to take.  }
PROCEDURE TAKEIT;
BEGIN
    IF (ATLOC[LOCATION]<>NIL)AND(ATLOC[LOCATION]^.NXT=NIL) THEN BEGIN
	{ Check if there's a dwarf here also.  }
	FOR I:=1 TO 5 DO IF (DLOC[I]=LOCATION)AND(DFLAG>=2) THEN BEGIN
	    VERBHUH;
	    EXIT
	END;
	{ If there was a dwarf then GOTBOTH will be false. }
	IF GOTBOTH THEN BEGIN
	    OBJ:=ATLOC[LOCATION]^.OBJ;
	    GETOBJ
	END
    END ELSE VERBHUH;
END;

