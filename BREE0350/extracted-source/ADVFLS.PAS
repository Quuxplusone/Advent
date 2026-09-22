{********>>>>>>>><<<<<<<<********

>>	PROGRAM:   ADVFLS
		   29-OCT-80

		When built into a task, this program is invoked by the command 
	RUN ADVFLS.   It reads in the  database  and  database  pointers  from 
	ADVENTURE.DAT and creates  files  ADVTXT.DTA,  KATAB.DTA,  ADVDAT.DTA,
	and ADVENT.DTA for the normal (non-VT-100) use of ADVENTURE.TSK.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE ADVFLS 
	.IDENT /V0/  }

{  For a complete description of the database refer to the comments in
module ADVINI.  }

VAR TXTFILE:TEXT;
    ADVENT,ADVDAT:FILE OF INTEGER;

{ This procedure transfers text records from ADVENTURE.DAT to ADVTXT.DTA and
keeps track of the record numbers.  }
PROCEDURE GETTEXT;
VAR REC,I,J,LOC,OLDLC:INTEGER;
    LINES:ARRAY[1..72] OF CHAR;

PROCEDURE PUTTXT;
BEGIN
    OLDLC:=LOC;
    WHILE LOC=OLDLC DO BEGIN
	READLN(TXTFILE,LINES);
	ADVTXT^.LOC:=LOC;
	ADVTXT^.TXT:=LINES;
	PUT(ADVTXT);
	REC:=REC+1;
	SEEK(ADVTXT,REC);
	READ(TXTFILE,LOC)
    END
END;

BEGIN { gettxt }
    REWRITE(ADVTXT,'ADVTXT.DTA/SEEK');
    REC:=1;
    FOR I:=1 TO LOCSIZ DO BEGIN
	LTEXT[I]:=0;
	STEXT[I]:=0
    END;
    FOR I:=1 TO MAXTRS DO PTEXT[I]:=0;
    FOR I:=1 TO 300 DO RTEXT[I]:=0;
    CTEXT:=0;
    FOR I:=1 TO 100 DO MTEXT[I]:=0;
    WRITELN('Now loading section  1');
    SEEK(ADVTXT,REC);
    READ(TXTFILE,LOC);
    WHILE LOC<>-1 DO BEGIN
	LTEXT[LOC]:=REC;
	PUTTXT
    END;
    READLN(TXTFILE,LINES);
    WRITELN('Now loading section  2');
    READ(TXTFILE,LOC);
    WHILE LOC<>-1 DO BEGIN
	STEXT[LOC]:=REC;
	PUTTXT
    END;
    READLN(TXTFILE,LINES);
    WRITELN('Now loading section  3');
    READ(TXTFILE,LOC);

    WHILE LOC<>-1 DO BEGIN
	PTEXT[LOC]:=REC;
	PUTTXT;
	WHILE (LOC=0)OR(LOC>MAXTRS) DO PUTTXT
    END;
    READLN(TXTFILE,LINES);
    WRITELN('Now loading section  4');
    READ(TXTFILE,LOC);
    WHILE LOC<>-1 DO BEGIN
	RTEXT[LOC]:=REC;
	PUTTXT
    END;
    READLN(TXTFILE,LINES);
    WRITELN('Now loading section  5');
    READ(TXTFILE,LOC);
    CTEXT:=REC;
    WHILE LOC<>-1 DO PUTTXT;
    READLN(TXTFILE,LINES);
    WRITELN('Now loading section  6');
    READ(TXTFILE,LOC);
    WHILE LOC<>-1 DO BEGIN
	MTEXT[LOC]:=REC;
	PUTTXT
    END;
    READLN(TXTFILE,LINES);
    CLOSE(ADVTXT)
END;

{ This procedure reads the vocabulary section of ADVENTURE.DAT and creates
the tree structured version in KATAB.DTA }
PROCEDURE READVOCAB;
VAR LINES:WORD;
    LOC,I,J,K,L,M,N:INTEGER;
    LTR:CHAR;

PROCEDURE GETLTR;
BEGIN
    I:=I+1;
    IF I<=5 THEN LTR:=LINES[I] ELSE LTR:=' ';
    IF LTR='2' THEN LTR:='T' ELSE IF (LTR='?')OR(LTR='"') THEN LTR:='Q';
    K:=ORD(LTR)
END;

PROCEDURE NEWREC;
BEGIN
    SEEK(KATAB,L);
    FOR J:=65 TO 90 DO KATAB^.NXT[J]:=0;
    FOR J:=1 TO 2 DO KATAB^.VAL[J]:=-1;
    L:=L+1;
    GETLTR
