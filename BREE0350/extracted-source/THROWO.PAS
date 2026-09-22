{********>>>>>>>><<<<<<<<********

>>	MODULE:	THROWO
		16-OCT-80

		This module contains the procedure THROWOBJ.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	THROWO
	.IDENT	/V0/	 }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION RAN(RANGE:INTEGER):INTEGER;EXTERNAL;

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DROP(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

{ ******************* VERB THROW ****************** }

PROCEDURE DROPOBJ;EXTERNAL;

PROCEDURE FEEDIT;EXTERNAL;

PROCEDURE KILLIT;EXTERNAL;

PROCEDURE SNARF;EXTERNAL;

PROCEDURE THROWAXE;EXTERNAL;

{ "throw" -- Same as discard unless axe.  Then same as attack except ignore 
bird and if dwarf present then one might be killed (only way to do so!).
Axe is also special for dragon, bear, and troll.  Treasures special for troll. }
PROCEDURE THROWOBJ;
VAR	I:INTEGER;
BEGIN
	IF TOTING(ROD2) AND(OBJ=ROD)AND NOT TOTING(ROD) THEN OBJ:=ROD2;
	IF NOT TOTING(OBJ) THEN RSPEAK(ACTSPK[VERB])
	ELSE IF (OBJ>=50)AND(OBJ<=MAXTRS)AND AT(TROLL) THEN 
	{ Snarf a treasure for the troll! } 
	SNARF
	ELSE IF (OBJ=FOOD)AND HERE(BEAR) THEN BEGIN
		{ Throwing food at the bear is okay! }
		OBJ:=BEAR;
		FEEDIT
	END
	{ Anything else other than the axe, drop it.  }
	ELSE IF OBJ<>AXE THEN DROPOBJ
	{ Else he's thrown his axe at something...  }
	ELSE THROWAXE; 

	IF OBJ=0 THEN KILLIT
END;

