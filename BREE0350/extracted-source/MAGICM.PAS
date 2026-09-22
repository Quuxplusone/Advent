{********>>>>>>>><<<<<<<<********

>>	MODULE:	MAGICM
		12-NOV-80

		This module contains the "wizardry" routines used when someone
	invokes "magic mode": MAINTINENCE, NEWHRS, and MOTD.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE	MAGICM
	.IDENT /V0/    }

{  *************** DATA STRUCTURE ROUTINES  ******************  }

FUNCTION BIT(N:INTEGER):INTEGER;EXTERNAL;

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE MSPEAK(MSG:INTEGER);EXTERNAL;

FUNCTION ASKM(I,J,K: INTEGER): BOOLEAN;EXTERNAL;

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

{ "Message of the day" -- If X is false, print current message. If X is true
then get new message from Wizard and save.  }
PROCEDURE MOTD(X:BOOLEAN);
VAR MSGLINE,BLNKLINE:LINE;
    REC,I,J:INTEGER;
BEGIN
    FOR I:=1 TO 72 DO BLNKLINE[I]:=' ';
    MSGLINE:=BLNKLINE;
    RESET(ADVWIZ,'ADVWIZ.DTA/RW/SEEK');
    REC:=861;
    SEEK(ADVWIZ,REC);
    IF X THEN BEGIN
	MSPEAK(23);
	MSGLINE[1]:='A';
	FOR I:=1 TO 10 DO BEGIN
	    IF MSGLINE<>BLNKLINE THEN BEGIN
		READLN(MSGLINE);
		WHILE (MSGLINE[71]<>' ')OR(MSGLINE[72]<>' ') DO BEGIN
		    MSPEAK(24);
		    READLN(MSGLINE)

		END
	    END;
	    FOR J:=1 TO 70 DO BEGIN
		ADVWIZ^:=ORD(MSGLINE[J])-ORD('A');
		PUT(ADVWIZ);
		REC:=REC+1;
		IF (J<>70)OR(I<>10) THEN SEEK(ADVWIZ,REC)
	    END
	END;
	IF MSGLINE<>BLNKLINE THEN BEGIN
	    MSPEAK(25);
	    REPEAT READLN(MSGLINE) UNTIL MSGLINE=BLNKLINE
	END
    END
    ELSE BEGIN
	WRITELN;
	FOR I:=1 TO 10 DO BEGIN
	    FOR J:=1 TO 70 DO BEGIN
		MSGLINE[J]:=CHR(ADVWIZ^+ORD('A'));
		IF (I<>10)OR(J<>70) THEN GET(ADVWIZ)
	    END;
	    IF MSGLINE=BLNKLINE THEN EXIT ELSE WRITELN(MSGLINE)
	END
    END;
    CLOSE(ADVWIZ)
END;

PROCEDURE DATIME(VAR D:UNSIGNED;VAR T:INTEGER);EXTERNAL;

FUNCTION WIZARD:BOOLEAN;EXTERNAL;

PROCEDURE HOURS;EXTERNAL;

{  This procedure is used to set up new hours for the cave.  Specified as
inverse -- i.e., when is it closed due to prime time?  See HOURS for description
of variables.  }
PROCEDURE NEWHRS;
TYPE	STRNG8=ARRAY[1..8] OF CHAR;

VAR REC:INTEGER;

{  Local procedure to input prime time specs and set up a word of internal 
format and write it to the magic parameters file.  }
PROCEDURE NEWHRX(TITLE:STRNG8);
VAR FROM,TILL,I,J,K:INTEGER;
    PRMTIM:ARRAY[1..2] OF INTEGER;
BEGIN
    FROM:=0;
    TILL:=0;
    PRMTIM[1]:=0;
    PRMTIM[2]:=0;
    WRITELN;
    WRITE('Prime time on ',TITLE,',');
    WHILE (FROM>=0)AND(FROM<=23)AND(TILL>=FROM)AND(TILL<=24) DO BEGIN
	FROM:=-1;
	TILL:=-1;
	WRITE(' from: ');

	READLN(FROM);
	IF (FROM>=0)AND(FROM<=23) THEN BEGIN
	    WRITE(' till: ');
	    READLN(TILL);
	    TILL:=TILL-1;
	    IF (TILL>=FROM)AND(TILL<=23) THEN 
	    FOR I:=FROM TO TILL DO IF I<=14 THEN PRMTIM[1]:=PRMTIM[1]+BIT(I)
	    ELSE PRMTIM[2]:=PRMTIM[2]+BIT(I-15)
	END
    END;
    ADVWIZ^:=PRMTIM[1];
    PUT(ADVWIZ);
    REC:=REC+1;
    SEEK(ADVWIZ,REC);
    ADVWIZ^:=PRMTIM[2];
    PUT(ADVWIZ);
    REC:=REC+1;
    SEEK(ADVWIZ,REC)
