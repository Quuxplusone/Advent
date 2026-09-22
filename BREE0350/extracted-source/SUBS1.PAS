{********>>>>>>>><<<<<<<<********

>>	MODULE:	SUBS1
		21-OCT-80

		This module contains the procedure MOVE and it's special case
	relatives: CARRY,DROP,JUGGLE and DESTROY; the functions AT, LIQ, and
	VOCAB; and the procedure PSPEAK.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE SUBS1
	.IDENT /V0/   }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

{  This procedure moves an object from it's present location to WHERE by
first updating PLACE[OBJ] (or FIXED[OBJ] for the second of two placed 
objects), and then removing it's objlink from the ATLOC chain for it's present 
location (or creating a new objlink if none exists) and linking it into the 
ATLOC chain for the new location.  }

PROCEDURE MOVE(OBJ,WHERE:INTEGER);
VAR FROM:INTEGER;
    LINK,LINK0:OBJLINK;
BEGIN
    { First do single placed objects and first location of two-placed objects. }
    IF OBJ<100 THEN BEGIN
	{  Save prior location.  }
	FROM:=PLACE[OBJ];
	{  Set new location.  }
	PLACE[OBJ]:=WHERE
    END ELSE 
    { OBJ+100 indicates second copy of two-placed objects, so FIXED is updated 
    rather than PLACE. }
    BEGIN
	{  Save prior location.   }
	FROM:=FIXED[OBJ-100];
	{  Set new location.  }
	FIXED[OBJ-100]:=WHERE
    END;
    {  Point to first objlink in ATLOC for prior location.  }
    LINK0:=ATLOC[FROM];
    {  If it is not an empty list, search for this object's objlink.  }
    IF LINK0<>NIL THEN IF LINK0^.OBJ=OBJ THEN BEGIN
	{  If it's at the head of the list, pull it off.  }
	LINK:=LINK0;
	ATLOC[FROM]:=LINK0^.NXT
    END
    {  Otherwise look further down the list.  }
    ELSE BEGIN
	{  While we have another objlink to check and it's not the one we're 
	looking for, advance down the list. }
	WHILE (LINK0^.NXT<>NIL)AND(LINK0^.NXT^.OBJ<>OBJ) DO LINK0:=LINK0^.NXT;
	{  If we found it, pull it out and collapse the list. }
	IF LINK0^.NXT<>NIL THEN BEGIN
	    LINK:=LINK0^.NXT;
	    LINK0^.NXT:=LINK^.NXT
	END
	{  Otherwise it's not here so create a new objlink.  }
	ELSE NEW(LINK)
    END
    {  If the list at FROM is empty, create a new objlink.  }
    ELSE NEW(LINK);
    { Now link the objlink into the head of the list at it's new location. }
    LINK^.NXT:=ATLOC[WHERE];
    LINK^.OBJ:=OBJ;
    ATLOC[WHERE]:=LINK

END;

{   Carry an object by incrementing the player holding count and moving object
to player's clutches (location -1).  }

PROCEDURE CARRY(OBJ:INTEGER);
BEGIN
    HOLDING:=HOLDING+1;
    MOVE(OBJ,-1)
END;

{  Drop an object (reverse of carry),  decrement holding count and move object
from player to somewhere else (need not be present location).  }
PROCEDURE DROP(OBJ,WHERE:INTEGER);
BEGIN
    HOLDING:=HOLDING-1;
    MOVE(OBJ,WHERE)
END;

{  Bring an object to the top of it's ATLOC list, causing it to be listed
first when WHATSHERE or WHATYOUGOT prints objects at a location or being
carried.  }
PROCEDURE JUGGLE(OBJ:INTEGER);
BEGIN
    MOVE(OBJ,PLACE[OBJ]);
    IF FIXED[OBJ]>0 THEN MOVE(OBJ+100,FIXED[OBJ])
END;

{  This procedure destroys an object by moving it to "non-existant" 
location 0.  }
PROCEDURE DESTROY(OBJ:INTEGER);
BEGIN
    MOVE(OBJ,0)
END;

{  This function is true if an object is at the player's current location (not
being carried).  Ususally used for two placed objects which can't be carried
anyway.  }
FUNCTION AT(OBJ:INTEGER):BOOLEAN;
BEGIN
    AT:=(PLACE[OBJ]=LOCATION) OR (FIXED[OBJ]=LOCATION)
END;

{  This function returns the object number of the liquid in the bottle, or 0 if
the bottle is empty.  }
FUNCTION LIQ:INTEGER;
BEGIN
    IF PROP[BOTTLE]=0 THEN LIQ:=WATER
    ELSE IF PROP[BOTTLE]=2 THEN LIQ:=OIL
    ELSE LIQ:=0
END;

{  This function is used to hunt for a player input word in the vocabulary tree
file and return it's numeric definition as defined in the original database.
If the argument INIT is -1, the first listed (lowest value) is returned for 
words with multiple definitions.  Otherwise, INIT is taken as the word type 
and the value returned is for that type only and is stripped of it's type 

information.  If a word is undefined, the value -1 is returned.

   As an example, take the case of the word "steps" which is defined as a motion
verb (34) and as an object (1007).  VOCAB('STEPS',-1) would return the value 34,
same as VOCAB('STEPS',0), however VOCAB('STEPS',1) would return the value 7.  
Other examples:		VOCAB('FEE  ',-1)=2025
			VOCAB('FEE  ',2)=25
			VOCAB('FEE  ',3)=1
			VOCAB('FEE  ',0)=-1 (undefined)   }

FUNCTION VOCAB(ID: WORD; INIT: INTEGER): INTEGER;
VAR I,J,K:INTEGER;
    LTR:CHAR;

{  This local procedure extracts one letter from the input word string and
increments an index into the string.  We do a little fiddling with the input
letter before returning in order to be compatible with the way in which the
KATAB file is linked: only ASCII values of capital letters are allowed.  So,
the "2" in "Y2" is treated as a "T" and the quote and question mark are 
replaced by "Q".  GETIN automatically converts smalls to caps, so all other 
ASCII characters are returned as a "Z" in order to kick out funny input.
If we index off the end of the input word we return a blank.  
   The resulting letter (or blank) is left in variable LTR and the ORD is left
in K, both of which are local to VOCAB.  }

PROCEDURE GETLTR;
VAR DIF:INTEGER;
BEGIN
    I:=I+1;
    IF I<=5 THEN LTR:=ID[I] ELSE LTR:=' ';
    IF LTR='2' THEN LTR:='T'
    ELSE IF (LTR='"')OR(LTR='?') THEN LTR:='Q';
    IF (LTR<>' ')AND((LTR<'A')OR(LTR>'Z')) THEN LTR:='Z';
    K:=ORD(LTR)
END;

BEGIN { vocab } 
    { Reset index}
    I:=0;
    { Get the first letter from the input word to LTR and it's ORD to K.  }
    GETLTR;
    { If we have a starting letter, index by ORD (ASCII value with OMSI) into 
    the vocabulary file.  Otherwise, exit with -1. }
    IF LTR<>' ' THEN J:=KTAB[K] ELSE J:=0;
    IF J<>0 THEN BEGIN
	SEEK(KATAB,J);
	{ Get the second letter, if any. }
	GETLTR;
	{ If the second letter is non-blank, then index down the tree until we 
	run out of input letters or run out of tree.  }
	IF LTR<>' ' THEN WHILE KATAB^.NXT[K]<>0 DO BEGIN
	    J:=KATAB^.NXT[K];
	    SEEK(KATAB,J);
	    GETLTR;
	    IF LTR=' ' THEN EXIT
	END;
	{ If LTR is a blank then we reached a node in the tree corresponding to 

	the input, otherwise return with -1. }
	IF LTR=' ' THEN
	{ If INIT=-1 then return the first, full definition.  Otherwise look 
	for one of type INIT = value div 1000. }
	IF INIT=-1 THEN VOCAB:=KATAB^.VAL[1] ELSE BEGIN
	    IF (KATAB^.VAL[1] DIV 1000)=INIT
	    THEN VOCAB:=KATAB^.VAL[1] MOD 1000
	    ELSE IF (KATAB^.VAL[2] DIV 1000)=INIT
	    THEN VOCAB:=KATAB^.VAL[2] MOD 1000
	    ELSE VOCAB:=-1
	END ELSE VOCAB:=-1
    END ELSE VOCAB:=-1
END;

{ *************  I/O  SUBROUTINES   ********************}

{  This procedure prints out object descriptions for object number OBJ.
PROP controls which message to use, -1 says inventory message, otherwise
use description numbered PROP*100. (see database description in ADVINI)  }
PROCEDURE PSPEAK(OBJ,PROP:INTEGER);
VAR I,OLDLOC,REC:INTEGER;
    OFF:ARRAY[1..3] OF CHAR;
BEGIN
    {  Point to the starting record in ADVTXT for this object.  }
    REC:=PTEXT[OBJ];
    SEEK(ADVTXT,REC);
    WITH ADVTXT^ DO BEGIN
	{ If PROP is -1, print out the inventory message for the object, (which 
	we are already pointing to), otherwise seek out the description 
	corresponding to PROP value.  }
	IF PROP>=0 THEN REPEAT 
	    GET(ADVTXT);
	    REC:=REC+1;
	UNTIL (LOC DIV 100) = PROP;
	{  Check for the 'no message' flag, '>$<' }
	FOR I:=1 TO 3 DO OFF[I]:=TXT[I];
	IF OFF<>'>$<' THEN BEGIN
	    {  Save line number for multi-line messages.  }
	    OLDLOC:=LOC;
	    {  If it's not the inventory message then space a line. }
	    IF PROP<>-1 THEN WRITELN;
	    {  Output the message.  }
	    WHILE LOC=OLDLOC DO BEGIN
		WRITELN(TXT);
		GET(ADVTXT)
	    END
	END
    END
END;

