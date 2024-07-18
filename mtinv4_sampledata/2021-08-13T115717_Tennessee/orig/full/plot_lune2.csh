#!/bin/csh
set argc=$#argv

####################################### DEFAULTS ############################################################
set color=y          ### cpt color rainbow or grayscale 
set plt_pts=n        ### randomly simulated eigenvalue pts on lune
set plt_user_pt=y    ### user supplied eigenvalues on lune as diamond symbol
set plt_label_vr=y   ### best fitting full-mt label the star with percent variance reduction value
set pltmech=y        ### plot best fitting full-MT solution on lune with percent VR label
set mech_panel=y     ### side panel with MT plots for iso, DC and best full-MT
####################################### DEFAULTS ############################################################

if( $argc != 6 ) then
  echo " $0 requires 6 arguments got $argc  (all options y or n) " 
  echo " Usage: $0 color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y " 
  exit(-1)
endif

foreach variable ( $argv )
  eval set ${variable}
  echo ${variable}
end

gmt set FONT_TITLE             14p,Helvetica-Bold,black
gmt set FONT_LABEL             14p,Helvetica-Bold,black
gmt set FONT_ANNOT_PRIMARY     12p,Helvetica-Bold,black
gmt set MAP_GRID_PEN_PRIMARY    0.08p,black
gmt set MAP_GRID_PEN_SECONDARY  0.05p,black

set TITLE_TXT="2021-08-13 Franklin Mine, TN"
set INPUT=best.all.out
set  PS=${INPUT}.lune.ps
set JPG=${INPUT}.lune.jpg
set R=" -R-30/30/-90/90 "
set J=" -JH0/3i "

#### create surface grid from lon, lat, pVR points  -Vl(long verbosy) -Vq(quiet) -Vn(err only)
####
gmt surface best.all.out $R -I0.5/0.5 -Glune.grd -T0.25 -C0.1 -Vn

### create the points and label for the special Lune DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack 
###
### format: lon lat label justify-code 
###
cat >! points.xy << EOF
  0   0  DC    0 
  0 -90 -ISO   1 
  0 +90 +ISO   2 
+30   0 -CLVD  1 
+30 -60 -Crack 1 
+30 -35 -LVD   1 
-30 +60 +Crack 2 
-30 +35 +LVD   2 
-30   0 +CLVD  2 
EOF

#### Contour interval
####
cat >! cint.xy << EOF
0  A
10 C
20 A
30 C
40 A
50 C
60 A
70 C
80 A
90 C
EOF

### Color Palete   ####
#### -Z continous 24bit color ###
if( ${color} == "y" ) then
 gmt makecpt -Crainbow -T0/85/2 -D -Vq >! vred.cpt
else
 gmt makecpt -Cgray -T0/85/10 -D -Vq >! vred.cpt
endif

#### Start GMT plot - plot the grid image
####
gmt grdimage $R $J lune.grd -Cvred.cpt -P -K -Vq >! ${PS}
####
gmt psbasemap $R $J -Bxf180g10a180 -Byf180g10a180 -Bnsew+t"${TITLE_TXT}" -O -K -Vq >> ${PS}

#### mark the individual lat/lon lune points
####
if( ${plt_pts} == "y" ) then
  gmt psxy ${INPUT} $R $J -Sc0.075i -Cvred.cpt -W0.5p,black -t90 -O -K -Vq >> ${PS}
endif

#### plot the contours on the lune
####
gmt grdcontour lune.grd $R $J -Ccint.xy -W1p,black -O -K -Vn  -L+10/+100 -S10 -Gd1.5i -A+20+ap+c0.02i+o+f14p,Helvetica-Bold,black+gwhite+p0.25p,black >> ${PS}

### plot the special lune points and labels DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack 
####
gmt psxy points.xy $R $J -N -Sc0.1i -W1p,black -Gblack -O -K -Vq >> ${PS}

### DC only
awk '{ if( $4 == 0 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D+0.12i/+0.12i -F+f14p,Helvetica-Bold,black+jBL -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}

