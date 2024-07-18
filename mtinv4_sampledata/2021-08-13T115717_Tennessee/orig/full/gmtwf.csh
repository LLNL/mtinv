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
################## W50A ista = 1 ########################
###
gmt psbasemap -R0/54/-1149/1184 -JX1.5i/0.75i -Bxf14a54+l"sec" -Byf296a1184+l"nm" -BS -P -K -Vq >! gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        W50A.N4.D.000.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        W50A.N4.D.000.z.syn.xy >> gmtwf.000.ps
echo "0 1184 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   W50A.N4.D.000.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         W50A.N4.D.000.r.syn.xy >> gmtwf.000.ps
echo "0 1184 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   W50A.N4.D.000.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         W50A.N4.D.000.t.syn.xy >> gmtwf.000.ps
echo "0 1184 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 N4.W50A.00
1.5 0.6 @~D@~=84 @%7% km @%%
1.5 0.4 @~f@~=207@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.22e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## TKL ista = 2 ########################
###
gmt psbasemap -R0/54/-1056/1045 -JX1.5i/0.75i -Bxf14a54 -Byf261a1045+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        TKL.IM.D.001.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        TKL.IM.D.001.z.syn.xy >> gmtwf.000.ps
echo "0 1045 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   TKL.IM.D.001.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         TKL.IM.D.001.r.syn.xy >> gmtwf.000.ps
echo "0 1045 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   TKL.IM.D.001.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         TKL.IM.D.001.t.syn.xy >> gmtwf.000.ps
echo "0 1045 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 IM.TKL.--
1.5 0.6 @~D@~=104 @%7% km @%%
1.5 0.4 @~f@~=103@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.00e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## U49A ista = 3 ########################
###
gmt psbasemap -R0/54/-1011/947 -JX1.5i/0.75i -Bxf14a54 -Byf237a947+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        U49A.N4.D.002.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        U49A.N4.D.002.z.syn.xy >> gmtwf.000.ps
echo "0 947 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   U49A.N4.D.002.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         U49A.N4.D.002.r.syn.xy >> gmtwf.000.ps
echo "0 947 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   U49A.N4.D.002.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         U49A.N4.D.002.t.syn.xy >> gmtwf.000.ps
echo "0 947 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 N4.U49A.00
1.5 0.6 @~D@~=106 @%7% km @%%
1.5 0.4 @~f@~=312@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.86e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## W52A ista = 4 ########################
###
gmt psbasemap -R0/65/-1072/1027 -JX1.5i/0.75i -Bxf16a65 -Byf257a1027+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        W52A.N4.D.003.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        W52A.N4.D.003.z.syn.xy >> gmtwf.000.ps
echo "0 1027 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   W52A.N4.D.003.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         W52A.N4.D.003.r.syn.xy >> gmtwf.000.ps
echo "0 1027 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   W52A.N4.D.003.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         W52A.N4.D.003.t.syn.xy >> gmtwf.000.ps
echo "0 1027 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 N4.W52A.00
1.5 0.6 @~D@~=124 @%7% km @%%
1.5 0.4 @~f@~=134@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.00e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## T50A ista = 5 ########################
###
gmt psbasemap -R0/65/-959/894 -JX1.5i/0.75i -Bxf16a65 -Byf224a894+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        T50A.N4.D.004.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        T50A.N4.D.004.z.syn.xy >> gmtwf.000.ps
echo "0 894 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   T50A.N4.D.004.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         T50A.N4.D.004.r.syn.xy >> gmtwf.000.ps
echo "0 894 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   T50A.N4.D.004.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         T50A.N4.D.004.t.syn.xy >> gmtwf.000.ps
echo "0 894 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 N4.T50A.00
1.5 0.6 @~D@~=127 @%7% km @%%
1.5 0.4 @~f@~=2@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.76e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## TZTN ista = 6 ########################
###
gmt psbasemap -R0/75/-1117/1031 -JX1.5i/0.75i -Bxf19a75 -Byf258a1031+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        TZTN.US.D.005.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        TZTN.US.D.005.z.syn.xy >> gmtwf.000.ps
echo "0 1031 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   TZTN.US.D.005.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         TZTN.US.D.005.r.syn.xy >> gmtwf.000.ps
echo "0 1031 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   TZTN.US.D.005.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         TZTN.US.D.005.t.syn.xy >> gmtwf.000.ps
echo "0 1031 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 US.TZTN.00
1.5 0.6 @~D@~=142 @%7% km @%%
1.5 0.4 @~f@~=58@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 2.05e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## X51A ista = 7 ########################
###
gmt psbasemap -R0/75/-844/841 -JX1.5i/0.75i -Bxf19a75 -Byf210a841+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        X51A.N4.D.006.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        X51A.N4.D.006.z.syn.xy >> gmtwf.000.ps
echo "0 841 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   X51A.N4.D.006.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         X51A.N4.D.006.r.syn.xy >> gmtwf.000.ps
echo "0 841 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   X51A.N4.D.006.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         X51A.N4.D.006.t.syn.xy >> gmtwf.000.ps
echo "0 841 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 N4.X51A.00
1.5 0.6 @~D@~=146 @%7% km @%%
1.5 0.4 @~f@~=179@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.60e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

