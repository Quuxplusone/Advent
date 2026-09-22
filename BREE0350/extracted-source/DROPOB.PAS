{********>>>>>>>><<<<<<<<********

>>	MODULE:	DROPOB
		23-SEP-80

		This module contains the procedure DROPOBJ.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	DROPOB
	.IDENT	/V0/	 }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DROP(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION LIQ:INTEGER;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

{ ******************* VERB DROP ********** }

PROCEDURE WAKEDWARVES;EXTERNAL;

{ Discard object.  "Throw" also comes here for most objects.  Special cases
for bird (might attack snake or dragon) and cage (might contain bird) and vase.
Drop coins at vending machine for extra batteries.  }
PROCEDURE DROPOBJ;

{ Local procedure for normal drop (no side effects) ... }
PROCEDURE DROPIT;
BEGIN
    { "Okay.."  }
    RSPEAK(54);
    {  Deal with liquids......  }
    IF (LIQ=OBJ)OR((OBJ=BOTTLE)AND(LIQ<>0)) THEN BEGIN
	OBJ:=BOTTLE;
	DESTROY(LIQ)
    END;
    {  Handle bird and cage stuff.....  }
    IF (OBJ=CAGE)AND (PROP[BIRD]<>0) THEN DROP(BIRD,LOCATION);
    IF OBJ=BIRD THEN PROP[BIRD]:=0;
    {  Drop it!  }
    DROP(OBJ,LOCATION)
END;

BEGIN { dropobj }
    { Might be dropping special rod....  }
    IF TOTING(ROD2)AND(OBJ=ROD)AND NOT TOTING(ROD) THEN OBJ:=ROD2;
    { Can't drop something you're not carrying.... }

    IF NOT TOTING(OBJ) THEN RSPEAK(ACTSPK[VERB])
    { Handle objects with side effects at special locations....}
    ELSE CASE OBJ OF
	BIRD: IF HERE(SNAKE) THEN BEGIN
	    { Birds scare snakes.... }
	    RSPEAK(30);
	    IF CLOSED THEN WAKEDWARVES ELSE BEGIN
		DESTROY(SNAKE);
		PROP[SNAKE]:=1;
		PROP[BIRD]:=0;
		DROP(BIRD,LOCATION)
	    END
	END ELSE IF AT(DRAGON)AND (PROP[DRAGON]=0) THEN BEGIN
	    { Dragons eat birds.... }
	    RSPEAK(154);
	    DROP(BIRD,LOCATION);
	    DESTROY(BIRD);
	    PROP[BIRD]:=0;
	    IF PLACE[SNAKE]=PLACE1[SNAKE] THEN TALLY2:=TALLY2+1;
	END 
	{  Other cases simply drop the fool bird.... }
	ELSE DROPIT;
	{  Coins? ...maybe buying batteries...  }
	COINS: IF HERE(VENDING) THEN BEGIN
	    DROP(COINS,LOCATION);
	    DESTROY(COINS);
	    MOVE(BATTERY,LOCATION);
	    PSPEAK(BATTERY,0)
	END
	{  If not, drop em.... }
	ELSE DROPIT;
	{  Bear may be chasing troll away.....  }
	BEAR: IF AT(TROLL) THEN BEGIN
	    RSPEAK(163);
	    DESTROY(TROLL);
	    DESTROY(TROLL+100);
	    MOVE(TROLL2,PLACE1[TROLL]);
	    MOVE(TROLL2+100,FIXED1[TROLL]);
	    JUGGLE(CHASM);
	    PROP[TROLL]:=2;
	    DROP(BEAR,LOCATION)
	END ELSE DROPIT;
	{  Gotta be careful dropping them vases..... }
	VASE: IF (LOCATION<>PLACE1[PILLOW]) THEN BEGIN
	    IF AT(PILLOW) THEN PROP[VASE]:=0 ELSE PROP[VASE]:=2;
	    PSPEAK(VASE,PROP[VASE]+1);
	    IF PROP[VASE]<>0 THEN FIXED[VASE]:=-1;
	    DROP(VASE,LOCATION)
	END ELSE DROPIT;
    	{  All other objects okay to just drop.... }
	ELSE DROPIT
    END { Case of obj }
END;

