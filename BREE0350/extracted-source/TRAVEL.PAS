{********>>>>>>>><<<<<<<<********

>>	MODULE:	TRAVEL
		29-OCT-81

		This module contains the procedures TRAVEL, TRVTBL, and SEARCH
	which oversee the response to motion verbs.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE TRAVEL
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION FORCED(LOC:INTEGER):BOOLEAN;EXTERNAL;

PROCEDURE DROP(OBJ,WHERE:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION PCT(N:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

{ *********************  TRAVEL PROCEDURE *********************** }

PROCEDURE BACKUP(VAR I:INTEGER;VAR LINK:KEYLINK);EXTERNAL;

PROCEDURE TRVTBL;FORWARD;

{  Figure out the new location.....

Given the current location in LOCATION, and a motion verb number in K, put the
new location in NEWLOC.  The current LOCATION is saved in OLDLOC in case he 
wants to retreat.  The current OLDLOC is saved in OLDLC2, in case he dies.  
(If he does, NEWLOC will be in limbo, and OLDLOC will be what killed him, so
we need OLDLC2, which is the last place he was safe.)  }
PROCEDURE TRAVEL;
VAR LINK1:KEYLINK;
    I:INTEGER;
BEGIN
    { Get the key link for the present location. }
    LINK1:=KEY[LOCATION];
    { If the motion is forced, don't bother with the verb, just move.  }
    IF FORCED(LOCATION) THEN BEGIN
	NEWLOC:=LINK1^.NEWLOC[2];
	OLDLC2:=OLDLOC;
	OLDLOC:=LOCATION
    END
    {  Otherwise, check for a few special verbs... }
    ELSE CASE K OF
	{  Do nothing. }
	NULL: NEWLOC:=LOCATION;
	{  Handle "go back".  Look for verb which goes from LOCATION to OLDLOC,
	or to OLDLC2 if OLDLOC has forced motion.  BACKUP returns with
	I=LOCATION if we forgot how he got here, or with a new LINK1 such that
	I=LINK1^.NEWLOC[2], if we found a verb to retreat with. }
	BACK: BEGIN
	    I:=0;
	    BACKUP(I,LINK1);
	    IF I=LOCATION THEN BEGIN
		RSPEAK(91);

		NEWLOC:=LOCATION
	    END 
	    {  If we exited without finding a path, you can't get there 
	    from here.  }
	    ELSE IF I<>LINK1^.NEWLOC[2] THEN BEGIN
		RSPEAK(140);
		NEWLOC:=LOCATION
	    END
	    {  Otherwise, grab the first verb for this path and go to 
	    normal travel table search. }
	    ELSE BEGIN
		K:=LINK1^.NXTVERB^.VERBVAL;
		TRVTBL
	    END
	END;  {Case K of BACK}
	{ Look.  Cant't give more detail.  Pretend it wasn't dark (though it 
	may "now" be dark) so he won't fall into a pit while staring out into 
	the gloom.  }
	LOOK:BEGIN
	    IF DETAIL<3 THEN RSPEAK(15);
	    DETAIL:=DETAIL+1;
	    WZDARK:=FALSE;
	    ABB[LOCATION]:=0;
	    NEWLOC:=LOCATION
	END;
	{ Cave.  Different messages depending on whether above ground.  }
	CAVE:BEGIN
	    IF LOCATION<8 THEN RSPEAK(57) ELSE RSPEAK(58);
	    NEWLOC:=LOCATION
	END;
	ELSE
	{ All other cases look for verb in travel table.  See database
	description for explanation of conditional motions.  }
	BEGIN
	    OLDLC2:=OLDLOC;
	    OLDLOC:=LOCATION;
	    TRVTBL
	END  {  of else clause in case of K  }
    END; {Case of K}
    MOVED:=TRUE
END;  

{ *************** TRAVEL TABLE PROCEDURES *********************** }

PROCEDURE SEARCH(VAR LINK1:KEYLINK);FORWARD;

PROCEDURE NOWAY;EXTERNAL;

{ Search the travel table for the new location.  See database description in
module ADVINI for explanation of travel table conditions.  }
PROCEDURE TRVTBL;
VAR LINK1:KEYLINK;
BEGIN
    {  Get the keylink pointer for this location.  }
    LINK1:=KEY[LOCATION];
    NEWLOC:=0;
    REPEAT SEARCH(LINK1) UNTIL (NEWLOC<>0)OR(LINK1=NIL);

    IF NEWLOC=0 THEN NOWAY
END;

{ Dummy procedure to force overlay mechanism to pull in entire tree path to
here when PARSE is loaded.  }
PROCEDURE OVRLOAD;
BEGIN
END;

PROCEDURE PLOVER;EXTERNAL;

PROCEDURE TROLLBRIDGE;EXTERNAL;

{   Search the travel table for the new location.  See database description in
module ADVINI for an explanation of travel table conditions.  }

PROCEDURE SEARCH; {VAR LINK1:KEYLINK}
VAR LINK2:VERBLINK;
    COND:INTEGER;
    FOUND:BOOLEAN;

BEGIN
    LINK2:=LINK1^.NXTVERB;
    WHILE (LINK2^.VERBVAL<>K) AND (LINK2<>NIL) DO LINK2:=LINK2^.NXTVERB;
    IF LINK2=NIL THEN LINK1:=LINK1^.NXTLINK ELSE REPEAT { until found }
	FOUND:=TRUE;
	IF LINK1<>NIL THEN BEGIN
	    IF LINK1^.NEWLOC[1]<>0 THEN BEGIN
		COND:=LINK1^.NEWLOC[1];
		IF COND<=100 THEN BEGIN
		    IF NOT PCT(COND) THEN BEGIN
			LINK1:=LINK1^.NXTLINK;
			FOUND:=FALSE
		    END
		END ELSE IF COND<300 THEN BEGIN
		    IF NOT TOTING(COND MOD 100)
		    AND((COND<200)OR NOT AT(COND MOD 100))
		    THEN BEGIN
			LINK1:=LINK1^.NXTLINK;
			FOUND:=FALSE
		    END
		END ELSE IF COND>300 THEN BEGIN
		    IF PROP[COND MOD 100]=((COND DIV 100)-3)
		    THEN BEGIN
			LINK1:=LINK1^.NXTLINK;
			FOUND:=FALSE
		    END
		END
	    END;
	    IF FOUND THEN BEGIN
		IF LINK1^.NEWLOC[2]<=300 THEN NEWLOC:=LINK1^.NEWLOC[2]
		ELSE IF LINK1^.NEWLOC[2]>500 THEN BEGIN
		    NEWLOC:=LOCATION;
		    RSPEAK(LINK1^.NEWLOC[2]-500)
		END ELSE CASE LINK1^.NEWLOC[2] OF
		    {  Travel 301.  Plover-alcove passage.  }
		    301: PLOVER;

		    {  Travel 302.  Plover transport.  Drop the emerald (only 
		    use special travel if toting it), so he's forced to use the 
		    plover-passage to get it out.  Having dropped it, go back 
		    and pretend he wasn't carrying it after all.  }
		    302:BEGIN
			IF TOTING(EMERALD) THEN DROP(EMERALD,LOCATION);
			LINK1:=LINK1^.NXTLINK;
			FOUND:=FALSE
		    END;
		    {  Travel 303.  Troll Bridge. }
		    303: TROLLBRIDGE
		END {Case of newloc[2]}
	    END { If found }
	END { If link1<>nil }
    UNTIL FOUND
END;
