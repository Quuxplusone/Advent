{********>>>>>>>><<<<<<<<********

>>	MODULE:	CALSCO
		20-OCT-80

		This module contains the procedures CALSCORE and WRTSCORE.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE CALSCO
	.IDENT /V0/    }

{******************* SCORING AND GAME END ***************************}

{  This procedure is used by the "score" command and at the end of the game
to compute his score and the maximum score possible.  }

PROCEDURE CALSCORE;
VAR I,K:INTEGER;
BEGIN
    SCORE:=0;
    MAXSCOR:=0;

{  The present scoring algorithm is as follows:
	Objective:		Points:		Present Total Possible:
Getting well into the cave	  25			25
Each treasure < chest		  12			60
Treasure chest itself		  14			14
Each treasure > chest		  16		       144
Surviving		     (max-num)*10		30
Not quitting			   4			 4
Reaching "closing"		  25			25
"Closed": quit/killed		  10
	  klutzed		  25
	  wrong way		  30
	  success		  45			45
Came to Witt's End		   1			 1
Round out the total		   2			 2
						Total: 350
(Points can also be deducted for using hints.)

    First tally up the treasures.  Must be in building and not broken.  Give
    the poor guy 2 points just for finding each treasure.  }
    
    FOR I:=50 TO MAXTRS DO BEGIN
	IF I=CHEST THEN K:=14 ELSE IF I>CHEST THEN K:=16 ELSE K:=12;
	IF (PLACE[I]=3) THEN SCORE:=SCORE+K
	ELSE IF PROP[I]>=0 THEN SCORE:=SCORE+2;
	MAXSCOR:=MAXSCOR+K
    END;
    {  Now look at how he finished and how far he got.  MAXDIE and NUMDIE tell 
    us how well he survived.  GAVEUP says whether he exited via "quit".  DFLAG 
    will tell us if he ever got suitably deep into the cave.  CLOSING indicates 
    whether he reached the endgame.  And if he got as far as "Cave Closed" 
    (indicated by CLOSED), then bonus is zero for mundane exits or 133,134,135 
    if he blew it (so to speak).  }
    SCORE:=SCORE+(MAXDIE-NUMDIE)*10;
    MAXSCOR:=MAXSCOR+MAXDIE*10;
    IF NOT (SCORING OR GAVEUP) THEN SCORE:=SCORE+4;
    MAXSCOR:=MAXSCOR+4;
    IF DFLAG<>0 THEN SCORE:=SCORE+25;
    MAXSCOR:=MAXSCOR+25;
    IF CLOSING THEN SCORE:=SCORE+25;
    MAXSCOR:=MAXSCOR+25;
    IF CLOSED THEN CASE BONUS OF
	0:   SCORE:=SCORE+10;

	135: SCORE:=SCORE+25;
	134: SCORE:=SCORE+30;
	133: SCORE:=SCORE+45
    END;
    MAXSCOR:=MAXSCOR+45;
    { Did he leave magazines in Witt's End as he should?  }
    IF PLACE[MAGAZINE]=108 THEN SCORE:=SCORE+1;
    MAXSCOR:=MAXSCOR+1;
    {  Round it off.  }
    SCORE:=SCORE+2;
    MAXSCOR:=MAXSCOR+2;
    { Deduct points for hints. Hints < 4 are special; see database description.}
    FOR I:=1 TO HNTMAX DO IF HINTED[I] THEN SCORE:=SCORE-HINTS[I,2]
END;

{  This procedure writes out the player score at the end of the game.  }
PROCEDURE WRTSCORE;
BEGIN
	WRITELN;WRITELN;
	WRITE('You scored');
	WRITE(SCORE:4);
	WRITE(' out of a possible');
	WRITE(MAXSCOR:4);
	WRITE(' using');
	WRITE(TURNS:4);
	WRITELN(' turns.')
END;

