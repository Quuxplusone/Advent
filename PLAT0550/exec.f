C*    Dave Platt's A-code executive.
C*    Converted to Prime F77 by Anon, on an unknown date.
C*    Optimised for Prime as version 09.02 29 Mar 1989, by Mike Arnautov.
C*    Removed Prime optimisation and converted for use with g77,
C*                                       Mike Arnautov, 19 Jan 2003.
C
      INCLUDE 'exec.i'
      INTEGER*4 KEY
      Z_DIGITS='0123456789'
      Z_UC='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
      Z_LC='abcdefghijklmnopqrstuvwxyz'
      DBT = 9
      INUNIT = 5
      DBI = 11
      FREEZER = 12
      INHAND = -1
      LOGICALS(1) = 31
      LOGICALS(2) = 51
      LOGICALS(3) = 52
      PRINT '('' Loading data. Please wait...'')'
      CALL OPENDB
      CALL WEBSTER
      LINELEN=0
      CHARPOS = 1
      Z_OUTLINE = ' '
      CALL SETVAL(STATUS,0)
      CALL SETBIT(STATUS,0)
      DO 100 KEY=INIT * LONGTHOUSAND,  INIT * LONGTHOUSAND + NINIT - 1
         CALL PROCESS(KEY * LONGTHOUSAND, JUMP)
         IF (JUMP.NE.0) GOTO 200
100   CONTINUE
200   CONTINUE
      DO 999 KEY=REPEAT * LONGTHOUSAND, REPEAT * LONGTHOUSAND + NREP - 1
         CALL PROCESS(KEY * LONGTHOUSAND, JUMP)
         IF (JUMP.NE.0) GOTO 200
999   CONTINUE
      GOTO 200
      END
C*
C*******************************************************************
C*
      INTEGER*4 FUNCTION BSRCH(KEY, POS)
      INCLUDE 'exec.i'
      INTEGER*2 L,H,M,RS
      INTEGER*4 KEY, POS
C
      L=0
      H=NUMKEYS+1
      BSRCH=1
C
10    M=(L+H)/2
      IF (L.GT.H)RETURN
      IF (M.EQ.0)RETURN
C
      IF (KEY-KEYSBF(1,M))20,30,40
C
20    H=M-1
      GOTO 10
C
30    BSRCH=0
      POS = KEYSBF(2,M)
      RETURN
C
40    L=M+1
      GOTO 10
C
      END
C*
C*******************************************************************
C*
      INTEGER*4 FUNCTION CLASS(EDITKEY)
      INCLUDE 'exec.i'
      INTEGER*4 EVAL
      INTEGER*2 BITPOS(2,16)
      INTEGER*4 LBITPS(16)
      EQUIVALENCE (BITPOS, LBITPS)
      DATA LBITPS/1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,
     +            16384,32768/
      CLASS = EDITKEY / 1000
      RETURN
      ENTRY EVAL(ID)
      CLASS = ID / 1000
      INDEX = MOD (ID, 1000)
      IF (CLASS .EQ. 0) GOTO 1300
      IF (CLASS .EQ. OBJECT) GOTO 1100
      IF (CLASS .EQ. VARIABLE) GOTO 1200
      IF (ID .EQ. 9999) GOTO 1400
      IF (ID .EQ. 9998) GOTO 1500
      PRINT '('' Glitch! Bad EVAL - key = '',I4,'' on record '',F9.3,''
     +after loc '',I6)', ID, FLOAT(REC)/1000, BP
      EVAL = 0
      RETURN
1100  EVAL = OBJVAL(INDEX)
      RETURN
1200  EVAL = VARVAL(INDEX)
      RETURN
1300  EVAL = INDEX
      RETURN
1400  EVAL = -1
      RETURN
1500  EVAL = -2
      RETURN
C
      ENTRY BITVAL(ID)
      ID1 = ID
1900  CLASS = ID1 / 1000
      INDEX = MOD(ID1, 1000)
      IF (CLASS .EQ. OBJECT ) GOTO 2100
      IF (CLASS .EQ. VARIABLE) GOTO 2200
      IF (CLASS .EQ. PLACE) GOTO 2300
      IF (CLASS .EQ. INIT) GOTO 2400
      BITVAL = 0
      RETURN
2100  BITVAL = OBJBIT(INDEX)
      RETURN
2200  CLASS = VARVAL(INDEX) / 1000
      IF (CLASS .NE. PLACE .AND. CLASS .NE. OBJECT) GOTO 2210
      ID1 = VARVAL(INDEX)
      GOTO 1900
2210  BITVAL = VARBIT(INDEX)
      RETURN
2300  BITVAL = PLACEBIT(INDEX)
      RETURN
2400  BITVAL = BITPOS(2, ID + 1)
      IF (BITVAL .EQ. 0) BITVAL = BITPOS(1, ID + 1)
      RETURN
C
      ENTRY REF(ID)
      CLASS = ID / 1000
      IF (CLASS .EQ. VARIABLE) GOTO 3100
      REF = ID
      RETURN
3100  REF = VARVAL(MOD(ID, 1000))
      RETURN
C
      ENTRY WHERE(I)
      INDEX = I / 1000
      IF (INDEX .EQ. OBJECT) GOTO 4100
      PRINT '('' Glitch! Bad WHERE - key = '',I4,'' on record '',F9.3,''
     + after loc '',I6)', I, FLOAT(REC)/1000, BP
      RETURN
4100  INDEX = MOD(I, 1000)
      WHERE = OBJLOC(INDEX)
      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE COMMAND
      INCLUDE 'exec.i'
      CHARACTER*15 Z_HOLDARG
      SAVE HOLDARG,Z_HOLDARG
      LINELEN = 0
      IF (AND(BITVAL(STATUS),PLSCLRFY) .NE. 0) THEN
          HOLDARG   = LINEWORD(1)
          Z_HOLDARG = Z_ARG(1)
          ENDIF
      DO 5 I=1, LINESIZE
5     LINEWORD(I) = -1
      CALL READIN
10    IF (PARSE(0) .NE. 0) GOTO 100
      CALL FIND(Z_LEX, RESULT, LINEVAL)
      IF (RESULT .NE. 0) GOTO 50
      IF (CLASS(LINEVAL) .EQ. NULL) GOTO 10
20    LINELEN = LINELEN + 1
      LINEWORD(LINELEN) = LINEVAL
      Z_ARG(LINELEN)=Z_LEX
      IF (LINEVAL .LT. 0 .OR. LINELEN .GE. LINESIZE) GOTO 100
      GOTO 10
50    LINEVAL = -RESULT
      GOTO 20
100   CONTINUE
      IF (LINELEN .NE. 1 .OR. AND(BITVAL(STATUS),PLSCLRFY) .EQ. 0)
     +    GOTO 110
      I = CLASS(LINEWORD(1))
      J = CLASS(HOLDARG)
      IF (I .EQ. PLACE) I = OBJECT
      IF (J .EQ. PLACE) J = OBJECT
      IF ((I .EQ. VERB .AND. J .EQ. OBJECT). OR.
     +    (I .EQ. OBJECT .AND. J .EQ. VERB)) THEN
          LINEWORD(2) = LINEWORD(1)
          Z_ARG(2)    = Z_ARG(1)
          LINEWORD(1) = HOLDARG
          Z_ARG(1)    = Z_HOLDARG
          LINELEN     = 2
          ENDIF
