{********>>>>>>>><<<<<<<<********

>>	MODULE:	DUMPWD
		17-OCT-80

		This module contains the procedures DUMPWD1, SMALL, DOWHAT
	NOTHERE,and VERBHUH.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE DUMPWD
	.IDENT /V0/   }

{ ************** OBJECT,VERB DEFAULT PROCEDURES *********** }

{  This procedure is used to throw away the first player input word and get the
second for analysis.  }  
PROCEDURE DUMPWD1;
BEGIN
	WRD1:=WRD2;
	WRD1X:=WRD2X;
	WRD2:='     '
END;

{  This procedure is used by the default procedures below to echo player's
input in small letters.  Input in other characters is left as is.    }
PROCEDURE SMALL(WRD:WORD);
VAR I,J:INTEGER;
BEGIN
    J:=ORD('A')-ORD('a');
    FOR I:=1 TO 5 DO IF WRD[I]<>' ' THEN 	
	IF (WRD[I]>='A')AND(WRD[I]<='Z') THEN
	WRITE(CHR(ORD(WRD[I])-J))
	ELSE WRITE(WRD[I]);
END;

{  This procedure responds to verbs requiring a definite object and none has
been given or can be deduced.  I changed this from the original program.  It
used to echo the verb with "huh?", it now echoes the verb with "what?" which
seems to read easier.  For example,
	Player is carrying six things and says "DROP".
	We say "Drop what?"  (makes more sense than "Drop huh?")  
Purists, you may simply change it back if you like.......  }

PROCEDURE VERBHUH;
VAR	J,I:INTEGER;
BEGIN
	{ Echo the first letter capitalized and the rest small.  }
	J:=ORD('A')-ORD('a');
	WRITELN;
	IF (WRD1[1]>='a')AND(WRD1[1]<='z') THEN
	WRITE(CHR(ORD(WRD1[1])+J))
	ELSE WRITE(WRD1[1]);
	FOR I:=2 TO 5 DO
	IF WRD1[I]<>' ' THEN 
		IF (WRD1[I]>='A')AND(WRD1[I]<='Z') THEN
		WRITE(CHR(ORD(WRD1[I])-J))
		ELSE WRITE(WRD1[I]);
	SMALL(WRD1X);
	WRITELN(' what?');
	OBJ:=0;
	GOTBOTH:=FALSE
END;

{  Reverse of VERBHUH, applies when object given with no verb:
	"What do you want me to do with the snake?"    }
PROCEDURE DOWHAT;

VAR I,J:INTEGER;
BEGIN
	WRITELN;
	WRITE('What do you want me to do with the ');
	SMALL(WRD1);
	SMALL(WRD1X);
	WRITELN('?')
END;

{  An object must be here in order to do something with it.   This procedure 
echoes the player input if it is defined as an object but it isn't here to 
muck with:    "I see no pyramid here."  }
PROCEDURE NOTHERE;
BEGIN
	WRITELN;
	WRITE('I see no ');
	SMALL(WRD1);
	SMALL(WRD1X);
	WRITELN(' here.')
END;