END;

BEGIN { Newhrs }
    RESET(ADVWIZ,'ADVWIZ.DTA/RW/SEEK');
    REC:=11;
    SEEK(ADVWIZ,REC);
    MSPEAK(21);
    NEWHRX('Weekdays');
    NEWHRX('Weekends');
    NEWHRX('Holidays');
    CLOSE(ADVWIZ);
    MSPEAK(22);
    HOURS
END;

{  This procedure is called when someone says the magic word to invoke
maintenence mode.  If so, let him tweak all sorts of things and update the magic 
parameters file.  }
PROCEDURE MAINTINENCE;
VAR REC,LATENCY,T,I:INTEGER;
    MAGICNUMBER:ARRAY[1..5] OF INTEGER;
    MAGICWORD:WORD;
    D,HBEGIN,HEND:UNSIGNED;
    HNAME:ARRAY[1..20] OF CHAR;
BEGIN
    IF WIZARD THEN BEGIN
	IF ASKM(10,0,0) THEN HOURS;
	IF ASKM(11,0,0) THEN NEWHRS;
	RESET(ADVWIZ,'ADVWIZ.DTA/RW/SEEK');
	IF ASKM(26,0,0) THEN BEGIN
	    MSPEAK(27);
	    READLN(HBEGIN);
	    MSPEAK(28);
	    READLN(HEND);
	    DATIME(D,T);
	    HBEGIN:=HBEGIN+D;
	    HEND:=HBEGIN+HEND-1;
	    MSPEAK(29);
	    READLN(HNAME);

	    REC:=17;
	    SEEK(ADVWIZ,REC);
	    ADVWIZ^:=HBEGIN;
	    PUT(ADVWIZ);
	    REC:=REC+1;
	    SEEK(ADVWIZ,REC);
	    ADVWIZ^:=HEND;
	    PUT(ADVWIZ);
	    REC:=REC+1;
	    SEEK(ADVWIZ,REC);
	    FOR I:=1 TO 20 DO BEGIN
		ADVWIZ^:=ORD(HNAME[I])-ORD('A');
		PUT(ADVWIZ);
		REC:=REC+1;
		SEEK(ADVWIZ,REC)
	    END
	END;
	SEEK(ADVWIZ,39);
	SHORT:=ADVWIZ^;
	WRITELN;
	WRITE('Length of a short game (zero to leave at ',SHORT:3,' turns): ');
	READLN(I);
	IF I>0 THEN BEGIN
	    ADVWIZ^:=I;
	    PUT(ADVWIZ)
	END;
	MSPEAK(12);
	READLN(MAGICWORD);
	IF MAGICWORD<>'     ' THEN BEGIN
	    REC:=1;
	    SEEK(ADVWIZ,REC);
	    FOR I:=1 TO 5 DO BEGIN
		ADVWIZ^:=ORD(MAGICWORD[I])-ORD('A');
		PUT(ADVWIZ);
		REC:=REC+1;
		SEEK(ADVWIZ,REC)
	    END
	END;
	MSPEAK(13);
	READLN(MAGICNUMBER[5]);
	IF MAGICNUMBER[5]<>0 THEN BEGIN
	    FOR I:=5 DOWNTO 2 DO BEGIN
		MAGICNUMBER[I-1]:=MAGICNUMBER[I] DIV 10;
		MAGICNUMBER[I]:=MAGICNUMBER[I] MOD 10
	    END;
	    REC:=6;
	    SEEK(ADVWIZ,REC);
	    FOR I:=1 TO 5 DO BEGIN
		ADVWIZ^:=MAGICNUMBER[I];
		PUT(ADVWIZ);
		REC:=REC+1;
		SEEK(ADVWIZ,REC)
	    END
	END;
	SEEK(ADVWIZ,41);
	LATENCY:=ADVWIZ^;
	WRITELN;

	WRITELN('Latency for restart (zero to leave at',LATENCY:4,' minutes):');
	READLN(I);
	IF I>0 THEN BEGIN
	    IF I<45 THEN MSPEAK(30) ELSE BEGIN
		ADVWIZ^:=I;
		PUT(ADVWIZ)
	    END
	END;
	CLOSE(ADVWIZ);
	IF ASKM(14,0,0) THEN MOTD(TRUE);
	MSPEAK(15);
	MSPEAK(37)
    END;
    MOVED:=TRUE
END;

