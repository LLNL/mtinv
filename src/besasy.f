c.................................................................
c numerical recipes (https://numerical.recipes) used by greensf.f
c.................................................................
c called by greensf.f that is used in mkgrnlib.c
c.................................................................
c
        SUBROUTINE BESASY(N,X,NTERMS,PNX,QNX)
C
C BESASY GENERATES P AND Q FUNCTIONS IN HANKEL ASYMPTOTIC EXP.
C
        ANN      = 4*N*N
        X8       = .125/X
        PNX      = 1.
        QNX      = X8*(ANN-1.)
        TERM     = QNX
        AI       = 2.
        AJ       = 3.
        DO 100 K = 1,NTERMS
        TERM     = -TERM*X8*(ANN-AJ**2)/AI
        PNX      = PNX + TERM
        AI       = AI + 1.
        AJ       = AJ + 2.
        TERM     =  TERM*X8*(ANN-AJ**2)/AI
        QNX      = QNX + TERM
        AI       = AI + 1.
        AJ       = AJ + 2.
 100    CONTINUE
        RETURN
        END

