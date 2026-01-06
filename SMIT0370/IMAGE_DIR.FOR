      subroutine image_dir (dir)

      implicit none

      external jpi$_imagname
      integer*4 sys$getjpi
      integer*2 len
      integer*4 status
      integer*4 i
      structure /itmlist/ 
         union
            map
               integer*2 buflen
               integer*2 code
               integer*4 bufadr
               integer*4 retlenadr
            end map
            map
               integer*4 end_list
            end map
         end union
      end structure

      record /itmlist/ itmlst
      character*128 imagname
      character*128 dir
      itmlst.buflen=80
      itmlst.code       = %loc(jpi$_imagname)
      itmlst.bufadr     = %loc(imagname)
      itmlst.retlenadr  = %loc(len)

      status = sys$getjpi (%val(1),,,itmlst,,,)	! Get myself

      do 10,i=len,1,-1
        if (imagname (i:i) .eq. ']') goto 20
   10 continue

   20 dir = imagname(1:i)

      return 
      end
