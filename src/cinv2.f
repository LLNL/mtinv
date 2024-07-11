c.................................................................
c numerical recipes (https://numerical.recipes) used by greensf.f
c.................................................................
c.................................................................
c called by greensf.f that is used in mkgrnlib.c
c.................................................................
c
        subroutine cinv2(a,ainv)
        complex*16 a(2,2),ainv(2,2),det,one
        one=cmplx(1.0,0.0)
        a(1,1)=one+a(1,1)
        a(2,2)=one+a(2,2)
        det=a(1,1)*a(2,2)-a(1,2)*a(2,1)
        ainv(1,1)=a(2,2)/det
        ainv(1,2)=-a(1,2)/det
        ainv(2,1)=-a(2,1)/det
        ainv(2,2)=a(1,1)/det
        return
        end

