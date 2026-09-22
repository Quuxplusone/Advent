{********>>>>>>>><<<<<<<<********

>>	MODULE:	PARSE
		17-OCT-80

		This module contains the procedures PARSE, DOVERB, and DOOBJ.
	This is the third and final "main" level in the overlay structure below 
	MAIN and RESPONSE.  The procedure PARSE is used to do a rough sort of 
	the player input commands and generally calls TRAVEL for motion verbs, 
	DOOBJ for object words, and DOVERB for action verbs.  

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	PARSE
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION PCT(N:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION VOCAB(ID: WORD; INIT: INTEGER): INTEGER;EXTERNAL;

{ **************** I/O PROCEDURES ***************** }

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

{ ****************** INPUT PARSING ********************* }

PROCEDURE TRANSITIVE;EXTERNAL;

PROCEDURE VERBHUH;EXTERNAL;

PROCEDURE DUMPWD1;EXTERNAL;

PROCEDURE ASKSCORE;EXTERNAL;

PROCEDURE QUITGAME;EXTERNAL;

PROCEDURE TAKEIT;EXTERNAL;

PROCEDURE LOCKIT;EXTERNAL;

PROCEDURE LAMPON;EXTERNAL;

PROCEDURE LAMPOFF;EXTERNAL;

PROCEDURE KILLIT;EXTERNAL;

PROCEDURE WHATYOUGOT;EXTERNAL;

PROCEDURE FILLIT;EXTERNAL;

PROCEDURE BLASTIT;EXTERNAL;

PROCEDURE FEEFOO;EXTERNAL;

PROCEDURE BRIEF;EXTERNAL;

PROCEDURE READIT;EXTERNAL;

PROCEDURE EATIT;EXTERNAL;

PROCEDURE DRINKIT;EXTERNAL;


PROCEDURE POURIT;EXTERNAL;

PROCEDURE SAVE;EXTERNAL;

PROCEDURE HOURS;EXTERNAL;

{  Procedure to analyze a verb.  Remember what it was and go back for analysis
of second word if not blank (or if verb isn't "say" (which snarfs arbitrary 
second word or input).  }
PROCEDURE DOVERB;
BEGIN
    { Save verb value. }
    VERB:=K;
    IF (WRD2<>'     ')AND(VERB<>SAY) THEN BEGIN
	DUMPWD1;
	NODUMP:=FALSE
    END
    {  If verb is "say" and we still have a WRD2 or if object has been given, 
    go do a transitive verb. }
    ELSE IF ((OBJ<>-1)AND(OBJ<>0))OR((VERB=SAY)AND(WRD2<>'     ')) 
    THEN TRANSITIVE 
    {  Otherwise, analyze an intransitive verb, i.e. no object given yet.  }
    ELSE CASE VERB OF 
	{take}	1:TAKEIT; 
	{  We require all these guys to specify objects. }
	{drop} 2,
	{say}  3,
	{wave} 9,
	{calm}10,
	{rub} 16,
	{toss}17,
	{find}19,
	{feed}21,
	{brek}28,
	{wake}29: VERBHUH;
	{ Other intransitive verbs have procedures... }
	{open} 4,
	{lock} 6: LOCKIT;
	{noth} 5: RSPEAK(54);
	{on}   7: LAMPON;
	{off}  8: LAMPOFF;
	{walk}11: RSPEAK(ACTSPK[VERB]);
	{kill}12: KILLIT;
	{pour}13: POURIT;
	{eat} 14: EATIT;
	{drnk}15: DRINKIT;
	{quit}18: QUITGAME;
	{inve}20: WHATYOUGOT;
	{fill}22: FILLIT;
	{blst}23: BLASTIT;
	{scor}24: ASKSCORE;
	{foo} 25: FEEFOO;
	{brf} 26: BRIEF;

	{read}27: READIT;
	{susp}30: SAVE;
	{hour}31: BEGIN
			MSPEAK(6);
			HOURS
	END
    END 
END;  

PROCEDURE TRAVEL;EXTERNAL;

PROCEDURE NOTHERE;EXTERNAL;

PROCEDURE DOWHAT;EXTERNAL;

PROCEDURE SAYIT;EXTERNAL;

FUNCTION TSTOBJ:BOOLEAN;EXTERNAL;

{  Procedure to analyze an object word.  See if the thing is here, whether
we've got a verb yet, and so on.  Object must be here unless verb is "find or
"inventory" (and no new verb yet to be analyzed).  Water and oil are also funny,
since they are never actually dropped at any location, but might be here inside
the bottle or as a feature of the LOCATION.  (see TSTOBJ)  }
PROCEDURE DOOBJ;

{  Local procedure to deal with object once it's determined to be here. }
PROCEDURE OBJECT;
BEGIN
    {  Go get second word, if there is one.  }
    IF WRD2<>'     ' THEN BEGIN
	DUMPWD1;
	NODUMP:=FALSE
    END
    {  Else if we've got a verb, analyze transitive.  }
    ELSE IF VERB<>0 THEN TRANSITIVE
    {  Otherwise, what do you want me to do with this thing?  }
    ELSE BEGIN
	DOWHAT;
	GOTBOTH:=FALSE
    END
END;

BEGIN { Doobj }
    { If second word available and is "say" then SAYIT.  }
    IF WRD2='SAY  ' THEN SAYIT ELSE BEGIN
	{  Save object definition.  }
	OBJ:=K;
	{ If the object is here then analyze... }
	IF AT(OBJ) OR HERE(OBJ) THEN OBJECT
	{  If the object is not here, check a few special cases....  }
	ELSE IF TSTOBJ THEN 
	{  If TSTOBJ returned true with OBJ=0, a knife disappeared
	so do nothing else analyze object. }
	IF OBJ=0 THEN ELSE OBJECT 
	{  If TSTOBJ returns false with OBJ=0, an object word has been 
	magically transformed into a travel verb... }

	ELSE IF OBJ=0 THEN TRAVEL
	{  Object need not be here for "find" or "inventory"... }
	ELSE IF ((VERB=FIND)OR(VERB=INVENTORY))AND(WRD2<>'     ') THEN OBJECT
	{  Otherwise, OBJ is just not anywhere in sight.  }
	ELSE NOTHERE
    END
END;

{  This procedure gets the player input word definition and calls a 
procedure to handle the word according to type.   }
PROCEDURE PARSE;
VAR I:INTEGER;
BEGIN
	NODUMP:=TRUE;
	{ If he's typing "west" instead of "w", then inform him he can
	abbreviate if he wants to.  }
	IF WRD1='WEST ' THEN BEGIN
		IWEST:=IWEST+1;
		IF IWEST>=5 THEN RSPEAK(17)
	END;
	{ If we already have an intransitive verb "say", echo this input... }
	IF VERB=SAY THEN TRANSITIVE ELSE BEGIN
	    {  Get first listed full definition of the input word.  }
	    KK:=VOCAB(WRD1,-1);
	    {  Extract the word type.  }
	    WORDTYPE:=KK DIV 1000;
	    {  Remove the wordtype from the definition.  }
	    K:=KK MOD 1000;
	    {  If it's defined, go according to word type....  }
	    IF KK<>-1 THEN CASE WORDTYPE OF
		0: TRAVEL;
		1: DOOBJ;
		2: DOVERB;
		3: RSPEAK(K)
	    END ELSE BEGIN  {  Gee! I don't understand!   }
		I:=60;
		IF PCT(25) THEN I:=61;
		IF PCT(25) THEN I:=13;
		RSPEAK(I)
	    END
	END
END;

