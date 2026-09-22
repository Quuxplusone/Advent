{********>>>>>>>><<<<<<<<********

>>	MODULE:	RESPON
		13-OCT-80

		This module contains the procedure RESPONSE.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE RESPON
	.IDENT /V0/   }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION RAN(RANGE:INTEGER):INTEGER;EXTERNAL;

FUNCTION BITSET(LOC,N:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION LIQLOC(LOC:INTEGER):INTEGER;EXTERNAL;

FUNCTION DARK:BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE GETIN(VAR WORD1,WORD1X,WORD2,WORD2X: WORD);EXTERNAL;

{*************  MAIN PROGRAM PROCEDURES  ********************}

PROCEDURE HINTROUTINE;EXTERNAL;

PROCEDURE FINISH;EXTERNAL;

PROCEDURE MAINTINENCE;EXTERNAL;

PROCEDURE CLSECLUE;EXTERNAL;

PROCEDURE ENDDEMO;EXTERNAL;

PROCEDURE CLOSECAVE;EXTERNAL;

PROCEDURE CAVECLOSING;EXTERNAL;

PROCEDURE LAMPDIM;EXTERNAL;

PROCEDURE DUMPWD1;EXTERNAL;

PROCEDURE OVRLOAD;EXTERNAL;

PROCEDURE PARSE;EXTERNAL;

{  This procedure is the next most important nested block below the main program
block.  It gets the player input and controls a number of conditions on each
turn (counting down to cave closing, for example).  }
PROCEDURE RESPONSE;
BEGIN
    {  Player input is parsed into a verb or object value (or both) in order to 
    understand what he wants us to do.  Sometimes it takes two input turns to 
    make sense (if possible) out of what the jerk typed. 
	Once we've got a valid defintion and responded to his instructions,
    the object and verb values are zeroed out before getting more input.  }
    OBJ:=0;
    VERB:=0;
    REPEAT  { until gotboth }
	{  GOTBOTH is used to indicate whether we need further input before 

	responding.  It is set "true" initially in case because we assume his 
	input will be complete.  Should he type "drop", for example, and he's 
	carrying six things, procedure VERBHUH would be called to respond 
	"Drop what?", GOTBOTH would be set false and the input loop would be 
	repeated from here without first zeroing out VERB again.  }
	GOTBOTH:=TRUE;
	{  Before getting his input, go do the "hints" stuff.  }
	HINTROUTINE;
	{  If we're in the repository, go see whether to give him the clue on 
	the oyster (which really isn't much help anyway).  }
	IF CLOSED THEN CLSECLUE;
	{  Keep track if it's dark twice in a row.  }
	WZDARK:=DARK;
	{  Get rid of the dwarf's knife if moved from scene of attack. }
	IF (KNFLOC>0) AND (KNFLOC<>LOCATION) THEN KNFLOC:=0;
	{  Kick the random number generator just to add variety to the chase.  }
	I:=RAN(100);
	{  Get the next command from our Adventurer.  }
	GETIN(WRD1,WRD1X,WRD2,WRD2X);
	{  If trying to kill the dragon and blowing it, the player input comes 
	from KILLOBJ so the loop would be repeated from here.  DRAGFLG is set 
	FALSE by KILLOBJ if this happens.  }
	REPEAT { until dragflg } 
	    DRAGFLG:=TRUE;
	    {  Every input, check "FOOBAR" flag.  If zero, nothing's going on.  
	    If positive, make negative.  If negative and VERB is not "say", 
	    he skipped a word, so make it zero.  }
	    IF FOOBAR>0 THEN FOOBAR:=-FOOBAR ELSE IF VERB<>SAY THEN FOOBAR:=0;
	    { Chalk up another turn. }
	    TURNS:=TURNS+1;
	    {  Tick CLOCK2 once CLOCK1 has run out.  }
	    IF CLOCK1<0 THEN CLOCK2:=CLOCK2-1;
	    {  Invoke wizardry routines if he says "Magic Mode" on his first 
	    turn and terminate demo games during prime time, otherwise
	    continue with the turn.  }
	    IF (WRD1='MAGIC')AND(WRD2='MODE ')AND(TURNS=1) THEN MAINTINENCE
	    ELSE IF DEMO AND(TURNS>=SHORT) THEN ENDDEMO
	    ELSE BEGIN
		{  When CLOCK2 runs out, close the cave.  }
		IF CLOCK2=0 THEN BEGIN
		    CLOSECAVE;
		    MOVED:=TRUE
		END
		{  If his lamp is dead and he's outside the cave, force him to 
		give up since there's no way to get to the batteries.  }
		ELSE IF (LIMIT<0)AND(LOCATION<=8) THEN BEGIN
		    RSPEAK(185);
		    GAVEUP:=TRUE;
		    FINISH
		END
		ELSE BEGIN
		    {  If he's found all the treasures, tick CLOCK1 when deep 
		    into the cave (and not at "Y2").  }
		    IF (TALLY=0)AND(LOCATION>=15) 
		    AND(LOCATION<>33) THEN CLOCK1:=CLOCK1-1;
		    {  If CLOCK1 runs out, begin to close the cave.  Otherwise 
		    go handle the lamp limit stuff.  }

		    IF CLOCK1=0 THEN CAVECLOSING ELSE LAMPDIM;
		    {  Special case for waders.  }
		    IF (WRD1='ENTER')AND((WRD2='STREA')OR(WRD2='WATER')) THEN
		    IF LIQLOC(LOCATION)=WATER THEN RSPEAK(70) ELSE RSPEAK(43)
		    ELSE BEGIN
			{ Other instances of "enter" look at second word only.  }
			IF (WRD1='ENTER')AND(WRD2<>'     ') THEN DUMPWD1
			{  Special case where "water" and "oil" are used 
			as verbs.  }
			ELSE IF ((WRD1='WATER')OR(WRD1='OIL  '))
			AND((WRD2='PLANT')OR(WRD2='DOOR ')) THEN WRD2:='POUR ';
			{  Now jump down to the next nesting level and parse 
			the current command.  Dummy procedure OVRLOAD is called
			to pull in the most common overlay tree.  }
			REPEAT OVRLOAD;PARSE UNTIL NODUMP
		    END
		END
	    END
	UNTIL DRAGFLG
    UNTIL GOTBOTH
END;