110   CALL SETBIT(STATUS, AND(BITVAL(STATUS), NOT(PLSCLRFY)))
      I = CLASS(LINEWORD(1))
      IF (LINELEN .EQ. 2 .AND.
     +    CLASS(LINEWORD(2)) .EQ. VERB .AND. I .NE. VERB) THEN
          HOLDARG     = LINEWORD(1)
          Z_HOLDARG   = Z_ARG(1)
          LINEWORD(1) = LINEWORD(2)
          Z_ARG(1)    = Z_ARG(2)
          LINEWORD(2) = HOLDARG
          Z_ARG(2)    = Z_HOLDARG
          ENDIF
      DO 120 I=1, LINESIZE
      K = 0
      J = CLASS(LINEWORD(I))
      IF (J .EQ. OBJECT .OR. J .EQ. PLACE) K = BITVAL(LINEWORD(I))
      IF (J .EQ. VERB) K = XVERB
      IF (LINEWORD(I) .LT. 0) K = BADWORD
      CALL SETVAL(ARGWORDS(I), LINEWORD(I))
120   CALL SETBIT(ARGWORDS(I), K)
      CALL SETVAL(STATUS, LINELEN)
      IF (LINELEN .EQ. 0) GOTO 999
      I = CLASS(LINEWORD(1))
      IF (I .EQ. VERB)
     + CALL SETBIT(STATUS, OR(BITVAL(STATUS), XVERB))
      IF (I .EQ. OBJECT)
     + CALL SETBIT(STATUS, OR(BITVAL(STATUS), XOBJECT))
      IF (I .EQ. PLACE)
     + CALL SETBIT(STATUS, OR(BITVAL(STATUS), XPLACE))
999   CONTINUE
      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE FIND(Z_ID, RESULT, RFIND)
      INCLUDE 'exec.i'
      CHARACTER*(*) Z_ID
      CHARACTER*12  Z_IDE
C
      RESULT = 0
      Z_IDE = Z_ID
      LOW = 0
      HIGH = SYMCNT + 1
      LENGTH=NLEN(Z_IDE,WORDSIZE)
C
      DO 5 I=1, LENGTH
      J = INDEX('[#]', Z_IDE(I:I))
      IF (J .NE. 0) Z_ID(I:I) = '~'
      IF (Z_IDE(I:I).LT.Z_LC(1:1).OR.Z_IDE(I:I).GT.Z_LC(26:26)) GOTO 5
      J = INDEX(Z_LC, Z_IDE(I:I))
      IF (J .NE. 0) Z_IDE(I:I) = Z_UC(J:J)
5     CONTINUE

10    RFIND = (LOW + HIGH) / 2
      IF (LOW .EQ. RFIND) GOTO 40
      IF (Z_IDE .EQ. Z_WORD(RFIND)) GOTO 30
      IF (Z_IDE .LT. Z_WORD(RFIND)) GOTO 20
      LOW = RFIND
      GOTO 10
C
20    HIGH = RFIND
      GOTO 10
C
30    Z_ID = Z_WORD(RFIND)
      LOW=RFIND
      RFIND = FILEKEY(RFIND)
      IF (RFIND.LT.0) RFIND=-RFIND
      IF (LENGTH.GT.2) RETURN
      IF (LOW.EQ.SYMCNT) RETURN
      DO 35 I=LOW+1,SYMCNT
         IF (Z_ID.NE.Z_WORD(I)(1:LENGTH)) RETURN
         IF (RFIND.NE.FILEKEY(I).AND.RFIND.NE.-FILEKEY(I)) GOTO 35
         Z_ID=Z_WORD(I)
         RETURN
35    CONTINUE
      RETURN
C*
40    IF (LENGTH.LT.2) GOTO 50
      RFIND=RFIND+1
      IF (Z_IDE.NE.Z_WORD(RFIND)(1:LENGTH)) GOTO 50
      HIGH=RFIND
      KEYVL=FILEKEY(RFIND)
      IF (KEYVL.LT.0) KEYVL=-KEYVL
C*
45    IF (HIGH.EQ.SYMCNT) GOTO 30
      HIGH=HIGH+1
      IF (Z_IDE.NE.Z_WORD(HIGH)(1:LENGTH)) GOTO 30
      IF (KEYVL.EQ.FILEKEY(HIGH).OR.KEYVL.EQ.-FILEKEY(HIGH)) GOTO 45
C*
C*    Now for a special case. If the clashing word is just 1 character
C*    longer than original match, the match is assumed to be a 'stopword'
C*    deliberately introduced to intercept abbreviations.
C*
      LOW=NLEN(Z_WORD(RFIND),WORDSIZE)
      IF (LOW.LT.3) GOTO 47
      IF (Z_WORD(RFIND)(1:LOW).NE.Z_WORD(HIGH)(1:LOW)) GOTO 47
      IF (LOW+1.EQ.NLEN(Z_WORD(HIGH),WORDSIZE)) GOTO 45
C*
47    RESULT = 2
      GOTO 60
C*
50    RESULT = 1
C*
60    Z_BADVAR=Z_IDE
      RETURN
C*
      END
C*
C*******************************************************************
C*
      INTEGER*4 FUNCTION NEAR(I)
      INCLUDE 'exec.i'
      INTEGER*4 EVAL
      WHRE = WHERE(I)
      BTS = BITVAL(I)
      HEAH = EVAL(HERE)
      NEAR = 1
      IF (WHRE .EQ. HEAH) RETURN
      IF (WHRE .NE. HEAH-1 .OR. AND(BTS, DUAL) .EQ. 0) NEAR = 0
      RETURN
      END
C*
C*******************************************************************
C*
      INTEGER*4 FUNCTION PARSE(DUMARG)
      INCLUDE 'exec.i'
      PARSE = 0
      DO 10 I=LINEX,LINEND
         IF (Z_LINE(I:I) .NE. ' ') GOTO 15
10    CONTINUE
      PARSE = 1
      RETURN
C*
15    DO 20 J=I,LINEND + 1
         Z_SEP = Z_LINE(J:J)
         IF (Z_SEP .EQ. ' ' .OR. Z_SEP .EQ. ',') GOTO 25
20    CONTINUE
      J = LINEND + 1
C*
25    K=J
      IF (Z_SEP .NE. ' ') GOTO 35
      DO 30 K=J,LINEND + 1
         Z_SEP = Z_LINE(K:K)
         IF (Z_SEP .EQ. ' ') GOTO 30
         IF (Z_SEP .NE. ',') Z_SEP = ' '
         GOTO 35
30    CONTINUE
C*
35    Z_LEX = Z_LINE(I:J - 1)
      LINEX = K
      IF (Z_SEP .EQ. ',') LINEX = LINEX + 1
      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE PROCESS(RECNO, JUMP)
      INCLUDE 'exec.i'
      INTEGER*4 RECNO, POSN
      INTEGER*4 EVAL
      CHARACTER*1 Z_RESP
      JUMP=1
      REC=RECNO
      CALLS=0
      DOS=0
