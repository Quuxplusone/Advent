{********>>>>>>>><<<<<<<<********

>>	MODULE:	BACKUP
		11-OCT-80

		This module contains the procedure BACKUP.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE BACKUP
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION FORCED(LOC:INTEGER):BOOLEAN;EXTERNAL;

{ ************ VERBS BACKUP, RETREAT, ETC.  ********************* }

{  Handle "go back".  Look for verb which goes from LOCATION to OLDLOC,
or to OLDLC2 if OLDLOC has forced motion.  BACKUP returns with
I=LOCATION if we forgot how he got here, or with a new LINK1 such that
I=LINK1^.NEWLOC[2], if we found a verb to retreat with. }

PROCEDURE BACKUP(VAR I:INTEGER;VAR LINK1:KEYLINK);
BEGIN
    {  Set up what he wants NEWLOC to be in I.  }
    IF FORCED(OLDLOC) THEN I:=OLDLC2 ELSE I:=OLDLOC;
    {  Save new OLDLC2 and OLDLOC as usual.  }
    OLDLC2:=OLDLOC;
    OLDLOC:=LOCATION;
    {  If where he wants to go is here, we forgot how he got here,
    so just return.  Otherwise, search for a way back.   }
    IF I<>LOCATION THEN BEGIN
	{ Point to the key link for LOCATION.  }
	LINK1:=KEY[LOCATION];
	{ Traverse the key links until we find I as a new location for 
	LOCATION or we run out of key links.  }
	WHILE (LINK1<>NIL)AND(LINK1^.NEWLOC[2]<>I) DO
	{ If new location is forced to I, then replace I with the 
	forced location (which terminates the "WHILE" loop); otherwise 
	go to the next key link.  }
	IF LINK1^.NEWLOC[2]<=LOCSIZ THEN
	IF FORCED(LINK1^.NEWLOC[2]) AND
	(KEY[LINK1^.NEWLOC[2]]^.NEWLOC[2]=I)
	THEN I:=LINK1^.NEWLOC[2] ELSE
	LINK1:=LINK1^.NXTLINK  ELSE LINK1:=LINK1^.NXTLINK
    END
END;

