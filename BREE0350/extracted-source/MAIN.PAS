{********>>>>>>>><<<<<<<<********

>>	MODULE:	MAIN
		28-OCT-80

		This module contains the main program block.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE MAIN
	.IDENT /V0/   }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION FORCED(LOC:INTEGER):BOOLEAN;EXTERNAL;

{  *****************  INITIALIZATION PROCEDURES  ****************  }

PROCEDURE INITIALIZE;EXTERNAL;

FUNCTION START:BOOLEAN;EXTERNAL;

PROCEDURE MOTD(X:BOOLEAN);EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

FUNCTION ASKR(I,J,K: INTEGER): BOOLEAN;EXTERNAL;

{********************  DWARF PROCEDURE  *******************}

PROCEDURE DWARFSTUFF;EXTERNAL;

{***************** MAIN PROGRAM PROCEDURES  ******************}

PROCEDURE BLOCKED;EXTERNAL;

PROCEDURE ALIVE;EXTERNAL;

PROCEDURE WHEREAREWE;EXTERNAL;

PROCEDURE TRAVEL;EXTERNAL;

PROCEDURE WHATSHERE;EXTERNAL;

PROCEDURE RESPONSE;EXTERNAL;

{ *****************  MAIN PROGRAM  ******************}

{  The structure of the main program block roughly follows the  F4P  version but
the spaghetti has been somewhat straightened out.   It  was  very  difficult  to 
preserve the exact flow without using massive random "goto"'s  as  was  done  in
the original.  The way I got around this problem was adding a few  program  flow 
control booleans which, in essence, cause repeat loops to exit and jump back  to 
a lower nesting level under certain conditions (check out  DRAGFLG,  which  pops 
you back from KILLOBJ to the middle of RESPONSE).   As  a  result  this  program 
probably only rates a B- for structure but  from  the  player's  standpoint,  at 
least, it appears to operate exactly as the original with a few minor changes.  
   You may argue that the thing is overly overlaid, but it does run in less than 
12Kwords  despite the size of OMSI's runtime stuff.  In the  future  I  plan  to 
investigate shrinking the OMSI  stuff  further  and  possibly  reorganizing  the 
order of things so that it will run even smaller.  The  time  required  to  swap 
out overlays is presently only slightly noticable by the player.  }

BEGIN {MAIN PROGRAM}
    {  Initialize everything.  }
    INITIALIZE;

    {  Print the "message of the day".  }
    MOTD(FALSE);
    {  Start-up, check for "prime time", saved games, etc.  }
    DEMO:=START;
    {  If allowed to start, then go....  }
    IF NOT DONE THEN BEGIN
	{  Do this only at the beginning of a new game....  }
	IF NOT SETUP THEN BEGIN
	    {  Welcome the player and (optionally) give "instructions".  
	    If he accepts it will cost him points.  }
	    HINTED[3]:=ASKR(65,1,0);
	    {  If he asked for instructions, then give him some extra lamp time.  }
	    IF HINTED[3] THEN LIMIT:=1000 ELSE LIMIT:=330;
	END;
	REPEAT {Until DONE is true}
	    MOVED:=FALSE;
	    {The beginning of this loop is repeated every time the player
	    has moved to a new location, which is indicated by MOVED 
	    becoming true. 
		First check to see if the new location is outside the cave and
	    it's closing or if a dwarf's blocking his way  (either case
	    returns with NEWLOC=LOCATION). }
	    BLOCKED;
	    {  Move to the new location.  }
	    LOCATION:=NEWLOC;
	    { Go do the dwarf stuff.  }
	    DWARFSTUFF;
	    {  Go see if this guy's still alive or not, returns with MOVED
	    true if he dies and is reincarnated or with MOVED and DONE true
	    if he's dead and that's it..   }
	    ALIVE;
	    IF NOT (MOVED OR DONE) THEN { Give him another turn..} BEGIN
		{ Tell him where he is.    }
		WHEREAREWE;
		{  If location has forced motion, do it.  }
		IF FORCED(LOCATION) THEN TRAVEL
		ELSE
		{  Otherwise, tell him what's here and respond to his
		instructions until he moves again.  }
		BEGIN
		    WHATSHERE;
		    REPEAT RESPONSE UNTIL MOVED;
		END
	    END
	UNTIL DONE;
    END;
    {  Set VT-100's back to VT-52 mode.  }
    IF VT100 THEN WRITE(CHR(33B),'[?2l')
END.
