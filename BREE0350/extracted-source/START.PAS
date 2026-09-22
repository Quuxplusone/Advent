{********>>>>>>>><<<<<<<<********

>>	MODULE:	START
		30-OCT-80

		This module contains the "wizardry" routines START and
	UNSAVE.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	START
	.IDENT /V0/   }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION BIT(N:INTEGER):INTEGER;EXTERNAL;

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE GETIN(VAR WORD1,WORD1X,WORD2,WORD2X: WORD);EXTERNAL;

FUNCTION ASKM(I,J,K: INTEGER): BOOLEAN;EXTERNAL;

{ ****************** MAGIC MODE PROCEDURES ********************* }

{  All of the data required for the wizardry routines, including saving games,
is stored encoded in one random access file of integers: ADVWIZ.
The current record usage assignments for this file are as follows,
	1..5	  --	MAGICWORD
	6..10	  --	MAGICNUMBER
	11..12	  --	WKDAY
	13..14	  --	WKEND
	15..16	  --	HOLID
	17	  --	HBEGIN
	18	  --	HEND
	19..38	  --	HNAME
	39	  --	SHORT
	40	  --	Count of saved games.
	41	  --	LATENCY
	42..61	  --	Name of saved game 1.
	62..81	  --	Name of saved game 2.
	82..101	  --	Name of saved game 3.
	102..354  --	Saved game 1.
	355..607  -- 	Saved game 2.
	608..860  --  	Saved game 3.
	861..1560 --	Message of the day.      }

PROCEDURE RDWIZ(VAR X:INTEGER);EXTERNAL;

{  Procedure to restore a saved Adventure.  Var SAV tells us which of the 
storage locations to use.  The name for the saved game is wiped out so saved
games can only be restarted once.  }
PROCEDURE UNSAVE(SAV:INTEGER);
VAR SAVED,SAVET,I,PLAC,FIXD:INTEGER;

{  Local procedure to restore the value of a boolean.  }
PROCEDURE RDWIZB(VAR X:BOOLEAN);
VAR Z:INTEGER;
BEGIN
    RDWIZ(Z);
    IF Z=0 THEN X:=FALSE ELSE X:=TRUE

END;

BEGIN
    MSPEAK(40);
    RESET(ADVWIZ,'ADVWIZ.DTA/RW/SEEK');
    SEEK(ADVWIZ,40);
    ADVWIZ^:=ADVWIZ^-1;
    PUT(ADVWIZ);
    SEEK(ADVWIZ,20*SAV+42);
    FOR I:=1 TO 20 DO BEGIN
	IF I=8 THEN MSPEAK(41);
	IF I=19 THEN MSPEAK(42);
	ADVWIZ^:=ORD(' ')-ORD('A');
	PUT(ADVWIZ);
	SEEK(ADVWIZ,20*SAV+42+I)
    END;
    SEEK(ADVWIZ,253*SAV+102);
    RDWIZ(SAVED);
    RDWIZ(SAVET);
    RDWIZ(LOCATION);
    RDWIZ(HOLDING);
    FOR I:=1 TO MAXTRS DO BEGIN
	IF I=25 THEN MSPEAK(43);
	RDWIZ(PLAC);
	RDWIZ(FIXD);
	RDWIZ(PROP[I]);
	MOVE(I,PLAC);
	IF (FIXD>0)OR((FIXD=0)AND(PLAC=0)) 
	THEN MOVE(I+100,FIXD) ELSE FIXED[I]:=FIXD
    END;
    MSPEAK(44);
    FOR I:=MAXTRS DOWNTO 1 DO JUGGLE(I);
    RDWIZ(TALLY);
    RDWIZ(TALLY2);
    RDWIZ(DFLAG);
    RDWIZ(TURNS);
    RDWIZ(IWEST);
    RDWIZ(KNFLOC);
    RDWIZ(NUMDIE);
    RDWIZ(DKILL);
    RDWIZ(CLOCK1);
    RDWIZ(CLOCK2);
    RDWIZ(LIMIT);
    FOR I:=1 TO 10 DO BEGIN
	RDWIZ(HINTLC[I]);
	RDWIZB(HINTED[I]);
    END;
    MSPEAK(45);
    FOR I:=1 TO 6 DO BEGIN
	RDWIZ(ODLOC[I]);
	RDWIZ(DLOC[I]);
	RDWIZB(DSEEN[I])
    END;
    RDWIZB(WZDARK);
    RDWIZB(LMWARN);
    RDWIZB(CLOSING);
    RDWIZB(PANIC);

    RDWIZB(CLOSED);
    RDWIZ(NEWLOC);
    RDWIZ(OLDLC2);
    RDWIZ(OLDLOC);
    CLOSE(ADVWIZ)
