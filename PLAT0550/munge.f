C*    Dave Platt's A-code munger.
C*    Converted to Prime F77 by Anon, on an unknown date.
C*    Optimised for Prime as version 09.02 29 Mar 1989, by Mike Arnautov.
C*    Removed Prime optimisation and converted for use with g77,
C*                                       Mike Arnautov, 19 Jan 2003.
C*
C*    All 'C*' comments by added were added by me.
C*                                       Mike Arnautov, 21 Jan 2003.
C
      INCLUDE 'munge.i'
C
      INTEGER*2 PRIMARY(0:TABLESIZE)
      INTEGER KEYSBF*4, NUMKEYS*2
      INTEGER*4 EKEYSBF(1)
      EQUIVALENCE (KEYSBF, EKEYSBF)
      COMMON /KEYSCM/ KEYSBF(2, KEYSBFSIZE), NUMKEYS
      COMMON /PRMARY/ PRIMARY
      CHARACTER*11 Z_DB, Z_FILENAME
      CHARACTER*8 Z_COMMAND(MAJOR)
      DIMENSION CLASS(NOPTS)
      INTEGER*2 J
      DATA Z_COMMAND/'TEXT', 'OBJECT',
     + 'PLACE', 'VERB', 'INITIAL', 'LABEL', 'REPEAT', 'AT',
     + 'ACTION', 'SYNONYM', 'VARIABLE', 'NULLWORD', 'LIST',
     + 'NOLIST', 'DEFINE', 'INCLUDE', 'XREF'/
      DATA CLASS/ -1, -1, -1, -1, -1, 2, 2, 2, 1, 1, 0, 0, 0,
     + 1, 1, 2, 2, 2, 2, 1, 2, 1, 1, 2, 2, 0, 0, 0,
     + 1, 1, 0, 2, 2, 2, 2, 1, 1, 0, 2, 0, 2, 0, 1, 2, 2,
     + 2, 2, 2, 2, 1, 0, 0, 2, 1, 3, 1/

      INSTFILE=10
      TEXTFILE=11
      INMAIN=12
      ININCLUDE=13
      OUTFILE=14
      OPEN (UNIT=TEXTFILE,STATUS='OLD',FILE='ADVINIT2.DAT',ERR=5)
      CLOSE (UNIT=TEXTFILE,STATUS='DELETE')
5     OPEN (UNIT=TEXTFILE,STATUS='UNKNOWN',FILE='ADVINIT2.DAT',
     +      ACCESS='DIRECT',RECL=120)
      OPEN (UNIT=INSTFILE,STATUS='UNKNOWN',FILE='ADVINIT1.DAT',
     +      FORM='UNFORMATTED')
      OPEN (UNIT=INMAIN  ,STATUS='OLD',FILE='ADVENTURE.ACODE')
      NUMKEYS = 0
      CALL INITIAL
      DO 10 I=0,TABLESIZE
10    PRIMARY(I)=1
C
C     INITIAL LOOP
C
100   ASSIGN 200 TO CONTINUE
      ASSIGN 400 TO NEW
C
C     GET A LINE - GO WHEREVER WE SHOULD
C
200   CALL READIN(CODE)
      IF (CODE.NE.0) GOTO NEW
      GOTO CONTINUE
C
C
400   IF (LINEX.GT.0) GOTO 405
      IF (INUNIT.EQ.INMAIN) GOTO 9000
      INUNIT=INMAIN
      CLOSE(UNIT=ININCLUDE)
      Z_FID = ' '
      OUTKEY = 0
      GOTO 200
405   IF (LINEX .LT. 0) GOTO 9000
      ASSIGN 200 TO CONTINUE
      ASSIGN 400 TO NEW
      IF (PARSE(0).NE.0) GOTO 200
      DO 410 CODE=1,MAJOR
410   IF (Z_LEX(1:3) .EQ. Z_COMMAND(CODE)(1:3)) GOTO 420
      IF (FIND(Z_LEX) .GE. 0) GOTO 2905
415   CALL SNAPIT
      PRINT '('' --Bad command: '',A100)', Z_LEX
      TROUBLE = 1
      GOTO 100
C
C
420   GOTO (2000,2100,2200,2300,2400,2500,
     + 2600,2800,2900,3500,2700,3600,3700,3800,3900,4000,4200),CODE
C
C
1000  ASSIGN 1300 TO CONTINUE
      OUTKEY = FLOAT(KEY) / 1000
      IF (PARSE(0).NE.0) GOTO 200
      SHORTKEY = KEY / 1000
      CALL DEFINE(Z_LEX, SHORTKEY)
      SMELCH=FIND(Z_LEX)
      IF (Z_SEP .NE. ',') GOTO 200
