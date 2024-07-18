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
################## ARLS ista = 1 ########################
###
gmt psbasemap -R0/54/-1462/1921 -JX1.5i/0.75i -Bxf14a54+l"sec" -Byf480a1921+l"nm" -BS -P -K -Vq >! gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        ARLS.KR.D.000.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        ARLS.KR.D.000.z.syn.xy >> gmtwf.000.ps
echo "0 1921 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   ARLS.KR.D.000.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         ARLS.KR.D.000.r.syn.xy >> gmtwf.000.ps
echo "0 1921 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   ARLS.KR.D.000.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         ARLS.KR.D.000.t.syn.xy >> gmtwf.000.ps
echo "0 1921 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KR.ARLS.--
1.5 0.6 @~D@~=80 @%7% km @%%
1.5 0.4 @~f@~=80@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 3.22e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## EKS2 ista = 2 ########################
###
gmt psbasemap -R0/54/-1085/1158 -JX1.5i/0.75i -Bxf14a54 -Byf290a1158+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        EKS2.KN.D.001.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        EKS2.KN.D.001.z.syn.xy >> gmtwf.000.ps
echo "0 1158 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   EKS2.KN.D.001.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         EKS2.KN.D.001.r.syn.xy >> gmtwf.000.ps
echo "0 1158 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   EKS2.KN.D.001.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         EKS2.KN.D.001.t.syn.xy >> gmtwf.000.ps
echo "0 1158 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.EKS2.--
1.5 0.6 @~D@~=108 @%7% km @%%
1.5 0.4 @~f@~=18@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.14e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## UCH ista = 3 ########################
###
gmt psbasemap -R0/54/-1498/1400 -JX1.5i/0.75i -Bxf14a54 -Byf350a1400+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        UCH.KN.D.002.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        UCH.KN.D.002.z.syn.xy >> gmtwf.000.ps
echo "0 1400 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   UCH.KN.D.002.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         UCH.KN.D.002.r.syn.xy >> gmtwf.000.ps
echo "0 1400 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   UCH.KN.D.002.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         UCH.KN.D.002.t.syn.xy >> gmtwf.000.ps
echo "0 1400 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.UCH.--
1.5 0.6 @~D@~=108 @%7% km @%%
1.5 0.4 @~f@~=60@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.76e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## KBK ista = 4 ########################
###
gmt psbasemap -R0/75/-1120/1153 -JX1.5i/0.75i -Bxf19a75 -Byf288a1153+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        KBK.KN.D.003.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        KBK.KN.D.003.z.syn.xy >> gmtwf.000.ps
echo "0 1153 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   KBK.KN.D.003.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         KBK.KN.D.003.r.syn.xy >> gmtwf.000.ps
echo "0 1153 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   KBK.KN.D.003.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         KBK.KN.D.003.t.syn.xy >> gmtwf.000.ps
echo "0 1153 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.KBK.--
1.5 0.6 @~D@~=165 @%7% km @%%
1.5 0.4 @~f@~=51@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.16e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## CHM ista = 5 ########################
###
gmt psbasemap -R0/75/-853/882 -JX1.5i/0.75i -Bxf19a75 -Byf221a882+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        CHM.KN.D.004.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        CHM.KN.D.004.z.syn.xy >> gmtwf.000.ps
echo "0 882 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CHM.KN.D.004.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CHM.KN.D.004.r.syn.xy >> gmtwf.000.ps
echo "0 882 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   CHM.KN.D.004.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         CHM.KN.D.004.t.syn.xy >> gmtwf.000.ps
echo "0 882 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.CHM.--
1.5 0.6 @~D@~=180 @%7% km @%%
1.5 0.4 @~f@~=38@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.65e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## USP ista = 6 ########################
###
gmt psbasemap -R0/86/-1005/731 -JX1.5i/0.75i -Bxf22a86 -Byf183a731+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        USP.KN.D.005.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        USP.KN.D.005.z.syn.xy >> gmtwf.000.ps
echo "0 731 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   USP.KN.D.005.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         USP.KN.D.005.r.syn.xy >> gmtwf.000.ps
echo "0 731 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   USP.KN.D.005.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         USP.KN.D.005.t.syn.xy >> gmtwf.000.ps
echo "0 731 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.USP.--
1.5 0.6 @~D@~=193 @%7% km @%%
1.5 0.4 @~f@~=28@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.65e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## TKM2 ista = 7 ########################
###
gmt psbasemap -R0/108/-892/888 -JX1.5i/0.75i -Bxf27a108 -Byf222a888+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        TKM2.KN.D.006.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        TKM2.KN.D.006.z.syn.xy >> gmtwf.000.ps
echo "0 888 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   TKM2.KN.D.006.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         TKM2.KN.D.006.r.syn.xy >> gmtwf.000.ps
echo "0 888 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   TKM2.KN.D.006.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         TKM2.KN.D.006.t.syn.xy >> gmtwf.000.ps
echo "0 888 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.TKM2.--
1.5 0.6 @~D@~=225 @%7% km @%%
1.5 0.4 @~f@~=53@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.70e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

###
################## ULHL ista = 8 ########################
###
gmt psbasemap -R0/118/-1108/952 -JX1.5i/0.75i -Bxf30a118 -Byf238a952+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        ULHL.KN.D.007.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        ULHL.KN.D.007.z.syn.xy >> gmtwf.000.ps
echo "0 952 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   ULHL.KN.D.007.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         ULHL.KN.D.007.r.syn.xy >> gmtwf.000.ps
echo "0 952 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   ULHL.KN.D.007.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         ULHL.KN.D.007.t.syn.xy >> gmtwf.000.ps
echo "0 952 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 KN.ULHL.--
1.5 0.6 @~D@~=243 @%7% km @%%
1.5 0.4 @~f@~=76@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.96e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.03_0.10Hz
EOF

####
################## mechanism ########################
####
gmt psmeca -R0/10/0/10 -JX1.5i/1.5i -Sm1i/0 -T0/1p,black -W1p,black -Ggray -N -X+2.15i -O -K -Vq >> gmtwf.000.ps << EOF
5 5 1 2.9129 0.835904 1.16456 -0.298457 -0.00272366 -0.0810381 22 x y 2009/12/22T05:54:35.00 Kambrata, Kyrgyzstan
EOF

###
### MOMENT TENSOR/FOCAL MECHANISM INFORMATION LABEL 
###
gmt pstext -R0/10/0/10 -JX1.5i/1.8i -D0i/0i -F+jML+f10p,Palatino-Bold,black -N -O -Vq -X-1i -Y-1.5i >> gmtwf.000.ps << EOF
9 9 Kambrata, Kyrgyzstan
9 8 2009/12/22T05:54:35.00
9 7 Depth =   1.0 @%7%km@%%
9 6 M@-w@- =  4.25
9 5 M@-0@- =  2.95@%12%\264@%%10@+15@+ N@%12%\264@%%m
9 4 DC=14% CLVD=31% ISO=55%
9 3 VR=  74.5%
9  2 M@-xx@-=+0.84 M@-xy@-=+0.08
9  1 M@-xz@-=-0.30 M@-yy@-=+1.16
9  0 M@-yz@-=+0.00 M@-zz@-=+2.91
9 -1 Lune @%12%d@%%=59.99 @%12%g@%%=-19.95
EOF

# gs gmtwf.000.ps
ps2pdf gmtwf.000.ps
gmt psconvert -Tj -E600 -A -Vq gmtwf.000.ps
# open gmtwf.000.jpg
