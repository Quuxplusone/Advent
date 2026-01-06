      SUBROUTINE SVCOMN(QNAME,NAME) 
* 
*  ADVENTURE SUBROUTINE TO SAVE ALL COMMON BLOCKS.  (SEE LDCOMN)
  
      IMPLICIT INTEGER (A-Z)
      CHARACTER*148 FILENAME
      Character*128 filedir
      COMMON /AAAAAA/ CORE(1)
      COMMON /ZZZZZZ/  THEEND 

      LOGICAL QNAME 

      if (qname) then
         filename = filedir//'funadv.common'
      else
         filename = 'sys$login:funadv.adv'
      endif       
  
      open (unit=2,file=filename,status='new',err=9999,
     1form='unformatted',carriagecontrol='none')
      BEGIN=1 
      END=(%LOC(THEEND)-%LOC(CORE(1))+4)/4     ! Account for wordsize
      WRITE(2,err=9998) (CORE(I1),I1=BEGIN,END)
      close(2)
      RETURN
 9998 close (2)
 9999 write (6,10)
  10  Format (' Error creating save file.')
      return
      END 