1000  BP=1
      IF (BSRCH(REC, POSN).NE.0) GOTO 1500
      BP = POSN
1100  SKIPFLAG=0
      NEGATE=0
      PASSON=0
      LOGICMODE = 1
1200  OPCODE=BUFFER(BP)
      BP=BP+1
      IF (OPCODE.GT.0) GOTO 2000
1300  REC = REC + LONGONE
      GOTO 1000

1500  IF (CALLS.NE.0) GOTO 1510
      JUMP=0
      RETURN
1510  REC   = CALLREC(CALLS)
      BP    = CALLBP(CALLS)
      DOS   = CALLDOS(CALLS)
      CALLS = CALLS - 1
      GOTO 1100

2000  CONTINUE
      GOTO (2100,2200,2300,2400,2500,2600,2700,2800,
     + 2900,3000,3100,3200,3300,3400,3500,3600,3700,
     + 3800,3900,4000,4100,4200,4300,4400,4500,4600,
     + 4700,4800,4900,5000,5100,5200,5400,5500,
     + 5600,5700,5800,5900,6000,6100,6200,6300,6400,
     + 6500, 6600, 6700, 6800, 6900, 7000, 7100, 5100, 5100,
     + 7200, 7300, 7400, 7500), OPCODE
      PRINT '('' Glitch! Bad opcode: '',I5,'' - rec = '',F10.3,'' after
     +loc '',I6)',OPCODE, FLOAT(REC)/1000, BP
      GOTO 1300
C
C     "KEYWORD"
C
 2100 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      DO 2110 I=1,LINELEN
 2110 IF (WORD1.EQ.LINEWORD(I)) GOTO 1200
      GOTO 1300
C
C     "HAVE"
C
 2200 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (WHERE(REF(WORD1)).EQ.INHAND) GOTO 1200
      GOTO 1300
C
C     "NEAR"
C
 2300 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      I=REF(WORD1)
      J=WHERE(I)
      IF (J .EQ. INHAND .OR. NEAR(I) .NE. 0) GOTO 1200
      GOTO 1300
C
C     "AT"
C
 2400 CONTINUE
      TRUTH=0
 2410 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (TRUTH .EQ. 0 .AND. (WORD1 .EQ. EVAL(HERE))) TRUTH = 1
      IF (BUFFER(BP).NE.4) GOTO 2420
      BP=BP+1
      GOTO 2410
 2420 IF (TRUTH .NE. 0) GOTO 1200
      GOTO 1300
C
C     "ANYOF"
C
 2500 CONTINUE
      TRUTH=0
 2510 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      DO 2520 I=1,LINELEN
 2520 IF (TRUTH .EQ. 0 .AND. (WORD1 .EQ. LINEWORD(I))) TRUTH = 1
      IF (BUFFER(BP).NE.5) GOTO 2530
      BP=BP+1
      GOTO 2510
 2530 IF (TRUTH .NE. 0) GOTO 1200
      GOTO 1300
C
C     "IFEQ"
C
 2600 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (EVAL(WORD1).EQ.EVAL(WORD2)) TRUTH = 1
C
C     -- GENERAL "IF" PROCESSING --
C
 2610 TRUTH = TRUTH + NEGATE
      IF (TRUTH .GT. 1) TRUTH = 0
      IF (LOGICMODE .EQ. 1 .AND. TRUTH .EQ. 0) TRUTH = PASSON
      IF (LOGICMODE .EQ. 2 .AND. TRUTH .NE. 0) TRUTH = PASSON
      IF (LOGICMODE .EQ. 3) TRUTH = TRUTH + PASSON
      IF (TRUTH .GT. 1) TRUTH = 0
      PASSON=0
      NEGATE=0
      DO 2615 LOGICMODE = 1, 3
2615  IF (BUFFER(BP) .EQ. LOGICALS(LOGICMODE)) GOTO 2616
      LOGICMODE = 1
      GOTO 2620
2616  CONTINUE
      BP=BP+1
      PASSON=TRUTH
      GOTO 1200
 2620 IF (SKIPFLAG.EQ.0) GOTO 2630
      SKIPFLAG=SKIPFLAG+1
      GOTO 1200
 2630 IF (TRUTH .EQ. 0) SKIPFLAG=1
      GOTO 1200
C
C     "IFLT"
C
 2700 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (EVAL(WORD1).LT.EVAL(WORD2)) TRUTH=1
      GOTO 2610
C
C     "IFGT"
C
 2800 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (EVAL(WORD1).GT.EVAL(WORD2)) TRUTH=1
      GOTO 2610
C
C     "IFAT"
C
 2900 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (REF(WORD1).EQ.EVAL(HERE)) TRUTH=1
      GOTO 2610
C
C     "CHANCE"
C
 3000 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (EVAL(WORD1).GT.MYRAND(100)) TRUTH=1
      GOTO 2610
C
C     "ELSE"
C
 3100 CONTINUE
      IF (SKIPFLAG.GT.1) GOTO 1200
      SKIPFLAG=1-SKIPFLAG
      GOTO 1200
C
C     "FIN"
C
 3200 CONTINUE
      SKIPFLAG=MAX(SKIPFLAG-1,0)
      GOTO 1200
C
C     "EOF"
C
 3300 CONTINUE
      SKIPFLAG=0
      GOTO 1200
C
C     "GET"
C
 3400 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL MOVEOBJ(REF(WORD1), INHAND)
      CALL SETBIT(STATUS, OR(BITVAL(STATUS), JUGGLED))
      I = EVAL(REF(WORD1))
      IF (I .LT. 0) CALL SETVAL(REF(WORD1), -1 - I)
      GOTO 1200
C
C     "DROP"
C
 3500 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL MOVEOBJ(REF(WORD1), EVAL(HERE))
      CALL SETBIT(STATUS, OR(BITVAL(STATUS), JUGGLED))
      GOTO 1200
C
C     "APPORT"
C
3600  CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL MOVEOBJ(REF(WORD1), REF(WORD2))
      CALL SETBIT(STATUS, OR(BITVAL(STATUS), JUGGLED))
      GOTO 1200
C
C     "SET"
C
 3700 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,EVAL(WORD2))
      GOTO 1200
C
C     "ADD"
C
 3800 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,EVAL(WORD1)+EVAL(WORD2))
      GOTO 1200
C
C     "SUB"
C
 3900 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,EVAL(WORD1)-EVAL(WORD2))
      GOTO 1200
C
C     "GOTO"
C
 4000 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      I = EVAL(HERE)
      J = BITVAL(HERE)
      CALL SETVAL(HERE, REF(WORD1))
      CALL SETBIT(HERE, BITVAL(REF(WORD1)))
      CALL SETVAL(THERE, I)
      CALL SETBIT(THERE, J)
      CALL SETBIT(STATUS, OR(BITVAL(STATUS), MOVED))
      GOTO 1200
