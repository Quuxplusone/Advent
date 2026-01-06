      SUBROUTINE LDCOMN(QNAME,NAME) 
* 
*   ADVENTURE SUBROUTINE TO LOAD ALL COMMON BLOCKS FROM EITHER
*     THE SYSTEM FILE (QNAME=.TRUE.) OR A PREVIOUSLY SAVED FILE 
*     (QNAME=.FALSE.) 
* 
*   IF THE SYSTEM FILE IS NOT FOUND, ZERO ARRAYS. 
* 
*   IF THE NAMED FILE IS NOT FOUND, DIE 
  
      IMPLICIT INTEGER (A-Z)
      CHARACTER*148 FILENAME
      Character*128 filedir
      COMMON /AAAAAA/ CORE(1)
      COMMON /ZZZZZZ/ THEEND

      LOGICAL QNAME 
      
      call image_dir (filedir)

      if (qname) then
         filename = filedir//'funadv.common'
      else
         filename = 'sys$login:funadv.adv'
      endif       

      open (unit=2,file=filename,status='old',err=1000,
     1form='unformatted',carriagecontrol='none')
      BEGIN=1 
      END=((%LOC(THEEND)-%LOC(CORE(1))+1)/4)+1
      READ(2,err=9999) (CORE(I1),I1=BEGIN,END) 
      
      CLOSE(2)
      RETURN

 1000 if (qname) RETURN
      type*,'Saved game can not be opened...'
      RETURN

 9999 type*,' '
      type*,'Fatal error loading saved game - ABORT.'
      STOP
      END 
