c.................................................................
c called by greensf.f that is used in mkgrnlib.c
c.................................................................
c.................................................................
c
        subroutine genrefl(ak)
c
c  Purpose : compute the generalized reflection and transmission
c            coefficients
********************************************************************************
*        1         2         3         4         5         6         7         8
*2345678901234567890123456789012345678901234567890123456789012345678901234567890
********************************************************************************
	integer*4 MXL
	parameter( MXL = 1024 )
        real*8 dens(MXL)
        logical up
        complex*16 ka(MXL),kb(MXL),nu(MXL),ga(MXL),kbeta(MXL),z(MXL),
     +         I11(2,2),I12(2,2),I21(2,2),I22(2,2),I31(2,2),I32(2,2),
     +         Ish11,Ish12,Ish21,Ish22,Ish31,Ish32,aux(2,2),ainv(2,2),
     +         Td(MXL,2,2),Tu(MXL,2,2),Rd(MXL,2,2),Ru(MXL,2,2),
     +         HTd(MXL,2,2),HTu(MXL,2,2),HRd(MXL,2,2),HRu(0:MXL,2,2),
     +         Tdsh(MXL),Tush(MXL),Rdsh(MXL),Rush(MXL),
     +         HTdsh(MXL),HTush(MXL),HRdsh(MXL),HRush(0:MXL),
     +         det,a1,a2,a3,a4,a5,a6,a7,a8,exd(MXL,2),exu(0:MXL,2),
     +         dj,djj,d1j,d1jj,d2j,d2jj,d3,d4,d5,d6,d7,d8,d9,d10,d11,
     +         d12,d13,d14,d15,d16,d17,d18,d19,d20,r1,r2,ak,ak2,a2k,
     +         a2k2,one,zero,rdepn,sdepn
        common/para/one,zero,ga,nu,kbeta,ka,kb,dens,z,I11,I12,
     +              I21,I22,I31,I32,Ish11,Ish12,Ish21,Ish22,
     +              Ish31,Ish32,sdepn,rdepn,up,nlay,ns,nr
        common/matrix/Td,Tu,Rd,Ru,Tdsh,Tush,Rdsh,Rush,exd,exu,
     +                HTd,HTu,HRd,HRu,HTdsh,HTush,HRdsh,HRush
c
c  motion-stress matrices
        ak2=ak*ak
        a2k=ak+ak
        a2k2=ak2+ak2
c
c  p-sv wave
        a1=ak
        a2=ga(nr)
        a3=nu(nr)
        a4=2.0*ak*nu(nr)/(kb(nr)*dens(nr))
        a5=kbeta(nr)/dens(nr)
        a6=2.0*ak*ga(nr)/(kb(nr)*dens(nr))
        a8=2.0*ka(nr)/kb(nr)-one
        a7=(2.0*a8-a2k2/kb(nr))/dens(nr)
        a8=a7-a8/dens(nr)
        if(up)then
          if(nr.eq.ns)then
            d3=exp(-nu(nr)*(rdepn-sdepn))
            d4=exp(-ga(nr)*(rdepn-sdepn))
          else
            d3=exp(-nu(nr)*rdepn)
            d4=exp(-ga(nr)*rdepn)
          endif
          d5=exp(-nu(nr)*(z(nr)-rdepn))
          d6=exp(-ga(nr)*(z(nr)-rdepn))
        else
          d3=exp(-nu(nr)*rdepn)
          d4=exp(-ga(nr)*rdepn)
          if(nr.eq.ns)then
            d5=exp(-nu(nr)*(sdepn-rdepn))
            d6=exp(-ga(nr)*(sdepn-rdepn))
          else
            d5=exp(-nu(nr)*(z(nr)-rdepn))
            d6=exp(-ga(nr)*(z(nr)-rdepn))
          endif
        endif
