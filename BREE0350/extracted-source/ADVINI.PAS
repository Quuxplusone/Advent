{********>>>>>>>><<<<<<<<********

>>	MODULE:	ADVINI
		29-OCT-80

		This module contains the procedure  INITIALIZE.   It  reads  in
	the database and database pointers and initializes all variables before
	the game commences.  

********>>>>>>>><<<<<<<<********}


{$C	.TITLE ADVINI 
	.IDENT /V0/ }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

PROCEDURE JUGGLE(OBJ:INTEGER);EXTERNAL;

FUNCTION BIT(I:INTEGER):INTEGER;EXTERNAL;

{******************** I/O PROCEDURES ************************}

PROCEDURE GETIN(VAR WORD1,WORD1X,WORD2,WORD2X:WORD);EXTERNAL;

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

{  *****************  INITIALIZATION PROCEDURES  ****************  }

{	The two main data files, ADVENTURE.DAT and ADVENTURE.100, are  organized
the same and contain all the information required by  the  game  modules.   Each
main database file is organized into twelve sections.   Each section  ends  with
a line containing "-1". 

	Section 1 contains the long form descriptions of locations.   Each  line
contains a location number, LOC, and a line of text.  The  set  of  (necessarily 
adjacent) lines whose numbers are LOC form the long description of location LOC.

	Section 2 contains the short form descriptions.   The format is the same
as the long form descriptions,  however,  not  all  locations  have  short  form
descriptions.

	Section 3 contains the object descriptions.   Again,  each line contains
a number followed by a line of text.   If the number,  OBJ, is in the range of 1
to  99,  the message is the "inventory" message for object  OBJ.   Otherwise the
number, m, is 000,100,200,etc., and the message should be the description of the
preceding object when it's "property" (PROP[OBJ]) is m/100.   The  m/100 is used
only to distinguish multiple messages from  multi-line  messages;  the prop info
actually requires all messages for an object  to  be  present  and  consecutive.
Properties which produce no message should be given the message ">$<".

	Section 4 contains arbitrary messages.   Same  format  as  sections 1,2,
and 3, except the numbers bear no relationship  to  anything  (except  for  some
special verbs in the vocabulary and some special travel table entries).

	Section 5 contains the class messages.  Each line contains  a  number  n
and a line describing a classification of player.   The  scoring section selects
the  appropriate  message,  where each message is considered to apply to players
whose scores are higher than or equal to the previous  n  but  not  higher  than
this n.   Note that these scores probably change with  every  modification  (and
particularly expansion) of the program.

	Section 6 contains the magic messages.  Identical  to  section  4 except
put in a separate  section  for  easier  reference.   Magic messages are used by
the maintenence mode routine (MAINTINENCE) and startup routine (START).

	Section 7 contains the vocabulary.   Each  line contains a  number,  KK, 
and a five letter word.  Let m=KK div 1000 and K=KK mod 1000.   If  m=0  (K=KK), 
then the word is a motion verb for use in travelling  (see section 8).   Else if 

m=1, the word is an object and K is the object number (same number as in section
3).  Else, if m=2, the word is an action verb (such  as  "carry"  or  "attack"). 
Else,  if m=3,  the  word  is  a special case verb and K is a message number  in
section 4.

	Section 8 contains the travel array.   Each  line  contains  a  location
number  (LOC),  a conditional motion number  (COND),  a  second  location number
(NEWLOC),  and a list of motion verb  numbers  (see  section  7).   In  general, 
all verbs in a line will cause him to go to NEWLOC  from  LOC  under  conditions
of COND and NEWLOC as follows:

	If COND=0		the motion is unconditional.
	If 0<COND<100		the motion is done with COND % probability.
	If 100<COND<200		he must be carrying object COND-100.
	If 200<COND<300		he must be carrying or in the same room 
				as object COND-200.
	If COND>300		PROP[COND mod 100] must *not* be (COND div
				100)-3. For example, if COND=531, the property
				of the dragon, PROP[31], must not be 2.

	If NEWLOC<=300		it is the location to go to.
	If 300<NEWLOC<=500	NEWLOC-300 is used in a case statement in a 
				special section of code.  (Plover room travel
				and troll bridge).
	If NEWLOC>500		message NEWLOC-500 from section 4 is given and
				he remains at LOC.

	If the condition (if any) is not met, then the next  *different*  NEWLOC
value is used (unless it fails to meet *its* conditions, in which case the  next
is found,  etc.).   Typically,  the next destination will be for one of the same
verbs, so that its only use is as the alternate  destination  for  those  verbs.  
For instance:
	15,110,22,29,31,34,35,23,42
	15,0,14,29
This says that, from LOC 15, any of the verbs 29,31, etc. will take  him  to  22
if he's carrying object 10, and otherwise he will go to 14.
	11,303,8,49
	11,0,9,50
This says that,  from 11,  49 takes him to 8 unless PROP[3]=0,  in which case he
goes to 9.  Verb 50 takes him to 9 regardless of PROP[3].

	Section 9 contains the initial object  locations.   Each  line  contains
an object number, OBJ, followed by two initial location numbers, PLACE1[OBJ] and
FIXED1[OBJ].   If PLACE1[OBJ]=0,  the object has no initial  location.   If  the 
object is immovable, FIXED1[OBJ]=-1.  If it has two locations  (e.g., the grate)
then FIXED1[OBJ]=second location and the object is assumed immovable.

	Section 10 contains  the  action  verb  default  responses.   Each  line 
contains an "action-verb" number and the number in section  4  of  it's  default
response message.

	Section 11 contains the location assets information.  Each line contains
a number,  n,  and up to ten location numbers.   Some values of n have two lines 
and up to twenty locations.  Bit n  (where  0  is  the  units  bit)  is  set  in
CONDI[LOC] for each LOC listed.  The CONDI bits currently assigned are:
	0 -- light
	1 -- if bit 2 is on: on for oil, off for water

	2 -- set if liquid present at LOC, see bit 1 for type
	3 -- pirate doesn't go here unless following him
	(other bits are used to indicate location is eligible for a hint)
	4 -- trying to get into cave
	5 -- trying to catch bird
	6 -- trying to deal with snake
	7 -- lost in maze
	8 -- pondering dark room
	9 -- at Witt's End
CONDI[LOC] is set to 2 (Bit 1  on),  overriding  all  other  bits,  if  LOC  has
forced motion.

	Section 12 contains the hints information. Each  line  contains  a  hint 
number (corresponding to a CONDI bit,  section 11),  the number of turns he must
be at the right LOC(s) before triggering  the  hint,  the  points  deducted  for 
taking the hint, the  message  number  (section 4)  of  the  question,  and  the 
message  number  of  the  hint.   These  values  are stashed in the HINTS array.
HNTMAX is set  to  the  maximum  hint  number.   The HINTED array keeps track of
hints we have given.    Numbers 1-3 are unusable since CONDI bits are  otherwise
assigned, so 2 is used to remember if he's read the clue in the repository,  and
3 is used to remember whether he asked for instructions (gets  more  turns,  but
loses points).

	The files initialization programs (ADVFLS and 100FLS) transfer sections 
1,2,3,4,5, and 6 to the file ADVTXT.DTA (or ADVTXT.100) and the starting record
numbers for each message to arrays LTEXT,STEXT, PTEXT, RTEXT, CTEXT, and  MTEXT
respectively.  Also,  section  7  is  translated  into  a  tree  structure  and
transfered to KATAB.DTA with the starting  letter  record  pointers  stored  in
array KTAB.   The seven record pointer arrays are then stored in ADVDAT.DTA (or
ADVDAT.100).    The remainder of the database,  sections 8-12, are changed from 
ASCII to integers and duplicated in the file ADVENT.DTA.   These  sections  are 
identical  in both the normal and VT100 databases.

	INITIALIZE first asks whether the user is using a VT100 or  not.   Then 
the appropriate version of ADVTXT and ADVDAT are opened, the pointer arrays are 
read in, and ADVDAT is closed.  Then KATAB and ADVENT are opened.  Sections  8,
9,10,11, and 12 are then read in and ADVENT is closed. 
	
	 INITIALIZE then set up other initial variable values  and  returns  to 
the main program block, MAIN.	}

