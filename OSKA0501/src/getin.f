       SUBROUTINE GETIN(WORD1,WORD1X,WORD2,WORD2X)

C  GET A COMMAND FROM THE ADVENTURER.  SNARF OUT THE FIRST WORD, PAD IT WITH
C  BLANKS, AND RETURN IT IN WORD1.  CHARS 6 THRU 10 ARE RETURNED IN WORD1X, IN
C  CASE WE NEED TO PRINT OUT THE WHOLE WORD IN AN ERROR MESSAGE.  ANY NUMBER OF
C  BLANKS MAY FOLLOW THE WORD.  IF A SECOND WORD APPEARS, IT IS RETURNED IN
C  WORD2 (CHARS 6 THRU 10 IN WORD2X), ELSE WORD2 IS SET TO ZERO.

       IMPLICIT INTEGER(A-Z)
       LOGICAL BLKLIN
       LOGICAL PROMPT
       COMMON /BLKCOM/ BLKLIN
       COMMON /MYRCOM/ PROMPT
       CHARACTER A, TEMP
       DIMENSION A(70),TEMP(70)
C      REAL*8 WRD(2),WORD1,WORD1X,WORD2,WORD2X
       CHARACTER*8 WRD(2), WORD1, WORD1X, WORD2, WORD2X

       IF(BLKLIN)PRINT 1
1       FORMAT()
C      READ(1,3)A
 2      FORMAT('?','>',$) !question prompt for yes/no stuff
 22     FORMAT('QUESTION:')
        IF(PROMPT) THEN
           WRITE(*,2)
        ELSE
           WRITE(*,22)
        END IF
        CALL FLUSH( ) ! for tcl script
        READ( UNIT=*, FMT=3 ) A
C        READ *, A
3       FORMAT(70A1)
      DO 1001 I=1,70
C **************************
C convert lowercase to upper
C **************************
C      IF(A(I).GE.'a'.AND.A(I).LE.'z')A(I)=AND(A(I),:157777)
1001   CONTINUE
      WORD1='     '
      WORD1X='        '
      WORD2=CHAR(0)
10    J=1
      CALL A1TOA5(A,J,WRD,TERM)
C      IF(TERM.EQ.'; '.OR.TERM.EQ.0)RETURN
      IF(TERM.EQ.ICHAR(';').OR.TERM.EQ.0)RETURN !WARNING I DONT THINK THIS
      WORD1=WRD(1)                              !IS CORRECT
      WORD1X=WRD(2)
      CALL A1TOA5(A,J,WRD,TERM)
C      IF(TERM.EQ.'; '.OR.TERM.EQ.0)RETURN
      IF(TERM.EQ.ICHAR(';').OR.TERM.EQ.0)RETURN !NOR THIS
      WORD2=WRD(1)
      WORD2X=WRD(2)
      RETURN
      END
