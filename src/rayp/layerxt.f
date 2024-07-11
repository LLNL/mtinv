c LAYERXT calculates dx and dt for ray in layer with linear velocity gradient
c
c Inputs:   p     =  horizontal slowness
c           h     =  layer thickness
c           utop  =  slowness at top of layer
c           ubot  =  slowness at bottom of layer
c Returns:  dx    =  range offset
c           dt    =  travel time
c           irtr  =  return code
c                 = -1,  zero thickness layer
c                 =  0,  ray turned above layer
c                 =  1,  ray passed through layer
c                 =  2,  ray turned within layer, 1 segment counted in dx,dt
c
      subroutine LAYERXT(p,h,utop,ubot,dx,dt,irtr)

      if (p.ge.utop) then      !ray turned above layer
         dx=0.
         dt=0.
         irtr=0
         return     
      else if (h.eq.0.) then        !zero thickness layer
         dx=0.
         dt=0.
         irtr=-1
         return         
      end if

      u1=utop
      u2=ubot
      v1=1./u1
      v2=1./u2
      b=(v2-v1)/h             !slope of velocity gradient

      eta1=sqrt(u1**2-p**2)

      if (b.eq.0.) then       !constant velocity layer
         dx=h*p/eta1
         dt=h*u1**2/eta1
         irtr=1
         return
      end if

      x1=eta1/(u1*b*p)
      tau1=(alog((u1+eta1)/p)-eta1/u1)/b

      if (p.ge.ubot) then     !ray turned within layer,
         dx=x1                !no contribution to integral
         dtau=tau1            !from bottom point
         dt=dtau+p*dx
         irtr=2
         return
      end if

      irtr=1

      eta2=sqrt(u2**2-p**2)
      x2=eta2/(u2*b*p)
      tau2=(alog((u2+eta2)/p)-eta2/u2)/b

      dx=x1-x2
      dtau=tau1-tau2

      dt=dtau+p*dx

      return
      end