C
C     "MOVE"
C
 4100 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      IF (WORD1 .NE. LINEWORD(1) .AND. (LINELEN .LT. 2 .OR.
     + WORD1 .NE. LINEWORD(2) .OR. (LINEWORD(1) .NE. EXPLORE
     + .AND. LINEWORD(1) .NE. SAYXX))) GOTO 1200
      I = EVAL(HERE)
      J = BITVAL(HERE)
      CALL SETVAL(HERE, REF(WORD2))
      CALL SETBIT(HERE, BITVAL(REF(WORD2)))
      CALL SETVAL(THERE, I)
      CALL SETBIT(THERE, J)
      CALL SETBIT(STATUS, OR(BITVAL(STATUS), MOVED))
      RETURN
C
C     "CALL"
C
 4200 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALLS=CALLS+1
      CALLREC(CALLS)=REC
      CALLBP(CALLS)=BP
      CALLDOS(CALLS)=DOS
      REC=REF(WORD1)*LONGTHOUSAND
      J = CLASS(REF(WORD1))
      IF (J.EQ.OBJECT.OR.J.EQ.PLACE) REC=REC+000500
      GOTO 1000
C
C     "SAY"
C
 4300 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SAY(REF(WORD1))
      GOTO 1200
C
C     "NAME"
C
 4400 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      DO 4410 I=1,4
4410  IF (WORD2.EQ.ARGWORDS(I)) GOTO 4420
      GOTO 4430
4420  CALL SAYNAME(REF(WORD1), WORD2)
      GOTO 1200
4430  CONTINUE
      CALL SAYNAME(REF(WORD1),REF(WORD2))
      GOTO 1200
C
C     "VALUE"
C
 4500 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SAYVALUE(REF(WORD1),REF(WORD2))
      GOTO 1200
C
C     "PROCEED"
C
 4600 CONTINUE
      IF (SKIPFLAG.GT.0) GOTO 1200
      GOTO 1300
C
C     "QUIT"
C
 4700 CONTINUE
      IF (SKIPFLAG.GT.0) GOTO 1200
      RETURN
C
C     "STOP"
C
 4800 CONTINUE
      IF (SKIPFLAG.GT.0) GOTO 1200
      CLOSE (UNIT=DBI)
      CLOSE (UNIT=DBT)
9991  STOP
C
C     "IFHAVE"
C
 4900 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (WHERE(REF(WORD1)).EQ.INHAND) TRUTH=1
      GOTO 2610
C
C     "IFNEAR"
C
 5000 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      IF (SKIPFLAG.GT.0) GOTO 2610
      I=REF(WORD1)
      J=WHERE(I)
      TRUTH = 0
      IF (J .EQ. INHAND .OR. NEAR(I) .NE. 0) TRUTH = 1
      GOTO 2610
C
C     "OR" - SHOULD NEVER GET HERE!
C
 5100 CONTINUE
      PRINT  '('' Glitch! Ill-placed logical in record '',F10.3,''after
     +loc '',I6)',FLOAT(REC)/1000,BP
      GOTO 1200
C
C     "RANDOM"
C
 5200 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,MYRAND(EVAL(WORD2)))
      GOTO 1200
C
C     "BITST"
C
 5400 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (AND(BITVAL(WORD1),BITVAL(WORD2)).NE.0) TRUTH=1
      GOTO 2610
C
C     "BISET"
C
 5500 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETBIT(WORD1,OR(BITVAL(WORD1),BITVAL(WORD2)))
      GOTO 1200
C
C     "BICLEAR"
C
 5600 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETBIT(WORD1,AND(BITVAL(WORD1),NOT(BITVAL(WORD2))))
      GOTO 1200
C
C     "ITOBJ"
C
 5700 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      DOS=DOS+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      DOVAR(DOS)=WORD1
      CALL SETVAL(WORD1,OBJECT*1000)
      DOPOINT(DOS)=BP
      DOMODE(DOS)=-32767
      GOTO 1200
C
C     "ITPLACE"
C
 5800 CONTINUE
      WORD1=BUFFER(BP)
      BP=BP+1
      DOS=DOS+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      DOVAR(DOS)=WORD1
      CALL SETVAL(WORD1,PLACE*1000)
      DOPOINT(DOS)=BP
      DOMODE(DOS)=-32766
      GOTO 1200
C
C     "EOI"
C
 5900 CONTINUE
      IF (SKIPFLAG.EQ.0) GOTO 5910
5905  DOS=DOS-1
      GOTO 1200
 5910 I=EVAL(DOVAR(DOS))+1
      CALL SETVAL(DOVAR(DOS),I)
      IF (DOMODE(DOS).EQ.-32767) GOTO 5920
      IF (DOMODE(DOS).EQ.-32766) GOTO 5930
      IF (I.GT.DOMODE(DOS)) GOTO 5905
      GOTO 5980
5920  IF (MOD(I, 1000) .EQ. NOBJ) GOTO 5905
      GOTO 5980
5930  IF (MOD(I, 1000) .EQ. NPLACE) GOTO 5905
      GOTO 5980
C*
5980  BP=DOPOINT(DOS)
      GOTO 1200
C
C     "IFLOC"
C
6000  CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH=0
      IF (WHERE(REF(WORD1)).EQ.REF(WORD2)) TRUTH=1
      GOTO 2610
C
C     "INPUT"
C
 6100 CONTINUE
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL COMMAND
      GOTO 1200
C
C     "LOCATE"
C
6200  CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,WHERE(REF(WORD2)))
      GOTO 1200
C
C     "NOT"
C
 6300 CONTINUE
      IF (SKIPFLAG.GT.0) GOTO 1200
      NEGATE = 1 - NEGATE
      GOTO 1200
C
C     "IFKEY"
C
 6400 CONTINUE
      WORD1 = BUFFER(BP)
      BP = BP + 1
      IF (SKIPFLAG.GT.0) GOTO 2610
      TRUTH = 0
      DO 6410 I = 1,LINELEN
      IF (TRUTH .EQ. 0 .AND. WORD1 .EQ. LINEWORD(I)) TRUTH = 1
6410  IF (TRUTH .NE. 0) GOTO 6420
 6420 CONTINUE
      GOTO 2610
C
C     "LDA"
C
 6500 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1, WORD2)
      GOTO 1200
C
C     "EVAL"
C
 6600 CONTINUE
      WORD1 = BUFFER(BP)
      WORD2 = BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1, EVAL(REF(WORD2)))
      GOTO 1200
C
C     "MULTIPLY"
C
 6700 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,EVAL(WORD1)*EVAL(WORD2))
      GOTO 1200
C
C     "DIVIDE"
C
 6800 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(WORD1,EVAL(WORD1)/EVAL(WORD2))
      GOTO 1200
C
C     "SVAR"
C
 6900 CONTINUE
      WORD1 = BUFFER(BP)
      WORD2 = BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      I = EVAL(WORD2)
      CALL SVAR(EVAL(WORD1), I)
      CALL SETVAL(WORD2, I)
      GOTO 1200
C
C     "EXEC"
C
 7000 CONTINUE
      WORD1 = BUFFER(BP)
      WORD2 = BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SPECIAL
      GOTO 1200