1300  IF (LINEX .GE. LINEND) GOTO 1350
      IF (Z_LINE(LINEX:LINEX) .EQ. '/') GOTO 1320
      IF (Z_LINE(LINEX:LINEX) .NE. '%') GOTO 1350
      KEY = 10 * (1 + KEY / 10)
1320  LINEX = LINEX + 1
      IF (LINEND .LT. LINEX) LINEND = LINEX
1350  CONTINUE
      CALL KEYWRITE(KEY, Z_LINE(LINEX:LINEND))
      AUXVAL(SMELCH)=MOD(KEY,500)/10
C
C     For objects, AUXVAL will be set equal to the number of states
C     that the object has defined (not including in-hand status) and
C     may be fetched via the @name construct.
C
      IF (LIST .NE. 0) CALL SNAPIT
      KEY = KEY + 1
      GOTO 200
C
C TEXT
C
2000  CALL SETUP(KEY, NEXTTEXT)
      GOTO 1000
C
C OBJECT
C
2100  CALL SETUP(KEY, NEXTOBJECT)
      GOTO 1000
C
C PLACE
C
2200  CALL SETUP(KEY, NEXTPLACE)
      GOTO 1000
C
C VERB
C
2300  FIRST = 1
      CALL SETUP(KEY, NEXTVERB)
2310  IF (PARSE(0).NE.0) GOTO 100
      SHORTKEY = KEY / 1000
      CALL DEFINE(Z_LEX, SHORTKEY)
      IF (FIRST .NE. 0) GOTO 2320
      I = FIND(Z_LEX)
      Z_UNREF(I)=' '
      PRIMARY(I)=0
2320  FIRST = 0
      GOTO 2310
C
C INITIAL
C
2400  BP = 1
      CALL SETUP(KEY, NEXTINIT)
      GOTO 2950
C
C LABEL
C
2500  IF (PARSE(0).NE.0) GOTO 415
      CALL DEFINE(Z_LEX, NEXTLABEL)
      BP = 1
      CALL SETUP(KEY, NEXTLABEL)
      GOTO 2950
C
C REPEAT
C
2600  BP = 1
      CALL SETUP(KEY, NEXTREPEAT)
      GOTO 2950
C
C VARIABLE
C
2700  IF (PARSE(0).NE.0) GOTO 100
      CALL DEFINE(Z_LEX, NEXTVAR)
      NEXTVAR = NEXTVAR + 1
      GOTO 2700
C
C AT
C
2800  IF (PARSE(0).NE.0) GOTO 415
      WHERE = FIND(Z_LEX)
      IF (WHERE .LT. 0) GOTO 2802
      KWHERE = KEYS(WHERE)
      IF (2000 .LE. KWHERE .AND. KWHERE .LT. 3000) GOTO 2810
2802  CALL SNAPIT
      PRINT '('' --That is no place: '',A100)', Z_LEX
      TROUBLE = 1
      GOTO 100
2810  IF (AUXVAL(WHERE) .LT. 500) AUXVAL(WHERE) = 500
      GOTO 2920
C
C ACTION
C
2900  IF (PARSE(0).NE.0) GOTO 415
2905  WHERE = FIND(Z_LEX)
      IF (WHERE .GE. 0) GOTO 2910
      CALL DEFINE(Z_LEX, NEXTVERB)
      CALL SNAPIT
      PRINT '('' >> Verb defined by default <<'')'
      NEXTVERB = NEXTVERB + 1
      GOTO 2905
2910  KWHERE = KEYS(WHERE)
      IF (3000 .LE. KWHERE .AND. KWHERE .LT. 4000) GOTO 2925
      IF (1000 .LE. KWHERE .AND. KWHERE .LT. 2000) GOTO 2920
      CALL SNAPIT
      PRINT '('' --That is not a verb or object: '',A100)', Z_LEX
      TROUBLE = 1
      GOTO 100
2920  AUXVAL(WHERE) = MAX(AUXVAL(WHERE), 500)
2925  KEY = LONGTHOUSAND * KEYS(WHERE) + AUXVAL(WHERE)
      OUTKEY = FLOAT(KEY)/1000
      AUXVAL(WHERE) = AUXVAL(WHERE) + 1
      BP = 1
2940  CONTINUE
      IF (PARSE(0).NE.0) GOTO 2950
      WHERE = EVAL(Z_LEX)
2930  BUFFER(BP) = 1
      BUFFER(BP + 1) = WHERE
      BP = BP + 2
      GOTO 2940
