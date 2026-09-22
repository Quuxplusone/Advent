{********>>>>>>>><<<<<<<<********

>>	MODULE:	VERBS5
		21-OCT-80

		This module contains the procedures QUITGAME, WHATYOUGOT,
	BRIEF, and TSTOBJ. 

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	VERBS5
	.IDENT /V0/     }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION LIQ:INTEGER;EXTERNAL;

FUNCTION LIQLOC(LOC:INTEGER):INTEGER;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

FUNCTION ASKR(I,J,K: INTEGER): BOOLEAN;EXTERNAL;

{*************  MAIN PROGRAM PROCEDURES  ********************}

PROCEDURE FINISH;EXTERNAL;

{*********************  QUIT PROCEDURE  *****************************}

{ "quit" --  Intransitive only.  Verify intent and exit if that's what 
he wants.  }
PROCEDURE QUITGAME;
BEGIN
    GAVEUP:=ASKR(22,54,54);
    IF GAVEUP THEN FINISH
END;

{*********************  INVENTORY PROCEDURE  *****************************}

{ "inventory" -- Report on the current burden.  }
PROCEDURE WHATYOUGOT;
VAR LINK:OBJLINK;
BEGIN
    LINK:=ATLOC[-1];
    IF LINK=NIL THEN RSPEAK(98) ELSE BEGIN
	RSPEAK(99);
	WHILE LINK<>NIL DO BEGIN
	    IF LINK^.OBJ<>BEAR THEN PSPEAK(LINK^.OBJ,-1);
	    LINK:=LINK^.NXT
	END;
	IF TOTING(BEAR) THEN RSPEAK(141)
    END
END;

{ ********************* VERB BRIEF ****************** }

{  Intransitive verb "brief".  If he wants brief descriptions, skyrocket 

ABBNUM and force removal of "detail" response to "look".  }
PROCEDURE BRIEF;
BEGIN
    RSPEAK(156);
    ABBNUM:=10000;
    DETAIL:=6
END;

{ ***************  OBJECT PROCEDURES ******************** }

{ Function to test if special case objects are here.  Returns true with OBJ<>0
if object is here; true with OBJ=0 if object disappears (dwarf's knife); false 
with OBJ<>0 if it is not here; and false with OBJ=0 if object (grate) has been
replaced by a travel verb.  }
FUNCTION TSTOBJ:BOOLEAN;
VAR X:BOOLEAN;
BEGIN
    CASE OBJ OF
	{  Interpret "grate" as travel verb "depression" if near but not at the 
	grate.  Other places outside the cave use "entrance".  Return false with
	OBJ=0 if this is the case.  Otherwise return false with OBJ<>0.  }
	GRATE: BEGIN
	    TSTOBJ:=FALSE;
	    IF (LOCATION=1)OR(LOCATION=4)OR(LOCATION=7) THEN K:=DEPRESSION
	    ELSE IF (LOCATION>9)AND(LOCATION<15) THEN K:=ENTRANCE;
	    IF K<>GRATE THEN OBJ:=0
	END;
	{  Dwarf?  See if one's here. }
	DWARF: BEGIN
	    X:=FALSE;
	    FOR I:=1 TO 5 DO
	    IF (DLOC[I]=LOCATION)AND(DFLAG>=2) THEN X:=TRUE;
	    TSTOBJ:=X
	END;
	{  Plant? Might be phony plant (seen in twopit room only when real 
	plant is large enough).  }
	PLANT: IF AT(PLANT2) AND(PROP[PLANT2]<>0) THEN BEGIN
	    OBJ:=PLANT2;
	    TSTOBJ:=TRUE
	END ELSE TSTOBJ:=FALSE;
	{  Dwarves' knives disappear.......  }
	KNIFE: IF KNFLOC=LOCATION THEN BEGIN
	    TSTOBJ:=TRUE;
	    OBJ:=0;
	    KNFLOC:=-1;
	    RSPEAK(116)
	END ELSE TSTOBJ:=FALSE;
	{  Rod may be special rod...  }
	ROD: IF HERE(ROD2) THEN BEGIN
	    OBJ:=ROD2;
	    TSTOBJ:=TRUE
	END ELSE TSTOBJ:=FALSE;
	{  If not one of the above objects, check for water and oil...   }
	ELSE IF ((K=LIQ)AND HERE(BOTTLE))OR(K=LIQLOC(LOCATION)) 
	THEN TSTOBJ:=TRUE ELSE TSTOBJ:=FALSE
    END  { Case of obj }
END;