END;

PROCEDURE DATIME(VAR D:UNSIGNED;VAR T:INTEGER);EXTERNAL;

FUNCTION WIZARD:BOOLEAN;EXTERNAL;

PROCEDURE PRIMETIME;EXTERNAL;

PROCEDURE HOURS;EXTERNAL;

{  This function is called after initialization at the beginning of the game
to check for "prime time" and suitable latency after saved games.  It returns a
value of true if a demonstration game is being allowed, otherwise always false.
Global variable DONE is set to true if play is disallowed for any reason.  }
FUNCTION START:BOOLEAN;

VAR DELAY,LATENCY,SAV,D,T:INTEGER;
    PRIMTM:ARRAY[1..2] OF INTEGER;
    SAVED,SAVET:UNSIGNED;
    SOON,WAYTOOSOON,FOUND,PTIME:BOOLEAN;

{ Local procedure to check saved game names. }
PROCEDURE MATCH(WRD:WORD);
VAR J,K:INTEGER;
BEGIN
    FOR J:=1 TO 5 DO BEGIN
	RDWIZ(K);
	IF WRD[J]<>CHR(K+ORD('A')) THEN FOUND:=FALSE
    END
END;

BEGIN { Start }
    { Check to see if this is "prime time".  If so, only wizards may play, 
though others may be allowed a short game for demonstration purposes.  If
this is a saved game, check for suitable latency.  Return true if this is
a demo game, return DONE true if play not allowed, restore data for saved
games.  }
    PRIMETIME;
    DATIME(D,T);
    IF (D>=HBEGIN)AND(D<=HEND) THEN PRIMTM:=HOLID
    ELSE IF (D MOD 7)<=1 THEN PRIMTM:=WKEND
    ELSE PRIMTM:=WKDAY;
    IF (T DIV 60)<15 THEN PTIME:=((PRIMTM[1] AND BIT(T DIV 60))<>0)
    ELSE PTIME:=((PRIMTM[2] AND BIT((T DIV 60)-15))<>0);
    RESET(ADVWIZ,'ADVWIZ.DTA/SEEK');
    SEEK(ADVWIZ,39);
    RDWIZ(SHORT);
    RDWIZ(SAV);
    RDWIZ(LATENCY);
    SETUP:=FALSE;
    SOON:=FALSE;
    WAYTOOSOON:=FALSE;

    IF SAV<>0 THEN IF ASKM(31,7,7) THEN BEGIN
	MSPEAK(46);
	GETIN(WRD1,WRD1X,WRD2,WRD2X);
	FOR SAV:=0 TO 2 DO BEGIN
	    FOUND:=TRUE;
	    MATCH(WRD1);
	    MATCH(WRD1X);
	    MATCH(WRD2);
	    MATCH(WRD2X);
	    IF FOUND THEN EXIT
	END;
	IF NOT FOUND THEN MSPEAK(47) ELSE BEGIN
	    MSPEAK(48);
	    SETUP:=TRUE;
	    SEEK(ADVWIZ,102+(SAV*253));
	    RDWIZ(SAVED);
	    RDWIZ(SAVET);
	    DELAY:=(D-SAVED)*1440+(T-SAVET);
	    IF DELAY<LATENCY THEN BEGIN
		WRITELN;
		WRITE('This Adventure was suspended a mere');
		WRITE(DELAY:4);
		WRITELN(' minutes ago.');
		SOON:=TRUE;
		WAYTOOSOON:=(DELAY<(LATENCY DIV 3))
	    END
	END
    END;
    CLOSE(ADVWIZ);
    START:=FALSE;
    IF WAYTOOSOON THEN BEGIN
	MSPEAK(2);
	DONE:=TRUE
    END
    ELSE IF SOON THEN BEGIN
	MSPEAK(8);
	IF NOT WIZARD THEN BEGIN
	    MSPEAK(9);
	    DONE:=TRUE
	END
	ELSE UNSAVE(SAV)
    END
    ELSE IF PTIME THEN BEGIN
	MSPEAK(3);
	HOURS;
	MSPEAK(4);
	IF NOT WIZARD THEN BEGIN
	    IF NOT SETUP THEN 
		IF NOT ASKM(5,7,7) THEN DONE:=TRUE
		ELSE START:=TRUE
	    ELSE BEGIN
		MSPEAK(9);
		DONE:=TRUE
	    END
	END
	ELSE IF SETUP THEN UNSAVE(SAV)
    END

    ELSE IF SETUP THEN UNSAVE(SAV)
END;