C*
2950  ASSIGN 3000 TO CONTINUE
      ASSIGN 3100 TO NEW
      OUTKEY = FLOAT(KEY)/1000
      IF (LIST .NE. 0) CALL SNAPIT
      GOTO 200
3000  IF (PARSE(0).NE.0) GOTO 200
3005  LOW = 1
      HIGH = NOPTS
      IF (Z_LEX(1:4) .EQ. 'ITLI') Z_LEX = 'ITOBJ'
3006  ISAM = (LOW + HIGH) / 2
      IF (Z_LEX(1:4) .EQ. Z_OPT(ISAM)) GOTO 3020
      IF (LOW .GE. HIGH) GOTO 3015
      IF (Z_LEX(1:4) .GT. Z_OPT(ISAM)) GOTO 3007
      HIGH = ISAM - 1
      GOTO 3006
3007  LOW = ISAM + 1
      GOTO 3006
3015  CALL SNAPIT
      PRINT '('' --Bad option: '',A100)', Z_LEX
      TROUBLE = 1
      GOTO 200
3016  CALL SNAPIT
      PRINT '('' --Missing required parameter: '',A100)', Z_LEX
      TROUBLE = 1
      GOTO 200
3020  OPT = OPTVAL(ISAM)
      IF (CLASS(OPT) .GE. 0) GOTO 3050
3030  CONTINUE
      IF (PARSE(0).NE.0) GOTO 200
      WHERE = EVAL(Z_LEX)
      BUFFER(BP) = OPT
      BUFFER(BP + 1) = WHERE
      BP = BP + 2
      GOTO 3030
3050  BUFFER(BP) = OPT
      DO 3060 I=1,CLASS(OPT)
      IF (PARSE(0).NE.0) GOTO 3016
      BUFFER(BP + I) = EVAL(Z_LEX)
3060  CONTINUE
      BP = BP + 1 + CLASS(OPT)
      GOTO 200
3100  IF (BP.EQ.0) GOTO 400
      BP = BP - 1
      CALL BUFWRITE(BP)
      BP = 0
      GOTO 400
C
C SYNONYM
C
3500  IF (PARSE(0).NE.0) GOTO 415
      VALUE = EVAL(Z_LEX)
3510  CONTINUE
      IF (PARSE(0).NE.0) GOTO 100
      CALL DEFINE(Z_LEX, VALUE)
      I = FIND(Z_LEX)
      Z_UNREF(I)=' '
      PRIMARY(I)=0
      GOTO 3510
C
C NULLWORD
C
3600  IF (PARSE(0).NE.0) GOTO 100
      CALL DEFINE(Z_LEX, NEXTNULL)
      GOTO 3600
C
C LIST
C
3700  LIST = 1
      GOTO 100
C
C NOLIST
C
3800  LIST = 0
      GOTO 100
C
C DEFINE
C
3900  IF (PARSE(0).NE.0) GOTO 100
      I = FIND(Z_LEX)
      IF (I .GE. 0) GOTO 3910
      CALL SNAPIT
      PRINT '('' --Undefined symbol: '',A100)', Z_LEX
      GOTO 3900
3910  REFIT(I) = 1
      GOTO 3900
C
C INCLUDE
C
4000  IF (INUNIT.NE.ININCLUDE) GOTO 4010
      CALL SNAPIT
      PRINT '('' INCLUDEs may not be nested.'')'
      GOTO 100
4010  IF (PARSE(0).NE.0) GOTO 415
      Z_FID = Z_LEX
      OPEN (UNIT=ININCLUDE,FILE=Z_FID,ERR=4100,ACCESS='SEQUENTIAL',
     +      STATUS='UNKNOWN')
      INUNIT=ININCLUDE
      FNUM=FNUM+1
      GOTO 100
4100  CALL SNAPIT
      PRINT '('' Unable to open INCLUDE file: '',A100)', Z_LEX
      GOTO 100
C
C XREF
C
4200  XREF=1
      GOTO 100
C
C
9000  CONTINUE
      REFDEF(0)=1
      CALL DEFINE('NOBJ', MOD(NEXTOBJECT, 1000))
      CALL DEFINE('NPLACE', MOD(NEXTPLACE, 1000))
      CALL DEFINE('NREP', MOD(NEXTREPEAT, 1000))
      CALL DEFINE('NINIT', MOD(NEXTINIT, 1000))
      CALL DEFINE('NVARS', MOD(NEXTVAR, 1000))
      ADDON = MOD(NEXTOBJECT, 1000) + MOD(NEXTPLACE, 1000) +
     +        MOD(NEXTVERB, 1000) + MOD(NEXTLABEL, 1000)
      CACHEHW = CACHEHW + ADDON
      CACHEDW = CACHEDW + ADDON
      SYMCNT = -1
      DO 9005 I=0, TABLESIZE - 1
      IF (Z_NAME(I) .EQ. ' ') GOTO 9005
      SYMCNT = SYMCNT + 1
      IF (PRIMARY(I).NE.0.AND.KEYS(I).GT.0) KEYS(I) = -KEYS(I)
      Z_NAME(SYMCNT) = Z_NAME(I)
      KEYS(SYMCNT) = KEYS(I)
      Z_UNREF(SYMCNT) = Z_UNREF(I)
      REFIT(SYMCNT) = REFIT(I)