END;

BEGIN { readvocab }
    WRITELN('Now loading section  7');
    FOR I:=65 TO 90 DO KTAB[I]:=0;
    REWRITE(KATAB,'KATAB.DTA/SEEK');
    L:=1;

    READ(TXTFILE,LOC);
    WHILE LOC<>-1 DO BEGIN
	READLN(TXTFILE,LINES);
	I:=0;
	GETLTR;
	M:=KTAB[K];
	IF M<>0 THEN BEGIN
	    SEEK(KATAB,M);
	    GETLTR;
	    IF LTR<>' ' THEN WHILE KATAB^.NXT[K]<>0 DO BEGIN
		N:=KATAB^.NXT[K];
		SEEK(KATAB,N);
		GETLTR;
		IF LTR=' ' THEN EXIT
	    END;
	END ELSE BEGIN
	    KTAB[K]:=L;
	    NEWREC
	END;
	WHILE LTR<>' ' DO BEGIN
	    KATAB^.NXT[K]:=L;
	    PUT(KATAB);
	    NEWREC
	END;
	IF KATAB^.VAL[1]=-1 THEN KATAB^.VAL[1]:=LOC
	ELSE KATAB^.VAL[2]:=LOC;
	PUT(KATAB);
	READ(TXTFILE,LOC)
    END;
    CLOSE(KATAB)
END;

{ This procedure saves all the record pointer arrays created as the text and 
vocabulary were transferred.  }
PROCEDURE SAVPTR;
VAR REC,I:INTEGER;

PROCEDURE PUTPTR(X:INTEGER);
BEGIN
    ADVDAT^:=X;
    PUT(ADVDAT);
    REC:=REC+1;
    SEEK(ADVDAT,REC)
END;

BEGIN
    REWRITE(ADVDAT,'ADVDAT.DTA/SEEK');
    REC:=1;
    SEEK(ADVDAT,REC);
    FOR I:=1 TO LOCSIZ DO PUTPTR(LTEXT[I]);
    FOR I:=1 TO LOCSIZ DO PUTPTR(STEXT[I]);
    FOR I:=1 TO MAXTRS DO PUTPTR(PTEXT[I]);
    FOR I:=1 TO 300 DO PUTPTR(RTEXT[I]);
    FOR I:=1 TO 100 DO PUTPTR(MTEXT[I]);
    PUTPTR(CTEXT);
    FOR I:=ORD('A') TO ORD('Z') DO PUTPTR(KTAB[I]);
    CLOSE(ADVDAT)

END;

PROCEDURE SAVADV;
VAR I,SEC,REC:INTEGER;
BEGIN
    REWRITE(ADVENT,'ADVENT.DTA/SEEK');
    REC:=1;
    WRITELN('Now loading section  8');
    READ(TXTFILE,ADVENT^);
    WHILE ADVENT^<>-1 DO BEGIN
	PUT(ADVENT);
	REC:=REC+1;
	SEEK(ADVENT,REC);
	READ(TXTFILE,ADVENT^)
    END;
    WRITELN('Now loading section  9');
    PUT(ADVENT);
    REC:=REC+1;
    SEEK(ADVENT,REC);
    READ(TXTFILE,ADVENT^);
    WHILE ADVENT^<>-1 DO FOR I:=1 TO 3 DO BEGIN
	PUT(ADVENT);
	REC:=REC+1;
	SEEK(ADVENT,REC);
	READ(TXTFILE,ADVENT^)
    END;
    PUT(ADVENT);
    REC:=REC+1;
    SEEK(ADVENT,REC);
    FOR SEC:=10 TO 12 DO BEGIN
	WRITELN('Now loading section',SEC:3);
	READ(TXTFILE,ADVENT^);
	WHILE ADVENT^<>-1 DO BEGIN
	    PUT(ADVENT);
	    REC:=REC+1;
	    SEEK(ADVENT,REC);
	    READ(TXTFILE,ADVENT^)
	END;
	PUT(ADVENT);
	REC:=REC+1;
	IF SEC<>12 THEN SEEK(ADVENT,REC)
    END;
    CLOSE(ADVENT)
END;

BEGIN	{ FILES INITIALIZATION PROGRAM }
    WRITELN('Files Creation for Adventure');
    WRITELN('Be patient, it takes a while to do this.....');
    RESET(TXTFILE,'ADVENTURE.DAT');
    GETTEXT;
    READVOCAB;
    SAVPTR;
    SAVADV;
    CLOSE(TXTFILE);
    WRITELN('....done!')
END.