C
C     "QUERY"
C
7100  CONTINUE
      WORD1 = BUFFER(BP)
      BP = BP + 1
      IF (SKIPFLAG .GT. 0) GOTO 2610
7110  CALL SAY(REF(WORD1))
      PRINT '('' '', $)'
      READ (INUNIT, '(A1)', END=7120, ERR=7120) Z_RESP
      TRUTH = 0
      IF ((Z_RESP.EQ.'Y') .OR. (Z_RESP.EQ.'y') .OR. (Z_RESP.EQ.' '))
     +     TRUTH = 1
      IF (TRUTH .NE. 0 .OR. (Z_RESP.EQ.'N'.OR.Z_RESP.EQ.'n')) GOTO 2610
7120  PRINT '('' Please answer the question.'')'
      GOTO 7110
C
C        "DEPOSIT"
C
 7200 CONTINUE
      WORD1 = BUFFER(BP)
      WORD2 = BUFFER(BP+1)
      BP=BP+2
      IF (SKIPFLAG.GT.0) GOTO 1200
      CALL SETVAL(REF(WORD1),EVAL(WORD2))
      GOTO 1200
C
C     "ITERATE"
C
 7300 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      WORD3=BUFFER(BP+2)
      IF (CLASS(WORD2).EQ.VARIABLE) WORD2=EVAL(WORD2)
      IF (CLASS(WORD3).EQ.VARIABLE) WORD3=EVAL(WORD3)
      BP=BP+3
      DOS=DOS+1
      IF (SKIPFLAG.GT.0) GOTO 1200
      DOVAR(DOS)=WORD1
      CALL SETVAL(WORD1,WORD2)
      DOPOINT(DOS)=BP
      DOMODE(DOS)=WORD3
      GOTO 1200
C
C     "SMOVE"
C
 7400 CONTINUE
      WORD1=BUFFER(BP)
      WORD2=BUFFER(BP+1)
      WORD3=BUFFER(BP+2)
      BP=BP+3
      IF (SKIPFLAG.GT.0) GOTO 1200
      IF (WORD1 .NE. LINEWORD(1) .AND. (LINELEN .LT. 2 .OR.
     + WORD1 .NE. LINEWORD(2) .OR. (LINEWORD(1) .NE. EXPLORE
     + .AND. LINEWORD(1) .NE. SAYXX))) GOTO 1200
      I = EVAL(HERE)
      J = BITVAL(HERE)
      CALL SETVAL(HERE, REF(WORD2))
      CALL SETBIT(HERE, BITVAL(REF(WORD2)))
      CALL SETVAL(THERE, I)
      CALL SETBIT(THERE, J)
      CALL SETBIT(STATUS, OR(BITVAL(STATUS), MOVED))
      CALL SAY(REF(WORD3))
      RETURN
C
C     "DEFAULT"
C
7500  CONTINUE
      WORD1 = BUFFER(BP)
      BP = BP + 1
      IF (SKIPFLAG.GT.0) GOTO 1200
      IF (LINELEN .NE. 1) GOTO 1200
      LINELEN = 2
      K = -1
      DO 7510 I=OBJECT*1000,OBJECT*1000+NOBJ-1
      IF (NEAR(I) .EQ. 0 .OR. AND(BITVAL(I), BITVAL(WORD1)) .EQ. 0)
     + GOTO 7510
      IF (K .LT. 0) GOTO 7505
      K = -2
      GOTO 7515
7505  K = I
7510  CONTINUE
7515  CALL SETVAL(ARGWORDS(2), K)
      LINEWORD(2) = K
      IF (K .LT. 0) GOTO 1200
      CALL SETBIT(ARGWORDS(2), BITVAL(K))
      IF (K.GT.0) K=-K
      DO 7520 I=1,SYMCNT
7520  IF (FILEKEY(I) .EQ. K) GOTO 7530
      Z_ARG(2) = ' '
      GOTO 1200
7530  Z_ARG(2) = Z_WORD(I)
      LINELEN = 2
      CALL SETVAL(STATUS, 2)
      GOTO 1200
C*
      END
C*
C*******************************************************************
C*
      INTEGER*4 FUNCTION MYRAND(IX)
      INTEGER*4 IX
C*
      INTEGER*4 LNGRND
      SAVE LNGRND
      REAL*4 RND

      DATA LNGRND/12345/

      LNGRND = AND(LNGRND * 8693 + 6923, 32767)
      RND = LNGRND / 32767.0
      MYRAND = RND * IX

      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE READIN
      INCLUDE 'exec.i'
      IF (CHARPOS .NE. 1) CALL WRITETEXT
      LINEX = -1
      IF (Z_SEP .EQ. ',') THEN
         I = INDEX(Z_LINE, ',') + 1
         J = INDEX(Z_LINE, ' ') + 1
         Z_LEX = Z_LINE(I:)
         IF (Z_LEX .NE. ' ' .AND. J .LT. I) THEN
            Z_LINE(J:) = Z_LEX
            GOTO 5
            ENDIF
         ENDIF
C
      PRINT '('' > '',$)'
      READ (INUNIT, '(A139)', END=2) Z_LINE
2     CONTINUE
      PRINT '('' '')'
C
5     DO 10, LINEX = 1, 140
10    IF (Z_LINE(LINEX:LINEX) .NE. ' ') GOTO 20
      LINEX = 140
C
20    DO 30, LINEND = 140, LINEX, -1
30    IF (Z_LINE(LINEND:LINEND) .NE. ' ') GOTO 40
      LINEND = LINEX
C
40    RETURN
      END
C*
C*******************************************************************
C*
      INTEGER*4 FUNCTION READTEXT(KEY)
      INCLUDE 'exec.i'
      INTEGER*4 KEY, POSN
      INTEGER*2     ILINE(60), JLINE(60)
      CHARACTER*120 Z_TLINE
      EQUIVALENCE   (JLINE, Z_TLINE)
C
      READTEXT = 1
      IF (BSRCH(KEY, POSN) .NE. 0) RETURN
C
      READTEXT = 0
      Z_TLINE = ' '
      READ(DBT, ERR=10, REC=POSN) ILINE
      J = 1
      DO 5 I = 1, 60, 2
         K = ILINE(J)
         IF (0 .LE. K .AND. K .LE. 60) GOTO 6

4        JLINE(I) = -K
         JLINE(I + 1) = -ILINE(61 - J)
         J = J + 1
5     CONTINUE
6     Z_TEXT = Z_TLINE
      RETURN
10    PRINT '('' Text file too small!'')'
      STOP
      END
C*
C*******************************************************************
C*
      SUBROUTINE SAY(KEY)
      INCLUDE 'exec.i'
      INTEGER*4 EVAL
      INTEGER*4 KEYVAL
      CHARACTER Z_SUB*160, Z_CODE*15, Z_TYPE
      MODE  = 1
      SUB_VALUE = REF(KEY)
      IF (CLASS(KEY) .EQ. OBJECT) SUB_VALUE = EVAL(KEY)
      GOTO 100
      ENTRY SAYNAME(KEY,EVL)
      MODE = 2
      SUB_VALUE = EVL
      GOTO 100
      ENTRY SAYVALUE(KEY,EVL)
      MODE = 3
      SUB_VALUE = EVL
      IF (CLASS(EVL) .EQ. OBJECT) SUB_VALUE = EVAL(EVL) + 1
      VALUTA = SUB_VALUE