9005  CONTINUE
      CALL SORT
      I = -1
      DO 9002 J=0, SYMCNT
      IF (REFIT(J) .EQ. 0) GOTO 9002
      I = I + 1
      IF (I .EQ. J) GOTO 9002
      Z_NAME(I) = Z_NAME(J)
      KEYS(I) = KEYS(J)
      Z_UNREF(I) = Z_UNREF(J)
9002  CONTINUE
      SYMCNT = I

      CLOSE (UNIT=INSTFILE)
      OPEN  (UNIT=INSTFILE, STATUS='UNKNOWN',FILE='ADVINIT4.DAT',
     +       FORM='UNFORMATTED')
      J = SYMCNT + 1
      WRITE(INSTFILE) J
      WRITE(INSTFILE) (Z_NAME(I),I=0,SYMCNT)
      WRITE(INSTFILE) (KEYS(I),I=0,SYMCNT)
      PRINT '(''Maximum buffer length     '',I5)', MAXBUFF+1
      PRINT '(''Vocabulary size           '',I5)', SYMCNT+1
      PRINT '(''Directory entries         '',I5)', CACHEDW
      PRINT '(''Data entries              '',I5)', CACHEHW

      CLOSE (UNIT=TEXTFILE)
      CLOSE (UNIT=INSTFILE)

      CALL KEYSORT(KEYSBF, NUMKEYS, 4, 2)

      OPEN(UNIT=OUTFILE, STATUS='UNKNOWN', FILE='ADVINIT3.DAT',
     +     FORM='UNFORMATTED')
      WRITE(OUTFILE)NUMKEYS
      WRITE(OUTFILE)(EKEYSBF(I),I=1,2*NUMKEYS)
      CLOSE(UNIT=OUTFILE)
      CLOSE(UNIT=INMAIN)
      IF (TROUBLE.EQ.0) GOTO 9500
      PRINT '('' *** Trouble ***'')'
      STOP
C*
9500  STOP
      END
C*
C*********************************************************************
C*
      SUBROUTINE BUFWRITE(LEN)
      INCLUDE 'munge.i'
      INTEGER KEYSBF*4, BUFPOS*4, NUMKEYS*2
      INTEGER*2 WORDS
      SAVE BUFPOS
      COMMON /KEYSCM/ KEYSBF(2, KEYSBFSIZE), NUMKEYS
      DATA BUFPOS/1/
      IF (LEN .GE. BUFFSIZE) GOTO 100
      WORDS = LEN + 1

10    BUFFER(WORDS) = -999
      WRITE(INSTFILE)WORDS
      WRITE(INSTFILE)(BUFFER(I),I=1,WORDS)
      NUMKEYS = NUMKEYS + 1
      IF (NUMKEYS .GT. KEYSBFSIZE) STOP 'Keysbf too small'
      KEYSBF(1, NUMKEYS) = KEY
      KEYSBF(2, NUMKEYS) = BUFPOS
      BUFPOS = BUFPOS + LEN + 1
      OUTKEY = FLOAT(KEY) / 1000
      KEY = KEY + 1
      MAXBUFF = MAX(MAXBUFF, LEN)
      IF (KEY/1000000 .GT. 0) CACHEHW = CACHEHW + LEN + 1
      IF (KEY/1000000 .GT. 0) CACHEDW = CACHEDW + 1
      RETURN

100   CALL SNAPIT
      PRINT '('' -- Maximum buffer size exceeded!!'')'
      TROUBLE = 1
      GOTO 10

      END
C*
C*********************************************************************
C*
      SUBROUTINE DEFINE(Z_TEXT, VAL)
      INCLUDE 'munge.i'
      CHARACTER*(*) Z_TEXT
      INTEGER*4 VAL
      Z_TEXAN = Z_TEXT
      I = MASH(Z_TEXAN, 12)
      REHASH = REHASHVALUES(MOD(I, REHASHN))
      I = MOD(I, TABLESIZE)
      IF (XREF .EQ. 0) GOTO 10
      WRITE (99, '(A12,1X,I2,1X,A9,1X,F9.3)') Z_TEXAN,FNUM,Z_FID,
     + FLOAT(NREC)/1000
