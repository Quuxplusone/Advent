{*******>>>>>>>><<<<<<<<********

>>	MODULE:	LOOP0
		29-OCT-80

		This module contains the main program loop procedures which are
	used after each turn in which the player has moved from one location to 
	another:  DWARFSTUFF, BLOCKED, ALIVE, DEATH, PITDEATH, WHEREAREWE, 
	and WHATSHERE.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	LOOP0
	.IDENT	/V0/    }

{ *************** DATA STRUCTURE ROUTINES ******************** }

FUNCTION FORCED(LOC:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION BITSET(LOC,N:INTEGER):BOOLEAN;EXTERNAL;

PROCEDURE DROP(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION DARK:BOOLEAN;EXTERNAL;

FUNCTION PCT(N:INTEGER):BOOLEAN;EXTERNAL;

PROCEDURE MOVE(OBJ,LOC:INTEGER);EXTERNAL;

FUNCTION AT(OBJ:INTEGER):BOOLEAN;EXTERNAL;

PROCEDURE CARRY(OBJ:INTEGER);EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION RAN(N:INTEGER):INTEGER;EXTERNAL;

{ ************** I/O SUBROUTINES **************************** }

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

FUNCTION ASKR(I,J,K: INTEGER): BOOLEAN;EXTERNAL;

PROCEDURE SPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

{ **************** MAIN PROGRAM PROCEDURES ********************* }

{  After having moved, check to see if his way is blocked... }
PROCEDURE BLOCKED;
BEGIN
    { If the cave is closing and the player has exited from the cave and is not 
    dead, put him back where he was and tell him he's stuck. The first time this 
    happens, set the time until closure to 15 turns and remember he's panicked.}
    IF (NEWLOC<9)AND(NEWLOC<>-1)AND CLOSING THEN BEGIN
	RSPEAK(130);
	NEWLOC:=LOCATION;
	IF NOT PANIC THEN CLOCK2:=15;
	PANIC:=TRUE
    END;
    {  See if a dwarf has seen him and has come from where he wants to go.  If 
    so, the dwarf's blocking his way, put him back and tell him about it.  If 
    coming from place forbidden to pirate (dwarves rooted in place) let him get 
    out (and attacked). }

    IF (NEWLOC<>LOCATION)AND NOT FORCED(LOCATION) 
    AND NOT BITSET(LOCATION,3) THEN FOR I:=1 TO 5 DO
    IF (ODLOC[I]=NEWLOC) AND DSEEN[I] AND (NEWLOC<>LOCATION) THEN BEGIN
	NEWLOC:=LOCATION;
	RSPEAK(2);
	EXIT
    END
END;

PROCEDURE FINISH;EXTERNAL;

{   "You're dead, Jim."

    If the current LOCATION is -1, it means the clown got himself killed.  We'll 
allow this  MAXDIE  times.   MAXDIE is set based on the number of snide messages 
available.   Each  death  results  in  a  message  (81, 83, etc.)  which  offers
reincarnation; if accepted, this results in message 82, 84, etc.  The last time,
if he  wants  another  chance,  he  gets  a  snide  remark  as  we  exit.   When
reincarnated,  all objects being carried get dropped at OLDLC2  (presumably  the 
last place prior to being killed)  without  change  of  PROPs.   The  loop  runs
backwards  to  assure  that the bird is dropped before the cage.   The lamp is a 
special case  (it wouldn't do to leave it in the cave).   It is turned  off  and
left outside the building  (only if he was carrying it, of course).   He himself
is left inside the building  (and heaven help him if he tries to XYZZY back into
the cave without the lamp!).   OLDLOC is zapped so he can't just  "retreat".   }

PROCEDURE DEATH;FORWARD;

PROCEDURE PITDEATH;FORWARD;

PROCEDURE ALIVE;
BEGIN
    IF LOCATION=-1 THEN DEATH 
    {  If it's dark two or more moves in a row then he has a 35% chance of 
    falling into a pit.  This is the easiest way to get killed. }
    ELSE IF DARK AND WZDARK AND PCT(35) THEN PITDEATH
END;

PROCEDURE DEATH;
VAR J:INTEGER;
    YEA:BOOLEAN;
BEGIN
    IF NOT CLOSING THEN BEGIN
	YEA:=ASKR(81+NUMDIE*2,82+NUMDIE*2,54);
	NUMDIE:=NUMDIE+1;
	IF (NUMDIE=MAXDIE)OR NOT YEA THEN FINISH ELSE BEGIN
	    DESTROY(WATER);
	    DESTROY(OIL);
	    IF TOTING(LAMP) THEN PROP[LAMP]:=0;
	    FOR J:=MAXTRS DOWNTO 1 DO IF TOTING(J) THEN 
	    IF J=LAMP THEN DROP(J,1) ELSE DROP(J,OLDLC2);
	    NEWLOC:=3;
	    LOCATION:=NEWLOC;
	    OLDLOC:=LOCATION;
	    OLDLC2:=OLDLOC;
	    MOVED:=TRUE
	END

    END ELSE BEGIN
	{  He died during closing time.  No resurrection.  Tally up a death
	and exit.  }
	RSPEAK(131);
	NUMDIE:=NUMDIE+1;
	FINISH
    END
END;

{  The easiest way to get killed is to fall into a pit in the darkness.  }
PROCEDURE PITDEATH;
BEGIN
    RSPEAK(23);
    OLDLC2:=LOCATION;
    DEATH
END;

{  Tell him where he is using the long description followed by ABBNUM short 
descriptions.  }
PROCEDURE WHEREAREWE;
BEGIN
    {  If it's dark and not a forced location, warn him about pit death (he 
    can't see where he is).  Otherwise tell him where he is.    }
    IF DARK AND NOT FORCED(LOCATION) THEN RSPEAK(16) 
    ELSE BEGIN
	{  Is he being followed by the bear? }
	IF TOTING(BEAR) THEN RSPEAK(141);
	{  Use either long or short description.  }
	IF (ABBNUM=0) OR ((ABB[LOCATION] MOD ABBNUM)=0)
	OR (STEXT[LOCATION]=0) THEN SPEAK(LTEXT[LOCATION])
	ELSE SPEAK(STEXT[LOCATION]);
	{  Increment the number of times we've described this location.   }
	ABB[LOCATION]:=ABB[LOCATION]+1
    END;
    {  At "Y2", 25% of the time a hollow voice says "Plugh"}
    IF (LOCATION=33)AND PCT(25) AND NOT CLOSING THEN RSPEAK(8)
END;

{  This procedure describes the objects at the current location and keeps track 
of newly discovered treasures.  }
PROCEDURE WHATSHERE;
VAR LINK:OBJLINK;
    OBJ,I:INTEGER;
BEGIN
    {  If it's dark, he can't see what's here.  }
    IF NOT DARK THEN BEGIN
	{  Point to the head of the ATLOC list for this location.  }
	LINK:=ATLOC[LOCATION];
	{  While there are things in the list, describe them.  }
	WHILE LINK<>NIL DO BEGIN
	    {  Get object number.  }
	    OBJ:=LINK^.OBJ;
	    {  If it's the second of two placed objects, use the normal object 
	    number.  Two placed objects have OBJ+100 linked into the second 
	    location's list.  }
	    IF OBJ>100 THEN OBJ:=OBJ-100;
	    {  Skip the steps description if he's toting the gold. }

	    IF ((OBJ<>STEPS) OR NOT TOTING(NUGGET)) THEN BEGIN
		{ If PROP is -1 and we're not in the repository, he's found 
		another treasure.  }
		IF (PROP[OBJ]<0)AND NOT CLOSED THEN BEGIN
		    {  Make it's property zero.  }
		    PROP[OBJ]:=0;
		    {  Rug is special case; once seen, it's PROP is 1 (dragon 
		    on it) till dragon is killed.  Similarly for chain; PROP is
		    initially 1 (locked to bear).  This is because PROP=0 is 
		    needed to get full score.  }
		    IF (OBJ=RUG)OR(OBJ=CHAIN) THEN PROP[OBJ]:=1;
		    {  Tally off another treasure.  }
		    TALLY:=TALLY-1;
		    {  If the remaining treasures are too elusive (if the snake 
		    eats the bird, for example), then zap his lamp.  }
		    IF (TALLY=TALLY2)AND(TALLY<>0)AND(LIMIT>35) THEN LIMIT:=35;
		END;
		I:=PROP[OBJ];
		{  Use the description for the bottom of the steps if at the 
		east end of the hall of mists. }
		IF (OBJ=STEPS)AND(LOCATION=FIXED[STEPS]) THEN I:=1;
		{  Describe the object according to it's current property 
		value. Skip the description for objects in the repository which 
		haven't been moved yet (PROP=-1). }
		IF I>=0 THEN PSPEAK(OBJ,I)
	    END;
	    { Advance to the next objlink and repeat if not nil.  }
	    LINK:=LINK^.NXT
	END  { While LINK<>NIL }
    END { If not DARK }
END;

{****************  DWARF PROCEDURE  *******************}

{  This procedure handles activation, movement, and attack routines for the
dwarves and the pirate.  }
PROCEDURE DWARFSTUFF;
VAR	I,J,DTOTAL,ATTACK,STICK:INTEGER;

{  When we know what the dwarves are up to we call this local procedure to
tell the poor sucker about it!   }
PROCEDURE DWARFATTACK;
VAR K:INTEGER;
BEGIN
    IF DTOTAL=1 THEN RSPEAK(4) ELSE BEGIN
	WRITELN;
	WRITE('There are ');
	WRITE(DTOTAL:1);
	WRITELN(' threatening little dwarves in the room with you.');
    END;
    IF ATTACK>0 THEN BEGIN
	IF DFLAG=2 THEN DFLAG:=3;
	IF ATTACK=1 THEN BEGIN
	    RSPEAK(5);
	    K:=52
	END ELSE BEGIN
	    WRITELN;

	    WRITE(ATTACK:1);
	    WRITELN(' of them throw knives at you!');
	    K:=6
	END;
	IF STICK<=1 THEN RSPEAK(K+STICK) ELSE BEGIN
	    WRITELN;
	    WRITE(STICK:1);
	    WRITELN(' of them get you!')
	END;
	IF STICK<>0 THEN BEGIN
	    {  Too bad --  This guy got killed.  }
	    OLDLC2:=LOCATION;
	    LOCATION:=-1
	END
    END
END;

{  This procedure is called if the pirate's spotted him.  He leaves him alone
once we've found the chest (the call is not made).  }
PROCEDURE PIRATE;
VAR J,K:INTEGER;
BEGIN
    {  K counts if a treasure is here.  If not, and TALLY=TALLY2 plus one (for 
    an unseen chest), let the pirate be spotted.   }
    K:=0;
    FOR J:=50 TO MAXTRS DO BEGIN
	{  Pirate won't take the pyramid from the plover room or the dark room 
	(too easy!).    }
	IF ((J<>PYRAMID)OR((LOCATION<>PLACE1[PYRAMID])
	AND(LOCATION<>PLACE1[EMERALD])))AND TOTING(J) THEN BEGIN
	    {  This guy has treasure to steal.   }
	    RSPEAK(128);
	    {  First time this happens put the chest in the maze and the clue 
	    in the other maze.   }
	    IF PLACE[MESSAGE]=0 THEN MOVE(CHEST,CHLOC);
	    MOVE(MESSAGE,CHLOC2);
	    {  Now steal everything.  }
	    FOR J:=50 TO MAXTRS DO BEGIN
		IF (J<>PYRAMID)OR((LOCATION<>PLACE1[PYRAMID])
		AND(LOCATION<>PLACE1[EMERALD])) THEN BEGIN
		    IF AT(J) AND(FIXED[J]=0) THEN CARRY(J);
		    IF TOTING(J) THEN DROP(J,CHLOC)
		END
	    END;
	    {  Finally, move the pirate to his chest location.  }
	    DLOC[6]:=CHLOC;
	    ODLOC[6]:=CHLOC;
	    DSEEN[6]:=FALSE
	END;
	IF J>MAXTRS THEN J:=MAXTRS;
	{  Set K if there is any treasure here at all.  }
	IF HERE(J) THEN K:=1
    END;
    IF DSEEN[6] THEN BEGIN
	{  If there are no treasures here *and* the rest can't be found *and* 
	the lamp is here and on *and* the pirate hasn't stolen anything, we let 
	the pirate be spotted and pop him and his chest back to the maze.   }

	IF (TALLY=(TALLY2+1))AND(K=0)AND(PLACE[CHEST]=0)
	AND HERE(LAMP) AND(PROP[LAMP]=1) THEN BEGIN
	    RSPEAK(186);
	    MOVE(CHEST,CHLOC);
	    MOVE(MESSAGE,CHLOC2);
	    DLOC[6]:=CHLOC;
	    ODLOC[6]:=CHLOC;
	    DSEEN[6]:=FALSE
	END;
	{  Finally, if the pirate sees him but there is no treasure to be had 
	and there is more to be found, he hears the pirate sneaking around 
	("faint rustling noises in the darkness").   }
	IF (ODLOC[6]<>DLOC[6])AND PCT(20) THEN RSPEAK(127)
    END
END;

{  This procedure moves a dwarf "I" at random by searching the travel table for
all valid moves from a given location and selecting one at random.  Backward 
movement is not allowed, dwarves may not move into forced travel locations, and
they may not go to locations<15.   }

PROCEDURE MOVEDWARF(I:INTEGER);
VAR COND,J,NEWLOC:INTEGER;
    LINK:KEYLINK;
    TK:ARRAY[0..20] OF INTEGER;
BEGIN
    {  Zero out the array of possible new locations.  }
    FOR J:=0 TO 20 DO TK[J]:=0;
    {  Set the number of possible new locations to 1.   }
    J:=1;
    {  Get the first key link from the travel table for this dwarf's location. }
    LINK:=KEY[DLOC[I]];
    {  Now search the keylinks for valid new locations for our friend.   }
    WHILE LINK<>NIL DO BEGIN
	NEWLOC:=LINK^.NEWLOC[2];
	COND:=LINK^.NEWLOC[1];
	{  Check if new location is allowed and different.  }
	IF (NEWLOC<=LOCSIZ)AND(NEWLOC>=15)AND(NEWLOC<>ODLOC[I])
	AND((J=1)OR(TK[J-1]<>NEWLOC))AND(J<20)AND(NEWLOC<>DLOC[I])
	THEN IF (NOT FORCED(NEWLOC))AND((I<>6)OR(NOT BITSET(NEWLOC,3)))
	AND(COND<>100) THEN BEGIN
	    {  If okay, save it and increment the total.   }
	    TK[J]:=NEWLOC;
	    J:=J+1
	END;
	{  Advance to the next keylink. }
	LINK:=LINK^.NXTLINK
    END;
    {  Put the old location in as a last possible choice.  }
    TK[J]:=ODLOC[I];
    {  If we have other locations available, don't allow the old one.   }
    IF J>=2 THEN J:=J-1;
    {  Select a new location at random from the list of possibilities.   }
    J:=RAN(J);
    {  Make the current location old.  }
    ODLOC[I]:=DLOC[I];
    {  Move the dwarf.   }

    DLOC[I]:=TK[J];
    {  Has the fearless Adventurer been spotted?   }
    DSEEN[I]:=(DSEEN[I] AND (LOCATION>=15))OR(DLOC[I]=LOCATION)
    OR(ODLOC[I]=LOCATION);
    {  If so, stay with him and create havoc.   }
    IF DSEEN[I] THEN BEGIN
	{  This threatening little dwarf is in the room with him!  }
	DLOC[I]:=LOCATION;
	{  If the dwarf is really a dwarf and not the pirate then.....}
	IF I<>6 THEN BEGIN
	    {  Increment the total of dwarves who have seen him.   }
	    DTOTAL:=DTOTAL+1;
	    {  If twice at the same location, then attack!!}
	    IF ODLOC[I]=DLOC[I] THEN BEGIN
		ATTACK:=ATTACK+1;
		{  Make the knife available for attempted recovery.  }
		IF KNFLOC>=0 THEN KNFLOC:=LOCATION;
		{  Randomly decide if this knife stuck him or not.  Note that 
		the odds go up as DFLAG increases (dwarves get *mad* ).   }
		IF RAN(1000)<(95*(DFLAG-2)) THEN STICK:=STICK+1
	    END
	END
	{  .....otherwise, the pirate's spotted him!  }
	ELSE IF (LOCATION<>CHLOC)AND(PROP[CHEST]<0) THEN PIRATE
    END
END;

BEGIN { Dwarfstuff }
    {  If this guy's not dead and the location doesn't have forced motion and 
    the new location is not forbidden to the pirate then go........  }
    IF (LOCATION<>-1) THEN IF NOT FORCED(LOCATION)
    AND NOT BITSET(LOCATION,3) THEN BEGIN
	CASE DFLAG OF
	    {  Activate the dwarves after he is at or beyond the hall of mists 
	    (location 15).   }
	    0: IF LOCATION>=15 THEN DFLAG:=1;
	    {  When we encounter the first dwarf, we kill 0,1, or 2 of the 5 
	    dwarves.  If any of the survivors is at the current LOCATION, we 
	    replace him with the alternate.   }
	    1: IF (LOCATION>=15) AND PCT(10) THEN BEGIN
		DFLAG:=2;
		FOR I:=1 TO 2 DO BEGIN
		    J:=RAN(5);
		    IF PCT(50) THEN DLOC[J]:=0
		END;
		FOR I:=1 TO 5 DO BEGIN
		    IF DLOC[I]=LOCATION THEN DLOC[I]:=DALTLC;
		    ODLOC[I]:=DLOC[I]
		END;
		RSPEAK(3);
		MOVE(AXE,LOCATION)
	    END;
	    {  Things are in full swing.  Move each dwarf at random, except if 
	    he's seen us he sticks with us.  Dwarves never go to locations<15. 
	    If wandering at random, they don't back up unless there's no 
	    alternative.  If they don't have to move, they attack.  And, of 
	    course, dead dwarves don't do much of anything.   }

	    ELSE BEGIN
		DTOTAL:=0;
		ATTACK:=0;
		STICK:=0;
		{  Move every dwarf still alive.   }
		FOR I:=1 TO 6 DO IF DLOC[I]<>0 THEN MOVEDWARF(I);
		{  Now we know what's happening.  Let's tell the poor sucker 
		about it.   }
		IF DTOTAL>0 THEN DWARFATTACK
	    END
	END  { Case of dflag. }
    END
END;

