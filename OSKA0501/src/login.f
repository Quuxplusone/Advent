C***   LOGIN
      SUBROUTINE LOGIN
      IMPLICIT INTEGER(A-Z)
      DIMENSION VEC(15)
      LOGICAL ACTIVE
      INTEGER*4 MESSGS
      COMMON/LNKCOM/ACTIVE(32),USER(15,32),MESSGS(32),MONITO(32),
     1  TEXT(70,32)
      COMMON/WRUCOM/ME
C OK THIS IS GOING TO COUSE TROUBLE BUT I DONT HAVE TIMDAT
C I ASSUME IT HAS SOME USER INFORMATION AND STUFF BUT THIS
C IS (HOPEFULLY) NOT NECCESARY TO PLAY THE GAME. M.V.
C      CALL TIMDAT(VEC,15)
      ME=VEC(12)
C      CALL TIMDAT(USER(1,ME),15)
      ACTIVE(VEC(12))=.TRUE.
      RETURN
      END
