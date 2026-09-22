{********>>>>>>>><<<<<<<<********

>>	MODULE:	KILLIT
		23-SEP-80

		This module contains the procedures KILLIT and KILLOBJ.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	KILLIT
	.IDENT	/V0/	 }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

PROCEDURE GETIN(VAR WRD1,WRD1X,WRD2,WRD2X:WORD);EXTERNAL;

{ ******************* VERB KILL (INTRANSITIVE)  ********** }

PROCEDURE VERBHUH;EXTERNAL;

PROCEDURE KILLOBJ;FORWARD;

{  This procedure and KILLOBJ for "attack" -- Assume target if unambiguous.
"Throw" also links here.  Attackable objects fall into two categories: enemies
(snake, dwarf, etc.) and others (bird, clam).  Ambiguous if two enemies, or if
no enemies but two others.  }
PROCEDURE KILLIT;
VAR	I,J:INTEGER;
BEGIN
	J:=0;
	FOR I:=1 TO 5 DO IF (DLOC[I]=LOCATION)AND(DFLAG>=2)
	THEN BEGIN
		J:=J+1;
		OBJ:=DWARF;
		EXIT
	END;
	IF HERE(SNAKE) THEN BEGIN
		J:=J+1;
		OBJ:=SNAKE
	END;
	IF AT(DRAGON) AND(PROP[DRAGON]=0) THEN BEGIN
		J:=J+1;
		OBJ:=DRAGON
	END;
	IF AT(TROLL) THEN BEGIN
		J:=J+1;
		OBJ:=TROLL
	END;
	IF HERE(BEAR) AND (PROP[BEAR]=0) THEN BEGIN
		J:=J+1;
		OBJ:=BEAR
	END;

	IF J>1 THEN VERBHUH
	ELSE IF OBJ<>0 THEN KILLOBJ
	ELSE BEGIN
		J:=0;
		{ Can't attack bird by throwing axe.  }
		IF HERE(BIRD) AND(VERB<>THROW) THEN BEGIN
			J:=J+1;
			OBJ:=BIRD
		END;
		{ Clam and oyster both treated as clam for intransitive case. }
		IF HERE(CLAM) OR HERE(OYSTER) THEN BEGIN
			J:=J+1;
			OBJ:=CLAM
		END;
		IF J>1 THEN VERBHUH
		ELSE IF OBJ<>0 THEN KILLOBJ
		ELSE RSPEAK(44)
	END
END;

{ ******************* VERB KILL (TRANSITIVE) ********** }

PROCEDURE WAKEDWARVES;EXTERNAL;

{ "attack" -- See KILLIT for sorting out intransitive targets.  }
PROCEDURE KILLOBJ;
VAR I,K:INTEGER;
BEGIN
	CASE OBJ OF
		BIRD: IF CLOSED THEN RSPEAK(137) ELSE BEGIN
			DESTROY(BIRD);
			PROP[BIRD]:=0;
			IF PLACE[SNAKE]=PLACE1[SNAKE] THEN TALLY2:=TALLY2+1;
			RSPEAK(45)
		END;
		CLAM,
		OYSTER: RSPEAK(150);
		SNAKE: RSPEAK(46);
		DWARF: IF CLOSED THEN WAKEDWARVES ELSE RSPEAK(49);
		TROLL: RSPEAK(157);
		BEAR: RSPEAK(165+((PROP[BEAR]+1) DIV 2));
		{ Fun stuff for dragon.  If he insists on attacking it, win!
		Set PROP to dead, move dragon to central location (still fixed),
		move rug there (not fixed), and move him there too.  Then return
		with MOVED set to get a new description.  If he declines the 
		challenge, reset DRAGFLG to branch back and use his input as new
		turn.  }
		DRAGON: IF PROP[DRAGON]<>0 THEN RSPEAK(167) ELSE BEGIN
			RSPEAK(49);
			VERB:=0;
			OBJ:=0;
			GETIN(WRD1,WRD1X,WRD2,WRD2X);
			CASE WRD1[1] OF
				'Y': BEGIN
				    PSPEAK(DRAGON,1);
				    PROP[DRAGON]:=2;
				    PROP[RUG]:=0;

				    K:=(PLACE1[DRAGON]+FIXED1[DRAGON]) DIV 2;
				    MOVE(DRAGON+100,0);
				    MOVE(RUG+100,0);
				    MOVE(DRAGON,K);
				    MOVE(RUG,K);
				    FOR I:=1 TO MAXTRS DO
				    IF (PLACE[I]=PLACE1[DRAGON])
				    OR(PLACE[I]=FIXED1[DRAGON])
				    THEN MOVE(I,K);
				    NEWLOC:=K;
				    MOVED:=TRUE
				END;
				ELSE DRAGFLG:=FALSE
			END
		END;
		ELSE RSPEAK(ACTSPK[VERB])
	END
END;

