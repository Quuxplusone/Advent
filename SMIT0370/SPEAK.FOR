      SUBROUTINE SPEAK(N) 
  
*  PRINT THE MESSAGE WHICH STARTS AT LINES(N).  PRECEDE IT WITH A BLANK 
*  UNLESS BLKLIN IS FALSE.
  
      IMPLICIT INTEGER(A-Z) 

      STRUCTURE /TEXTYPE/
        Integer*4 count
        Character*80 string
      end structure

      LOGICAL BLKLIN

      record /textype/ lines 

      Character*80 text

      COMMON /TXTCOM/ RTEXT,LINES,LINUSE
      COMMON /STXCOM/ TEXT
      COMMON /IOSCOM/ BLKLIN
      DIMENSION RTEXT(250), LINES(2000), TEXT(80)

      IF(N.EQ.0)RETURN
      IF (LINES(N).string .EQ. '>$<') RETURN
      IF(BLKLIN)WRITE(6,1)
    1 FORMAT(1X)
      I = N
   10 CONTINUE 
      WRITE(6,20)lines(I).string
   20 FORMAT(1x,80A)
   30 I = I + 1
      IF (LINES(I).COUNT .GE. 0) GO TO 10 
      RETURN
  
      END 
