*(************************************************************************************
***  Calculating frequency wavenumber reflectivity synthetic Green's functions for
***  the 10 fundamental faulting orientations ZSS,ZDS,ZDD,ZEP,RSS,RDS,RDD,REP,TSS,TDS
***  in a layered medium over a half space.  Source is double couple.
***  
***  green.f Original Author: Yuehua Zeng Sept 1992 at Univ. Nevada Reno
***  
***  Reference: Zeng and Anderson (1995) Bull. Seismol. Soc. Am.
***
***   modifications by gene ichinose May 2006 at URS for ORI
********************************************************************************
*        1         2         3         4         5         6         7         8
*2345678901234567890123456789012345678901234567890123456789012345678901234567890
********************************************************************************
	subroutine greensf( iverbose )
	integer iverbose

	integer*4 MXL
	parameter( MXL = 1024 )

*** Greens
        character filename*256, knet*8, kstnm*8, kloc*8
        real*4 stla,stlo,stel,evla,evlo,evdp
        real*4 rdist,az,baz
        real*4 t0,dt,twin,fmax,damp,eps,smin,rigidity
	real*4 redv,ts0,tstart,tend
        integer*4 kmax,nt

*** Greens.VelMod
        character modfile*256, modpath*256
        integer*4 mynlay, maxlay
        real*4 hh(MXL),zz(MXL),va(MXL),vb(MXL),qa(MXL),qb(MXL),rho(MXL)
        real*4 sigma(MXL)

*** Greens.Greens_Function
        real*4 g01(4096),g02(4096),g03(4096),g04(4096),g05(4096)
        real*4 g06(4096),g07(4096),g08(4096),g09(4096),g10(4096)
*** rotational greens functions (w1SS,w2DS,w3DD,w3EX)
        real*4 g11(4096),g12(4096),g13(4096),g14(4096)
        real*4 g15(4096),g16(4096),g17(4096),g18(4096)
        real*4 g19(4096),g20(4096),g21(4096),g22(4096)

*** local variables
	dimension h(MXL),vp(MXL),Qp(MXL),vs(MXL),Qs(MXL),dens(MXL)
	complex*16 ka(MXL),kb(MXL),nu(MXL),ga(MXL),kbeta(MXL),z(MXL),
     +         I11(2,2),I12(2,2),I21(2,2),I22(2,2),I31(2,2),I32(2,2),
     +         Ish11,Ish12,Ish21,Ish22,Ish31,Ish32,cvp(MXL),cvs(MXL),
     +         Td(MXL,2,2),Tu(MXL,2,2),Rd(MXL,2,2),Ru(MXL,2,2),
     +         HTd(MXL,2,2),HTu(MXL,2,2),HRd(MXL,2,2),HRu(0:MXL,2,2),
     +         Tdsh(MXL),Tush(MXL),Rdsh(MXL),Rush(MXL),exd(MXL,2),
     +         HTdsh(MXL),HTush(MXL),HRdsh(MXL),HRush(0:MXL),
     +         exu(0:MXL,2),Sd(2),Su(2),Sdsh,Sush,Es(2),Eu(2),Ed(2),
     +         Eush,Edsh,aj0,aj1,sdepn,ak,ak2,dkn,ci,cw,src(4096),
     +         dUr0,Ur0,dUf1,Uf1,dUr1,Ur1,aux(2,2),ainv(2,2),auxi(2),
     +         tmp,r0,one,zero,rdepn,dS0(4),S0(4),dS1(6),S1(6)

	complex*16 rss(8192), rds(8192), rdd(8192), rep(8192)
	complex*16 zss(8192), zds(8192), zdd(8192), zep(8192)
	complex*16 tss(8192), tds(8192), tt(8192)
	complex*16 au(8192), mu(MXL), ctmp

	real*8 al, dens
	real*4 rk
	logical up

*** typedef struct Greens
        common/grn/filename,knet,kstnm,kloc,
     $    stla,stlo,stel,evla,evlo,evdp,
     $    rdist,az,baz,t0,dt,twin,fmax,damp,eps,smin,rigidity,redv,
     $    ts0,tstart,tend,Ptakeoff,Prayparameter,Pttime,Praybottom,
     $    kmax,nt,modfile,modpath,mynlay,maxlay,
     $    hh,zz,va,vb,qa,qb,rho,sigma,
     $    g01,g02,g03,g04,g05,g06,g07,g08,g09,g10,
     $    g11,g12,g13,g14,g15,g16,g17,g18,g19,g20,
     $    g21,g22

*** local common block
	common/para/one,zero,ga,nu,kbeta,ka,kb,dens,z,I11,I12,
     +              I21,I22,I31,I32,Ish11,Ish12,Ish21,Ish22,
     +              Ish31,Ish32,sdepn,rdepn,up,nlay,ns,nr
        common/matrix/Td,Tu,Rd,Ru,Tdsh,Tush,Rdsh,Rush,exd,exu,
     +                HTd,HTu,HRd,HRu,HTdsh,HTush,HRdsh,HRush

***
*** print out the common block for struct Greens to debug
***
	if( iverbose .eq. 1 ) then
	  write(*,*)'iverbose=', iverbose
	  write(*,*)'filename=', filename
	  write(*,*)'kstnm=', kstnm
	  write(*,*)'knet=', knet
	  write(*,*)'kloc=', kloc
	  write(*,*)'stla=', stla
	  write(*,*)'stlo=',  stlo
	  write(*,*)'stel=', stel
	  write(*,*)'evla=', evla
	  write(*,*)'evlo=', evlo
	  write(*,*)'evdp=', evdp
	  write(*,*)'rdist=', rdist
	  write(*,*)'az=', az
	  write(*,*)'baz=', baz
	  write(*,*)'t0=', t0
	  write(*,*)'dt=', dt
	  write(*,*)'twin=', twin
	  write(*,*)'fmax=', fmax
	  write(*,*)'damp=', damp
	  write(*,*)'eps=', eps
	  write(*,*)'smin=', smin
	  write(*,*)'rigidity=', rigidity
	  write(*,*)'redv=', redv
	  write(*,*)'ts0=', ts0
	  write(*,*)'tstart=', tstart
	  write(*,*)'tend=', tend
	  write(*,*)'kmax=', kmax
	  write(*,*)'nt=', nt
	  write(*,*)'modfile=', modfile
	  write(*,*)'modpath=', modpath
	  write(*,*)'mynlay=', mynlay
	  write(*,*)'maxlay=', maxlay
	endif
****
**** end debug section
****
	sdep = evdp
	r    = rdist
	fi   = az
	rdep = 0
	nlay = mynlay

	pi   = acos(-1.)
	pi2  = pi*2.
	ci   = cmplx(0.0,1.0)
	one  = cmplx(1.0,0.0)
	zero = cmplx(0.0,0.0)
	er   = 0.1e-10
	nw   = nt/2
	df=1./twin
	wi=-damp*pi/twin
	wmax=pi2
	smax = 0.

*** compute t0 using redv
*** precompute t0 from redv
***
	if( redv .le. 0. ) then
	  t0 = 0.
	else
	  t0 = (r/redv)
	endif

9898    format('------------------------------------------------------')
	if(iverbose.eq.1)then

	  write(*,9898)
	  write(*,*) 'MKGRNLIB Mon Jan 13 22:38:59 EST 2014'
	  if( sdep .le. 0.0 ) then
		write(*,*)'ERROR DEPTH LESS THAN ZERO'
		stop
	  endif
	  write(*,*)'dist=',rdist,' azimuth=',az, ' srcdep=',evdp
	  write(*,7979)nt,twin,dt,t0
7979    format('NT=',i4,' TWIN=',f9.3,' s DT=',f6.3,' s T0=',f8.3,
     $' sec (assumes pt src)')
          write(*,8989)fmax,damp,kmax,eps,smin
8989    format('FMAX=',f6.3,' Hz DAMP=',f5.2,' KMAX=',i6,' EPS=',e12.5,
     $' SMIN=',e12.5)
	  write(*,9898)
	endif

	do 12 i=1,nlay
	  dens(i)=1.0D+00*rho(i)
	  h(i) = hh(i)
	  vp(i) = va(i)
	  Qp(i) = qa(i)
	  vs(i) = vb(i)
	  Qs(i) = qb(i)

	  if( iverbose .eq. 1 )then	
	  write(*,11)i,h(i),zz(i),vp(i),Qp(i),vs(i),Qs(i),dens(i)
11	  format(i2,7f9.2)
	  endif
cccc**********************************************
	  zlay0 = zz(i)
	  zlay1 = zz(i) + h(i)
	  if( sdep .eq. zlay0 .or. sdep .eq. zlay1 ) then
	    write(*,*)"INTERFACE sdep=",sdep,"i=",i,"zlay0,1=",zlay0,zlay1
	   else if( sdep .gt. zlay0 .and. sdep .lt. zlay1 ) then
	    write(*,*)"INSIDE sdep=",sdep,"i=",i,"zlay0,1=",zlay0,zlay1
	  endif
cccccccccccccccc
c	  if( sdep .gt. zlay0 .and. sdep .lt. zlay1 )then
c	    rigidity = (vs(i) * 1.0E+05)**2 * dens(i)
c	  endif
ccccccccccccccc

12	continue
cccc	write(*,*)"rigidity=",rigidity," z=",sdep
	if( iverbose .eq.  1 ) write(*,9898)

********************************************************************************
********************************************************************************
********************************************************************************
c
c  set repeated source interval
c
	al=vp(nlay)*twin+r
	dk=pi2/al

c
c model averages
c
	vs1=0.0
	vp1=0.0
	dens1=0.0

	do 5 i=1,nlay-1
	   vs1=vs1+vs(i)
	   vp1=vp1+vp(i)
	   dens1=dens1+dens(i)
  5	continue

	if(nlay.gt.1)then
	  vs1=vs1/float(nlay-1)
	  vp1=vp1/float(nlay-1)
	  dens1=dens1/float(nlay-1)
	else
	  vs1=vs(1)
	  vp1=vp(1)
	  dens1=dens(1)
	endif
c
c normalize density
c
	do 10 i=1,nlay
	   dens(i)=dens1/dens(i)
 10	continue
c
c compute sdep,rdep,ns,nr,up
c
	do 25 n=1,2

	  if(n.eq.1)then
	    dep=rdep
	  else
	    dep=sdep
	  endif

c	  write(*,*)"n,rdep,sdep,dep=", n,rdep,sdep,dep

	  depth=0.0
	  do 15 i=1,nlay-1
	     depth=depth+h(i)
	     if(dep.le.depth) then 
	       nn=i
	       dep=dep-depth+h(i)
	       goto 20
	     endif
 15	  continue

	  nn=nlay
	  dep=dep-depth
 20	  continue

c	  write(*,*) "n,dep,nlay=", n,dep,nlay

	  if(n.eq.1)then
	    smax=dep
	    ns=nn
	  else
	    rdep=dep
	    nr=nn
	  endif

c	  write(*,*) "n,dep,nlay,ns,nr,smax=", n,dep,nlay,ns,nr,smax

 25	continue

	sdep=smax
	up=.true.
	if(ns.gt.nr.or.(ns.eq.nr.and.sdep.gt.rdep))up=.not.up

c        write(*,*)"sdep=",sdep,"dep=",dep,"rdep=",rdep,
c     $   "ns=",ns,"nr=",nr,"up=",up

c	fi=fi-azm
c	a1= cos(rak)*cos(dip)*cos(fi)-sin(rak)*cos(2.0*dip)*sin(fi)
c	a2=-cos(rak)*cos(dip)*sin(fi)-sin(rak)*cos(2.0*dip)*cos(fi)
c	a3= 0.5*sin(rak)*sin(2.0*dip)
c	a4= 0.5*(cos(rak)*sin(dip)*sin(2.0*fi)
c     +	   -sin(rak)*sin(2.0*dip)*(sin(fi))**2)
c	a5=-a3-a4
c	a6= cos(rak)*sin(dip)*cos(2.0*fi)
c     +	   -0.5*sin(rak)*sin(2.0*dip)*sin(2.0*fi)
c

c
c  loop for frequency response
c
	f=-df
	smax=0.0
	k1=20
	do 180 iw=1,nw
	f=f+df
	if(f.ge.fmax)goto 180
	w=f*pi2
	cw=cmplx(w,wi)
c
c  ramp time function with time shift t0 
c  assumes only delta function for source... use makedisp to convolve source later
c
	tr = 0.0001
c
c## original 2.15.2024
c
 	src(iw)=(exp(-ci*cw*tr)-1)/(cw*cw*tr)*exp(ci*cw*t0)

c
c## test this, chandan suggest this change, does not work
c
c       src(iw)=cmplx(0.001,0.0)
c       or
c       src(iw)=cmplx(tr,0.0)

	if( abs(src(iw)) .gt. smax) smax = abs(src(iw))

	if( abs(src(iw))/smax .le. smin ) then
	  zss(iw) = zero
	  zds(iw) = zero
	  zdd(iw) = zero
	  zep(iw) = zero
	  rss(iw) = zero
	  rds(iw) = zero
	  rdd(iw) = zero
          rep(iw) = zero
	  tss(iw) = zero
	  tds(iw) = zero
	  goto 180
	endif

	do 40 i=1,nlay
c
c  compute the complex velocity for P and S waves
c
	  cvp(i)=vp(i)/((one-log(cw/wmax)/(pi*Qp(i)))
     +	              *cmplx(1.0,-0.5/Qp(i)))
	  cvs(i)=vs(i)/((one-log(cw/wmax)/(pi*Qs(i)))
     +	              *cmplx(1.0,-0.5/Qs(i)))
c
	  ka(i)=(vs1/cvp(i))**2
	  kb(i)=(vs1/cvs(i))**2
	  z(i)=h(i)*cw/vs1
 40	continue

	rdepn=rdep*cw/vs1
	sdepn=sdep*cw/vs1
	r0=r*cw/vs1
	dkn=dk*vs1/cw
c
	Ur0=zero
	Ur1=zero
	Uf1=zero
	do 45 i=1,4
	  S0(i)=zero
	  S1(i)=zero
 45	continue

	S1(5)=zero
	S1(6)=zero

c
c  loop for the wavenumber sum
c
	ak=-dkn
	do 160 k=1,kmax
	ak=ak+dkn
	ak2=ak*ak
	do 50 j=1,nlay	
	   nu(j)=cdsqrt(ak2-ka(j))
	   ga(j)=cdsqrt(ak2-kb(j))
	   if(dble(nu(j)).lt.0.)nu(j)=-nu(j)
	   if(dble(ga(j)).lt.0.)ga(j)=-ga(j)
	   kbeta(j)=2.0*ak2/kb(j)-one
	   if(j.ne.ns)then
	     exd(j,1)=exp(-nu(j)*z(j))
	     exd(j,2)=exp(-ga(j)*z(j))
	     exu(j-1,1)=exd(j,1)
	     exu(j-1,2)=exd(j,2)
	   else
	     exd(j,1)=exp(-nu(j)*(z(j)-sdepn))
	     exd(j,2)=exp(-ga(j)*(z(j)-sdepn))
	     exu(j-1,1)=exp(-nu(j)*sdepn)
	     exu(j-1,2)=exp(-ga(j)*sdepn)
	   endif
 50	continue
	call genrefl(ak)
c
c  calculate the source field and propagate them to the receiver
c
	do 60 i=1,2
	  do 61 j=1,2
	    aux(i,j)=zero
	    do 62 l=1,2
	       if(up)then
	         aux(i,j)=aux(i,j)-exu(ns-1,i)*HRu(ns-1,i,l)
     +	                 *exd(ns,l)*HRd(ns,l,j)
	       else
	         aux(i,j)=aux(i,j)-exd(ns,i)*HRd(ns,i,l)
     +	                 *exu(ns-1,l)*HRu(ns-1,l,j)
	       endif
 62         continue
 61       continue
 60	continue

	call cinv2(aux,ainv)
c
	do 140 n=1,2

	  if(n.eq.1)then
	    Sd(1)=dens(ns)
  	    Sd(2)=dens(ns)*ak/ga(ns)
	    Su(1)=-dens(ns)
  	    Su(2)=-dens(ns)*ak/ga(ns)
	  else
	    Sd(1)=dens(ns)*ak/nu(ns)
  	    Sd(2)=dens(ns)
	    Su(1)=dens(ns)*ak/nu(ns)
  	    Su(2)=dens(ns)
	  endif

	  if(up)then

	    do 65 i=1,2
	      auxi(i)=Sd(i)
	      do 66 j=1,2
	        auxi(i)=auxi(i)+exu(ns-1,i)*HRu(ns-1,i,j)*Su(j)
 66 	      continue
 65	    continue

	  else
	    do 70 i=1,2
	      auxi(i)=Su(i)
	      do 71 j=1,2
	        auxi(i)=auxi(i)+exd(ns,i)*HRd(ns,i,j)*Sd(j)
 71	      continue
 70	    continue

	  endif
	  do 80 i=1,2
	    Es(i)=zero
	    do 81 j=1,2
	      Es(i)=Es(i)+ainv(i,j)*auxi(j)
 81	    continue
 80	  continue
c
	  if(up)then
	    do 100 i=ns,nr-1

	      do 90 j=1,2
	        Ed(j)=zero
	        do 91 l=1,2
	           Ed(j)=Ed(j)+HTd(i,j,l)*Es(l)
 91	        continue
 90	      continue
	      Es(1)=Ed(1)
	      Es(2)=Ed(2)
100	    continue

	    Ed(1)=Es(1)
	    Ed(2)=Es(2)

	    do 105 i=1,2
	      Eu(i)=zero
	      do 106 j=1,2
	         Eu(i)=Eu(i)+HRd(nr,i,j)*Ed(j)
106	      continue
105	    continue

	  else

	    do 120 i=ns-1,nr,-1

	      do 110 j=1,2
	        Eu(j)=zero
	        do 111 l=1,2
	           Eu(j)=Eu(j)+HTu(i,j,l)*Es(l)
111	        continue
110	      continue

	      Es(1)=Eu(1)
	      Es(2)=Eu(2)
120	    continue

	    Eu(1)=Es(1)
	    Eu(2)=Es(2)

	    do 130 i=1,2
	      Ed(i)=zero
	      do 131 j=1,2
	         Ed(i)=Ed(i)+HRu(nr-1,i,j)*Eu(j)
131	      continue
130	    continue

	  endif
	  if(n.eq.1)then
	dUr0=I11(1,1)*Ed(1)+I11(1,2)*Ed(2)+I12(1,1)*Eu(1)+I12(1,2)*Eu(2)
      dS0(1)=I21(1,1)*Ed(1)+I21(1,2)*Ed(2)+I22(1,1)*Eu(1)+I22(1,2)*Eu(2)
      dS0(2)=I21(2,1)*Ed(1)+I21(2,2)*Ed(2)+I22(2,1)*Eu(1)+I22(2,2)*Eu(2)
      dS0(4)=I31(1,1)*Ed(1)+I31(1,2)*Ed(2)+I32(1,1)*Eu(1)+I32(1,2)*Eu(2)
      dS0(3)=I31(2,1)*Ed(1)+I31(2,2)*Ed(2)+I32(2,1)*Eu(1)+I32(2,2)*Eu(2)
	  else
	dUr1=I11(1,1)*Ed(1)+I11(1,2)*Ed(2)+I12(1,1)*Eu(1)+I12(1,2)*Eu(2)
      dS1(1)=I21(1,1)*Ed(1)+I21(1,2)*Ed(2)+I22(1,1)*Eu(1)+I22(1,2)*Eu(2)
      dS1(3)=I21(2,1)*Ed(1)+I21(2,2)*Ed(2)+I22(2,1)*Eu(1)+I22(2,2)*Eu(2)
      dS1(5)=I31(1,1)*Ed(1)+I31(1,2)*Ed(2)+I32(1,1)*Eu(1)+I32(1,2)*Eu(2)
      dS1(4)=I31(2,1)*Ed(1)+I31(2,2)*Ed(2)+I32(2,1)*Eu(1)+I32(2,2)*Eu(2)
	  endif
140	continue
c
  	Sdsh=dens(ns)*kb(ns)/ga(ns)
  	Sush=Sdsh
	if(up)then
	  Edsh=(Sdsh+Sush*exu(ns-1,2)*HRush(ns-1))
     +	      /(one-exd(ns,2)*exu(ns-1,2)*HRdsh(ns)*HRush(ns-1))

	  do 145 i=ns,nr-1
	    Edsh=HTdsh(i)*Edsh
145	  continue

	  Eush=HRdsh(nr)*Edsh
	else
	  Eush=(Sush+Sdsh*exd(ns,2)*HRdsh(ns))
     +	      /(one-exd(ns,2)*exu(ns-1,2)*HRdsh(ns)*HRush(ns-1))

	  do 150 i=ns-1,nr,-1
	    Eush=HTush(i)*Eush
150	  continue

	  Edsh=HRush(nr-1)*Eush
	endif
	dUf1=Ish11*Edsh+Ish12*Eush
	dS1(2)=Ish21*Edsh+Ish22*Eush
	dS1(6)=Ish31*Edsh+Ish32*Eush
c
c  compute the discrete wave-number sum for the single force
c  displacement and stress
	rk=real(dble(r0*ak))
cc
cc changed nterms from 20 to 1
cc
	call bessel(rk,20.0,1,besj0,besj1)

	aj0=besj0*ak
	aj1=besj1*ak
	dUr0=dUr0*aj1
	dS0(1)=dS0(1)*aj1
	dS0(2)=dS0(2)*aj0
	dS0(3)=dS0(3)*aj0
	dS0(4)=dS0(4)*aj0
c
	tmp=(dUr1+dUf1)/r0*besj1
	dUr1=-dUr1*aj0+tmp
	dUf1=-dUf1*aj0+tmp
	tmp=(dS1(1)+dS1(2))/r0*besj1
	dS1(1)=-dS1(1)*aj0+tmp
	dS1(2)=-dS1(2)*aj0+tmp
	dS1(3)=dS1(3)*aj1
	dS1(4)=dS1(4)*aj1
	dS1(5)=dS1(5)*aj1
	dS1(6)=dS1(6)*aj1
c
	Ur0=Ur0+dUr0
	Ur1=Ur1+dUr1
	Uf1=Uf1+dUf1
	do 155 i=1,4
	  S0(i)=S0(i)+dS0(i)
	  S1(i)=S1(i)+dS1(i)
155	continue
	S1(5)=S1(5)+dS1(5)
	S1(6)=S1(6)+dS1(6)
	if(k.le.k1)goto 160
        amax=max(abs(dUr0)/(abs(Ur0)+er),abs(dUr1)/(abs(Ur1)+er),
     +             abs(dUf1)/(abs(Uf1)+er))
	if(amax.lt.eps)goto 170
160 	continue
	print*,' Please reset the maximum kmax !!!'
	stop
c
c  generate the displacement at the receiver due to a buried slip
c  source using reciprocity relations
170	S0(3)=S0(3)-2.0*Ur0/(kb(nr)*dens(nr)*r0)
	S0(4)=S0(4)-S0(3)
	tmp=2.0*(Ur1+Uf1)/(kb(nr)*dens(nr)*r0)
	S1(4)=S1(4)-tmp
	S1(5)=S1(5)-S1(4)
	S1(6)=S1(6)-tmp

**** keep slip4pi in src term
*   cw = complex frequency
*   dk = twopi/( vp(nlay)*twin + r )

*	mu = kb(nr)*kb(nr)*dens(nr)
*	tmp=cw*slip4pi*dk*vs1*dens1/mu(nr)
* 	tt(iw) = (cw*dk)*(vs1*dens1) / (kb(nr)*kb(nr)*dens(nr))

	tt(iw) = (cw*dk)/vs1

c	write(6,*) "iw,vs1,dens1,vp1,vp(nr),vs(nr),dens(nr),tt=",
c     +    iw,vs1,dens1,vp1,vp(nr),vs(nr),dens(nr),tt(iw)

	zss(iw) = 0.5*( -S0(3) + S0(4) )     *src(iw)*tt(iw)*df
	zds(iw) = S0(1)                      *src(iw)*tt(iw)*df
	zdd(iw) = (-S0(2)+0.5*(S0(3)+S0(4))) *src(iw)*tt(iw)*df
*	rss(iw) =  0.5*( S1(4) - S1(5) )     *src(iw)*tt(iw)*df
*	rds(iw) = -S1(1)                     *src(iw)*tt(iw)*df
*	rdd(iw) =  (S1(3)-0.5*(S1(4)+S1(5))) *src(iw)*tt(iw)*df
        rss(iw) = -0.5*( S1(4) - S1(5) )     *src(iw)*tt(iw)*df
        rds(iw) = +S1(1)                     *src(iw)*tt(iw)*df
        rdd(iw) = -(S1(3)-0.5*(S1(4)+S1(5))) *src(iw)*tt(iw)*df
*       tss(iw) =  S1(6) *src(iw)*tt(iw)*df
*       tds(iw) = -S1(2) *src(iw)*tt(iw)*df
	tss(iw) = -S1(6) *src(iw)*tt(iw)*df
	tds(iw) = +S1(2) *src(iw)*tt(iw)*df
***
*** see note from yuehua zeng below
*** EXPLOSION SOURCES G.A.Ichinose 2017DEC29 - similar for above, div out mu
***                                              rigidity added later
***          most codes use 1.0E+20 dyne*cm we use 1.2445146117e+16
***          so the ratio of the moments is 1.0E+20 / 1.2445146117e+16 = 8035
***          this is the mulitplication factor needed to compare Green functions
**** 20201016 gichinose
****
c tmp1=cw*slip4pi*dk*vs1*dens1/(3.0*dens(nr)/ka(nr)-4.0*mu(nr))
c
c	tt(iw) = (cw*dk)*( 0.40 * dens1 / ( vp1 * vs1 ) )
	
c	tt(iw) = (cw*dk)/(vp1*dens1)
c	mu = kb(nr)*kb(nr)*dens(nr)

	tt(iw) = (cw*dk)/(vp1*dens1)

********************************************************************************
*        1         2         3         4         5         6         7         8
*2345678901234567890123456789012345678901234567890123456789012345678901234567890
********************************************************************************
**	write(6,*)"iw,nr,ns,tt(iw)=",iw,nr,ns,tt(iw)
	
        rep(iw) = -(S1(3)+S1(4)+S1(5)) * src(iw)*tt(iw)*df
	zep(iw) = -(S0(2)+S0(3)+S0(4)) * src(iw)*tt(iw)*df
	
******* note from yuehua zeng 
*tmp=cw*slip4pi*dk*vs1*dens1/mu(nr)
*zss=0.5*(-S0(3)+S0(4))*tmp
*zds=-S0(1)*tmp
*zdd=(-S0(2)+0.5(S0(3)+S0(4)))*tmp
*rss=0.5*(S1(4)-S1(5))*tmp
*rds=S1(1)*tmp
*rdd=(S1(3)-0.5*(S1(4)+s1(5)))*tmp
*tss=-S1(6)*tmp
*tds=-S1(2)*tmp
*
*tmp1=cw*slip4pi*dk*vs1*dens1/(3.0*dens(nr)/ka(nr)-4.0*mu(nr))
*rep=-(S1(3)+S1(4)+S1(5))*tmp1
*zep=(S0(2)+S0(3)+S0(4))*tmp1
****************************************************************************
	if( iverbose .eq. 1 )then
7676	format( a13,i4,1x,i4,1x,f8.4,1x,i8,1x,2(e14.5,1x))
	write(6,7676)'nw,iw,freq,k,rss=',nw,iw,(iw-1)*df,k,rss(iw)
	endif

	k1=k
180 	continue

** rss
	k=nt+2
	do j=1,nw
	  au(j)=rss(j)
	  au(k-j)=dconjg(rss(j))
	enddo
	au(nw+1)=zero
	call fork(nt,au,1.)
	t=0.
	do j=1,nt
	  t=t+dt
          g01(j) = real(dble(au(j)))*exp(-wi*t)
	enddo

** rds
	k=nt+2
        do j=1,nw
          au(j)=rds(j)
          au(k-j)=dconjg(rds(j))
	enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g02(j) = real(dble(au(j)))*exp(-wi*t)
	enddo

** rdd
	k=nt+2
	do j=1,nw
          au(j)=rdd(j)
          au(k-j)=dconjg(rdd(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g03(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** rep 
	k=nt+2
	do j=1,nw
          au(j)=rep(j)
          au(k-j)=dconjg(rep(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g04(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** zss
	k=nt+2
	do j=1,nw
          au(j)=zss(j)
          au(k-j)=dconjg(zss(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g05(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** zds
	k=nt+2
	do j=1,nw
          au(j)=zds(j)
          au(k-j)=dconjg(zds(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g06(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** zdd
	k=nt+2
	do j=1,nw
          au(j)=zdd(j)
          au(k-j)=dconjg(zdd(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g07(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** zep
	k=nt+2
	do j=1,nw
          au(j)=zep(j)
          au(k-j)=dconjg(zep(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g08(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** tss
	k=nt+2
	do j=1,nw
          au(j)=tss(j)
          au(k-j)=dconjg(tss(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g09(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

** tds
	k=nt+2
	do j=1,nw
          au(j)=tds(j)
          au(k-j)=dconjg(tds(j))
        enddo
        au(nw+1)=zero
        call fork(nt,au,1.)
        t=0.
        do j=1,nt
          t=t+dt
          g10(j) = real(dble(au(j)))*exp(-wi*t)
        enddo

	return
	end