100   GOTO (9999,9998,9997),KEY-3999
      KEYVAL = KEY * LONGTHOUSAND
      IF (CLASS(KEY) .EQ. TEXT) GOTO 200
      IF (CLASS(KEY) .EQ. OBJECT) GOTO 50
      IF (CLASS(KEY) .EQ. PLACE) GOTO 70
      PRINT '('' Glitch! Bad SAY - key = '',I4,'' at rec '',I7,'' after
     +loc '',I6)', KEY, FLOAT(REC)/1000, BP
      RETURN
50    IF (WHERE(KEY) .EQ. INHAND) GOTO 200
         IF (EVAL(KEY) .LT. 0) GOTO 9999
      KEYVAL = KEYVAL + LONGTEN * (EVAL(KEY) + LONGONE)
      GOTO 200
70    I = BITVAL(STATUS)
         IF (AND(I, LOOKING) .NE. 0) GOTO 80
      IF ((AND(I, BRIEF) .NE. 0 .AND. AND(BITVAL(KEY), BEEN)
     + .NE. 0) .OR. AND(I, FAST) .NE. 0) GOTO 200
80    KEYVAL = KEYVAL + LONGTEN
200   IF (READTEXT(KEYVAL) .NE. 0) GOTO 9999
      KEYVAL = KEYVAL + LONGONE
      IF (Z_TEXT(1:3) .EQ. '>$<') GOTO 200
      IF (MODE .EQ. 1) GOTO 700
300   I = INDEX(Z_TEXT, '#')
      IF (I .EQ. 0) GOTO 700
410   IF (MODE .EQ. 3) GOTO  500
      DO 412 J=1,4
412   IF (EVL.EQ.ARGWORDS(J)) GOTO 413
      GOTO 417
413   Z_CODE=Z_ARG(J)
411   DO 416 K=1,15
      IF (Z_CODE(K:K) .LT. Z_UC(1:1) .OR. Z_CODE(K:K) .GT. Z_UC(26:26))
     + GOTO 416
      J = INDEX(Z_UC, Z_CODE(K:K))
415   Z_CODE(K:K) = Z_LC(J:J)
416   CONTINUE
      GOTO 600
417   CONTINUE

      JJ = -1
      DO 420 J=1, SYMCNT
      IF (FILEKEY(J) .EQ. -EVL) GOTO 430
      IF (FILEKEY(J) .EQ. EVL) JJ = J
420   CONTINUE
      IF (JJ .EQ. -1) GOTO 700
      J = JJ

430   Z_CODE = Z_WORD(J)
      GOTO 411

500   Z_CODE = ' '
      DO 510 J=6,1,-1
      K=MOD(VALUTA, 10)+1
      VALUTA = VALUTA / 10
      Z_CODE(J:J) = Z_DIGITS(K:K)
      IF (VALUTA .EQ. 0) GOTO 520
510   CONTINUE
520   IF (Z_CODE(1:1).NE.' ')GOTO 530
      Z_CODE = Z_CODE(2:WORDSIZE)
      GOTO 520
530   CONTINUE
600   Z_SUB = Z_TEXT(1:I-1) // Z_CODE
      J = I + 15
611   IF (Z_SUB(J:J).NE.' ')GOTO 610
      J = J - 1
      GOTO 611
610   CONTINUE
      Z_SUB = Z_SUB(1:J) // Z_TEXT(I+1:140)
      Z_TEXT = Z_SUB
      GOTO 300
C*
700   SUBSTART = INDEX(Z_TEXT, '[')
      IF (SUBSTART .EQ. 0) GOTO 800
      SUBEND = INDEX(Z_TEXT, ']')
      IF (SUBEND .EQ. 0) SUBEND = NLEN(Z_TEXT, 140) + 1
      BREAK = SUBSTART - 1
      SLASH = INDEX(Z_TEXT(SUBSTART+1:), '/')
      IF (SLASH .EQ. 0 .OR. SLASH .GT. SUBEND-SUBSTART) GOTO 720
      SLASH = SUBSTART + SLASH
      IF (SUB_VALUE .EQ. 1) GOTO 730
      SUBSTART = SLASH
C*
720   SLASH = SUBEND
C
730   IF (BREAK .EQ. 0) GOTO 740
      Z_SUB = Z_TEXT(1:BREAK)
      IF (SUBSTART .GE. SLASH-1) GOTO 750
      Z_SUB = Z_SUB(1:BREAK) // Z_TEXT(SUBSTART+1:SLASH-1)
      BREAK = BREAK + SLASH - SUBSTART - 1
      GOTO 750
C*
740   IF (SUBSTART . GE. SLASH-1) GOTO 760
      Z_SUB = Z_TEXT(SUBSTART+1:SLASH-1)
      BREAK = SLASH - SUBSTART - 1
C*
750   IF (SUBEND .LT. 140)
     +     Z_SUB = Z_SUB(1:BREAK) // Z_TEXT(SUBEND+1:140)
      GOTO 770
C*
760   Z_SUB = ' '
      IF (SUBEND .LT. 140) Z_SUB = Z_TEXT(SUBEND+1:140)
C*
770   Z_TEXT = Z_SUB
      GOTO 700
C*
800   NEWLINE = 1
      IF (Z_TEXT(1:1) .NE. '!') GOTO 1000
      IF (Z_TEXT(2:2) .EQ. '`') NEWLINE = 0
      Z_TEXT = Z_TEXT(3:)
c*
1000  CALL APPEND(Z_TEXT)
      IF (NEWLINE .NE. 0) CALL WRITETEXT
      GOTO 200
9997  PRINT '('' Ok.'')'
      GOTO 9999
9998  IF (CHRPOS .EQ. 0) THEN
         PRINT '('' '')'
      ELSE
         CALL WRITETEXT
      ENDIF
9999  RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE SETVAL(I, J)
      INCLUDE 'exec.i'
      CLASS = I / 1000
      INDEX = MOD(I, 1000)
      IF (CLASS .EQ. OBJECT) GOTO 1100
      IF (CLASS .EQ. VARIABLE) GOTO 1200
      PRINT '('' Glitch! Bad SETVALUE - key = '',I4,'' on record '',F9.3
     +,'' after loc '',I6)', I, FLOAT(REC)/1000, BP
      RETURN
1100  OBJVAL(INDEX) = J
      RETURN
1200  VARVAL(INDEX) = J
      RETURN
C
      ENTRY SETBIT(I, J)
      CLASS = I / 1000
      INDEX = MOD(I, 1000)
1999  IF (CLASS .EQ. OBJECT) GOTO 2100
      IF (CLASS .EQ. VARIABLE) GOTO 2200
      IF (CLASS .EQ. PLACE) GOTO 2300
      PRINT '('' Glitch! Bad SETBIT - key = '',I4,'' on record '',F9.3,
     +'' after loc '',I6)', I, FLOAT(REC)/1000, BP
      RETURN