10    DO 100, J=1, TABLESIZE
      IF (Z_NAME(I) .EQ. Z_TEXAN) GOTO 900
      IF (Z_NAME(I) .EQ. ' ') GOTO 200
      I = MOD(I + REHASH, TABLESIZE)
100   CONTINUE
      STOP 'Symbol table overflow - recompile'
200   Z_NAME(I) = Z_TEXT
      KEYS(I) = VAL
      AUXVAL(I) = 0
      REFIT(I) = REFDEF(VAL / 1000)
      Z_UNREF(I) = '*'
      RETURN
900   CALL SNAPIT
      PRINT '('' -- Duplicate symbol'')'
      TROUBLE = 1
      RETURN
      END
C*
C*********************************************************************
C*
      INTEGER*4 FUNCTION EVAL(Z_TEXT)
      INCLUDE 'munge.i'
      CHARACTER*(*) Z_TEXT
      CHARACTER*10 Z_DIGITS
      INTEGER*2 BITPOS(2,16)
      INTEGER*4 LBITPS(16)
      EQUIVALENCE (BITPOS,LBITPS)
      DATA LBITPS/1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,
     +     16384, 32768/
      DATA Z_DIGITS/'0123456789'/
      EVAL = 0
      START = 1
      BITPHRASE = 0
      SIGN = 1
      IF (Z_TEXT(1:1) .NE. '&') GOTO 5
      BITPHRASE = 1
      START = 2
5     IF (START .GE. LEN(Z_TEXT)) GOTO 1001
      NEXTSIGN = 1
      TERM = 0
      I = START
      IF (Z_TEXT(I:I) .EQ. '-') GOTO 200
      DO 10 I=START,LEN(Z_TEXT)
10    IF (Z_TEXT(I:I) .EQ. '+' .OR. Z_TEXT(I:I) .EQ. '-') GOTO 20
      I = LEN(Z_TEXT) + 1
20    TERM = FIND(Z_TEXT(START:I-1))
      IF (TERM .LT. 0) GOTO 50
      Z_UNREF(TERM) = ' '
      IF (Z_TEXT(START:START) .EQ. '@') GOTO 25
      TERM = KEYS(TERM)
      GOTO 200
25    TERM = AUXVAL(TERM)
      GOTO 200
50    TERM = 0
      IF (Z_DIGITS(1:1) .GT. Z_TEXT(START:START) .OR.
     +    Z_DIGITS(10:10) .LT. Z_TEXT(START:START)) GOTO 100
      DO 70 J=START, I - 1
      IF (Z_TEXT(J:J) .EQ. ' ') GOTO 200
      DO 60 K=1, 10
60    IF (Z_TEXT(J:J) .EQ. Z_DIGITS(K:K)) GOTO 65
      CALL SNAPIT
      PRINT '('' --Bad number: '',A,''--'')', Z_TEXT(START:I-1)
      TROUBLE = 1
      TERM = 0
      GOTO 200
65    TERM = 10 * TERM + K - 1
70    CONTINUE
      GOTO 200
100   CALL SNAPIT
      PRINT '('' --Undefined symbol: '',A,''--'')', Z_TEXT(START:I-1)
      TROUBLE = 1
200   START = I + 1
      IF (I .GT. LEN(Z_TEXT)) GOTO 205
      IF (Z_TEXT(I:I) .EQ. '-') NEXTSIGN = -1
205   CONTINUE
      IF (BITPHRASE .NE. 0) TERM = BITPOS(2,TERM+1)
      EVAL = EVAL + TERM * SIGN
      SIGN = NEXTSIGN
999   GOTO 5
1001  RETURN
      END
C*
C*********************************************************************
C*
      INTEGER*4 FUNCTION FIND(Z_TEXT)
      INCLUDE 'munge.i'
      CHARACTER*(*) Z_TEXT
      IF (Z_TEXT(1:1) .EQ. '@') GOTO 6
      Z_TEXAN = Z_TEXT
      GOTO 7
6     Z_TEXAN = Z_TEXT (2:)
7     FIND = MASH(Z_TEXAN, LEN(Z_TEXAN))
      REHASH = REHASHVALUES(MOD(FIND, REHASHSIZE))
      FIND = MOD(FIND, TABLESIZE)
      IF (XREF .EQ. 0) GOTO 10
      WRITE (99, '(A12,1X,I2,1X,A9,1X,F9.3)') Z_TEXAN,FNUM,Z_FID,
     + FLOAT(NREC)/1000
