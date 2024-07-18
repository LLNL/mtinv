#!/bin/csh

###
### compare rbh with y.zeng's Gfs
###

# /Users/ichinose1/Work/mtinv4_sampledata/compare_gfs

# foreach depth ( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 )
foreach depth ( 2 14 )

#### this is for Tennesse mine collapse
# compare_grn_gmt glib1=dev/hspec96_GFs/IM.TKL..cus.glib    glib2=dev/IM.TKL..cus.glib    z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.T50A.00.cus.glib glib2=dev/N4.T50A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.U49A.00.cus.glib glib2=dev/N4.U49A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.V48A.00.cus.glib glib2=dev/N4.V48A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.V53A.00.cus.glib glib2=dev/N4.V53A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.W50A.00.cus.glib glib2=dev/N4.W50A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.W52A.00.cus.glib glib2=dev/N4.W52A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/N4.X51A.00.cus.glib glib2=dev/N4.X51A.00.cus.glib z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/US.TZTN.00.cus.glib glib2=dev/US.TZTN.00.cus.glib z=${depth}


# compare_grn_gmt glib1=dev/hspec96_GFs/IU.CCM.00.cus.glib  glib2=dev/IU.CCM.00.cus.glib   z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/IU.CCM.10.cus.glib  glib2=dev/IU.CCM.10.cus.glib   z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/IU.WVT.00.cus.glib  glib2=dev/IU.WVT.00.cus.glib   z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/IU.WVT.10.cus.glib  glib2=dev/IU.WVT.10.cus.glib   z=${depth}
compare_grn_gmt glib1=dev/hspec96_GFs/NM.CGM3.00.cus.glib glib2=dev/NM.CGM3.00.cus.glib  z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/NM.GNAR.00.cus.glib glib2=dev/NM.GNAR.00.cus.glib  z=${depth}
compare_grn_gmt glib1=dev/hspec96_GFs/NM.HENM.00.cus.glib glib2=dev/NM.HENM.00.cus.glib  z=${depth}
compare_grn_gmt glib1=dev/hspec96_GFs/NM.PENM.00.cus.glib glib2=dev/NM.PENM.00.cus.glib  z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/NM.SIUC.00.cus.glib glib2=dev/NM.SIUC.00.cus.glib  z=${depth}
# compare_grn_gmt glib1=dev/hspec96_GFs/NM.SLM.00.cus.glib  glib2=dev/NM.SLM.00.cus.glib   z=${depth}

end
