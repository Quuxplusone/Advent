C***   A1TOA5

       SUBROUTINE A1TOA5(CHARS,CINDEX,WDS,PUNC)
       IMPLICIT INTEGER(A-Z)
       CHARACTER CHARS, PUNC, WDS*8
       INTEGER CINDEX
       DIMENSION CHARS(70), WDS(2)

       LOGICAL  PUNCT
C       CHARACTER CHARS, PUNC, TOUPPR
       CHARACTER TOUPPR
C       DIMENSION CHARS(70),WDS(2)
C      REAL*8 WDS,WDUMY(1)
C       CHARACTER*8 WDS
       CHARACTER SPACE, COMMA, PERIOD, SEMICO, LOWERA, LOWERZ
       DATA SPACE,COMMA,PERIOD,SEMICO /' ',',','.',';'/
      DATA LOWERA,LOWERZ/'a','z'/  /* THOSE ARE LOWER CASE!

      CHARACTER C
       PUNCT(C)=C.EQ.SPACE.OR.C.EQ.COMMA.OR.C.EQ.SEMICO
     1     .OR.C.EQ.PERIOD.OR.C.EQ.CHAR(0)
       
       

       WDS(1)=' '
       WDS(2)=' '
       
C DEBUG ATTEMT (M.V.):
C       PRINT *, CINDEX

       DO 2 CINDEX=CINDEX,70
       IF(.NOT.PUNCT(CHARS(CINDEX)))GOTO 4
       IF(CHARS(CINDEX).EQ.CHAR(0))GOTO 8
2       CONTINUE
       PUNC=CHAR(0)
       RETURN

4     DO 10 I=CINDEX,70
      IF(PUNCT(CHARS(I)))GOTO 8
      CHARS(I) = TOUPPR( CHARS( I ) )
C      IF(CHARS(I).GE.LOWERA.AND.CHARS(I).LE.LOWERZ)
C     1   CHARS(I)=AND(CHARS(I),:157777)
C     1     CHARS(I) = TOUPPR( CHARS(I ) )
10    CONTINUE
C      ENCODE(16,101,WDS)(CHARS(I),I=CINDEX,70)
C THIS IS AN ATTEMPT TO REMOVE THE 'MORE THAN SIX CHARS' BUG (M.V.)
      FINISH=70
      IF ((FINISH-CINDEX).GT.6) FINISH=CINDEX+5
C A TERRIBLE HACK, I KNOW, BUT GDB DOESN'T ALLOW ME TO DEBUG (M.V.)
      WRITE( UNIT=WDS, FMT=101 ) (CHARS(I),I=CINDEX,FINISH)
      PUNC=CHAR(0)
      CINDEX=71
      RETURN
8     PUNC=CHARS(I)
      FINISH=I-1
C      ENCODE(16,101,WDS)(CHARS(J),J=CINDEX,FINISH)
C THIS IS AN ATTEMPT TO REMOVE THE 'MORE THAN SIX CHARS' BUG (M.V.)
      IF ((FINISH-CINDEX).GT.5) FINISH=CINDEX+5

      WRITE( UNIT=WDS, FMT=101 ) (CHARS(J),J=CINDEX,FINISH)
      CINDEX=I
      RETURN
101   FORMAT(2(6A1,2X))
       END