10    IF (Z_NAME(FIND) .EQ. ' ') GOTO 99
      IF (Z_NAME(FIND) .EQ. Z_TEXAN) RETURN
      FIND = MOD(FIND + REHASH, TABLESIZE)
      GOTO 10
99    FIND = -1
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE HEAPIFY(HEAD, TOP)
      INCLUDE 'munge.i'
      INTEGER*4 HEAD, TOP
      I = HEAD
101   IF (I .GT. TOP / 2) GOTO 102
      K = I
      DO 10, J = 2*I, MIN(2*I+1, TOP)
      IF (Z_NAME(K) .LT. Z_NAME(J)) K = J
10    CONTINUE
      IF (I .EQ. K) RETURN
      CALL SWAP(I, K)
      I = K
100   GOTO 101
102   RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE INITIAL
      INCLUDE 'munge.i'
      CHARACTER*10 Z_OPTIONS(NOPTS)
      DATA  Z_OPTIONS/'KEYWORD', 'HAVE', 'NEAR', 'AT', 'ANYOF',
     + 'IFEQ', 'IFLT', 'IFGT', 'IFAT', 'CHANCE', 'ELSE', 'FIN',
     + 'EOF', 'GET', 'DROP', 'APPORT', 'SET', 'ADD', 'SUB',
     + 'GOTO', 'MOVE', 'CALL', 'SAY',
     + 'NAME', 'VALUE', 'PROCEED', 'QUIT', 'STOP',
     + 'IFHAVE', 'IFNEAR', 'OR', 'RANDOM', 'BIT', 'BIS',
     + 'BIC', 'ITOBJECT', 'ITPLACE', 'EOI', 'IFLOC',
     + 'INPUT', 'LOCATE', 'NOT', 'IFKEY', 'LDA', 'EVAL',
     + 'MULTIPLY', 'DIVIDE', 'SVARIABLE', 'EXECUTIVE',
     + 'QUERY', 'AND', 'EOR', 'DEPOSIT', 'ITERATE', 'SMOVE',
     + 'DEFAULT'/
      NEXTINIT = 0
      NEXTOBJECT = 1000
      NEXTPLACE = 2000
      NEXTVERB = 3000
      NEXTTEXT = 4000
      NEXTLABEL = 5000
      NEXTREPEAT = 6000
      NEXTVAR = 7000
      NEXTNULL = 8000
      TEXTPOS = 1
      Z_TAB = CHAR(9)
      INUNIT = INMAIN
      MAXBUFF = 0
      TROUBLE = 0
      LIST = 0
      BREAK = 0
      NREC = 0
      CACHEHW = 0
      CACHEDW = 0
      XREF = 0
      FNUM = 0
      DO 5 I = 0, NCLASS
         REFDEF(I) = 0
5     CONTINUE

      REFDEF(1) = 1
      REFDEF(3) = 1
      REFDEF(8) = 1
      DO 10 I = 1, NOPTS
         Z_OPT(I) = Z_OPTIONS(I)
         OPTVAL(I) = I
10    CONTINUE

      DO 20 I = 1, NOPTS-1
         K = I
         DO 15 J = I + 1, NOPTS
            IF (Z_OPT(J) .NE. Z_OPT(K)) GOTO 14
            PRINT 13, Z_OPTIONS(OPTVAL(J)), Z_OPTIONS(OPTVAL(K))
13          FORMAT (' Indistinguishable options: ',A,' AND ',A)
            Z_OPT(K) = 'ZZZZ'
14          IF (Z_OPT(J) .LT. Z_OPT(K)) K=J
15       CONTINUE
         IF (I .EQ. K) GOTO 20
         Z_S = Z_OPT(I)
         Z_OPT(I) = Z_OPT(K)
         Z_OPT(K) = Z_S
         J = OPTVAL(I)
         OPTVAL(I) = OPTVAL(K)
         OPTVAL(K) = J
20    CONTINUE
      Z_FID = ' '
      REHASHN = 0
      I = 50

101   IF (REHASHN.GE.REHASHSIZE) GOTO 102
      IF (RPRIME(I, TABLESIZE) .NE. 1) GOTO 100
      DO 50 J = 0, REHASHN - 1
         IF (RPRIME(I, REHASHVALUES(J)) .NE. 1) GOTO 100
50    CONTINUE

      REHASHVALUES(REHASHN) = I
      REHASHN = REHASHN + 1
100   I = I + 1
      GOTO 101

102   DO 110 I = 0, TABLESIZE
         Z_NAME(I) = ' '
110   CONTINUE
      LISTED = 1
      RETURN
      END
