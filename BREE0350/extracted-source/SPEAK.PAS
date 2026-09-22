{********>>>>>>>><<<<<<<<********

>>	MODULE:	SPEAK
		29-OCT-80

		This module contains the procedures SPEAK, RSPEAK, MSPEAK,
	and CSPEAK.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE SPEAK
	.IDENT /V0/   }

{ *************  I/O  SUBROUTINES   ********************}

{  This procedure outputs a message from ADVTXT starting at record
number REC and continuing as long as the message number at the start of each
line remains the same.  }
PROCEDURE SPEAK(REC: INTEGER);
VAR	OLDLC:INTEGER;
BEGIN
    SEEK(ADVTXT,REC);
    WITH ADVTXT^ DO BEGIN
	OLDLC:=LOC;
	{Supress printout if message is '>$<' }
	IF (TXT[1]<>'>')OR(TXT[2]<>'$')OR(TXT[3]<>'<') THEN BEGIN
	    WRITELN;
	    WHILE LOC=OLDLC DO BEGIN
		WRITELN(TXT);
		GET(ADVTXT)
	    END
	END
    END
END;

{  This procedure outputs message number MSG from the random messages group.  }
PROCEDURE RSPEAK(MSG:INTEGER);
BEGIN
	IF MSG<>0 THEN SPEAK(RTEXT[MSG])
END;

{  This procedure outputs message number MSG from the magic messages group.   }
PROCEDURE MSPEAK(MSG:INTEGER);
BEGIN
	IF MSG<>0 THEN SPEAK(MTEXT[MSG])
END;

{  This procedure is used at the end of the game to print out the player
classification according to his score.  }
PROCEDURE CSPEAK(SCORE,MAXSCOR:INTEGER);
VAR REC,MSG:INTEGER;
    WRD1:WORD;
BEGIN
    {  First point to the player classification messages in ADVTXT.  }
    REC:=CTEXT;
    SEEK(ADVTXT,CTEXT);
    WITH ADVTXT^ DO BEGIN
	{  Now search for the applicable message.  See database description in 
	ADVINI for explanation of messages.  }
	WHILE SCORE>=LOC DO BEGIN
	    GET(ADVTXT);
	    REC:=REC+1
	END;
	{  Save the breakpoint score for the next higher class. }
	MSG:=LOC
    END;
    {  Tell him his classification.  }

    SPEAK(REC);
    {  Find out how many points he needs to reach the next level.  }
    MSG:=MSG-SCORE;
    IF MSG=1 THEN WRD1:='.    ' ELSE WRD1:='s.   ';
    WRITELN;
    {  If he got a perfect score, congratulations!!  }
    IF SCORE=MAXSCOR THEN RSPEAK(203)
    {  Otherwise tell him how many points he needs to hit the next level.  }
    ELSE BEGIN
	WRITE('To achieve the next higher rating you need');
	WRITE(MSG:4);
	WRITE(' more point');
	WRITELN(WRD1)
    END
END;