### CLVD, LVD, ISO
awk '{ if( $4 == 2 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D-0.12i/0.0i   -F+f14p,Helvetica-Bold,black+jMR -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}
awk '{ if( $4 == 1 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D+0.12i/0.0i   -F+f14p,Helvetica-Bold,black+jML -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}

#### Best fitting DevMT
 echo " -20.17357 0.00000 " | gmt psxy $R $J -Ss0.18i -W0.5p,black -Gwhite -O -K -Vq >> ${PS}

#### This lune lat,lon for User supplied eigenvalues 
####
if( ${plt_user_pt} == "y" ) then
 echo " 14.98967 -53.07737 " | gmt psxy $R $J -Sd0.15i -W1p,black -Gwhite -O -K -Vq >> ${PS}
endif   #### plt_user_pt yes 

#### Plot Moment Tensor at best fitting FullMT on lune ####
####
if( ${pltmech} == "y" ) then

if( ${plt_label_vr} == "y" ) then

gmt psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -K -Vq  >> ${PS} << EOF
15.47264 -55.22027 0 -0.834 -0.316 -0.388 0.309 -0.125 0.003 24 15.47264 -55.22027 VR=89.4%
EOF

else

gmt psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF
15.47264 -55.22027 0 -0.834 -0.316 -0.388 0.309 -0.125 0.003 24 15.47264 -55.22027    
EOF

endif  #### plt_label_vr yes for pltmech yes 


else    ###### pltmech no 

#### This lune lat,lon for max percent VarRed
####
echo " 15.47264 -55.22027 " | gmt psxy $R $J -Sa0.3i -W1p,black -Gwhite -O -K -Vq >> ${PS}

#### VR label
####
if( ${plt_label_vr} == "y" ) then

 echo " 15.47264 -55.22027 VR=89.4% " | gmt pstext $R $J -F+f10p,Helvetica-Bold,black+jML -D+0.15i/+0.15i -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}

endif   ##### plt_label_vr yes 

endif   ##### pltmech no 

#############################################################################################
#### Plot Moment Tensor focal mmechanisms at certain locations on the lune               ####
#############################################################################################
#### (m) Seismic moment tensor (Harvard CMT, with zero trace):                           ####
####     X, Y, depth, mrr, mtt, mff, mrt,  mrf,  mtf, exp, newX, newY, event_title       ####
####                  mzz  mxx  myy  mxz  -myz  -mxy                                     ####
#############################################################################################
if( ${mech_panel} == "y" ) then

gmt psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -T0/0.3p,black -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF
0.9 0.95 1.0 -0.834 -0.316 -0.388 +0.309 -0.125 +0.003 24 0.9 0.95 (Full) VR=89.4
0.9 0.88 1.0 +0.564 -0.230 -0.334 +0.682 -0.253 -0.044 24 0.9 0.88 (Dev) VR=44.2
0.9 0.81 1.0 +0.237 -0.236 -0.002 +0.947 -0.213 +0.030 24 0.9 0.81 (DC) VR=-36.3
EOF

gmt psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF
0.9 0.74 1.0 +1.000 +1.000 +1.000 +0.000 +0.000 -0.000 24  0.9 0.74 (+Iso) VR=-1261.1
0.9 0.67 1.0 -1.000 -1.000 -1.000 +0.000 +0.000 +0.000 24  0.9 0.67 (-Iso) VR=-381.7
EOF

endif  ### mech_panel yes 

#### Color scale
####
gmt psscale -Dx3.5i/0.5i+w3i/0.2i+e0.1i -Bf10a20g20+l"Percent Variance Reduction" -Cvred.cpt  -O -Vq >> ${PS}

#### convert and deep clean
####
gmt psconvert -Tj -E600 -A ${PS}

/bin/rm -f ${PS}
#/bin/rm -f lune.grd
/bin/rm -f lune.out vred.cpt points.xy cint.xy

## use xdg-open or eog for some linux systems
# eog ${JPG}
# xdg-open ${JPG}

## use MacOS open -a preview 
# open ${JPG}