C*
C*********************************************************************
C*
      INTEGER FUNCTION PARSE(DUMARG)
      INCLUDE 'munge.i'
      PARSE = 1
      DO 10 I = LINEX,LINEND
         Z_CHR = Z_LINE(I:I)
         IF (Z_CHR .EQ. '*') RETURN
         IF (Z_CHR .EQ. '(' .OR. Z_CHR .EQ. '{') RETURN
         IF (Z_CHR .NE. ' ' .AND. Z_CHR .NE. Z_TAB) GOTO 15
10    CONTINUE
      RETURN

15    PARSE = 0
      DO 20 J = I,LINEND + 1
         Z_SEP = Z_LINE(J:J)
         IF (Z_SEP .EQ. ' ' .OR. Z_SEP .EQ. ',' .OR.
     +       Z_SEP .EQ. Z_TAB) GOTO 25
20    CONTINUE

      J = LINEND + 1
25    Z_LEX = Z_LINE(I:J - 1)
      LINEX = J + 1
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE READIN(CODE)
      INCLUDE 'munge.i'
      INTEGER*4 CODE
      CODE = 1
      IF (LIST .NE. 0 .OR. BREAK .NE. 0) CALL SNAPIT
      LINEX = -1
5     LISTED = 0
      READ (INUNIT, '(A140)', END=110, ERR=110) Z_LINE

      LINEND = NLEN(Z_LINE,140)+1
      DO 10 I=1, LINEND
        IF (Z_LINE(I:I) .NE. ' ') GOTO 20
10      CONTINUE

20    LINEX  = I
      NREC   = NREC + 1
      IF (Z_LINE(1:1) .EQ. '*') GOTO 200
      IF (Z_LINE(1:1) .NE. ' ') GOTO 100
      CODE = 0
      RETURN
100   OUTKEY = 0
      RETURN
110   LISTED = 1
      RETURN
200   IF (LIST .NE. 0) CALL SNAPIT
      GOTO 5
      END
C*
C*********************************************************************
C*
      INTEGER FUNCTION RPRIME(I1, I2)
      IMPLICIT INTEGER*4 (A-Z)
      N = I1
      R = I2
11    IF (R .LE. 0) GOTO 12
      M = N
      N = R
      R = MOD(M, N)
10    GOTO 11
12    RPRIME = N
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE SETUP(I, J)
      INCLUDE 'munge.i'
      INTEGER*4 I
      I = J * LONGTHOUSAND
      IF (I .LT. 0) PRINT '('' J= '',I8)',J
      OUTKEY = J
      J = J + 1
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE SNAPIT
      INCLUDE 'munge.i'
      CHARACTER*1 Z_FLAG
      BREAK = 0
      Z_FLAG = ' '
      IF (INUNIT.EQ.ININCLUDE) Z_FLAG='-'
      IF (LISTED .NE. 0) RETURN
      LISTED = 1
      PRINT '(1X,A9,1X,F5.3,1X,A1,A85,2X,F8.3)',
     + Z_FID, FLOAT(NREC)/1000,Z_FLAG,Z_LINE,OUTKEY
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE SORT
      INCLUDE 'munge.i'
      DO 10 I=SYMCNT/2, 0, -1
      CALL HEAPIFY(I, SYMCNT)
10    CONTINUE
      DO 20 I=0, SYMCNT - 1
      TOP = SYMCNT - I
      CALL SWAP(0, TOP)
      CALL HEAPIFY(0, TOP-1)
20    CONTINUE
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE SWAP(I, K)
      INCLUDE 'munge.i'
      IF (I .EQ. K) RETURN
      Z_SWAP = Z_NAME(K)
      Z_NAME(K) = Z_NAME(I)
      Z_NAME(I) = Z_SWAP
      ISWAP = KEYS(K)
      KEYS(K) = KEYS(I)
      KEYS(I) = ISWAP
      LSWAP = REFIT(K)
      REFIT(K) = REFIT(I)
      REFIT(I) = LSWAP
      Z_SWAP = Z_UNREF(K)
      Z_UNREF(K) = Z_UNREF(I)
      Z_UNREF(I) = Z_SWAP
      RETURN
      END
C*
C*********************************************************************
C*
      SUBROUTINE KEYWRITE(IOKEY, Z_TEXT)
      INCLUDE 'munge.i'
      INTEGER*4 IOKEY
      CHARACTER*120 Z_OLINE
      CHARACTER*(*) Z_TEXT
      INTEGER*2 I_LINE(60), TEXTBUFFER(60)
      EQUIVALENCE (Z_OLINE,I_LINE)
      INTEGER KEYSBF*4, NUMKEYS*2
      INTEGER*4 KEYPOS, NEXTPOS
      REAL*4 RND
      COMMON /KEYSCM/ KEYSBF(2, KEYSBFSIZE), NUMKEYS
      SAVE Z_OLINE, KEYPOS

      DATA Z_OLINE/' '/, KEYPOS/0/

      IF (Z_TEXT(1:1) .EQ. '*' .OR. Z_TEXT(1:3) .EQ. '>$<') GOTO 20
      L = LEN(Z_TEXT) / 2
      IF (L .LE. 60) GOTO 10
      CALL SNAPIT
      PRINT '('' --String too long'')'

