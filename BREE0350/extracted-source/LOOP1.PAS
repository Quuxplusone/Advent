{********>>>>>>>><<<<<<<<********

>>	MODULE:	LOOP1
		28-OCT-80

		This module contains the procedures ENDDEMO, CAVECLOSING and 
	CLOSECAVE.  It also contains the procedures HINTROUTINE, CLSECLUE, 
	LAMPDIM, LAMP0, LAMP1, and LAMP2.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE	LOOP1
	.IDENT /V0/   }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE MOVE(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE DROP(OBJ,WHERE:INTEGER);EXTERNAL;

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

PROCEDURE DESTROY(OBJ:INTEGER);EXTERNAL;

FUNCTION TOTING(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION HERE(OBJ:INTEGER):BOOLEAN;EXTERNAL;

FUNCTION BITSET(LOC,N:INTEGER):BOOLEAN;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE PSPEAK(OBJ,PROP:INTEGER);EXTERNAL;

FUNCTION ASKR(I,J,K:INTEGER):BOOLEAN;EXTERNAL;

{ ****************** MAGIC MODE PROCEDURES ********************* }

PROCEDURE FINISH;EXTERNAL;

{  This procedure is called to end a demonstration game.  }
PROCEDURE ENDDEMO;
BEGIN
    MSPEAK(1);
    FINISH
END;

{ ***************** CAVE CLOSING PROCEDURES ****************** }

{     CAVECLOSING and CLOSECAVE handle  the  closing  of  the  cave.   The  cave
closes CLOCK1 turns after the last treasure has  been  located   (including  the
Pirate's chest, which may of course never show up).   Note  that  the  treasures
need not have been taken yet,  just located.   Hence CLOCK1 must be large enough
to get out of the  cave  (it only ticks while inside the cave).   When  it  hits
zero,  we call CAVECLOSING to start closing the cave, and then sit back and wait
for him to try and get out.   If he doesn't within  CLOCK2  turns,  we close the
cave;  if he does try,  we assume he panics, and give him a few additional turns
to get frantic before we close.   When  CLOCK2  hits zero, we call  CLOSECAVE to
transport him into the final puzzle.   Note that the  puzzle  depends  upon  all
sorts of random things.   For instance,  there must be no water  or  oil,  since
there are beanstalks which we don't want to be able to  water,  since  the  code
can't handle it.   Also,  we can have no keys,  since there is a  grate  (having
moved the fixed object!)  there  separating  him  from all the treasures.   Most
of these problems arise from the use of negative PROP  numbers  to  supress  the

object descriptions until he's actually moved the objects.
 
     When the first warning comes (via CAVECLOSING),  we lock the grate, destroy
the bridge,  kill all the  dwarves  (and the pirate),  remove the troll and bear 
(unless dead),  and set CLOSING to true.   Leave the dragon; too much trouble to 
move it.   From now until CLOCK2 runs out,  he cannot unlock the grate,  move to 
any location outside the cave  (LOCATION<9),  or create the bridge.   Nor can he 
be resurrected if he dies.   Note that the snake is already gone,  since he  got 
to the treasure accesible only via the Hall of the Mt. King.   Also,  he's  been 
in Giant Room (to get eggs), so we can refer to it.   Also also, he's gotten the 
pearl, so we know the bivalve is an oyster.   *And*,  the dwarves must have been 
activated, since we've found the chest.

     Once he's panicked, and CLOCK2 has run out, we call CLOSECAVE to set up the
storage room.   The room has two LOCATIONs,  hardwired as 115 (ne) and 116 (sw).
At the ne end,  we place empty bottles,  a nursery of plants,  a bed of oysters,
a pile of lamps, rods with stars, sleeping dwarves,  and him.   At the sw end we
place grate over  treasures,  snake pit,  covey of caged birds,  more rods,  and 
pillows.   A mirror stretches across one wall.   Many of the objects  come  from
known locations and/or states (e.g. the snake is known to  have  been  destroyed
and needn't be carried away from it's old PLACE),  making  the  various  objects
be  handled  differently.   We  also drop all other objects he might be carrying
(lest we have some that could cause trouble, such as the keys).  We describe the
flash of light and trundle back.  }

PROCEDURE CAVECLOSING;
VAR I:INTEGER;
BEGIN
    PROP[GRATE]:=0;
    PROP[FISSURE]:=0;
    FOR I:=1 TO 6 DO BEGIN
	DSEEN[I]:=FALSE;
	DLOC[I]:=0
    END;
    DESTROY(TROLL);
    DESTROY(TROLL+100);
    MOVE(TROLL2,PLACE1[TROLL]);
    MOVE(TROLL2+100,FIXED1[TROLL]);
    JUGGLE(CHASM);
    IF PROP[BEAR]<>3 THEN DESTROY(BEAR);
    PROP[CHAIN]:=0;
    FIXED[CHAIN]:=0;
    PROP[AXE]:=0;
    FIXED[AXE]:=0;
    RSPEAK(129);
    CLOCK1:=-1;
    CLOSING:=TRUE
END;

PROCEDURE CLOSECAVE;
VAR I:INTEGER;

{This local function is used when setting up the repository. It moves objects or 
drops them (if being carried) and returns a value used to set new PROP value.  }
FUNCTION PUT(OBJ,WHERE,PVAL:INTEGER):INTEGER;
BEGIN
    IF TOTING(OBJ) THEN DROP(OBJ,WHERE) ELSE MOVE(OBJ,WHERE);

    PUT:=-1-PVAL
END;

BEGIN { Closecave }
    PROP[BOTTLE]:=PUT(BOTTLE,115,1);
    PROP[PLANT]:=PUT(PLANT,115,0);
    PROP[OYSTER]:=PUT(OYSTER,115,0);
    PROP[LAMP]:=PUT(LAMP,115,0);
    PROP[ROD]:=PUT(ROD,115,0);
    PROP[DWARF]:=PUT(DWARF,115,0);
    LOCATION:=115;
    NEWLOC:=115;
    OLDLOC:=115;
    OLDLC2:=115;
    {  Leave the grate with normal (non-negative) PROP  }
    I:=PUT(GRATE,116,0);
    PROP[SNAKE]:=PUT(SNAKE,116,1);
    PROP[BIRD]:=PUT(BIRD,116,1);
    PROP[CAGE]:=PUT(CAGE,116,0);
    PROP[ROD2]:=PUT(ROD2,116,0);
    PROP[PILLOW]:=PUT(PILLOW,116,0);
    PROP[MIRROR]:=PUT(MIRROR,115,0);
    FIXED[MIRROR]:=116;
    FOR I:=1 TO MAXTRS DO IF TOTING(I) THEN DROP(I,0);
    RSPEAK(132);
    CLOSED:=TRUE
END;

{ **************** HINT ROUTINE ********************* }

{  This procedure is called to check if the current LOCATION is eligible for any 
hints.  If been here long enough, calls local procedure GIVEHINT to give the 
hint.  }
PROCEDURE HINTROUTINE;
VAR I,J:INTEGER;

{  Local procedure to implement the giving of a hint.  }
PROCEDURE GIVEHINT(I:INTEGER);
BEGIN
    {  First ask the question unique to each hint.  Does he need help? }
    IF ASKR(HINTS[I,3],0,54) THEN BEGIN
	{  Let him know what it will cost to accept the hint.  }
	WRITELN;
	WRITE('I am prepared to give you a hint, but it will cost you');
	WRITE(HINTS[I,2]:3);
	WRITE(' points.');
	{  Ask if he still wants it and, if so, answer with hint.  }
	HINTED[I]:=ASKR(175,HINTS[I,4],54);
    END;
    {  If he took the hint, give him a little more lamp time.  }
    IF HINTED[I]AND(LIMIT>30) THEN LIMIT:=LIMIT+30*HINTS[I,2];
    {  Finally, clear HINTLC back to zero.  }
    HINTLC[I]:=0
END;

BEGIN    { Hintroutine }
    {  Hints less than 4 are special, see database description.  }

    FOR I:=4 TO HNTMAX DO IF NOT HINTED[I] THEN BEGIN
	{  Increment the HINTLC counter for this location if it is eligible for 
	a hint; all other counters are kept at zero.  }
	IF NOT BITSET(LOCATION,I) THEN HINTLC[I]:=-1;
	HINTLC[I]:=HINTLC[I]+1;
	{  If he's been here long enough, do some quick tests and, if they pass,
	give the hint.  }
	IF HINTLC[I]>=HINTS[I,1] THEN BEGIN
	    J:=I-3;
	    CASE J OF
		{Cave}	1: IF (PROP[GRATE]=0)AND NOT HERE(KEYS)
		THEN GIVEHINT(I) ELSE HINTLC[I]:=0;
		{Bird} 2: IF HERE(BIRD) AND TOTING(ROD) AND (OBJ=BIRD)
		THEN GIVEHINT(I);
		{Snake}3: IF HERE(SNAKE) AND NOT HERE(BIRD)
		THEN GIVEHINT(I) ELSE HINTLC[I]:=0;
		{Maze} 4: IF (ATLOC[OLDLC2]=NIL) AND (ATLOC[OLDLOC]=NIL)
		AND (ATLOC[LOCATION]=NIL) AND (HOLDING>1)
		THEN GIVEHINT(I) ELSE HINTLC[I]:=0;
		{Dark} 5: IF (PROP[EMERALD]<>-1) AND (PROP[PYRAMID]=-1)
		THEN GIVEHINT(I) ELSE HINTLC[I]:=0;
		{Witt} 6: GIVEHINT(I)
	    END
	END
    END
END;

{ ************ CLOSED CLUE ********************** }

{  In the repository,  if he happens to pick up the  oyster,  hmmmm.....   Also, 
check for any objects being toted with PROP<0 and set the PROP to -1-PROP.  This 
way objects won't be described  until  they've  been  picked  up  and  put  down 
separate from their respective piles.  }
PROCEDURE CLSECLUE;
BEGIN
    IF (PROP[OYSTER]<0) AND TOTING(OYSTER) THEN PSPEAK(OYSTER,1);
    FOR I:=1 TO MAXTRS DO IF TOTING(I) AND(PROP[I]<0) THEN PROP[I]:=-1-PROP[I]
END;

{ ********************* LAMP DIMMING PROCEDURES *************  }

{  Another way we can force an end to things is by having  the  lamp  give  out.
When it gets close, we call one of these procedures to warn him.   We call LAMP0
if the lamp and fresh batteries are here, in which case we replace the batteries
and continue.   LAMP1 is for other cases of lamp dying.   LAMP2 is used when the 
lamp goes out,  and LAMPDIM is used to tick away the lamp LIMIT and sort out the 
various  cases.   The  case where he's wandered outside the cave and the lamp is 
used up is handled in the procedure RESPONSE, where he is forced to giveup.  }

PROCEDURE LAMP0;
BEGIN
    RSPEAK(188);
    PROP[BATTERY]:=1;
    IF TOTING(BATTERY) THEN DROP(BATTERY,LOCATION);
    LIMIT:=LIMIT+2500;
    LMWARN:=FALSE
END;


PROCEDURE LAMP1;
BEGIN
    IF (NOT LMWARN)AND HERE(LAMP) THEN BEGIN
	LMWARN:=TRUE;
	IF PROP[BATTERY]=1 THEN RSPEAK(189)
	ELSE IF PLACE[BATTERY]=0 THEN RSPEAK(183) ELSE RSPEAK(187)
    END
END;

PROCEDURE LAMP2;
BEGIN
    LIMIT:=-1;
    PROP[LAMP]:=0;
    IF HERE(LAMP) THEN RSPEAK(184)
END;

PROCEDURE LAMPDIM;
BEGIN
    IF PROP[LAMP]=1 THEN LIMIT:=LIMIT-1;
    IF (LIMIT<=30)AND HERE(BATTERY) AND(PROP[BATTERY]=0)AND HERE(LAMP)
    THEN LAMP0 ELSE IF LIMIT=0 THEN LAMP2 ELSE IF LIMIT<=30 THEN LAMP1
END;

