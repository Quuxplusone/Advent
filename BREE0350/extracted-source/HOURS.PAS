{********>>>>>>>><<<<<<<<********

>>	MODULE:	HOURS
		21-OCT-80

		This module contains the "wizardry" routine HOURS and the 
	function LIQLOC.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE	HOURS
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION BIT(N:INTEGER):INTEGER;EXTERNAL;

FUNCTION BITSET(LOC,N:INTEGER):BOOLEAN;EXTERNAL;

{  This function returns the object value of the liquid (if any) present
at the argument location.  If there is none, the value zero is returned.  }
FUNCTION LIQLOC(LOC:INTEGER):INTEGER;
BEGIN
    IF BITSET(LOC,2) THEN
	IF BITSET(LOC,1) THEN LIQLOC:=OIL
	ELSE LIQLOC:=WATER
    ELSE LIQLOC:=0
END;

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

PROCEDURE DATIME(VAR D:UNSIGNED;VAR T:INTEGER);EXTERNAL;

PROCEDURE PRIMETIME;EXTERNAL;

{  This procedure is used to announce the current hours when the cave is open
for adventuring.  This info is stored in WKDAY, WKEND, and HOLID, which are all
two integer arrays.  The first integer covers the hours from 0 (midnight) to
14 (2 pm) where bit N is on if the hour from N:00 to N:59 is "prime time" (cave 
closed).  Bits 0 thru 8 of the second integer are similarly used for hours 15
thru 23. WKDAY is for weekdays, WKEND is for weekends, HOLID for holidays.  Next 
holiday is from HBEGIN to HEND, and has it's name stored in HNAME.  }
PROCEDURE HOURS;
TYPE	DBL=ARRAY[1..2] OF INTEGER;
    	STRNG8=ARRAY[1..8] OF CHAR;

VAR D:UNSIGNED;
    FROM,TILL,J,K,T:INTEGER;

{  Local procedure to grab one set of hours for display.  }
PROCEDURE HOURSX(PTIM:DBL;TITLE:STRNG8);
VAR FROM,TILL,J,K:INTEGER;
    FIRST:BOOLEAN;

BEGIN
    IF (PTIM[1]=0)AND(PTIM[2]=0) THEN WRITELN('	',TITLE,'    Open All Day')
    ELSE BEGIN
 	FIRST:=TRUE;
	FOR FROM:=0 TO 23 DO BEGIN
	    IF FROM<=14 THEN BEGIN
		J:=PTIM[1];
		K:=FROM
	    END ELSE BEGIN
		J:=PTIM[2];
		K:=FROM-15
	    END;
	    IF (FROM<>23)AND((J AND BIT(K))=0) THEN BEGIN
		FOR TILL:=FROM+1 TO 24 DO BEGIN
		    IF TILL<=14 THEN BEGIN
			J:=PTIM[1];
			K:=TILL
		    END ELSE BEGIN
			J:=PTIM[2];
			K:=TILL-15
		    END;
		    IF ((J AND BIT(K))<>0)OR(TILL=24) THEN BEGIN
			IF FIRST THEN WRITE('	',TITLE) 
			ELSE WRITE('		');
			WRITELN(' ',FROM:2,':00 to ',TILL:2,':00');
			FIRST:=FALSE;
			FROM:=TILL;
			EXIT
		    END
		END
	    END ELSE IF FIRST AND(FROM=23) 
	    THEN WRITELN('	',TITLE,'    Closed All Day')
	END
    END
END;

BEGIN { Hours }
    PRIMETIME;
    WRITELN;
    HOURSX(WKDAY,'Mon-Fri:');
    WRITELN;
    HOURSX(WKEND,'Sat-Sun:');
    WRITELN;
    HOURSX(HOLID,'Holidays');
    DATIME(D,T);
    IF NOT ((HEND<D)OR(HEND<HBEGIN)) THEN BEGIN
	IF HBEGIN<=D THEN BEGIN
	    WRITELN;
	    WRITE('Today is a holiday, namely ');

	    WRITELN(HNAME)
	END ELSE BEGIN 
	    D:=HBEGIN-D;
	    IF D=1 THEN WRD1:=' day ' ELSE WRD1:=' days';
	    WRITELN;
	    WRITE('The next holiday will be in');
	    WRITE(D:3);
	    WRITE(WRD1);
	    IF WRD1[5]='s' THEN WRITE(' ');
	    WRITE(', namely ');
	    WRITELN(HNAME)
	END
    END
END;

