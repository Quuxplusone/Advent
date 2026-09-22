{********>>>>>>>><<<<<<<<********

>>	PROGRAM:  100FLS
		  30-OCT-80

		When built into a task, this program is invoked  by  the command
	RUN  100FLS.    It  reads  in  the  text and text pointers from the file 
	ADVENTURE.100  and  creates new copies of  ADVTXT.100 and ADVDAT.100 for 
	use by ADVENTURE.TSK when a VT-100 terminal is being used.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE HUNFLS 
	.IDENT /V0/  }

{  *****************  INITIALIZATION PROCEDURES  ****************  }
{ For a detailed description of the database refer to the comments in module
ADVINI.  The data files transfer procedures are commented in ADVFLS.  }

VAR TXTFILE:TEXT;
    ADVDAT:FILE OF INTEGER;

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
    REWRITE(ADVTXT,'ADVTXT.100/SEEK');
    REC:=1;
    SEEK(ADVTXT,REC);
    FOR I:=1 TO LOCSIZ DO BEGIN
	LTEXT[I]:=0;
	STEXT[I]:=0
    END;
    FOR I:=1 TO MAXTRS DO PTEXT[I]:=0;
    FOR I:=1 TO 300 DO RTEXT[I]:=0;
    CTEXT:=0;
    FOR I:=1 TO 100 DO MTEXT[I]:=0;
    WRITELN('Now loading section  1');
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

PROCEDURE SAVPTR;
VAR REC:INTEGER;

PROCEDURE PUTPTR(X:INTEGER);
BEGIN
    ADVDAT^:=X;
    PUT(ADVDAT);
    REC:=REC+1;
    SEEK(ADVDAT,REC)
END;

BEGIN
    REWRITE(ADVDAT,'ADVDAT.100/SEEK');
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

BEGIN	{ VT-100 FILES INITIALIZATION PROGRAM }
    WRITELN('VT100 Files Creation for Adventure');
    WRITELN('Be patient, it takes a while to do this....');
    RESET(TXTFILE,'ADVENTURE.100');
    GETTEXT;

    RESET(ADVDAT,'ADVDAT.DTA/SEEK');
    I:=2*LOCSIZ+MAXTRS+402;
    SEEK(ADVDAT,I);
    FOR I:=ORD('A') TO ORD('Z') DO BEGIN
	KTAB[I]:=ADVDAT^;
	GET(ADVDAT)
    END;
    CLOSE(ADVDAT);
    SAVPTR;
    WRITELN('.....done!');
    CLOSE(TXTFILE)
END.