###
################## V48A ista = 8 ########################
###
gmt psbasemap -R0/75/-845/805 -JX1.5i/0.75i -Bxf19a75 -Byf201a805+l"nm" -BS -Y1.25i -X-3i -O -K -Vq >> gmtwf.000.ps

### vertical data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq        V48A.N4.D.007.z.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq        V48A.N4.D.007.z.syn.xy >> gmtwf.000.ps
echo "0 805 Z" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### radial data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   V48A.N4.D.007.r.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         V48A.N4.D.007.r.syn.xy >> gmtwf.000.ps
echo "0 805 R" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### transverse data and synthetics
gmt psxy -R -JX -W1p,black -O -K -Vq -X1.5i   V48A.N4.D.007.t.dat.xy >> gmtwf.000.ps
gmt psxy -R -JX -W1p,red,10_2:0p -O -K -Vq         V48A.N4.D.007.t.syn.xy >> gmtwf.000.ps
echo "0 805 T" | gmt pstext -R -JX -D0.1i/0.0i -F+jBL+f14p,Times-Bold,black -N -O -K -Vq >> gmtwf.000.ps

### net.sta label tag
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f11p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.8 N4.V48A.00
1.5 0.6 @~D@~=174 @%7% km @%%
1.5 0.4 @~f@~=266@+o@+
EOF
gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f9p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.2 1.57e+03 @%7%nm@%%
EOF

gmt pstext -R0/1/0/1 -JX1i/1i -D0i/0i -N -F+jML+f8p,Times-Bold,black -O -K -Vq >> gmtwf.000.ps << EOF
1.5 0.05 Invert 0.08_0.12Hz
EOF

####
################## mechanism ########################
####
gmt psmeca -R0/10/0/10 -JX1.5i/1.5i -Sm1i/0 -T0/1p,black -W1p,black -Ggray -N -X+2.15i -O -K -Vq >> gmtwf.000.ps << EOF
5 5 2 -2.13254 -0.751176 -0.911852 0.844395 -0.266314 -0.0343853 22 x y 2021/08/13T11:57:35.00 Franklin Mine, TN
EOF

###
### MOMENT TENSOR/FOCAL MECHANISM INFORMATION LABEL 
###
gmt pstext -R0/10/0/10 -JX1.5i/1.8i -D0i/0i -F+jML+f10p,Palatino-Bold,black -N -O -Vq -X-1i -Y-1.5i >> gmtwf.000.ps << EOF
9 9 Franklin Mine, TN
9 8 2021/08/13T11:57:35.00
9 7 Depth =   2.0 @%7%km@%%
9 6 M@-w@- =  4.21
9 5 M@-0@- =  2.56@%12%\264@%%10@+15@+ N@%12%\264@%%m
9 4 DC=24% CLVD=27% ISO=49%
9 3 VR=  90.5%
9  2 M@-xx@-=-0.75 M@-xy@-=+0.03
9  1 M@-xz@-=+0.84 M@-yy@-=-0.91
9  0 M@-yz@-=+0.27 M@-zz@-=-2.13
9 -1 Lune @%12%d@%%=-53.08 @%12%g@%%=14.99
EOF

# gs gmtwf.000.ps
ps2pdf gmtwf.000.ps
gmt psconvert -Tj -E600 -A -Vq gmtwf.000.ps
# open gmtwf.000.jpg