PROCEDURE INITIALIZE;
VAR VERB,LOC,OLDLC,COND,NXTLOC,I,J:INTEGER;
    TK:ARRAY[1..20] OF INTEGER;
    LINK1,LINK4:KEYLINK;
    LINK2,LINK3:VERBLINK;
    ADVENT,ADVDAT:FILE OF INTEGER;

PROCEDURE GETDT;FORWARD;

{ 	This procedure opens the normal text pointers file and gets the data.  }
PROCEDURE GETDAT;
BEGIN
    RESET(ADVDAT,'ADVDAT.DTA/SEEK');
    GETDT
END;


{	This procedure reads the data from either version of ADVDAT into the
record pointer arrays.   }
PROCEDURE GETDT;
BEGIN
    {   Long description pointers.    }
    FOR I:=1 TO LOCSIZ DO BEGIN
	LTEXT[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    {   Short description pointers.   }
    FOR I:=1 TO LOCSIZ DO BEGIN
	STEXT[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    {   Object description pointers.   }
    FOR I:=1 TO MAXTRS DO BEGIN
	PTEXT[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    {   Random message pointers.   }
    FOR I:=1 TO 300 DO BEGIN
	RTEXT[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    {   Magic message pointers.   }
    FOR I:=1 TO 100 DO BEGIN
	MTEXT[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    {   Class messages pointer.   }
    CTEXT:=ADVDAT^;
    GET(ADVDAT);
    {   Vocabulary tree starting letter pointers.   }
    FOR I:=65 TO 90 DO BEGIN
	KTAB[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    CLOSE(ADVDAT)
END;

{	This procedure opens the VT-100 text record pointers file and gets
the data.   }
PROCEDURE GET100;
BEGIN
    RESET(ADVDAT,'ADVDAT.100/SEEK');
    GETDT
END;

{  This procedure reads one integer from ADVENT. }
PROCEDURE RDADV(VAR I:INTEGER);
BEGIN
    I:=ADVENT^;
    GET(ADVENT)
END;

BEGIN	{ INITIALIZATION PROCEDURE }
    {  First find out which database to use }

    WRITELN;
    WRITELN('Are you using a VT100?');
    GETIN(WRD1,WRD1X,WRD2,WRD2X);
    WRITELN;
    WRITELN('Be patient, it takes a while to initialize.....');
    WRITELN;
    WRITELN('Now loading sections 1-7');
    IF WRD1[1]='Y' THEN BEGIN
	{ If yes, set VT100 in ANSI mode and read in .100 data }
	VT100:=TRUE;
	WRITE(CHR(33B),'<');
	GET100;
	RESET(ADVTXT,'ADVTXT.100/SEEK')
    END ELSE BEGIN
	{ If no, get normal database }
	VT100:=FALSE;
	GETDAT;
	RESET(ADVTXT,'ADVTXT.DTA/SEEK')
    END;
    {  Open the vocabulary file.  }
    RESET(KATAB,'KATAB.DTA/SEEK');
    {  Open the initial variables data file. }
    RESET(ADVENT,'ADVENT.DTA/SEEK');
    {***  Read in travel table  ***}
    MSPEAK(32);
    {  Set the array of pointers into the travel tree to NIL }
    FOR I:=1 TO LOCSIZ DO KEY[I]:=NIL;
    {  Read the first location number }
    RDADV(LOC);
    {  If it is zero or -1 there is an error in the database, don't read it. }
    IF (LOC<>-1)AND(LOC<>0) THEN REPEAT{Until LOC=-1,at the end of the section }
	{  Save the current location in OLDLC }
	OLDLC:=LOC;
	{  Read the conditions on this motion }
	RDADV(COND);
	{  Read the next location for this motion }
	RDADV(NXTLOC);
	{  Read in the various verbs which cause this motion }
	FOR I:=1 TO 20 DO RDADV(TK[I]);
	{  Create a new key link record for this location }
	NEW(LINK1);
	{  Store COND and NEWLOC in the new record }
	LINK1^.NEWLOC[1]:=COND;
	LINK1^.NEWLOC[2]:=NXTLOC;
	{  Point the key link back to the KEY pointer array for this location }
	LINK1^.NXTLINK:=KEY[LOC];
	{  Set the verb link pointer to nil }
	LINK1^.NXTVERB:=NIL;
	{  Now point KEY[LOC] to the new record }
	KEY[LOC]:=LINK1;
	{  Now add the verbs list and other motion entries for this same 
	location.  }
	WHILE OLDLC=LOC DO BEGIN
	    {  First check that there are some verbs listed.   }
	    IF TK[1]<>0 THEN BEGIN
		{  Okay, we got some verbs, create a new verb link.  }
		NEW(LINK2);

		{  Put the first verb in the link.   }
		LINK2^.VERBVAL:=TK[1];
		{  Make it's verblink pointer NIL.  }
		LINK2^.NXTVERB:=LINK1^.NXTVERB;
		{  Now point the key link's verb link pointer to this link.  }
		LINK1^.NXTVERB:=LINK2
	    END;
	    {  Now we link the remaining verbs onto the first verb link.  }
	    FOR I:=2 TO 20 DO IF TK[I]<>0 THEN BEGIN
		NEW(LINK3);
		LINK3^.VERBVAL:=TK[I];
		LINK3^.NXTVERB:=LINK2^.NXTVERB;
		LINK2^.NXTVERB:=LINK3;
		LINK2:=LINK3
	    END;
	    {  One complete line has now been read in.  Read the location number 
	    in the next line and see if it still the same.   }
	    RDADV(LOC);
	    {  If so, we read in the data, create a new key link for the same 
	    location, link it to the last keylink for this location, and loop 
	    back to link in the verbs.
	       Otherwise if it is different, we go back to the beginning of the 
	    loop and initialize KEY[LOC] for the new location (unless it's -1 or 
	    zero which terminate the repeat loop).  }
	    IF OLDLC=LOC THEN BEGIN
		RDADV(COND);
		RDADV(NXTLOC);
		FOR I:=1 TO 20 DO RDADV(TK[I]);
		NEW(LINK4);
		LINK4^.NEWLOC[1]:=COND;
		LINK4^.NEWLOC[2]:=NXTLOC;
		LINK4^.NXTVERB:=NIL;
		LINK4^.NXTLINK:=LINK1^.NXTLINK;
		LINK1^.NXTLINK:=LINK4;
		LINK1:=LINK4;
	    END { if oldlc=loc then }
	END { while oldlc=loc do }
    UNTIL (LOC=-1)OR(LOC=0);
    {  Zero out various arrays	}
    FOR I:=1 TO MAXTRS DO BEGIN
	PLACE1[I]:=0;
	FIXED1[I]:=0;
	PLACE[I]:=0;
	FIXED[I]:=0;
	ACTSPK[I]:=0;
	PROP[I]:=0
    END;
    FOR I:=1 TO LOCSIZ DO CONDI[I]:=0;
    FOR I:=1 TO 10 DO FOR J:=1 TO 4 DO HINTS[I,J]:=0;
    FOR I:=-1 TO LOCSIZ DO ATLOC[I]:=NIL;
    { Read in initial object locations	}
    MSPEAK(33);
    RDADV(OBJ);
    WHILE OBJ<>-1 DO BEGIN
	RDADV(PLACE1[OBJ]);
	RDADV(FIXED1[OBJ]);
	RDADV(OBJ);

    END;
    {  Read in default action verb responses	}
    MSPEAK(34);
    RDADV(VERB);
    WHILE VERB<>-1 DO BEGIN
	RDADV(ACTSPK[VERB]);
	RDADV(VERB)
    END;
    {  Read in the conditions for various locations	}
    MSPEAK(35);
    RDADV(I);
    WHILE I<>-1 DO BEGIN
	FOR K:=1 TO 10 DO RDADV(TK[K]);
	FOR K:=1 TO 10 DO
	IF TK[K]<>0 THEN CONDI[TK[K]]:=CONDI[TK[K]]+BIT(I);
	RDADV(I);
    END;
    {  Read in the hints information	}
    MSPEAK(36);
    RDADV(I);
    HNTMAX:=0;
    WHILE I<>-1 DO BEGIN
	FOR J:=1 TO 4 DO RDADV(TK[J]);
	IF I<>0 THEN FOR J:=1 TO 4 DO HINTS[I,J]:=TK[J];
	IF I>HNTMAX THEN HNTMAX:=I;
	RDADV(I);
    END;
    { Close out the initial variable data file. }
    CLOSE(ADVENT);
    {  Zero out the abbreviation counts and set CONDI[location]=2 if the 
    location has forced motion.	}
    FOR I:=1 TO LOCSIZ DO BEGIN
	ABB[I]:=0;
	IF (KEY[I]<>NIL) AND (KEY[I]^.NXTVERB^.VERBVAL=1) THEN CONDI[I]:=2
    END;
    {  Put all of the objects at their initial locations.	}
    FOR I:=MAXTRS DOWNTO 1 DO BEGIN
	PLACE[I]:=PLACE1[I];
	FIXED[I]:=FIXED1[I];
	JUGGLE(I)
    END;
    { Set the tally of "unseen" objects to zero.   }
    TALLY:=0;
    { Set the tally of impossible to get objects to zero.   }
    TALLY2:=0;
    { Set the initial property of treasures to -1 and initialize tally to the 
    number of treasures to be seen.   }
    FOR I:=50 TO MAXTRS DO BEGIN
	PROP[I]:=-1;
	TALLY:=TALLY+1
    END;
    { Set the "hinted" array to false and the hints location counters to zero. }
    FOR I:=1 TO HNTMAX DO BEGIN
	HINTED[I]:=FALSE;
	HINTLC[I]:=0
    END;
    { Initialize the dwarves.  DLOC is loc of dwarves, hard-wired in.  ODLOC  is

    prior location of  each  dwarf,  initially  garbage.   DALTLC  is  alternate 
    location for dwarf,  in case one of them starts out on top of the adventurer. 
    No two of the five initial locations are adjacent.  DSEEN is true  if  dwarf 
    has seen him.
    DFLAG controls the level of activation of all this:
	0	No dwarf stuff yet (wait until reaches the hall of mists)
	1	Reached hall of mists, but hasn't met first dwarf
	2	Met first dwarf, others start moving, no knives thrown yet
	3	A knife has been thrown (first set always misses)
	3+	Dwarves are mad (increases their accuracy)
    Sixth dwarf is speacial  (the pirate).   He always starts  at  his  chest's
    eventual location inside the maze.  This loc is saved  in  CHLOC  for  ref.
    The dead end in the other maze has its loc stored in CHLOC2.     }
    CHLOC:=114;
    CHLOC2:=140;
    FOR I:=1 TO 6 DO DSEEN[I]:=FALSE;
    DFLAG:=0;
    DLOC[1]:=19;
    DLOC[2]:=27;
    DLOC[3]:=33;
    DLOC[4]:=44;
    DLOC[5]:=64;
    DLOC[6]:=CHLOC;
    DALTLC:=18;
    {   Other random flags and counters:     }        
    STRTRAN:=TRUE;
    TURNS:=0;
    SETUP:=FALSE;
    LMWARN:=FALSE;
    IWEST:=0;
    KNFLOC:=0;
    DETAIL:=0;
    ABBNUM:=5;
    MAXDIE:=3;
    NUMDIE:=0;
    HOLDING:=0;
    DKILL:=0;
    FOOBAR:=0;
    BONUS:=0;
    CLOCK1:=30;
    CLOCK2:=50;
    CLOSING:=FALSE;
    PANIC:=FALSE;
    CLOSED:=FALSE;
    GAVEUP:=FALSE;
    SCORING:=FALSE;
    WZDARK:=FALSE;
    { Start the Adventurer out at the end of the road in front of the building.}
    LOCATION:=1;
    { Set the new location and both old locations to the present location.  }
    NEWLOC:=LOCATION;
    OLDLOC:=LOCATION;
    OLDLC2:=LOCATION;
    { The game now commences.... }
    DONE:=FALSE
END;

