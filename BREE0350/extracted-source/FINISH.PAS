{********>>>>>>>><<<<<<<<********

>>	MODULE:	FINISH
		17-SEP-80

		This module contains the procedures FINISH and WAKEDWARVES.

********>>>>>>>><<<<<<<<********}


{$C	.TITLE FINISH
	.IDENT /V0/   }

{ *************  I/O  SUBROUTINES   ********************}

PROCEDURE RSPEAK(MSG:INTEGER);EXTERNAL;

PROCEDURE CSPEAK(SCORE,MAXSCOR:INTEGER);EXTERNAL;

{******************* SCORING AND GAME END ***************************}

PROCEDURE CALSCORE;EXTERNAL;

PROCEDURE WRTSCORE;EXTERNAL;

{  This procedure implements the end of the game.  }
PROCEDURE FINISH;
BEGIN
	{  First caluculate the score.  }
	CALSCORE;
	{  Now write it out.  }
	WRTSCORE;
	{  Print out his player classification.  }
	CSPEAK(SCORE,MAXSCOR);
	WRITELN;
	WRITELN;
	{  Set program control flags to cause exit from the main program loop. }
	MOVED:=TRUE;
	DONE:=TRUE
END;

{ ************ WAKE DWARVES IN REPOSITORY ************* }

{  Oh dear,  he's disturbed the dwarves....  }
PROCEDURE WAKEDWARVES;
BEGIN
	RSPEAK(136);
	FINISH
END;

