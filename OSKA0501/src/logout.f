C***   LOGOUT
      LOGICAL FUNCTION LOGOUT(DUMMY)
      IMPLICIT INTEGER(A-Z)
      COMMON/LNKCOM/ACTIVE(32),USER(15,32),MESSGS(32),MONITO(32),
     1  TEXT(70,32)
      COMMON/WRUCOM/ME
      LOGICAL ACTIVE
      INTEGER*4 MESSGS
C      LOGOUT=.NOT.ACTIVE(ME)
C MAKING THIS A DUMMY RUTINE IN ORDER TO GET THINGS TO WORK
      LOGOUT=.FALSE.
      RETURN
      END