10    IF (Z_TEXT .EQ. Z_OLINE) GOTO 19

11    Z_OLINE = Z_TEXT
      KEYPOS = KEYPOS + 1
      J = 1
      IF (L .EQ. 0) GOTO 13
      DO 12 I = 1, L, 2
         TEXTBUFFER(J) = -I_LINE(I)
         TEXTBUFFER(61 - J) = -I_LINE(I + 1)
         J = J + 1
12    CONTINUE

13    IF (J .GT. 30) GOTO 15
      TEXTBUFFER(J) = 60 - J
      IF (L .LT. 10) L = 10
      DO 14 I = J + 1, 61 - J
         K = L * RND(0) + 1
         TEXTBUFFER(I) = TEXTBUFFER(K)
14    CONTINUE

15    WRITE (TEXTFILE,REC=KEYPOS) TEXTBUFFER

19    NUMKEYS = NUMKEYS + 1
      IF (NUMKEYS .GT. KEYSBFSIZE) STOP 'Keysbf too small'
      KEYSBF(1, NUMKEYS) = IOKEY
      KEYSBF(2, NUMKEYS) = KEYPOS

20    OUTKEY = FLOAT(IOKEY) / 1000
      RETURN
      END
C*
C*********************************************************************
C*
      INTEGER FUNCTION MASH(Z_LIN, LEN)
      CHARACTER*(*) Z_LIN
      INTEGER*4 LEN
      INTEGER*4 J4
      INTEGER*2 J2(2)
      EQUIVALENCE (J4,J2)
      CHARACTER*12 Z_LOCLIN
      INTEGER*2 LINE(1)
      EQUIVALENCE (LINE, Z_LOCLIN)

      Z_LOCLIN = Z_LIN
      IS=1
      L=(LEN+1)/2
      J4=LINE(1)+LEN

10    IF (IS.EQ.L)GOTO 20
      IS=IS+1
      J4=J4+LINE(IS)
      GOTO 10

20    MASH=J2(2)
      IF (MASH.LT.0) MASH=-MASH
      RETURN

      END
C*
C*********************************************************************
C*
      INTEGER*4 FUNCTION NLEN(Z_STRN,MAXLEN)
      CHARACTER*(*) Z_STRN
      INTEGER*4     MAXLEN
C
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
      SUBROUTINE KEYSORT(KEYSBUF, NUMKEYS, ENTRY, KEY)
      INTEGER*2 NUMKEYS
      INTEGER*4 KEYSBUF(2,NUMKEYS)
      INTEGER*4 ENTRY
      INTEGER*4 KEY
      INTEGER*2 STEP
      INTEGER*2 LAST
      INTEGER*2 START,STOP
      INTEGER*2 I
      INTEGER*4 TEMP
      INTEGER*4 SOME
C*
      STEP=1
10    STEP=3*STEP
      IF (STEP.LT.NUMKEYS) GOTO 10
      STEP=STEP/6
20    SOME=0
      DO 200 J=1,STEP
         START=STEP+J
         LAST=J
         DO 100 I=START,NUMKEYS,STEP
            IF (KEYSBUF(1,I)-KEYSBUF(1,LAST)) 50,100,70
50          TEMP=KEYSBUF(1,LAST)
            KEYSBUF(1,LAST)=KEYSBUF(1,I)
            KEYSBUF(1,I)=TEMP
            TEMP=KEYSBUF(2,LAST)
            KEYSBUF(2,LAST)=KEYSBUF(2,I)
            KEYSBUF(2,I)=TEMP
            SOME=1
70          LAST=I
100      CONTINUE
200   CONTINUE
      IF (SOME.NE.0) GOTO 20
      IF (STEP.EQ.1) RETURN
      STEP=STEP/3+1
      GOTO 20
      END
C*
C*********************************************************************
C*
      REAL*4 FUNCTION RND(DUMMY)
      INTEGER*4 DUMMY
      INTEGER*4 LNGRND
      SAVE LNGRND
      DATA LNGRND/54321/
C
      LNGRND = AND(LNGRND * 8693 + 6923, 32767)
      RND = LNGRND / 32767.0
      RETURN
      END
C*
C*********************************************************************