c
        I11(1,1)=-a1*d3
        I11(1,2)=a2*d4
        I11(2,1)=-a3*d3
        I11(2,2)=a1*d4
        I12(1,1)=-a1*d5
        I12(1,2)=a2*d6
        I12(2,1)=a3*d5
        I12(2,2)=-a1*d6
        I21(1,1)=a4*d3
        I21(1,2)=-a5*d4
        I21(2,1)=a5*d3
        I21(2,2)=-a6*d4
        I22(1,1)=-a4*d5
        I22(1,2)=a5*d6
        I22(2,1)=a5*d5
        I22(2,2)=-a6*d6
        I31(1,1)=a7*d3
        I31(1,2)=a6*d4
        I31(2,1)=a8*d3
        I31(2,2)=a6*d4
        I32(1,1)=a7*d5
        I32(1,2)=a6*d6
        I32(2,1)=a8*d5
        I32(2,2)=a6*d6
c
c  sh wave
        a6=ga(nr)/(kb(nr)*dens(nr))
        Ish11=d4
        Ish12=d6
        Ish21=-a6*d4
        Ish22=a6*d6
        a6=ak/(kb(nr)*dens(nr))
        Ish31=a6*d4
        Ish32=a6*d6
c
        djj=nu(1)*ga(1)
        d1jj=ak2+djj
        d2jj=ak2-djj
        d3=a2k2-kb(1)
        d4=d3*d3
        d7=4.*ak2*djj
        d5=-1./(d4-d7)
        d6=d4+d7
        d7=4.*ak*d3*d5
c
c reflection up at free surface
c
c  p-sv wave
        HRu(0,1,1)=d5*d6*exu(0,1)
        HRu(0,1,2)=-d7*ga(1)*exu(0,2)
        HRu(0,2,1)= d7*nu(1)*exu(0,1)
        HRu(0,2,2)=-d5*d6*exu(0,2)
c
c  sh wave
        HRush(0)=exu(0,2)
c
c  modified reflection and transmission coefficients
        do 10 j=1,nlay-1
c
c  p-sv wave
          jj=j+1
          dj=djj
          djj=nu(jj)*ga(jj)
          d1j=d1jj
          d1jj=ak2+djj
          d2j=d2jj
          d2jj=ak2-djj
          d3=dens(j)*nu(j)
          d4=dens(jj)*nu(jj)
          d5=dens(j)*ga(j)
          d6=dens(jj)*ga(jj)
          d7=d3+d4
          d8=d5+d6
          d9=d8*d7
          d10=d8*(d4-d3)
          d11=(d6-d5)*d7
          d12=2.*(dens(j)/kb(jj)-dens(jj)/kb(j))
          d13=ak*d12
          d14=ak*d13
          d15=a2k*d13
          d16=d14*d12
          r1=dens(jj)-dens(j)
          r2=ak2*r1*r1
          det=1./(d16*d2jj*d2j+d15*(dens(jj)*d2jj-dens(j)*d2j)+(r2-d9))
c
          d17=d16*d2jj*d1j+d15*(dens(jj)*d2jj-dens(j)*d1j)
          d18=a2k*(-d16*d2jj+d12*(dens(j)*a2k2-dens(jj)*d2jj)
     +        +dens(j)*r1)
          Rd(j,1,1)=-det*(d17+r2-d10)*exd(j,1)
          Rd(j,1,2)=-det*ga(j)*d18*exd(j,2)
          Rd(j,2,1)= det*nu(j)*d18*exd(j,1)
          Rd(j,2,2)= det*(d17+r2-d11)*exd(j,2)
c
          d17=-2.*(d14*(ga(jj)-ga(j))+d8)
          d18=-a2k*(d12*(ak2-nu(j)*ga(jj))+r1)
          d19=-a2k*(d12*(ak2-nu(jj)*ga(j))+r1)
          d20=-2.*(d14*(nu(jj)-nu(j))+d7)
          d3=dens(jj)*nu(j)
          d4=dens(j)*nu(jj)
          d5=dens(jj)*ga(j)
          d6=dens(j)*ga(jj)
          Td(j,1,1)=det*d3*d17*exd(j,1)
          Td(j,1,2)=det*d5*d18*exd(j,2)
          Td(j,2,1)=det*d3*d19*exd(j,1)
          Td(j,2,2)=det*d5*d20*exd(j,2)