2100  OBJBIT(INDEX) = J
      RETURN
2200  VARBIT(INDEX) = J
      INDEX = VARVAL(INDEX) / 1000
      IF (INDEX .NE. PLACE .AND. INDEX .NE. OBJECT) RETURN
      CLASS = INDEX
      INDEX = MOD(REF(I), 1000)
      GOTO 1999
2300  PLACEBIT(INDEX) = J
      RETURN
C
      ENTRY MOVEOBJ(I, J)
      CLASS = I / 1000
      INDEX = MOD(I, 1000)
      IF (CLASS .EQ. OBJECT) GOTO 3100
      PRINT '('' Glitch! Bad MOVEOBJ - key = '',I4,'' on record '',F9.3,
     +'' after loc '',I6)', I, FLOAT(REC)/1000, BP
      RETURN
3100  INDEX = MOD(I, 1000)
      OBJLOC(INDEX) = J
      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE WEBSTER
      INCLUDE 'exec.i'
      CHARACTER*12 Z_IT
      Z_IT='HERE'
      CALL FIND(Z_IT, RESULT, HERE)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='THERE'
      CALL FIND(Z_IT, RESULT, THERE)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='STATUS'
      CALL FIND(Z_IT, RESULT, STATUS)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='ARG1'
      CALL FIND(Z_IT, RESULT, ARG1)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='ARG2'
      CALL FIND(Z_IT, RESULT, ARG2)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='NOBJ'
      CALL FIND(Z_IT, RESULT, NOBJ)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='NPLACE'
      CALL FIND(Z_IT, RESULT, NPLACE)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='NREP'
      CALL FIND(Z_IT, RESULT, NREP)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='NINIT'
      CALL FIND(Z_IT, RESULT, NINIT)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='NVARS'
      CALL FIND(Z_IT, RESULT, NVARS)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='EXPLORE'
      CALL FIND(Z_IT, RESULT, EXPLORE)
      IF (RESULT .NE. 0) GOTO 9999
      Z_IT='SAY'
      CALL FIND(Z_IT, RESULT, SAYXX)
      IF (RESULT .NE. 0) GOTO 9999

      IF (NOBJ .GT. OBJECTS) STOP 'Too many objects'
      IF (NPLACE .GT. PLACES) STOP 'Too many places'
      IF (NVARS .GT. VARS) STOP 'Too many vars'
      CHECKSUM = MOD(NVARS*17**2 + NOBJ*17 + NPLACE*1, 10000)

C     DEFINE "STATUS" BITS

      MOVED    = BITVAL(0)
      BRIEF    = BITVAL(1)
      FAST     = BITVAL(2)
      LOOKING  = BITVAL(3)
      PLSCLRFY = BITVAL(4)
      JUGGLED  = BITVAL(5)
      BEEN     = BITVAL(1)
      DUAL     = BITVAL(3)
      XOBJECT  = BITVAL(15)
      XVERB    = BITVAL(14)
      XPLACE   = BITVAL(13)
      BADWORD  = BITVAL(12)

      DO 50 I=0,OBJECTS
      OBJVAL(I) = 0
      OBJLOC(I) = 0
50    OBJBIT(I) = XOBJECT

      DO 60 I=0,VARS
      VARVAL(I) = 0
60    VARBIT(I) = XVERB

      DO 70 I=0,PLACES
70    PLACEBIT(I) = XPLACE

      RETURN

9999  PRINT '('' Missing variable: '',A)', Z_BADVAR
      STOP
      END
C*
C*******************************************************************
C*
      SUBROUTINE SPECIAL
      INCLUDE 'exec.i'
      COMMON /COMTEMP/ COMTEMP
      CHARACTER*24 Z_KEY
      INTEGER*4 EVAL
      INTEGER*2 NAMELEN
      SAVE Z_KEY
C*
      MAXREC = 13 + 3*NOBJ + 2*NVARS + NPLACE
      IF (WORD1.GT.999) WORD1=EVAL(WORD1)
      LINELEN = EVAL(STATUS)
8     GOTO (100,200,300,400,500,600,700,800,900),WORD1
      PRINT '('' Glitch! Bad EXEC code: '',I4,'' on rec '',F10.3,'' afte
     +r loc '',I6)', WORD1, FLOAT(REC) / 1000, BP
      RETURN
C
C     EXEC 1 - dump current state of game to disc.
C
100   IF (LINELEN.GT.1) THEN
         Z_KEY=Z_ARG(2)
      ELSE
         PRINT '('' Name to save under: '', $)'
         READ (INUNIT,'(A12)')Z_KEY
      END IF
      NAMELEN=INDEX(Z_KEY, ' ')-1
      IF (NAMELEN.LT.0.OR.NAMELEN.GT.12) NAMELEN=12
      OPEN (UNIT=FREEZER, FILE=Z_KEY(1:NAMELEN), RECL=MAXREC,
     +  STATUS='UNKNOWN', ERR=150, FORM='UNFORMATTED')
      WRITE (UNIT=FREEZER) NOBJ,NVARS,NPLACE
      WRITE (UNIT=FREEZER)
     +     (OBJLOC(I), OBJVAL(I), OBJBIT(I), I=0, NOBJ),
     +     (VARVAL(I), VARBIT(I), I=0, NVARS),
     +     (PLACEBIT(I), I=0, NPLACE)
130   CALL SETVAL(WORD2, 0)
140   CLOSE (UNIT=FREEZER, STATUS='KEEP')
      RETURN
150   CONTINUE

9999  CALL SETVAL(WORD2, 1)
      RETURN
C
C     EXEC 2 - restore dumped game.
C
200   IF (LINELEN.GT.1) THEN
         Z_KEY=Z_ARG(2)
      ELSE
         PRINT '('' Name of saved game: '', $)'
         READ (INUNIT,'(A12)')Z_KEY
      END IF
      NAMELEN=INDEX(Z_KEY, ' ')-1
      IF (NAMELEN.LT.0.OR.NAMELEN.GT.12) NAMELEN=12
      OPEN (UNIT=FREEZER, FILE=Z_KEY(1:NAMELEN), RECL=MAXREC,
     +  STATUS='OLD', FORM='UNFORMATTED', ERR=220)
      READ (UNIT=FREEZER) ROBJ,RVARS,RPLACE
      IF (NOBJ.NE.ROBJ.OR.NVARS.NE.RVARS.OR.NPLACE.NE.RPLACE) GOTO 210
      READ (UNIT=FREEZER)
     +     (OBJLOC(I), OBJVAL(I), OBJBIT(I), I=0, NOBJ),
     +     (VARVAL(I), VARBIT(I), I=0, NVARS),
     +     (PLACEBIT(I), I=0, NPLACE)
      GOTO 130
210   CALL SETVAL(WORD2, 2)
      GOTO 140
220   CALL SETVAL(WORD2, 1)
      GOTO 140
