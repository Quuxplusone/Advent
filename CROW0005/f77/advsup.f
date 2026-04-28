       SUBROUTINE GETIN(TWOW,B,C,D)
       IMPLICIT INTEGER(A-Z)
       CHARACTER*5 A(5),B,C,D
       CHARACTER*1 UPCASE
6      READ (UNIT=*,FMT=1) (A(I),I=1,4)
1      FORMAT(4A5)
       TWOW=0
       S=0
       DO 7 J=1,4
       DO 7 K=1,5
       A(J)(K:K) = UPCASE(A(J)(K:K))
7      CONTINUE
       B=A(1)
       DO 2 J=1,4
       DO 2 K=1,5
       IF (A(J)(K:K).EQ.' ') GOTO 3
       IF(S.EQ.0)GOTO 2
       TWOW=1
       C(1:6-K) = A(J)(K:5)
       IF (K.NE.1) C(6-K+1:5) = A(J+1)(1:K-1)
       GOTO 4
3      IF(S.EQ.1)GOTO 2
       S=1
       IF (J.NE.1) GOTO 2
       DO 5 L=K,5
       B(L:L)=' '
5      CONTINUE
2      CONTINUE
4      D=A(2)
       RETURN
       END


       FUNCTION RAN(I)
C     Rand is often quite poor, should replace this -- MTR
       INTEGER I
       RAN = Rand(0)
       RETURN
       END

       FUNCTION UPCASE(CH)
       CHARACTER*1 CH,UPCASE
       INTEGER CODE
       CODE=IAChar(CH)
       IF (CODE.GE.97.AND.CODE.LE.122) CODE = CODE - 32
       UPCASE=AChar(CODE)
       RETURN
       END