c
          Tu(j,1,1)= det*d4*d17*exu(j,1)
          Tu(j,1,2)=-det*d6*d19*exu(j,2)
          Tu(j,2,1)=-det*d4*d18*exu(j,1)
          Tu(j,2,2)= det*d6*d20*exu(j,2)
c
          d17=d16*d2j*d1jj-d15*(dens(j)*d2j-dens(jj)*d1jj)
          d18=a2k*(d16*d2j+d12*(dens(jj)*a2k2-dens(j)*d2j)+dens(jj)*r1)
          Ru(j,1,1)= -det*(d17+r2+d10)*exu(j,1)
          Ru(j,1,2)= det*ga(jj)*d18*exu(j,2)
          Ru(j,2,1)=-det*nu(jj)*d18*exu(j,1)
          Ru(j,2,2)= det*(d17+r2+d11)*exu(j,2)
c
c  sh wave
          d17=ga(j)*kb(jj)*dens(jj)
          d18=ga(jj)*kb(j)*dens(j)
          det=1./(d17+d18)
          Rush(j)=(d18-d17)*det
          Tush(j)=2.*d18*det*exu(j,2)
          Rdsh(j)=-Rush(j)*exd(j,2)
          Tdsh(j)=2.*d17*det*exd(j,2)
          Rush(j)=Rush(j)*exu(j,2)
 10     continue
c
c  generalized reflection and transmission coefficients for
c  layers above the source.
c
c  p-sv wave
c
        do 60 n=1,ns-1
          do 20 i=1,2
          do 21 j=1,2
            aux(i,j)=zero
            do 22 k=1,2
               aux(i,j)=aux(i,j)-Rd(n,i,k)*HRu(n-1,k,j)
 22         continue
 21       continue
 20     continue

          call cinv2(aux,ainv)

          do 30 i=1,2
            do 31 j=1,2
              HTu(n,i,j)=zero
              do 32 k=1,2
                HTu(n,i,j)=HTu(n,i,j)+ainv(i,k)*Tu(n,k,j)
 32           continue
 31         continue
 30       continue

          do 50 i=1,2
          do 51 j=1,2
            HRu(n,i,j)=Ru(n,i,j)
            do 40 k=1,2
             do 41 l=1,2
               HRu(n,i,j)=HRu(n,i,j)+Td(n,i,k)*HRu(n-1,k,l)*HTu(n,l,j)
 41          continue
 40         continue
 51       continue
 50       continue

c
c  sh wave
c
          HTush(n)=Tush(n)/(1-Rdsh(n)*HRush(n-1))
          HRush(n)=Rush(n)+Tdsh(n)*HRush(n-1)*HTush(n)
c
 60     continue
c
c  generalized reflection and transmission coefficients for
c  layers below the source.
        HRd(nlay,1,1)=zero
        HRd(nlay,1,2)=zero
        HRd(nlay,2,1)=zero
        HRd(nlay,2,2)=zero
        HRdsh(nlay)=zero
c
        do 100 n=nlay-1,ns,-1
c
c  p-sv wave
        do 70 i=1,2
        do 71 j=1,2
          aux(i,j)=zero
          do 72 k=1,2
            aux(i,j)=aux(i,j)-Ru(n,i,k)*HRd(n+1,k,j)
 72       continue
 71     continue
 70     continue

        call cinv2(aux,ainv)
        do 80 i=1,2
        do 81 j=1,2
          HTd(n,i,j)=zero
          do 82 k=1,2
            HTd(n,i,j)=HTd(n,i,j)+ainv(i,k)*Td(n,k,j)
 82       continue
 81     continue
 80     continue

        do 90 i=1,2
        do 91 j=1,2
          HRd(n,i,j)=Rd(n,i,j)
          do 92 k=1,2
          do 93 l=1,2
            HRd(n,i,j)=HRd(n,i,j)+Tu(n,i,k)*HRd(n+1,k,l)*HTd(n,l,j)
 93     continue
 92     continue
 91     continue
 90     continue


c
c  sh wave
        HTdsh(n)=Tdsh(n)/(1-Rush(n)*HRdsh(n+1))
        HRdsh(n)=Rdsh(n)+Tush(n)*HRdsh(n+1)*HTdsh(n)
c
100     continue
c
        return
        end
