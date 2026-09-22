{********>>>>>>>><<<<<<<<********

>>	MODULE:	SUBS0
		15-OCT-80

		This module contains the data structure functions TOTING, 
	HERE, BITSET, FORCED, DARK, RAN, and PCT.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE SUBS0
	.IDENT /V0/   }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

{  True if player is carrying object.  }
FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;
BEGIN
	TOTING:=(PLACE[OBJ]=-1)
END;

{ This function is true if an object is at the player's current location or
being carried.  }
FUNCTION HERE(OBJ:INTEGER):BOOLEAN;
BEGIN
	HERE:=(PLACE[OBJ]=LOCATION) OR TOTING(OBJ)
END;

FUNCTION BIT(I:INTEGER):INTEGER;EXTERNAL;

{  This function is true if argument location has bit n set in CONDI.  }
FUNCTION BITSET(LOC,N:INTEGER):BOOLEAN;
BEGIN
    BITSET:=((CONDI[LOC] AND BIT(N))<>0)
END;

{  This function is true if loc has forced motion.  }
FUNCTION FORCED(LOC:INTEGER):BOOLEAN;
BEGIN
	FORCED:=(CONDI[LOC]=2)
END;

{  This function is true if the current LOCATION has no source of light.  }
FUNCTION DARK:BOOLEAN;
BEGIN
	DARK:= NOT BITSET(LOCATION,0) AND ((PROP[LAMP]=0) OR (NOT HERE(LAMP)))
END;

{  Random number function is a hybrid developed from the one in the F4P version 
and the one supplied as a demo program with OMSI.  It is seeded from the time
of day an returns an integer value from 1 to RANGE. }
FUNCTION RAN(RANGE:INTEGER):INTEGER;
FUNCTION RANDOM:REAL;
BEGIN
	SEED:=(SEED*13077+6925)MOD 32768;
	RANDOM:=SEED/32768.0
END;
BEGIN
	IF STRTRAN THEN
	BEGIN
		SEED:=TRUNC(TIME*1000.0);
		STRTRAN:=FALSE
	END;
	RAN:=TRUNC(RANDOM*RANGE)+1
END;

{  This function is true n percent of the time.  }

FUNCTION PCT(N:INTEGER):BOOLEAN;
VAR	I:INTEGER;
BEGIN
	I:=RAN(100);
	PCT:=(I<N)
END;
