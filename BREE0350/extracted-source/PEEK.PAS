{********>>>>>>>><<<<<<<<********

>>	PROGRAM:   PEEK
	29-OCT-80

		This self contained program is used for maintinence purposes
	to peek at the current contents of ADVWIZ.DTA.  It must be assembled
	using ADVGBL.PAS for variable declarations.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE PEEK
	.IDENT /V0/  }

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

PROCEDURE HOURS;EXTERNAL;

PROCEDURE MOTD(X:BOOLEAN);EXTERNAL;

VAR J:INTEGER;
BEGIN
    RESET(ADVWIZ,'ADVWIZ.DTA/SEEK');
    WRITE('The current magic word is ');
    FOR I:=1 TO 5 DO BEGIN
	WRITE(CHR(ADVWIZ^+ORD('A')));
	GET(ADVWIZ)
    END;
    WRITELN('.');
    WRITE('The current magic number is ');
    FOR I:=1 TO 5 DO BEGIN
	WRITE(ADVWIZ^:1);
	GET(ADVWIZ)
    END;
    WRITELN('.');
    SEEK(ADVWIZ,39);
    WRITELN('The number of turns in a demonstration game is',ADVWIZ^:3,'.');
    GET(ADVWIZ);
    I:=ADVWIZ^;
    WRITELN('The current number of saved games is',I:2,'.');
    GET(ADVWIZ);
    WRITELN('The current latency requirement is',ADVWIZ^:3,' minutes.');
    GET(ADVWIZ);
    IF I<>0 THEN BEGIN
	FOR I:=1 TO 3 DO BEGIN
	    WRITELN('The name under which game #',I:1,' is stored:');

	    WRITE('	');
	    FOR J:=1 TO 20 DO BEGIN
		WRITE(CHR(ADVWIZ^+ORD('A')));
		GET(ADVWIZ)
	    END;
	    WRITELN
	END;
    END;
    CLOSE(ADVWIZ);
    WRITELN('The current prime time specs are:');
    HOURS;
    WRITELN('The current message of the day is:');
    MOTD(FALSE)
END.
