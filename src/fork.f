c.................................................................
c numerical recipes (https://numerical.recipes) used by greensf.f
c.................................................................
c.................................................................
c called by greensf.f that is used in mkgrnlib.c
c.................................................................
c
      SUBROUTINE fork(LX,CX,SIGNI)
      COMPLEX*16 CX(LX),CARG,CDEXP,CW,CTEMP
      J=1
      DO I=1,LX
        IF(I.LE.J) THEN
          CTEMP=CX(J)
          CX(J)=CX(I)
          CX(I)=CTEMP
        END IF
        M=LX/2
 20     IF(J.LE.M)GO TO 30
        J=J-M
        M=M/2
        IF(M.GE.1)GO TO 20
 30     J=J+M
      END DO
      L=1
 40   ISTEP=2*L
      DO M=1,L
        CARG=(0.,1.)*(3.14159265*SIGNI*DFLOAT(M-1))/DFLOAT(L)
        CW=CDEXP(CARG)
        DO I=M,LX,ISTEP
          CTEMP=CW*CX(I+L)
          CX(I+L)=CX(I)-CTEMP
          CX(I)=CX(I)+CTEMP
        END DO
      END DO
      L=ISTEP
      IF(L.LT.LX)GO TO 40
      RETURN
      END
