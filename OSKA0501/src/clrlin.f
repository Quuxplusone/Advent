C***   CLRLIN

       SUBROUTINE CLRLIN

C  CLEARS OUT ALL CURRENT SYNTAX ARGS IN PREPARATION FOR A NEW INPUT LINE

       IMPLICIT INTEGER(A-Z)
!      REAL*8 VTXT,OTXT,IOTXT
       CHARACTER*8 VTXT,OTXT,IOTXT
       COMMON /WRDCOM/ VERBS(45),VTXT(45,2),VRBX,OBJS(45),OTXT(45,2),
     1  OBJX,IOBJS(15),IOTXT(15,2),IOBX,PREP,WORDS(45)

       DO 1 I=1,45
       OBJS(I)=0
       VERBS(I)=0
       DO 1 J=1,2
!1       VTXT(I,J)=0
1       VTXT(I,J)='        '

       DO 3 I=1,15
       IOBJS(I)=0
       DO 3 J=1,2
!       IOTXT(I,J)=0
       IOTXT(I,J)='        '
!3       OTXT(I,J)=0
3       OTXT(I,J)='        '

       VRBX=0
       OBJX=0
       IOBX=0
       PREP=0
       RETURN

       END
