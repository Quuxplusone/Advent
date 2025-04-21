       LOGICAL FUNCTION EQV(WD1,WD2)
         IMPLICIT INTEGER(A-Z)
         DIMENSION WD1(3),WD2(3)
         EQV=.FALSE.
         DO 10 I=1,3
10       IF (WD1(I).NE.WD2(I))RETURN
         EQV=.TRUE.
         RETURN
         END
