{********>>>>>>>><<<<<<<<********

>>	PROGRAM:   POOF
		   28-OCT-80

		This program is used to initialize the magic parameters file 
	ADVWIZ with dummy data prior to the "first" wizard tweaking things.
	It can also be used to restore it if for some reason it gets clobbered
	by the system.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE	POOF
	.IDENT	/V0/	}

{  All of the data required for the wizardry routines, including saving games,
is stored encoded in one random access file of integers: ADVWIZ.
The current record usage assignments for this file are as follows,
	1..5	 --	MAGICWORD
	6..10	 --	MAGICNUMBER
	11..12	 --	WKDAY
	13..14	 --	WKEND
	15..16	 --	HOLID
	17	 --	HBEGIN
	18	 --	HEND
	19..38	 --	HNAME
	39	 -- 	SHORT
	40	 --	Count of saved games.
	41	 --	LATENCY
	42..61	 --	Name of saved game 1.
	62..81	 --	Name of saved game 2.
	82..101	 --	Name of saved game 3.
	102..354 --	Saved game 1.
	355..607 -- 	Saved game 2.
	608..860 --  	Saved game 3.
	861..1560   --	Message of the day.      }

VAR MAGICNUMBER:INTEGER;
    MAGICWORD:ARRAY[1..5] OF CHAR;
BEGIN
    REWRITE(ADVWIZ,'ADVWIZ.DTA');
    { Set up initial magic word as 'dwarf' and initial magic number to 11111.  }
    MAGICWORD:='DWARF';
    MAGICNUMBER:=1;
    FOR I:=1 TO 5 DO BEGIN
	ADVWIZ^:=ORD(MAGICWORD[I])-ORD('A');
	PUT(ADVWIZ)
    END;
    FOR I:=1 TO 5  DO BEGIN
	ADVWIZ^:=MAGICNUMBER;
	PUT(ADVWIZ);
    END;
    {  Set up cave to be open at all hours with no holidays. }
    FOR I:=11 TO 18 DO BEGIN
	ADVWIZ^:=0;
	PUT(ADVWIZ)
    END;
    FOR I:=19 TO 38 DO BEGIN
	ADVWIZ^:=ORD(' ')-ORD('A');
	PUT(ADVWIZ)
    END;
    {  Set a short game to 30 turns.  }
    ADVWIZ^:=30;
    PUT(ADVWIZ);
    {  Set count of saved games to zero. }
    ADVWIZ^:=0;
    PUT(ADVWIZ);
    {  Set latency to 45 minutes.  }

    ADVWIZ^:=45;
    PUT(ADVWIZ);
    {  Fill the saved game names with blanks.  }
    FOR I:=42 TO 101 DO BEGIN
	ADVWIZ^:=ORD(' ')-ORD('A');
	PUT(ADVWIZ)
    END;
    {  Fill saved game area with zeroes.  }
    FOR I:=102 TO 860 DO BEGIN
	ADVWIZ^:=0;
	PUT(ADVWIZ)
    END;
    {  Blank out the message of the day.  }
    FOR I:=861 TO 1560 DO BEGIN
	ADVWIZ^:=ORD(' ')-ORD('A');
	PUT(ADVWIZ)
    END;
    CLOSE(ADVWIZ)
END.
