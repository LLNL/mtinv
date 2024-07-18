#!/bin/csh
### 
### gmtwf.csh - GMT 5.x.x script to make pretty publication quality figures!
### 
gmt set PS_PAGE_ORIENTATION        portrait  
gmt set MAP_ANNOT_OFFSET_PRIMARY   2p         
gmt set MAP_ANNOT_OFFSET_SECONDARY 2p      
gmt set MAP_LABEL_OFFSET           0p       
gmt set PS_MEDIA                   letter  
gmt set FONT_ANNOT_PRIMARY          9p,Helvetica,black   
gmt set FONT_ANNOT_SECONDARY        9p,Helvetica,black  
gmt set FONT_LABEL                  9p,Palatino-Bold,black 
gmt set FONT_LOGO                   9p,Helvetica,black     
gmt set FONT_TITLE                 12p,Times-Bold,black 


#########################################################
################## NEW PAGE   0 ########################
#########################################################

###
################## CGM3 ista = 1 ########################
###
gmt psbasemap -R0/54/-2340/2012 -JX1.5i/0.75i -Bxf14a54+l"sec" -Byf503a2012+l"nm" -BS -P -K -Vq >! gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        CGM3.NM.D.000.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        CGM3.NM.D.000.z.syn.xy >> gmtwf.000.ps
echo "0 2012 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CGM3.NM.D.000.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CGM3.NM.D.000.r.syn.xy >> gmtwf.000.ps
echo "0 2012 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CGM3.NM.D.000.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CGM3.NM.D.000.t.syn.xy >> gmtwf.000.ps
echo "0 2012 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 NM.CGM3.00
1.5 0.6 @~D@~=90 @%7% km @%%
1.5 0.4 @~f@~=61@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 4.14e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## PENM ista = 2 ########################
###
gmt psbasemap -R0/54/-3480/4189 -JX1.5i/0.75i -Bxf14a54 -Byf1047a4189+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        PENM.NM.D.001.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        PENM.NM.D.001.z.syn.xy >> gmtwf.000.ps
echo "0 4189 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   PENM.NM.D.001.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         PENM.NM.D.001.r.syn.xy >> gmtwf.000.ps
echo "0 4189 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   PENM.NM.D.001.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         PENM.NM.D.001.t.syn.xy >> gmtwf.000.ps
echo "0 4189 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 NM.PENM.00
1.5 0.6 @~D@~=96 @%7% km @%%
1.5 0.4 @~f@~=122@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 7.30e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## HENM ista = 3 ########################
###
gmt psbasemap -R0/54/-2608/3293 -JX1.5i/0.75i -Bxf14a54 -Byf823a3293+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        HENM.NM.D.002.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        HENM.NM.D.002.z.syn.xy >> gmtwf.000.ps
echo "0 3293 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   HENM.NM.D.002.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         HENM.NM.D.002.r.syn.xy >> gmtwf.000.ps
echo "0 3293 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   HENM.NM.D.002.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         HENM.NM.D.002.t.syn.xy >> gmtwf.000.ps
echo "0 3293 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 NM.HENM.00
1.5 0.6 @~D@~=98 @%7% km @%%
1.5 0.4 @~f@~=102@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 5.62e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## GNAR ista = 4 ########################
###
gmt psbasemap -R0/54/-2076/1941 -JX1.5i/0.75i -Bxf14a54 -Byf485a1941+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        GNAR.NM.D.003.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        GNAR.NM.D.003.z.syn.xy >> gmtwf.000.ps
echo "0 1941 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   GNAR.NM.D.003.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         GNAR.NM.D.003.r.syn.xy >> gmtwf.000.ps
echo "0 1941 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   GNAR.NM.D.003.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         GNAR.NM.D.003.t.syn.xy >> gmtwf.000.ps
echo "0 1941 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 NM.GNAR.00
1.5 0.6 @~D@~=115 @%7% km @%%
1.5 0.4 @~f@~=156@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 3.83e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## CCM ista = 6 ########################
###
gmt psbasemap -R0/75/-1350/1120 -JX1.5i/0.75i -Bxf19a75 -Byf280a1120+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        CCM.IU.D.005.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        CCM.IU.D.005.z.syn.xy >> gmtwf.000.ps
echo "0 1120 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CCM.IU.D.005.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CCM.IU.D.005.r.syn.xy >> gmtwf.000.ps
echo "0 1120 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CCM.IU.D.005.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CCM.IU.D.005.t.syn.xy >> gmtwf.000.ps
echo "0 1120 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 IU.CCM.10
1.5 0.6 @~D@~=142 @%7% km @%%
1.5 0.4 @~f@~=334@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.35e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## CCM ista = 5 ########################
###
gmt psbasemap -R0/75/-1332/1120 -JX1.5i/0.75i -Bxf19a75 -Byf280a1120+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        CCM.IU.D.004.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        CCM.IU.D.004.z.syn.xy >> gmtwf.000.ps
echo "0 1120 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CCM.IU.D.004.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CCM.IU.D.004.r.syn.xy >> gmtwf.000.ps
echo "0 1120 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CCM.IU.D.004.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CCM.IU.D.004.t.syn.xy >> gmtwf.000.ps
echo "0 1120 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 IU.CCM.00
1.5 0.6 @~D@~=142 @%7% km @%%
1.5 0.4 @~f@~=334@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.33e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## SIUC ista = 7 ########################
###
gmt psbasemap -R0/75/-2058/1891 -JX1.5i/0.75i -Bxf19a75 -Byf473a1891+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        SIUC.NM.D.006.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        SIUC.NM.D.006.z.syn.xy >> gmtwf.000.ps
echo "0 1891 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   SIUC.NM.D.006.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         SIUC.NM.D.006.r.syn.xy >> gmtwf.000.ps
echo "0 1891 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   SIUC.NM.D.006.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         SIUC.NM.D.006.t.syn.xy >> gmtwf.000.ps
echo "0 1891 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 NM.SIUC.00
1.5 0.6 @~D@~=148 @%7% km @%%
1.5 0.4 @~f@~=52@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 3.76e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

