      LOGICAL FUNCTION WIZARD(DUMMY)
      IMPLICIT INTEGER (A-Z)
      LOGICAL BIGWIZ
      CHARACTER*12 BIGWIZS (4)
      Character*12 Usernum
      Character*10 Junk,Junk2
      COMMON /USER/ USERNUM 
      DATA BIGWIZS / 'CTS' , ' ', ' ', ' ' /
  
      WIZARD=.FALSE.
      WRITE(6,10) 
 10   FORMAT(' Are you The Wizard ? ',$)
      READ(5,20,end=25)JUNK
 20   FORMAT(2A10)
      call str$upcase(JUNK,JUNK)
      IF(JUNK.EQ.'Y'.OR.JUNK.EQ.'YES') GOTO 40
      if(junk.eq.'MAYBE') goto 47
      IF(JUNK.EQ.'SAVE') GOTO 35
   25 WRITE(6,30) 
 30   FORMAT(' Very well.') 
      RETURN
 35   CALL SVCOMN(.TRUE.,0) 
 40   BIGWIZ=.FALSE.
      CALL USER(%ref(USERNUM))
      DO 45 LOOP = 1,4 
         IF ( USERNUM .NE. BIGWIZS(LOOP) ) GOTO 45
         BIGWIZ = .TRUE.
         GOTO 100 
   45 CONTINUE
 47   WRITE(6,50) 
 50   FORMAT(' Prove it! Say the magic word!')
      READ(5,20,end=200)JUNK,JUNK2
      call str$upcase(junk,junk)
      call str$upcase(junk2,junk2)
      IF(JUNK.EQ.'ORCS ARE S'.AND.JUNK2.EQ.'CUM       ') GOTO 60
      IF(JUNK.NE.'DWARF'.AND.JUNK.NE.'DWARVES') GOTO 200
      IF(JUNK.EQ.'DWARF') MSG=1 
      IF(JUNK.EQ.'DWARVES') MSG=2 
 60   WRITE(6,70) 
 70   FORMAT(' That is not what I thought it was.  Do you know what', 
     $' I thought it was?') 
      READ(5,20,end=200)JUNK
      call str$upcase(junk,junk)
      IF(JUNK.NE.'NO'.AND.JUNK.NE.'N') GOTO 200 
      WRITE(6,80) 
 80   FORMAT(/,' Wazu') 
      READ(5,20,end=200)JUNK
      call str$upcase(junk,junk)
      IF(JUNK.NE.'WAZU') GOTO 200 
      IF(MSG.NE.0) GOTO 190 
      WRITE(6,90) 
 90   FORMAT(' Oh dear, you really *are* a Wizard! Sorry to have ', 
     $'bothered you. . .')
      WIZARD=.TRUE. 
      RETURN
 100  WRITE(6,110)
 110  FORMAT(/,' Oh my!!! I''ve been detaining a *true* wizard! ! !'
     $,//) 
      WIZARD=.TRUE. 
      RETURN
 190  IF(MSG.EQ.1) WRITE(6,191) 
 191  FORMAT(' There are now several threatening little Dwarves'
     $,' in the room with you!',/,' Most of them throw knives'
     $,' at you!',/,' All of them get you!')
      IF(MSG.EQ.2) WRITE(6,195) 
 195  FORMAT(' There is a threatening  little dwarf in the room with' 
     $,' you!',/,' One sharp nasty knife is thrown at you!' 
     $,/,' It gets you!') 
      WRITE(6,199)
 199  FORMAT(/,/,/,' Just kidding....  You are not a Wizard, though.' 
     $,/,' ....in this game, anyway.')
      RETURN
 200  WRITE(6,210)
 210  FORMAT(' Foo, you are nothing but a charlatan!')
      RETURN
      END 
