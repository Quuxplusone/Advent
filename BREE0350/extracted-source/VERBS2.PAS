{********>>>>>>>><<<<<<<<********

>>	MODULE:	VERBS2
		26-MAY-81

		This module contains the procedures EATIT, EATOBJ, BLASTIT,
	DRINKIT, and POURIT.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	VERBS2
	.IDENT	/V0/	 }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE DROP(OBJ,LOC:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION LIQ:INTEGER;EXTERNAL;

FUNCTION LIQLOC(LOC:INTEGER):INTEGER;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

{**************** VERB EAT (INTRANSITIVE)  ******************}

PROCEDURE VERBHUH;EXTERNAL;

{ "eat" -- intransitive: assume food if present, else ask what.  }
PROCEDURE EATIT;
BEGIN
	IF NOT HERE(FOOD) THEN VERBHUH ELSE BEGIN
		IF TOTING(FOOD) THEN DROP(FOOD,LOCATION);
		DESTROY(FOOD);
		RSPEAK(72)
	END
END;

{****************  VERB EAT (TRANSITIVE)   ********************}

{ "eat" -- Food okay, some things lose appetite, others are ridiculous.  }
PROCEDURE EATOBJ;
BEGIN
	IF OBJ=FOOD THEN EATIT ELSE CASE OBJ OF
		BIRD,SNAKE,CLAM,OYSTER,DWARF,DRAGON,TROLL,BEAR:RSPEAK(71);
		ELSE RSPEAK(ACTSPK[VERB])
	END
END;

{*************  MAIN PROGRAM PROCEDURES  ********************}

PROCEDURE FINISH;EXTERNAL;

{*********************  VERB BLAST  *****************************}

{ "blast" -- No effect (heh-heh) unless you've got dynamite, which is a 

neat trick!  }
PROCEDURE BLASTIT;
BEGIN
	IF (PROP[ROD2]<0)OR NOT CLOSED THEN RSPEAK(ACTSPK[VERB])
	ELSE BEGIN
		IF HERE(ROD2) THEN BONUS:=135
		ELSE IF LOCATION=115 THEN BONUS:=134
		ELSE BONUS:=133;
		RSPEAK(BONUS);
		FINISH
	END
END;

{*********************  VERB DRINK   *****************************}

{ "drink" -- If no object, assume water and look for it here.  If water is in
the bottle, drink that, else must be at a water location, so drink the stream. }
PROCEDURE DRINKIT;
BEGIN
	IF (OBJ=0)AND(LIQLOC(LOCATION)<>WATER)AND((LIQ<>WATER)OR
	NOT HERE(BOTTLE)) THEN VERBHUH
	ELSE IF (OBJ<>0)AND(OBJ<>WATER) THEN RSPEAK(110)
	ELSE IF (LIQ<>WATER)OR NOT HERE(BOTTLE) THEN RSPEAK(ACTSPK[VERB])
	ELSE BEGIN
		PROP[BOTTLE]:=1;
		IF TOTING(BOTTLE) THEN DESTROY(WATER);
		RSPEAK(74)
	END
END;

{*********************  VERB POUR  *****************************}

{ "pour" -- If no object, or object is bottle, assume contents of bottle.
Special tests for pouring water or oil on plant or rusty door.  }
PROCEDURE POURIT;
BEGIN
	IF (OBJ=BOTTLE)OR(OBJ=0) THEN OBJ:=LIQ;
	IF OBJ=0 THEN VERBHUH
	ELSE IF NOT TOTING(OBJ) THEN RSPEAK(ACTSPK[VERB])
	ELSE IF (OBJ<>OIL)AND(OBJ<>WATER) THEN RSPEAK(78)
	ELSE BEGIN
		PROP[BOTTLE]:=1;
		DESTROY(OBJ);
		IF AT(PLANT) THEN IF OBJ<>WATER THEN RSPEAK(112) ELSE BEGIN
			PSPEAK(PLANT,PROP[PLANT]+1);
			PROP[PLANT]:=(PROP[PLANT]+2) MOD 6;
			PROP[PLANT2]:=PROP[PLANT] DIV 2;
			NEWLOC:=LOCATION;
			MOVED:=TRUE
		END
		ELSE IF AT(DOOR) THEN BEGIN
			IF OBJ=OIL THEN PROP[DOOR]:=1 ELSE PROP[DOOR]:=0;
			RSPEAK(113+PROP[DOOR])
		END
		ELSE RSPEAK(77)
	END
END;


