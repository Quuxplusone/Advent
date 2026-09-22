{********>>>>>>>><<<<<<<<********

>>	MODULE:	GETIT
		22-SEP-80

		This module contains the procedure GETIT, which along with
	GETOBJ implements carrying an object.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	GETIT
	.IDENT	/V0/	 }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE CARRY(OBJ:INTEGER);EXTERNAL;

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION LIQ:INTEGER;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

{ ****************** VERB GET (CARRY, TAKE, etc. ) ******************** }

{   Carry an object.  Special cases for bird and cage (if bird in cage, can't
take one without the other.  Liquids also special, since they depend on status
of bottle.  Also various side effects, etc.  }
PROCEDURE GETIT;
BEGIN
	{  Can't carry more than seven objects at once.  }
	IF HOLDING=7 THEN RSPEAK(92)
	ELSE BEGIN
		{  If trying to get the bird into the cage...  }
		IF (OBJ=BIRD)AND(PROP[BIRD]=0) THEN
			{  Rod scares the bird...  }
			IF TOTING(ROD) THEN RSPEAK(26)
			{  If he doesn't have the cage... }
			ELSE IF NOT TOTING(CAGE) THEN RSPEAK(27)
			{  Otherwise, catch him...  }
			ELSE BEGIN
				PROP[BIRD]:=1;
				CARRY(BIRD);
				{  List the cage first during inventory...  }
				JUGGLE(CAGE);
				RSPEAK(54)
			END
		ELSE BEGIN
			{  If getting bird and cage....  }
			IF ((OBJ=BIRD)OR(OBJ=CAGE))AND(PROP[BIRD]<>0) THEN BEGIN
				{  Get the one not mentioned...  }
				CARRY(BIRD+CAGE-OBJ);
				{  Get the one he asked for....  }
				CARRY(OBJ);
				{  List the cage first during inventory...  }
				JUGGLE(CAGE)
			END
			{  Else if getting a full bottle...  }
			ELSE IF (OBJ=BOTTLE)AND(LIQ<>0) THEN BEGIN
				{  Move liquid to player, but don't count 
				it in HOLDING by using MOVE instead of CARRY.  }

				MOVE(LIQ,-1);
				{  Snatch the bottle.  }
				CARRY(BOTTLE)
			END
			{  If none of the above, CARRY whatever he wants..  }
			ELSE CARRY(OBJ);
			RSPEAK(54)
		END
	END
END;