###
################## SLM ista = 8 ########################
###
gmt psbasemap -R0/86/-1838/1813 -JX1.5i/0.75i -Bxf22a86 -Byf453a1813+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        SLM.NM.D.007.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        SLM.NM.D.007.z.syn.xy >> gmtwf.000.ps
echo "0 1813 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   SLM.NM.D.007.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         SLM.NM.D.007.r.syn.xy >> gmtwf.000.ps
echo "0 1813 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   SLM.NM.D.007.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         SLM.NM.D.007.t.syn.xy >> gmtwf.000.ps
echo "0 1813 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 NM.SLM.00
1.5 0.6 @~D@~=194 @%7% km @%%
1.5 0.4 @~f@~=8@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 3.48e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.15Hz
EOF

####
################## mechanism ########################
####
gmt psmeca -R0/10/0/10 -JX1.5i/1.5i -Sm1i/0 -T0/1p,black -W1p,black -Ggray -N -X+2.15i -O -K -Vq >> gmtwf.000.ps << EOF
5 5 14 -0.0575327 1.28692 -0.731584 0.440394 -0.420725 0.897927 22 x y 2021/11/18T02:53:04.00 New Madrid, MO
EOF

###
### MOMENT TENSOR/FOCAL MECHANISM INFORMATION LABEL 
###
gmt pstext -R0/10/0/10 -JX1.5i/1.8i -D0i/0i -F+jML+f10p,Palatino-Bold,black -N -O -Vq -X-1i -Y-1.5i >> gmtwf.000.ps << EOF
9 9 New Madrid, MO
9 8 2021/11/18T02:53:04.00
9 7 Depth =  14.0 @%7%km@%%
9 6 M@-w@- =  4.09
9 5 M@-0@- =  1.67@%12%\264@%%10@+15@+ N@%12%\264@%%m
9 4 DC=88% CLVD=3% ISO=10%
9 3 VR=  86.5%
9  2 M@-xx@-=+1.29 M@-xy@-=-0.90
9  1 M@-xz@-=+0.44 M@-yy@-=-0.73
9  0 M@-yz@-=+0.42 M@-zz@-=-0.06
9 -1 Lune @%12%d@%%=7.74 @%12%g@%%=-0.71
EOF

# gs gmtwf.000.ps
ps2pdf gmtwf.000.ps
gmt psconvert -Tj -E600 -A -Vq gmtwf.000.ps
# open gmtwf.000.jpg
