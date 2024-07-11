c.................................................................
c numerical recipes (https://numerical.recipes) used by greensf.f
c.................................................................
c.................................................................
c called by greensf.f that is used in mkgrnlib.c
c.................................................................
c
        SUBROUTINE bessel(X,XBSY,NTERMS,BESJ0,BESJ1)
C
C        BESSEL FUNCTIONS OF THE FIRST KIND OF ORDER ZERO AND ONE
C
C FOR ABS(X).LT.8. : CHEBYSHEV POLYNOMIAL EXPANSIONS ARE USED
C     AS GIVEN IN NATIONAL PHYS. LAB. MATH. TABLES, VOL. 5, 1962.
C
      DATA BR0/.2827844947E8/,BR1/-.6852659891E7/,BR2/.38831312263E6/,
     1   BR3/-.90578674277E4/,BR4/.108306963E3/,BR5/-.73485335935/,
     2   BR6/.29212672487E-2/,BR7/-.65050170571E-5/,BR8/.64538018051E-8/
      DATA BS0/.2827844947E8/,BS1/.21695247743E6/,BS2/.70046825147E3/
      DATA BRR0/.98087274959E7/,BRR1/-.11425325721E7/,
     1   BRR2/.40946213625E5/,BRR3/-.66660119856E3/,
     2   BRR4/.57575414035E1/,BRR5/-.27904475519E-1/,
     3   BRR6/.73493132111E-4/,BRR7/-.84306821641E-7/
      DATA BSS0/.19617454991E8/,BSS1/.16711673184E6/,
     1   BSS2/.60777258247E3/
C
C FOR 8.0.LE.ABS(X).LE.XBSY : CHEBYSHEV RATIONAL POLYS ARE USED STILL
C
      DATA BA0/2.5323420902E2/,BA1/4.2217704118E1/,BA2/5.2443314672E-1/
      DATA BB0/.44884594896E3/,BB1/.75322048579E2/
      DATA BC0/-1.2339445551E1/,BC1/-2.7788921059/,BC2/-4.9517399126E-2/
      DATA BD0/.17496878239E3/,BD1/.4100554523E2/
      DATA BAA0/3.5451899975E2/,BAA1/5.5544843021E1/,
     1   BAA2/6.5223084285E-1/
      DATA BBB0/.62836856631E3/,BBB1/.97300094628E2/
      DATA BCC0/4.4822348228E1/,BCC1/9.7348068764/,BCC2/1.7725579145E-1/
      DATA BDD0/.21185478331E3/,BDD1/.46917127629E2/
C
C FOR ABS(X).GY.XBSY : HANKEL'S ASYMPTOTIC EXPANSIONS ARE USED
C     AS GIVEN IN ABROMOVITZ AND STEGUN.
C
      DATA PI2/.63661977236/,CPI4/.70710681/
C
        IF (X .NE. 0.) GO TO 10
 
C
C X.EQ.0.
C
        BESJ0=1.
        BESJ1=0.
        GO TO 100
C
 10     AX=ABS(X)
        IF (AX.GE.8.) GO TO 20
C
C AX.LT.8. (USE CHEBYSHEV EXPANSIONS FOR THIS RANGE)
C
        B=X*X
        BP0=((((BR8*B+BR7)*B+BR6)*B+BR5)*B+BR4)*B
        BP0=(((BP0+BR3)*B+BR2)*B+BR1)*B+BR0
        BP1=((((BRR7*B+BRR6)*B+BRR5)*B+BRR4)*B+BRR3)*B
        BP1=((BP1+BRR2)*B+BRR1)*B+BRR0
        BESJ0=BP0/(((B+BS2)*B+BS1)*B+BS0)
        BESJ1=X*BP1/(((B+BSS2)*B+BSS1)*B+BSS0)
        GO TO 100
C
 20     IF (AX.GT.XBSY) GO TO 30
C
C 8.0.LE.AX.LE.XBSY   (USE CHEYSHEV EXPANSIONS FOR THIS RANGE)
C
        B=(8./X)**2
        BP0=((BA2*B+BA1)*B+BA0)/((B+BB1)*B+BB0)
        BQ0=((BC2*B+BC1)*B+BC0)/(AX*((B+BD1)*B+BD0))
        BP1=((BAA2*B+BAA1)*B+BAA0)/((B+BBB1)*B+BBB0)
        BQ1=((BCC2*B+BCC1)*B+BCC0)/(AX*((B+BDD1)*B+BDD0))
        BCOS=COS(AX)
        BSIN=SIN(AX)
        BSQRT=SQRT(AX)
        BESJ0=(BCOS*(BP0+BQ0)+BSIN*(BP0-BQ0))/BSQRT
        BESJ1=(BCOS*(BQ1-BP1)+BSIN*(BQ1+BP1))/BSQRT
        IF (X.LT.0.) BESJ1=-BESJ1
        GO TO 100
C
C AX.GT.XBSY   (USE HANKEL'S ASYMP EXPANSIONS)
C
 30     B=SQRT(PI2/AX)
        BCOS=COS(AX)
        BSIN=SIN(AX)
        BC=CPI4*( BCOS+BSIN)
        BS=CPI4*(-BCOS+BSIN)
        CALL BESASY(0,AX,NTERMS,BP0,BP1)
        BESJ0=B*(BP0*BC-BP1*BS)
        CALL BESASY(1,AX,NTERMS,BP0,BP1)
        BESJ1=B*(BP0*BS+BP1*BC)
        IF (X.LT.0.) BESJ1=-BESJ1
C
 100    RETURN
        END

