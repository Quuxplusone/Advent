        SUBROUTINE MOTD(ALTER)
C  HANDLES MESSAGE OF THE DAY.  IF ALTER IS TRUE, READ A NEW MESSAGE FROM THE
C  WIZARD.  ELSE PRINT THE CURRENT ONE.
C  MS30:  Message is initialized to reflect the status of this expermental
C            ADVENT
        IMPLICIT INTEGER(A-Z)
        LOGICAL ALTER
        DIMENSION MSG(500),BUF(120),BUF2(60)
        DATA MSG/500*-1/
C  MS30:  Change standard message of the day
C       DATA MSG/9,'This ','is a ','new, ','exper','iment',
C    1    'al AD','VENT!',92*-1/
      CALL DATIME(D,T)
      HOUR=T/60
      MIN=T-HOUR*60
      WRITE(1,101)HOUR,MIN
101   FORMAT(/'<SPLIT> This is ADVENT.  The time is  ',B'##',':',B'##'//)
        IF(ALTER)GOTO 50
        K=1
10      IF(MSG(K).LT.0)RETURN
        L1=K+1
        L2=MSG(K)-1
        WRITE(1,20)(MSG(I),I=L1,L2)
20      FORMAT(' ',60A2)
        K=MSG(K)
        GOTO 10
50      M=1
        CALL MSPEAK(23)
C55      L1=M+1
C        L2=M+35
C        READ(1,56)(MSG(I),I=L1,L2),K
C56      FORMAT(36A2)
C        IF(K.EQ.' ')GOTO 60
C        CALL MSPEAK(24)
C        GOTO 55
C60      DO 62 I=1,35
C        K=M+36-I
C        IF(MSG(K).NE.' ')GOTO 65
C62      CONTINUE
C        GOTO 90
C65      MSG(M)=K+1
C        M=K+1
C        IF(M+35.LT.250)GOTO 55
55    READ(1,102)BUF
      DO 1001 I=1,120
      K=121-I
      IF(BUF(K).NE.' ') GOTO 1002
1001  CONTINUE
      GOTO 90
1002  L3=0
      DO 1003 I=1,K
      IF(BUF(I).EQ.'_'.OR.BUF(I).EQ.'%') GOTO 1004
      L3=L3+1
      GOTO 1003
1004  IF(BUF(I).EQ.'_')BUF(I)=1H
      IF(BUF(I).EQ.'%')BUF(I)=1H
1003  CONTINUE
C      WRITE(1,10001)K
10001       FORMAT('K = ',I10)
      ENCODE(120,102,BUF2)BUF
C      WRITE(1,20)BUF2
      K=(K+1)/2
      IF(L3.LT.80) GOTO 1005
1010  CALL MSPEAK(24)
      GOTO 55
102   FORMAT(121A1)
1005  DO 1006 I=1,K
      L1=I+M
      MSG(L1)=BUF2(I)
1006  CONTINUE
      MSG(M)=K+M+1
      M=M+K+1
      IF(M+40.LT.500) GOTO 55
        CALL MSPEAK(25)
90      MSG(M)=-1
        RETURN
      END
