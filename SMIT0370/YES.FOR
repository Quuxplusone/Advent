      LOGICAL FUNCTION YES(X,Y,Z,WCOUNT)
  
*  PRINT MESSAGE X, WAIT FOR YES/NO ANSWER.  IF YES, PRINT Y AND LEAVE Y
*  TRUE; IF NO, PRINT Z AND LEAVE YEA FALSE.  SPK IS RSPEAK.
  
      IMPLICIT INTEGER(A-Z) 
      LOGICAL BLKLIN,YESCALL
      Character*10 Reply, Dummy

      COMMON /IOSCOM/ BLKLIN,YESCALL
    1 IF(X.NE.0)CALL RSPEAK(X)
      WC=WCOUNT 
      YESCALL=.TRUE.
      CALL GETIN(REPLY,DUMMY,WC)
      YESCALL=.FALSE. 
      IF(REPLY.NE.'YES'.AND.REPLY.NE.'Y') GOTO 2
!!!!! WRITE(6,3) 'YES'
      WCOUNT=WC 
      GOTO 10 
    2 IF(REPLY.NE.'NO'.and.reply.ne.'N') GOTO 4 
!!!!! WRITE(6,3) 'NO' 
    3 FORMAT(' ',2H? ,A3) 
      WCOUNT=WC 
      GOTO 20 
    4 READ(5,5) REPLY 
    5 FORMAT(1A5 )
      call str$upcase(reply,reply)
      IF(REPLY.EQ.('YES').OR.REPLY.EQ.('Y'))GOTO 10 
      IF(REPLY.EQ.('NO' ).OR.REPLY.EQ.('N'))GOTO 20 
      WRITE(6,9)
    9 FORMAT(/,' Please answer the question.')
      IF(X.NE.0) CALL RSPEAK(X) 
      GOTO 4
   10 YES=.TRUE.
      IF(Y.NE.0)CALL RSPEAK(Y)
      RETURN
   20 YES=.FALSE. 
      IF(Z.NE.0)CALL RSPEAK(Z)
      RETURN
      END 