C
C     EXEC 3 - delete dumped game. (This only happens after a restore,
C              so Z_FULLKEY has a well defined value).
C
300   OPEN (UNIT=FREEZER, FILE=Z_KEY, RECL=MAXREC,
     +  STATUS='OLD', FORM='UNFORMATTED', ERR=9999)
      CLOSE (UNIT=FREEZER,STATUS='DELETE')
      CALL SETVAL(WORD2, 0)
      RETURN
C
C     EXEC 4 - display news.
C
400   CALL NEWS
      RETURN
C
C     EXEC 5 - check if starting in free time.
C
500   CALL HOURS(0, I)
      CALL SETVAL(WORD2, I)
      RETURN
C
C     EXEC 6 - print hours. As supplied, the hours routine says
C              'open at all times'.
C
600   CALL HOURS(-1, I)
      CALL SETVAL(WORD2, 0)
      RETURN
C
C     EXEC 7 - save a variable.
C
700   COMTEMP = EVAL(WORD2)
      RETURN
C
C     EXEC 8 - restore saved variable.
C
800   CALL SETVAL(WORD2, COMTEMP)
      RETURN
C
C     EXEC 9 - check demo games allowed.
C
900   CALL HOURS (1, I)
      CALL SETVAL(WORD2, I)
      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE OPENDB
      INCLUDE 'exec.i'
      CHARACTER*10 Z_VER
      INTEGER*4 POINT,I4
      INTEGER*2 LEN
C*
      OPEN(UNIT=DBI,STATUS='OLD',FILE='ADVINIT1.DAT',
     +     FORM='UNFORMATTED')
      POINT=1
10    CALL RECREAD(DBI,BUFFER,BUFFSIZE,POINT,CODE)
      IF (CODE.EQ.0) GOTO 10
      CLOSE (UNIT=DBI)
C*
      OPEN(UNIT=DBT,STATUS='OLD',FILE='ADVINIT2.DAT',
     +     ACCESS='DIRECT',RECL=120)
C*
      OPEN(UNIT=DBI,STATUS='OLD',FILE='ADVINIT3.DAT',
     +     FORM='UNFORMATTED')

      READ(DBI)NUMKEYS
      READ(DBI)(EKEYSBF(I),I=1,2*NUMKEYS)
      READ(DBI,ERR=50,END=50)I4
      PRINT '('' Keys buffer too small!'')'
      CLOSE(DBI)
      STOP
C*
50    CLOSE(UNIT=DBI)
C*
      OPEN(UNIT=DBI,STATUS='OLD',FILE='ADVINIT4.DAT',
     +     FORM='UNFORMATTED')
      READ(DBI)SYMCNT
      READ(DBI)(Z_WORD(I),I=1,SYMCNT)
      READ(DBI)(FILEKEY(I),I=1,SYMCNT)
      READ(DBI,ERR=60,END=60)I4
      PRINT '('' Vocabulary buffers too small'')'
      CLOSE(UNIT=DBI)
      STOP
C*
60    CLOSE(UNIT=DBI)
      IF (CODE.NE.0) RETURN
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE RECREAD(UNIT,BUFFER,BUFFSIZE,POINT,CODE)
C*
      INTEGER   UNIT
      INTEGER   BUFFSIZE
      INTEGER*2 BUFFER(BUFFSIZE)
      INTEGER   POINT
      INTEGER   CODE
      INTEGER*2 LEN
C*
      READ(UNIT,ERR=60,END=60)LEN
      IF (LEN.GT.0) GOTO 70
60    CODE=1
      RETURN
70    IF (POINT+LEN.GT.BUFFSIZE) GOTO 90
      READ(UNIT,END=90,ERR=90)(BUFFER(I),I=POINT,POINT+LEN-1)
      POINT=POINT+LEN
      CODE=0
      RETURN
C*
80    PRINT '('' Buffer too small!'')'
      CLOSE(UNIT)
      STOP
C*
90    PRINT '('' Unexpected end of file!'')'
      CLOSE(UNIT)
      STOP
      END
C*
C*********************************************************************
C*
      INTEGER*4 FUNCTION NLEN(Z_STRN,MAXLEN)
C*
      CHARACTER*(*) Z_STRN
      INTEGER*4     MAXLEN
C*
      NLEN=0
      BOTTOM=1
      TOP=MAXLEN
      IF (TOP.LE.0) RETURN
      J=MAXLEN
      DO 5 I=1,MAXLEN
         IF (Z_STRN(J:J) .NE. ' ') GOTO 6
         J=J-1
5     CONTINUE
6     NLEN=J
      RETURN        
      END
C*
C*********************************************************************
C*
      SUBROUTINE APPEND(Z_STRING)
C*
      INCLUDE 'exec.i'
C*
      CHARACTER*140 Z_STRING
      INTEGER*2 LENGTH
C*
      IF (CHARPOS .GT. 140) RETURN
      LENGTH = NLEN(Z_STRING, 140)
      IF (CHARPOS + LENGTH .GT. 141) LENGTH = 141 - CHARPOS
      Z_OUTLINE(CHARPOS:) = Z_STRING(1:LENGTH)
      CHARPOS = CHARPOS + LENGTH
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE WRITETEXT
C*
      INCLUDE 'exec.i'
C*
      PRINT '(1X,A79)', Z_OUTLINE
      CHARPOS = 1
      Z_OUTLINE = ' '
      RETURN
      END
C*
C*
C*********************************************************************
C*
C*    The following routines are supplied as working (and workable!)
C*    'stubs' for portability. To make them useful, insert machine
C*     or installation specific code, as appropriate.
C*
C*******************************************************************
C*
      SUBROUTINE HOURS(KEY, CODE)
C*
      INTEGER KEY
      INTEGER CODE
C*
      IF (KEY.GE.0) GOTO 10
C*
C****
C*   Print 'free' hours as appropriate.
C****
C*
      PRINT '(''                  Open at all times.'')'
      PRINT '('' '')'
      CODE=0
      RETURN
C*
C****
C*    Check it is Ok to start a game. Return CODE as 0 if yes, 1 if no.
C***
C*
10    IF (KEY.NE.0) GOTO 20
      CODE=0
      RETURN
C*
C****
C*    Is it Ok to start a demo game? Return CODE as 0 if yes, 1 if no.
C****
C*
20    CODE=0
      RETURN
      END
C*
C*******************************************************************
C*
      SUBROUTINE NEWS
C*
C*******
C*    Replace with machine specific code as required. I suggest
C*    listing a specific file, which can be easily edited by the
C*    ADVENTURE ADMINISTRATOR, but not players.
C*
C*
C*******
C*
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE SVAR(WORD1,WORD2)
C*
      INTEGER*4 WORD1
      INTEGER*4 WORD2
C*
C******
C*    Dummy for the SVAR directive. SVAR is called with a key (word1)
C*    Which indicates the system action required, and a variable (word2)
C*    In which a value is returned. At least two key values should be 
C*    catered for: KEY of 4 requests the return of the number of hours
C*    since midnight; KEY of 5 requests the return of minutes since the
C*    last hour. Since the implementation is system dependent, only
C*    this null stub is provided. The game is playable with this stub,
C*    but if the A-code source mandates a delay in restoring a saved
C*    game, the delay will not be enforced.
C****
C*
      RETURN
      END
C*
C*******************************************************************
