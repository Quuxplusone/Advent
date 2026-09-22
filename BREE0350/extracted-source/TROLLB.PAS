{********>>>>>>>><<<<<<<<********

>>	MODULE:	TROLLB
		13-OCT-80

		This module contains the procedure TROLLB.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE TROLLB
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DROP(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

{ **************** TRAVEL TABLE PROCEDURES ***************** }

{  Travel 303.  Troll Bridge.  Must be done only as special motion so that the 
dwarves won't wander across and encounter the bear.  (They won't follow the 
player there because that region is forbidden to the pirate.) If PROP[TROLL]=1, 
he's crossed since paying, so step out and block him.  (Standard travel entries 
check for PROP[TROLL]=0.) Also, special stuff for the bear.  }

PROCEDURE TROLLBRIDGE;
BEGIN
    IF PROP[TROLL]=1 THEN BEGIN
	PSPEAK(TROLL,1);
	PROP[TROLL]:=0;
	DESTROY(TROLL2);
	DESTROY(TROLL2+100);
	MOVE(TROLL,PLACE1[TROLL]);
	MOVE(TROLL+100,FIXED1[TROLL]);
	JUGGLE(CHASM);
	NEWLOC:=LOCATION
    END
    ELSE BEGIN
	NEWLOC:=PLACE1[TROLL]+FIXED1[TROLL]-LOCATION;
	IF PROP[TROLL]=0 THEN PROP[TROLL]:=1;
	IF TOTING(BEAR) THEN BEGIN
	    RSPEAK(162);
	    PROP[CHASM]:=1;
	    PROP[TROLL]:=2;
	    DROP(BEAR,NEWLOC);
	    FIXED[BEAR]:=-1;
	    PROP[BEAR]:=3;
	    IF PROP[SPICES]<0 THEN TALLY2:=TALLY2+1;
	    OLDLC2:=NEWLOC;
	    NEWLOC:=-1
	END
    END
END;
