      CHARACTER FUNCTION TOUPPR( C )
C COPYRIGHT 2001 MYRKRAVERK (MYRKRAVERK@USERS.SOURCEFORCE.NET)
C DISTRIBUTED UNDER GPL
C
C RETURNS CHARACTER C IN UPPERCASE, ASSUMES THAT A THROUGH Z
C ARE IN SEQUENCE AND A IS THE LOWEST AND THAT UPPERCASE ARE
C LOWER THAN LOWERCASE -- IN A WORD: ASCII
      
C THIS IS A TERRIBLE HACK, NEEDS TO BE FIXED!!!
      CHARACTER C, D
      INTEGER CDIFF
      TOUPPR = C
      CDIFF = ICHAR( 'a' ) - ICHAR( 'A' )
      CDIFF = ICHAR( C ) - CDIFF
      D = CHAR ( CDIFF )
      IF ( ICHAR( C ).GE.ICHAR( 'a' ) .AND.ICHAR( C ).LE.ICHAR( 'z ' ) )
C     1     C = CHAR( CDIFF )
     1 TOUPPR = D
      RETURN
      END

      
       
      
       
       
