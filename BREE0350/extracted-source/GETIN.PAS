{********>>>>>>>><<<<<<<<********

>>	MODULE:	ADV1
		29-SEP-80

		This module contains the procedures GETIN and CAPS.
	
********>>>>>>>><<<<<<<<********}


{$C	.TITLE GETIN
	.IDENT /V0/   }

{  This procedure converts smalls to caps in a word.  }
PROCEDURE CAPS(VAR WRD:WORD);
VAR I:INTEGER;
BEGIN
    FOR I:=1 TO 5 DO IF (WRD[I]>='a')AND(WRD[I]<='z') 
    THEN WRD[I]:=CHR(ORD(WRD[I])-ORD('a')+ORD('A'))
END;

{  This procedure gets the player input words.   The first five characters of
each word are packed into WORD1 and WORD2. If the word(s) contain more than five
characters, the next five characters are packed into WORD1X and WORD2X.  The
first five characters are used to return word definitions (see VOCAB).  The 
second five are used when echoing player input in order not to truncate words
having more than five letters (see VERBHUH and NOTHERE), words of more than ten
letters will be truncated to the first ten.   }
PROCEDURE GETIN(VAR WORD1,WORD1X,WORD2,WORD2X: WORD);
VAR	X: BOOLEAN;
	I,J:INTEGER;
	LINES:ARRAY[1..72] OF CHAR;
LABEL 1;
{  Local procedure to transfer up to five letters from input line buffer
LINES to a variable word, WORDN.  X indicates end of buffer reached. }
PROCEDURE GETWORD(VAR X: BOOLEAN; VAR WORDN: WORD);
BEGIN
	IF I<=72 THEN
		FOR J:=1 TO 5 DO
		IF LINES[I]<>' ' THEN 
		BEGIN
			WORDN[J]:=LINES[I];
			I:=I+1;
			IF I>72 THEN 
			BEGIN
				X:=TRUE;
				EXIT
			END
		END
		ELSE WORDN[J]:=' '
	ELSE WORDN:='     ';
END;

BEGIN {Getin}
	REPEAT {Until we get some input, WORD1<>'     ' }
	BEGIN
		{Skip a line from last output}
		WRITELN;
		{Write a prompt for input}
		WRITE('->');
		{Read raw player input into line buffer}
		READLN(LINES);
		X:=FALSE;
		I:=1;
		{Skip over any leading blanks}
		WHILE (LINES[I]=' ') AND (I<72) DO I:=I+1;
		{Get the first five non-blank characters}

		GETWORD(X,WORD1);
		{Exit if end of line buffer reached}
		IF X THEN GOTO 1;
		{Get up to five more non-blank characters of first word}
		IF (WORD1[5]<>' ') AND (LINES[I]<>' ') THEN
		GETWORD(X,WORD1X) ELSE WORD1X:='     ';  
		{Exit if end of line buffer reached}
		IF X THEN GOTO 1;
		{Skip over remaining characters of first word if more than 10}
		WHILE (LINES[I]<>' ') AND (I<72) DO I:=I+1;
		{Skip over blanks in between words}
		WHILE (LINES[I]=' ') AND (I<72) DO I:=I+1;
		{Get second word and extension in the same manner as first}
		GETWORD(X,WORD2);
		IF X THEN GOTO 1;
		IF (WORD2[5]<>' ') AND (LINES[I]<>' ') THEN
		GETWORD(X,WORD2X) ELSE WORD2X:='     ';
		1: { Jump here if line runs out.  }
	END
	UNTIL WORD1<>'     ';
	CAPS(WORD1);
	CAPS(WORD1X);
	CAPS(WORD2);
	CAPS(WORD2X)
END;

