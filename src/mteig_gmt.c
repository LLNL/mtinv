/***********************************************************************************/
/*** Copyright 2024 Gene A. Ichinose (LLNL)                                      ***/
/***                                                                             ***/
/*** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” ***/
/*** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ***/
/*** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   ***/
/*** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         ***/
/*** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        ***/
/*** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    ***/
/*** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ***/
/*** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     ***/
/*** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      ***/
/*** THE POSSIBILITY OF SUCH DAMAGE.                                             ***/
/***                                                                             ***/
/*** Prepared by LLNL under Contract DE-AC52-07NA27344.                          ***/
/***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "../include/mteig.h"  /** global datatype and structure declarations **/

extern char progname[128];

/*** Plot the NSS lune using GMT version 4.5.x ***/

void mteig_gmt4_plot( 
	char *input_filename,
        char *gmt_script_filename,
        char *title_text,
        Results *rbest,  /*** best fit results array  ***/
        int ifullmt,  /** pointer to fullmt ***/
        int idevmt,   /** pointer to devmt ***/
        int idcmt,    /** pointer to dc ***/
        int iuser,    /** pointer to dc **/
        int ipiso,    /** pointer to +iso **/ 
        int iniso,    /** pointer to -iso **/
        int Add_user_Eig,
        int Add_user_MT, 
        int color_or_gray,
        int doplt )
{
	FILE *fp;
	char command_line[512];
	char currentdir[128];
	char *username;

/*** START ***/
/*** get the username and current working directory from shell environment ***/
	username = getenv( "USER" );
	getcwd( currentdir, 128 );
	chdir( currentdir );

	fp = fopen( gmt_script_filename, "w" );

/*** begin C-shell script ****/
	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "set argc=$#argv\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "####################################### DEFAULTS ############################################################\n" );
        fprintf( fp, "set color=y          ### cpt color rainbow or grayscale \n" );
        fprintf( fp, "set plt_pts=n        ### randomly simulated eigenvalue pts on lune\n" );
        fprintf( fp, "set plt_user_pt=y    ### user supplied eigenvalues on lune as diamond symbol\n" );
        fprintf( fp, "set plt_label_vr=y   ### best fitting full-mt label the star with percent variance reduction value\n" );
        fprintf( fp, "set pltmech=y        ### plot best fitting full-MT solution on lune with percent VR label\n" );
        fprintf( fp, "set mech_panel=y     ### side panel with MT plots for iso, DC and best full-MT\n" );
        fprintf( fp, "####################################### DEFAULTS ############################################################\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "if( $argc != 6 ) then\n" );
        fprintf( fp, "  echo \" $0 requires 6 arguments got $argc  (all options y or n) \" \n" );
        fprintf( fp, "  echo \" Usage: $0 color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y \" \n" );
        fprintf( fp, "  exit(-1)\n" );
        fprintf( fp, "endif\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "foreach variable ( $argv )\n" );
        fprintf( fp, "  eval set ${variable}\n" );
        fprintf( fp, "  echo ${variable}\n" );
        fprintf( fp, "end\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "gmtset HEADER_FONT Helvetica-Bold HEADER_FONT_SIZE 14p\n" );
        fprintf( fp, "gmtset LABEL_FONT  Helvetica-Bold LABEL_FONT_SIZE  14p\n" );
        fprintf( fp, "gmtset ANNOT_FONT_PRIMARY Helvetica-Bold ANNOT_FONT_SIZE_PRIMARY 12p\n" );
	fprintf( fp, "gmtset ANNOT_FONT_SECONDARY Helvetica-Bold ANNOT_FONT_SIZE_PRIMARY 12p\n" );
        fprintf( fp, "gmtset GRID_PEN_PRIMARY    0.08p\n" );
        fprintf( fp, "gmtset GRID_PEN_SECONDARY  0.05p\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "set TITLE_TXT=\"%s\"\n", title_text );
        fprintf( fp, "set INPUT=%s\n", input_filename );
        fprintf( fp, "set  PS=${INPUT}.lune.ps\n" );
        fprintf( fp, "set JPG=${INPUT}.lune.jpg\n" );
        fprintf( fp, "set R=\" -R-30/30/-90/90 \"\n" );
        fprintf( fp, "set J=\" -JH0/3i \"\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### create surface grid from lon, lat, pVR points  -Vl(long verbosy) -Vq(quiet) -Vn(err only)\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "#### awk '{ print $1, $2, $3 }' ${INPUT} >! lune.out\n" );
	fprintf( fp, "####\n" );
        fprintf( fp, "surface %s $R -I0.5/0.5 -Glune.grd -T0.25 -C0.1 -Vn\n", input_filename );
        fprintf( fp, "\n" );

	fprintf( fp, "### create the points and label for the special Lune DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack \n" );
        fprintf( fp, "###\n" );
        fprintf( fp, "### format: lon lat label justify-code \n" );
        fprintf( fp, "###\n" );
        fprintf( fp, "cat >! points.xy << EOF\n" );
        fprintf( fp, "  0   0  DC    0 \n" );
        fprintf( fp, "  0 -90 -ISO   1 \n" );
        fprintf( fp, "  0 +90 +ISO   2 \n" );
        fprintf( fp, "+30   0 -CLVD  1 \n" );
        fprintf( fp, "+30 -60 -Crack 1 \n" );
        fprintf( fp, "+30 -35 -LVD   1 \n" );
        fprintf( fp, "-30 +60 +Crack 2 \n" );
        fprintf( fp, "-30 +35 +LVD   2 \n" );
        fprintf( fp, "-30   0 +CLVD  2 \n" );
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "#### Contour interval\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "cat >! cint.xy << EOF\n" );
        fprintf( fp, "0  A\n" );
        fprintf( fp, "10 C\n" );
        fprintf( fp, "20 A\n" );
        fprintf( fp, "30 C\n" );
        fprintf( fp, "40 A\n" );
        fprintf( fp, "50 C\n" );
        fprintf( fp, "60 A\n" );
        fprintf( fp, "70 C\n" );
        fprintf( fp, "80 A\n" );
        fprintf( fp, "90 C\n" );
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

fprintf( fp, "### Color Palete   ####\n" );
        fprintf( fp, "#### -Z continous 24bit color ###\n" );
        fprintf( fp, "if( ${color} == \"y\" ) then\n" );
        fprintf( fp, " makecpt -Crainbow -T0/85/2 -D -V >! vred.cpt\n" );
        fprintf( fp, "else\n" );
        fprintf( fp, " makecpt -Cgray -T0/85/10 -D -V >! vred.cpt\n" );
        fprintf( fp, "endif\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### Start GMT plot - plot the grid image\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "grdimage $R $J lune.grd -Cvred.cpt -P -K -V >! ${PS}\n" );
        fprintf( fp, "####\n" );

	fprintf( fp, "psbasemap $R $J -Bf180g10a180/f180g10a180:.\"${TITLE_TXT}\":nsew -O -K -V >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### mark the individual lat/lon lune points\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "if( ${plt_pts} == \"y\" ) then\n" );
        fprintf( fp, "  psxy ${INPUT} $R $J -Sc0.075i -Cvred.cpt -W0.5p/black -O -K -V >> ${PS}\n" );
        fprintf( fp, "endif\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### plot the contours on the lune\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "grdcontour lune.grd $R $J -A20+c0.02i+o+f1+s11p+gwhite+p0.25p/black -Ccint.xy -W1p/black -L+10/+100 -S10 -Gd1.5i -O -K -V >> ${PS}\n" );
        fprintf( fp, "\n" );

       fprintf( fp, "### plot the special lune points and labels DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack \n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "psxy points.xy $R $J -N -Sc0.1i -W1p/black -G0/0/0 -O -K -V >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "### DC only\n" );
        fprintf( fp, "awk '{ if( $4 == 0 ) print $1, $2, 14, 0, 1, \"BL\", $3 }' points.xy | pstext $R $J -N -D+0.12i/+0.12i -C0.01i -G0 -W255,o0.1p -O -K -V >> ${PS}\n" );
        fprintf( fp, "\n" );
        fprintf( fp, "### CLVD, LVD, ISO\n" );
        fprintf( fp, "awk '{ if( $4 == 2 ) print $1, $2, 14, 0, 1, \"MR\", $3 }' points.xy | pstext $R $J -N -D-0.12i/0.0i -C0.01i -G0 -W255,o0.1p -O -K -V >> ${PS}\n" );
        fprintf( fp, "awk '{ if( $4 == 1 ) print $1, $2, 14, 0, 1, \"ML\", $3 }' points.xy | pstext $R $J -N -D+0.12i/0.0i -C0.01i -G0 -W255,o0.1p -O -K -V >> ${PS}\n" );
        fprintf( fp, "\n" );

/*** Best fitting DevMT ***/

        fprintf( fp, "#### Best fitting DevMT\n" );
        fprintf( fp, " echo \" %.5f %.5f \" | psxy $R $J -Ss0.18i -W0.5p/0 -G255 -O -K -V >> ${PS}\n",
                rbest[idevmt].lune_lon,
                rbest[idevmt].lune_lat );
        fprintf( fp, "\n" );

/*** This lune lat,lon for User supplied eigenvalues ***/

        if( Add_user_Eig )
        {
         fprintf( fp, "#### This lune lat,lon for User supplied eigenvalues \n" );
         fprintf( fp, "####\n" );
         fprintf( fp, "if( ${plt_user_pt} == \"y\" ) then\n" );
         fprintf( fp, " echo \" %.5f %.5f \" | psxy $R $J -Sd0.15i -W1p/0 -G255 -O -K -V >> ${PS}\n",
		rbest[iuser].lune_lon, rbest[iuser].lune_lat );
         fprintf( fp, "endif   #### plt_user_pt yes \n" );
         fprintf( fp, "\n" );
        }

/*** Plot Moment Tensor at best fitting FullMT on lune ***/

        fprintf( fp, "#### Plot Moment Tensor at best fitting FullMT on lune ####\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "if( ${pltmech} == \"y\" ) then\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "if( ${plt_label_vr} == \"y\" ) then\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -K -V >> ${PS} << EOF\n" );
        fprintf( fp, "%.5f %.5f 0 %.3f %.3f %.3f %.3f %.3f %.3f 24 %.5f %.5f VR=%.1f%%\n",
                rbest[ifullmt].lune_lon, 
                rbest[ifullmt].lune_lat, 
                rbest[ifullmt].mzz, rbest[ifullmt].mxx, 
                rbest[ifullmt].myy, rbest[ifullmt].mxz,
                -rbest[ifullmt].myz, -rbest[ifullmt].mxy, 
                rbest[ifullmt].lune_lon, 
                rbest[ifullmt].lune_lat, 
                rbest[ifullmt].var_red );
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "else\n" );  /**** plt_label_vr no ****/

        fprintf( fp, "\n" );
        fprintf( fp, "psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -K -V >> ${PS} << EOF\n" );
        fprintf( fp, "%.5f %.5f 0 %.3f %.3f %.3f %.3f %.3f %.3f 24 %.5f %.5f    \n",
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat,
                rbest[ifullmt].mzz,
		rbest[ifullmt].mxx,
                rbest[ifullmt].myy,
		rbest[ifullmt].mxz,
                -rbest[ifullmt].myz,
		-rbest[ifullmt].mxy,
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat );
        fprintf( fp, "EOF\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "endif  #### plt_label_vr yes for pltmech yes \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "else    ###### pltmech no \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### This lune lat,lon for max percent VarRed\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "echo \" %.5f %.5f \" | gmt psxy $R $J -Sa0.3i -W1p/black -G255 -O -K -V >> ${PS}\n", 
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat );

        fprintf( fp, "\n" );
	fprintf( fp, "#### VR label\n" );
        fprintf( fp, "####\n" );

        fprintf( fp, "if( ${plt_label_vr} == \"y\" ) then\n" );

        fprintf( fp, "\n" );
        fprintf( fp, " echo \" %.5f %.5f VR=%.1f%% \" | gmt pstext $R $J -F+f10p,Helvetica-Bold,black+jML -D+0.15i/+0.15i -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n", 
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat, 
                rbest[ifullmt].var_red );

        fprintf( fp, "\n" );
        fprintf( fp, "endif   ##### plt_label_vr yes \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "endif   ##### pltmech no \n" );
        fprintf( fp, "\n" );

/**** Plot Moment Tensor focal mmechanisms at certain locations on the lune  ***/

	fprintf( fp, "#############################################################################################\n" );
        fprintf( fp, "#### Plot Moment Tensor focal mmechanisms at certain locations on the lune               ####\n" );
        fprintf( fp, "#############################################################################################\n" );
        fprintf( fp, "#### (m) Seismic moment tensor (Harvard CMT, with zero trace):                           ####\n" );
        fprintf( fp, "####     X, Y, depth, mrr, mtt, mff, mrt,  mrf,  mtf, exp, newX, newY, event_title       ####\n" );
        fprintf( fp, "####                  mzz  mxx  myy  mxz  -myz  -mxy                                     ####\n" ); 
        fprintf( fp, "#############################################################################################\n" );

        fprintf( fp, "if( ${mech_panel} == \"y\" ) then\n" );
        fprintf( fp, "\n" );
        fprintf( fp, "psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -T0/0.3p,black -L1p,black -G100 -E255 -N -O -K -V >> ${PS} << EOF\n" );

        fprintf( fp, "0.9 0.95 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.95 (Full) VR=%.1f\n",
                rbest[ifullmt].mzz,
		rbest[ifullmt].mxx,
		rbest[ifullmt].myy,
		rbest[ifullmt].mxz,
		-rbest[ifullmt].myz,
		-rbest[ifullmt].mxy,
		rbest[ifullmt].var_red );

        fprintf( fp, "0.9 0.88 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.88 (Dev) VR=%.1f\n",   
                rbest[idevmt].mzz,
		rbest[idevmt].mxx,
		rbest[idevmt].myy,
		rbest[idevmt].mxz,
		-rbest[idevmt].myz,
		-rbest[idevmt].mxy,
		rbest[idevmt].var_red );

        fprintf( fp, "0.9 0.81 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.81 (DC) VR=%.1f\n",    
                rbest[idcmt].mzz,
		rbest[idcmt].mxx,
		rbest[idcmt].myy,
		rbest[idcmt].mxz,
		-rbest[idcmt].myz,
		-rbest[idcmt].mxy,
		rbest[idcmt].var_red );

        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -L1p,black -G100 -E255 -N -O -K -V >> ${PS} << EOF\n" );

        fprintf( fp, "0.9 0.74 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24  0.9 0.74 (+Iso) VR=%.1f\n",  
                rbest[ipiso].mzz,
		rbest[ipiso].mxx,
		rbest[ipiso].myy,
		rbest[ipiso].mxz,
		-rbest[ipiso].myz,
		-rbest[ipiso].mxy,
		rbest[ipiso].var_red );

        fprintf( fp, "0.9 0.67 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24  0.9 0.67 (-Iso) VR=%.1f\n",  
                rbest[iniso].mzz,
		rbest[iniso].mxx,
		rbest[iniso].myy,
		rbest[iniso].mxz,
		-rbest[iniso].myz,
		-rbest[iniso].mxy,
		rbest[iniso].var_red );

        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "endif  ### mech_panel yes \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### Color scale\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "psscale -D3.6/1.5i/2.75i/0.2i -E0.1i -Bf10a20g20:\"Percent Variance Reduction\":S -Cvred.cpt -O -V >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### convert and deep clean\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "ps2raster -Tj -E600 -A ${PS}\n" );
        fprintf( fp, "\n" );
        fprintf( fp, "/bin/rm -f ${PS}\n" );
        fprintf( fp, "#/bin/rm -f lune.grd\n" );
        fprintf( fp, "/bin/rm -f lune.out vred.cpt points.xy cint.xy\n" );

 	fprintf( fp, "\n" );
        fprintf( fp, "## use xdg-open or eog for some linux systems\n" );
        fprintf( fp, "# eog ${JPG}\n" );
        fprintf( fp, "# xdg-open ${JPG}\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "## use MacOS open -a preview \n" );
        fprintf( fp, "# open ${JPG}\n" );       

        fclose(fp);

        chmod( gmt_script_filename, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );

        sprintf(
                command_line,
                "csh %s color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y",
                gmt_script_filename );

        if(doplt)
        {
                fprintf( stderr, "%s: %s: %s: =================== system call ===================  \n", progname, __FILE__, __func__ );
                system( command_line ); 
                fprintf( stderr, "%s: %s: %s: =================== system call ===================  \n", progname, __FILE__, __func__ );
        }
        fprintf( stderr, "\n%s: %s: %s: to plot use the following command:\n\n", progname, __FILE__, __func__ );
        fprintf( stderr, "\t %s\n\n", command_line );

} /**** end of subroutine: mteig_gmt4_plot()  *****/





/*** Plot the NSS lune using GMT version ++5.x.x ***/

void mteig_gmt5_plot(
	char *input_filename,
	char *gmt_script_filename,
	char *title_text,
	Results *rbest,  /*** best fit results array  ***/
	int ifullmt,  /** pointer to fullmt ***/
	int idevmt,   /** pointer to devmt ***/
	int idcmt,    /** pointer to dc ***/
	int iuser,    /** pointer to dc **/
	int ipiso,    /** pointer to +iso **/
	int iniso,    /** pointer to -iso **/
	int Add_user_Eig,
	int Add_user_MT,
	int color_or_gray,
	int doplt )
{
	FILE *fp;
	char command_line[512];
	char currentdir[128];
	char *username;


/*** START ***/

/*** get the username and current working directory from shell environment ***/
        username = getenv( "USER" );
	getcwd( currentdir, 128 );
        chdir( currentdir );

	fp = fopen( gmt_script_filename, "w" );

/*** begin C-shell script ****/
	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "set argc=$#argv\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "####################################### DEFAULTS ############################################################\n" );
	fprintf( fp, "set color=y          ### cpt color rainbow or grayscale \n" );
	fprintf( fp, "set plt_pts=n        ### randomly simulated eigenvalue pts on lune\n" );
	fprintf( fp, "set plt_user_pt=y    ### user supplied eigenvalues on lune as diamond symbol\n" );
	fprintf( fp, "set plt_label_vr=y   ### best fitting full-mt label the star with percent variance reduction value\n" );
	fprintf( fp, "set pltmech=y        ### plot best fitting full-MT solution on lune with percent VR label\n" );
	fprintf( fp, "set mech_panel=y     ### side panel with MT plots for iso, DC and best full-MT\n" );
	fprintf( fp, "####################################### DEFAULTS ############################################################\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "if( $argc != 6 ) then\n" );
	fprintf( fp, "  echo \" $0 requires 6 arguments got $argc  (all options y or n) \" \n" );
	fprintf( fp, "  echo \" Usage: $0 color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y \" \n" );
	fprintf( fp, "  exit(-1)\n" );
	fprintf( fp, "endif\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "foreach variable ( $argv )\n" );
	fprintf( fp, "  eval set ${variable}\n" );
	fprintf( fp, "  echo ${variable}\n" );
	fprintf( fp, "end\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "gmt set FONT_TITLE             14p,Helvetica-Bold,black\n" );
	fprintf( fp, "gmt set FONT_LABEL             14p,Helvetica-Bold,black\n" );
	fprintf( fp, "gmt set FONT_ANNOT_PRIMARY     12p,Helvetica-Bold,black\n" );
	fprintf( fp, "gmt set MAP_GRID_PEN_PRIMARY    0.08p,black\n" );
	fprintf( fp, "gmt set MAP_GRID_PEN_SECONDARY  0.05p,black\n" );
	fprintf( fp, "\n" );
	
	fprintf( fp, "set TITLE_TXT=\"%s\"\n", title_text );
	fprintf( fp, "set INPUT=%s\n", input_filename );
	fprintf( fp, "set  PS=${INPUT}.lune.ps\n" );
	fprintf( fp, "set JPG=${INPUT}.lune.jpg\n" );
	fprintf( fp, "set R=\" -R-30/30/-90/90 \"\n" );
	fprintf( fp, "set J=\" -JH0/3i \"\n" );
	fprintf( fp, "\n" );
	
	fprintf( fp, "#### create surface grid from lon, lat, pVR points  -Vl(long verbosy) -Vq(quiet) -Vn(err only)\n" );
	fprintf( fp, "####\n" );
	/* fprintf( fp, "awk '{ print $1, $2, $3 }' ${INPUT} >! lune.out\n" ); */
	fprintf( fp, "gmt surface %s $R -I0.5/0.5 -Glune.grd -T0.25 -C0.1 -Vn\n", input_filename );
	fprintf( fp, "\n" );

	fprintf( fp, "### create the points and label for the special Lune DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack \n" );
	fprintf( fp, "###\n" );
	fprintf( fp, "### format: lon lat label justify-code \n" );
	fprintf( fp, "###\n" );
	fprintf( fp, "cat >! points.xy << EOF\n" );
	fprintf( fp, "  0   0  DC    0 \n" );
	fprintf( fp, "  0 -90 -ISO   1 \n" );
	fprintf( fp, "  0 +90 +ISO   2 \n" );
	fprintf( fp, "+30   0 -CLVD  1 \n" );
	fprintf( fp, "+30 -60 -Crack 1 \n" );
	fprintf( fp, "+30 -35 -LVD   1 \n" );
	fprintf( fp, "-30 +60 +Crack 2 \n" );
	fprintf( fp, "-30 +35 +LVD   2 \n" );
	fprintf( fp, "-30   0 +CLVD  2 \n" );
	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "#### Contour interval\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "cat >! cint.xy << EOF\n" );
	fprintf( fp, "0  A\n" );
	fprintf( fp, "10 C\n" );
	fprintf( fp, "20 A\n" );
	fprintf( fp, "30 C\n" );
	fprintf( fp, "40 A\n" );
	fprintf( fp, "50 C\n" );
	fprintf( fp, "60 A\n" );
	fprintf( fp, "70 C\n" );
	fprintf( fp, "80 A\n" );
	fprintf( fp, "90 C\n" );
	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "### Color Palete   ####\n" );
	fprintf( fp, "#### -Z continous 24bit color ###\n" );
	fprintf( fp, "if( ${color} == \"y\" ) then\n" );
	fprintf( fp, " gmt makecpt -Crainbow -T0/85/2 -D -Vq >! vred.cpt\n" );
	fprintf( fp, "else\n" );
	fprintf( fp, " gmt makecpt -Cgray -T0/85/10 -D -Vq >! vred.cpt\n" );
	fprintf( fp, "endif\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "#### Start GMT plot - plot the grid image\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "gmt grdimage $R $J lune.grd -Cvred.cpt -P -K -Vq >! ${PS}\n" );
	fprintf( fp, "####\n" );

	fprintf( fp, "gmt psbasemap $R $J -Bxf180g10a180 -Byf180g10a180 -Bnsew+t\"${TITLE_TXT}\" -O -K -Vq >> ${PS}\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "#### mark the individual lat/lon lune points\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "if( ${plt_pts} == \"y\" ) then\n" );
	fprintf( fp, "  gmt psxy ${INPUT} $R $J -Sc0.075i -Cvred.cpt -W0.5p,black -t90 -O -K -Vq >> ${PS}\n" );
	fprintf( fp, "endif\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "#### plot the contours on the lune\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "gmt grdcontour lune.grd $R $J -Ccint.xy -W1p,black -O -K -Vn " );
	fprintf( fp, " -L+10/+100 -S10 -Gd1.5i -A+20+ap+c0.02i+o+f14p,Helvetica-Bold,black+gwhite+p0.25p,black >> ${PS}\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "### plot the special lune points and labels DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack \n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "gmt psxy points.xy $R $J -N -Sc0.1i -W1p,black -Gblack -O -K -Vq >> ${PS}\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "### DC only\n" );
	fprintf( fp, "awk '{ if( $4 == 0 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D+0.12i/+0.12i -F+f14p,Helvetica-Bold,black+jBL -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "### CLVD, LVD, ISO\n" );
	fprintf( fp, "awk '{ if( $4 == 2 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D-0.12i/0.0i   -F+f14p,Helvetica-Bold,black+jMR -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n" );
	fprintf( fp, "awk '{ if( $4 == 1 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D+0.12i/0.0i   -F+f14p,Helvetica-Bold,black+jML -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n" );
	fprintf( fp, "\n" );

/*** Best fitting DevMT ***/

	fprintf( fp, "#### Best fitting DevMT\n" );
        fprintf( fp, " echo \" %.5f %.5f \" | gmt psxy $R $J -Ss0.18i -W0.5p,black -Gwhite -O -K -Vq >> ${PS}\n",
                rbest[idevmt].lune_lon,
                rbest[idevmt].lune_lat );
        fprintf( fp, "\n" );

/*** This lune lat,lon for User supplied eigenvalues ***/

	if( Add_user_Eig )
	{
	 fprintf( fp, "#### This lune lat,lon for User supplied eigenvalues \n" );
         fprintf( fp, "####\n" );
	 fprintf( fp, "if( ${plt_user_pt} == \"y\" ) then\n" );
         fprintf( fp, " echo \" %.5f %.5f \" | gmt psxy $R $J -Sd0.15i -W1p,black -Gwhite -O -K -Vq >> ${PS}\n", rbest[iuser].lune_lon, rbest[iuser].lune_lat );
	 fprintf( fp, "endif   #### plt_user_pt yes \n" );
	 fprintf( fp, "\n" );
	}

/*** lot Moment Tensor at best fitting FullMT on lune ***/

	fprintf( fp, "#### Plot Moment Tensor at best fitting FullMT on lune ####\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "if( ${pltmech} == \"y\" ) then\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "if( ${plt_label_vr} == \"y\" ) then\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "gmt psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -K -Vq  >> ${PS} << EOF\n" );
	fprintf( fp, "%.5f %.5f 0 %.3f %.3f %.3f %.3f %.3f %.3f 24 %.5f %.5f VR=%.1f%%\n",
                rbest[ifullmt].lune_lon, 
		rbest[ifullmt].lune_lat, 
		rbest[ifullmt].mzz,
		rbest[ifullmt].mxx, 
		rbest[ifullmt].myy,
		rbest[ifullmt].mxz,
		-rbest[ifullmt].myz,
		-rbest[ifullmt].mxy, 
		rbest[ifullmt].lune_lon, 
		rbest[ifullmt].lune_lat, 
		rbest[ifullmt].var_red );
	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "else\n" );  /**** plt_label_vr no ****/

	fprintf( fp, "\n" );
	fprintf( fp, "gmt psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF\n" );
	fprintf( fp, "%.5f %.5f 0 %.3f %.3f %.3f %.3f %.3f %.3f 24 %.5f %.5f    \n",
		rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat,
                rbest[ifullmt].mzz,
		rbest[ifullmt].mxx,
                rbest[ifullmt].myy,
		rbest[ifullmt].mxz,
                -rbest[ifullmt].myz,
		-rbest[ifullmt].mxy,
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat );
	fprintf( fp, "EOF\n" );

	fprintf( fp, "\n" );
	fprintf( fp, "endif  #### plt_label_vr yes for pltmech yes \n" );
	fprintf( fp, "\n" );

	fprintf( fp, "\n" );
	fprintf( fp, "else    ###### pltmech no \n" );
	fprintf( fp, "\n" );

	fprintf( fp, "#### This lune lat,lon for max percent VarRed\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "echo \" %.5f %.5f \" | gmt psxy $R $J -Sa0.3i -W1p,black -Gwhite -O -K -Vq >> ${PS}\n", 
		rbest[ifullmt].lune_lon,
		rbest[ifullmt].lune_lat );

	fprintf( fp, "\n" );
	fprintf( fp, "#### VR label\n" );
	fprintf( fp, "####\n" );

	fprintf( fp, "if( ${plt_label_vr} == \"y\" ) then\n" );

	fprintf( fp, "\n" );
	fprintf( fp, " echo \" %.5f %.5f VR=%.1f%% \" | gmt pstext $R $J -F+f10p,Helvetica-Bold,black+jML -D+0.15i/+0.15i -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n", 
		rbest[ifullmt].lune_lon,
		rbest[ifullmt].lune_lat, 
		rbest[ifullmt].var_red );

	fprintf( fp, "\n" );
	fprintf( fp, "endif   ##### plt_label_vr yes \n" );
	fprintf( fp, "\n" );

	fprintf( fp, "endif   ##### pltmech no \n" );
	fprintf( fp, "\n" );

/**** Plot Moment Tensor focal mmechanisms at certain locations on the lune  ***/

	fprintf( fp, "#############################################################################################\n" );
	fprintf( fp, "#### Plot Moment Tensor focal mmechanisms at certain locations on the lune               ####\n" );
	fprintf( fp, "#############################################################################################\n" );
	fprintf( fp, "#### (m) Seismic moment tensor (Harvard CMT, with zero trace):                           ####\n" );
	fprintf( fp, "####     X, Y, depth, mrr, mtt, mff, mrt,  mrf,  mtf, exp, newX, newY, event_title       ####\n" );
	fprintf( fp, "####                  mzz  mxx  myy  mxz  -myz  -mxy                                     ####\n" ); 
	fprintf( fp, "#############################################################################################\n" );

	fprintf( fp, "if( ${mech_panel} == \"y\" ) then\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "gmt psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -T0/0.3p,black -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF\n" );

	fprintf( fp, "0.9 0.95 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.95 (Full) VR=%.1f\n",
		rbest[ifullmt].mzz,
		rbest[ifullmt].mxx,
		rbest[ifullmt].myy,
		rbest[ifullmt].mxz,
		-rbest[ifullmt].myz,
		-rbest[ifullmt].mxy,
		rbest[ifullmt].var_red );

	fprintf( fp, "0.9 0.88 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.88 (Dev) VR=%.1f\n",   
                rbest[idevmt].mzz,
		rbest[idevmt].mxx,
		rbest[idevmt].myy,
		rbest[idevmt].mxz,
		-rbest[idevmt].myz,
		-rbest[idevmt].mxy,
		rbest[idevmt].var_red );

	fprintf( fp, "0.9 0.81 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.81 (DC) VR=%.1f\n",    
		rbest[idcmt].mzz,
		rbest[idcmt].mxx,
		rbest[idcmt].myy,
		rbest[idcmt].mxz,
		-rbest[idcmt].myz,
		-rbest[idcmt].mxy,
		rbest[idcmt].var_red );

	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "gmt psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF\n" );

	fprintf( fp, "0.9 0.74 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24  0.9 0.74 (+Iso) VR=%.1f\n",  
		rbest[ipiso].mzz,
		rbest[ipiso].mxx,
		rbest[ipiso].myy,
		rbest[ipiso].mxz,
		-rbest[ipiso].myz,
		-rbest[ipiso].mxy,
		rbest[ipiso].var_red );

	fprintf( fp, "0.9 0.67 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24  0.9 0.67 (-Iso) VR=%.1f\n",  
		rbest[iniso].mzz,
		rbest[iniso].mxx,
		rbest[iniso].myy,
		rbest[iniso].mxz,
		-rbest[iniso].myz,
		-rbest[iniso].mxy,
		rbest[iniso].var_red );

	fprintf( fp, "EOF\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "endif  ### mech_panel yes \n" );
	fprintf( fp, "\n" );

	fprintf( fp, "#### Color scale\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "gmt psscale -Dx3.5i/0.5i+w3i/0.2i+e0.1i -Bf10a20g20+l\"Percent Variance Reduction\" -Cvred.cpt  -O -Vq >> ${PS}\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "#### convert and deep clean\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "gmt psconvert -Tj -E600 -A ${PS}\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "/bin/rm -f ${PS}\n" );
	fprintf( fp, "#/bin/rm -f lune.grd\n" );
	fprintf( fp, "/bin/rm -f lune.out vred.cpt points.xy cint.xy\n" );

	fprintf( fp, "\n" );
        fprintf( fp, "## use xdg-open or eog for some linux systems\n" );
        fprintf( fp, "# eog ${JPG}\n" );
        fprintf( fp, "# xdg-open ${JPG}\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "## use MacOS open -a preview \n" );
        fprintf( fp, "# open ${JPG}\n" );

	fclose(fp);

	chmod( gmt_script_filename, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );

	sprintf(
		command_line,
		"csh %s color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y",
		gmt_script_filename );

	if(doplt)
	{
		fprintf( stderr, "%s: %s: %s: =================== system call ===================  \n", progname, __FILE__, __func__ );
		system( command_line ); 
		fprintf( stderr, "%s: %s: %s: =================== system call ===================  \n", progname, __FILE__, __func__ );
	}
	fprintf( stderr, "\n%s: %s: %s: to plot use the following command:\n\n", progname, __FILE__, __func__ );
	fprintf( stderr, "\t %s\n\n", command_line );

} /*** end of subroutine void mteig_gmt5_plot() ***/



/*************************************************************************************/
/*** This is the search routine used to find best fitting VR row in eigenvalue     ***/
/***   list not eigen-vectors see mteig_subs.c  for different search routine       ***/
/***                                                                               ***/
/***  added new feature   ::                                                       ***/
/*** char *mt_type = "fullmt" searches all rows                                    ***/
/***   or                                                                          ***/
/****                "devmt" searches all rows with lat = 0                        ***/
/*************************************************************************************/

int search_largest_var_red( Results *rbest, int nsim_eig, char *sort, char *mt_type )
{
	int ir;
	float max;
	int imax;

	int *indx;
	float *vr;
	Results *rtmp;
	void indexx( int n, float *arrin, int *indx );
	void transfer_results( Results *in, Results *out );
	void print_results_all( Results *rbest, int nsim_eig, char *filename );

/*** sort on variance reduction ***/

	if( strcmp( sort, "sort" ) == 0 )
	{
		rtmp = (Results *) calloc( nsim_eig+1, sizeof(Results) );
		indx = (int *)     calloc( nsim_eig+1, sizeof(int) );
		vr   = (float *)   calloc( nsim_eig+1, sizeof(float) );

		for( ir = 1; ir <=nsim_eig; ir++ )
		{
			vr[ir] = rbest[ir-1].var_red;
		}

		indexx( nsim_eig, vr, indx );

	/*** copy rbest into rtmp sorted array ***/

		for( ir = 1; ir <=nsim_eig; ir++ )
		{

			/***debug***/
			/*
			fprintf( stdout, "ir=%d vr[indx[ir]=%d]=%g rbest.vr=%g gcarc=%g / %g\n",
				ir, indx[ir], vr[indx[ir]], 
				rbest[indx[ir]-1].var_red, rbest[indx[ir]-1].gcarc_dc, rbest[indx[ir]-1].gcarc_exp );
			*/

			transfer_results( &(rbest[ indx[ir] - 1 ]), &(rtmp[ ir - 1 ]) );
		}

		/* print_results_all( rtmp, nsim_eig, "best.all.out.sorted" ); */

	/*** preserve ordering in original required for gmt plots ***/

		free(rtmp);	
		free(vr);
		free(indx);
	}
	else
	{
		/*** does not produce sorted output ***/
	}

/*** if sorted, then should be last array row ***/

	if( strcmp( mt_type, "fullmt" ) == 0 || strcmp( mt_type, "FULLMT" ) == 0 )
	{
		fprintf( stdout, "%s: %s: %s: mt_type=%s\n", progname, __FILE__, __func__, mt_type );

		imax = 0;
		max  = rbest[0].var_red;

		for( ir = 1; ir < nsim_eig; ir++ )
		{
			if( rbest[ir].var_red > max ) 
			{
				max = rbest[ir].var_red;
				imax = ir;
			}
		}
	}
	else if( strcmp( mt_type, "devmt" ) == 0 || strcmp( mt_type, "DEVMT" ) == 0 )
	{
		fprintf( stdout, "%s: %s: %s: mt_type=%s\n", progname, __FILE__, __func__, mt_type );

		max = -999999;
		imax = -1;
		for( ir = 0; ir < nsim_eig; ir++ )
		{
			if( rbest[ir].lune_lat == 0 )
			{
				if( rbest[ir].var_red > max )
				{
					max = rbest[ir].var_red;
					imax = ir;
				}
			}
		}
	}

	return((int)imax);
}

/*** this is a specialty version of mteig_gmt5_plot() that does a normalized lune plot ***/
/*** maybe delete if no longer needed ***/
/*** this is only a GMT5+ vers ***/

void mteig_gmt_plot_lune_norm(
	char *input_filename,
	char *gmt_script_filename,
	char *title_text,
	Results *rbest,
	int ifullmt,
	int idevmt,
	int idcmt,
	int iuser,
	int ipiso,
	int iniso,
	int Add_user_Eig,
	int Add_user_MT,
	int color_or_gray,
	int doplt )
{
	FILE *fp;
	char command_line[512];
	char currentdir[128];
	char *username;

/*** START ***/

/*** get the username and current working directory from shell environment ***/
	username = getenv( "USER" );
	getcwd( currentdir, 128 );
	chdir( currentdir );

	fp = fopen( gmt_script_filename, "w" );

/*** begin C-shell script ****/

	fprintf( fp, "#!/bin/csh\n" );
	fprintf( fp, "set argc=$#argv\n" );
	fprintf( fp, "\n" );
	fprintf( fp, "####################################### DEFAULTS ############################################################\n" );
        fprintf( fp, "set color=y          ### cpt color rainbow or grayscale \n" );
        fprintf( fp, "set plt_pts=n        ### randomly simulated eigenvalue pts on lune\n" );
        fprintf( fp, "set plt_user_pt=y    ### user supplied eigenvalues on lune as diamond symbol\n" );
        fprintf( fp, "set plt_label_vr=y   ### best fitting full-mt label the star with percent variance reduction value\n" );
        fprintf( fp, "set pltmech=y        ### plot best fitting full-MT solution on lune with percent VR label\n" );
        fprintf( fp, "set mech_panel=y     ### side panel with MT plots for iso, DC and best full-MT\n" );
        fprintf( fp, "####################################### DEFAULTS ############################################################\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "if( $argc != 6 ) then\n" );
        fprintf( fp, "  echo \" $0 requires 6 arguments got $argc  (all options y or n) \" \n" );
        fprintf( fp, "  echo \" Usage: $0 color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y \" \n" );
        fprintf( fp, "  exit(-1)\n" );
        fprintf( fp, "endif\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "foreach variable ( $argv )\n" );
        fprintf( fp, "  eval set ${variable}\n" );
        fprintf( fp, "  echo ${variable}\n" );
        fprintf( fp, "end\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "gmt set FONT_TITLE             14p,Helvetica-Bold,black\n" );
        fprintf( fp, "gmt set FONT_LABEL             14p,Helvetica-Bold,black\n" );
        fprintf( fp, "gmt set FONT_ANNOT_PRIMARY     12p,Helvetica-Bold,black\n" );
        fprintf( fp, "gmt set MAP_GRID_PEN_PRIMARY    0.08p,black\n" );
        fprintf( fp, "gmt set MAP_GRID_PEN_SECONDARY  0.05p,black\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "set TITLE_TXT=\"%s\"\n", title_text );
        fprintf( fp, "set INPUT=%s\n", input_filename );
        fprintf( fp, "set  PS=${INPUT}.lune_norm.ps\n" );
        fprintf( fp, "set JPG=${INPUT}.lune_norm.jpg\n" );
	fprintf( fp, "\n" );
        fprintf( fp, "set R=\" -R-30/30/-90/90 \"\n" );
        fprintf( fp, "set J=\" -JH0/3i \"\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "### create the points and label for the special Lune DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack \n" );
        fprintf( fp, "###\n" );
        fprintf( fp, "### format: lon lat label justify-code \n" );
        fprintf( fp, "###\n" );
        fprintf( fp, "cat >! points.xy << EOF\n" );
        fprintf( fp, "  0   0  DC    0 \n" );
        fprintf( fp, "  0 -90 -ISO   1 \n" );
        fprintf( fp, "  0 +90 +ISO   2 \n" );
        fprintf( fp, "+30   0 -CLVD  1 \n" );
        fprintf( fp, "+30 -60 -Crack 1 \n" );
        fprintf( fp, "+30 -35 -LVD   1 \n" );
        fprintf( fp, "-30 +60 +Crack 2 \n" );
        fprintf( fp, "-30 +35 +LVD   2 \n" );
        fprintf( fp, "-30   0 +CLVD  2 \n" );
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### Contour interval\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "cat >! cint.xy << EOF\n" );
	fprintf( fp, "0.05 C\n" );
        fprintf( fp, "0.10 C\n" );
	fprintf( fp, "0.15 C\n" );
        fprintf( fp, "0.20 A\n" );
	fprintf( fp, "0.25 C\n" );
        fprintf( fp, "0.30 A\n" );
	fprintf( fp, "0.35 C\n" );
        fprintf( fp, "0.40 A\n" );
	fprintf( fp, "0.45 C\n" );
        fprintf( fp, "0.50 A\n" );
	fprintf( fp, "0.55 C\n" );
        fprintf( fp, "0.60 A\n" );
	fprintf( fp, "0.65 C\n" );
        fprintf( fp, "0.70 A\n" );
	fprintf( fp, "0.75 C\n" );
        fprintf( fp, "0.80 A\n" );
	fprintf( fp, "0.85 C\n" );
        fprintf( fp, "0.90 A\n" );
	fprintf( fp, "0.95 C\n" );
	fprintf( fp, "0.99 A\n" );
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "####\n" );
        fprintf( fp, "#### Color Palete -Z continous 24bit color ###\n" );
	fprintf( fp, "####\n" );
	fprintf( fp, "\n" );

        fprintf( fp, "if( ${color} == \"y\" ) then\n" );
        fprintf( fp, " gmt makecpt -Crainbow -T0/1/0.02 -D -Vq >! vred.cpt\n" );
        fprintf( fp, "else\n" );
        fprintf( fp, " gmt makecpt -Cgray -T0/1/0.02 -D -Vq >! vred.cpt\n" );
        fprintf( fp, "endif\n" );
        fprintf( fp, "\n" );

	fprintf( fp, "####\n" );
        fprintf( fp, "#### Start GMT plot - plot the grid image\n" );
        fprintf( fp, "####\n" );
	fprintf( fp, "\n" );

        fprintf( fp, "gmt grdimage $R $J lune_norm.grd -Cvred.cpt -P -K -Vq >! ${PS}\n" );
	fprintf( fp, "\n" );

	fprintf( fp, "gmt psbasemap $R $J -Bxf180g10a180 -Byf180g10a180 -Bnsew+t\"${TITLE_TXT}\" -O -Vq -K >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### mark the individual lat/lon lune points\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "if( ${plt_pts} == \"y\" ) then\n" );
        fprintf( fp, "  gmt psxy ${INPUT} $R $J -Sc0.075i -W0.5p,black -t90 -O -K -Vq >> ${PS}\n" );
        fprintf( fp, "endif\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### plot the contours on the lune\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "gmt grdcontour lune_norm.grd $R $J -Ccint.xy -W1p,black -O -K -Vq " );
        fprintf( fp, " -L0.5/1 -S10 -Gd1.5i -A+20+ap+c0.02i+o+f14p,Helvetica-Bold,black+gwhite+p0.25p,black >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "### plot the special lune points and labels DC,+/-ISO,+/-CLVD,+/-LVD,+/-Crack \n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "gmt psxy points.xy $R $J -N -Sc0.1i -W1p,black -Gblack -O -K -Vq >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "### DC only\n" );
        fprintf( fp, "awk '{ if( $4 == 0 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D+0.12i/+0.12i -F+f14p,Helvetica-Bold,black+jBL -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n" );
        fprintf( fp, "\n" );
        fprintf( fp, "### CLVD, LVD, ISO\n" );
        fprintf( fp, "awk '{ if( $4 == 2 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D-0.12i/0.0i   -F+f14p,Helvetica-Bold,black+jMR -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n" );
        fprintf( fp, "awk '{ if( $4 == 1 ) print $1, $2, $3 }' points.xy | gmt pstext $R $J -N -D+0.12i/0.0i   -F+f14p,Helvetica-Bold,black+jML -C0.01i -Gwhite -W0.1p,black -O -K -Vq >> ${PS}\n" );
        fprintf( fp, "\n" );

/*** Best fitting DevMT ***/

        fprintf( fp, "#### Best fitting DevMT\n" );
        fprintf( fp, " echo \" %.5f %.5f \" | gmt psxy $R $J -Ss0.18i -W0.5p,black -Gwhite -O -K -Vq >> ${PS}\n",
                rbest[idevmt].lune_lon,
                rbest[idevmt].lune_lat );
        fprintf( fp, "\n" );

/*** This lune lat,lon for User supplied eigenvalues ***/

        if( Add_user_Eig )
        {
         fprintf( fp, "#### This lune lat,lon for User supplied eigenvalues \n" );
         fprintf( fp, "####\n" );
         fprintf( fp, "if( ${plt_user_pt} == \"y\" ) then\n" );
         fprintf( fp, " echo \" %.5f %.5f \" | gmt psxy $R $J -Sd0.15i -W1p,black -Gwhite -O -K -Vq >> ${PS}\n", rbest[iuser].lune_lon, rbest[iuser].lune_lat );
         fprintf( fp, "endif   #### plt_user_pt yes \n" );
         fprintf( fp, "\n" );
        }

/*** lot Moment Tensor at best fitting FullMT on lune ***/

        fprintf( fp, "#### Plot Moment Tensor at best fitting FullMT on lune ####\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "if( ${pltmech} == \"y\" ) then\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "if( ${plt_label_vr} == \"y\" ) then\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "gmt psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -Vq -K -Vq  >> ${PS} << EOF\n" );
        fprintf( fp, "%.5f %.5f 0 %.3f %.3f %.3f %.3f %.3f %.3f 24 %.5f %.5f VR=%.1f%%\n",
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat,
                rbest[ifullmt].mzz, rbest[ifullmt].mxx,
                rbest[ifullmt].myy, rbest[ifullmt].mxz,
                -rbest[ifullmt].myz, -rbest[ifullmt].mxy,
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat,
                rbest[ifullmt].var_red );
        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "else\n" );  /**** plt_label_vr no ****/

        fprintf( fp, "\n" );
        fprintf( fp, "gmt psmeca $R $J -Sm0.25i/10p/3p -L1p,black -G100 -E255 -N -O -Vq -K -Vq   >> ${PS} << EOF\n" );
        fprintf( fp, "%.5f %.5f 0 %.3f %.3f %.3f %.3f %.3f %.3f 24 %.5f %.5f    \n",
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat,
                rbest[ifullmt].mzz, rbest[ifullmt].mxx,
                rbest[ifullmt].myy, rbest[ifullmt].mxz,
                -rbest[ifullmt].myz, -rbest[ifullmt].mxy,
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat );
        fprintf( fp, "EOF\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "endif  #### plt_label_vr yes for pltmech yes \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "else    ###### pltmech no \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### This lune lat,lon for max percent VarRed\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "echo \" %.5f %.5f \" | gmt psxy $R $J -Sa0.3i -W1p,black -Gwhite -O -K -Vq  >> ${PS}\n",
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat );

        fprintf( fp, "\n" );
        fprintf( fp, "#### VR label\n" );
        fprintf( fp, "####\n" );

        fprintf( fp, "if( ${plt_label_vr} == \"y\" ) then\n" );

        fprintf( fp, "\n" );
        fprintf( fp, " echo \" %.5f %.5f VR=%.1f%% \" | gmt pstext $R $J -F+f10p,Helvetica-Bold,black+jML -D+0.15i/+0.15i -C0.01i -Gwhite -W0.1p,black -O -K -Vq  >> ${PS}\n",
                rbest[ifullmt].lune_lon,
                rbest[ifullmt].lune_lat,
                rbest[ifullmt].var_red );

        fprintf( fp, "\n" );
        fprintf( fp, "endif   ##### plt_label_vr yes \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "endif   ##### pltmech no \n" );
        fprintf( fp, "\n" );

/**** Plot Moment Tensor focal mmechanisms at certain locations on the lune  ***/

        fprintf( fp, "#############################################################################################\n" );
        fprintf( fp, "#### Plot Moment Tensor focal mmechanisms at certain locations on the lune               ####\n" );
        fprintf( fp, "#############################################################################################\n" );
        fprintf( fp, "#### (m) Seismic moment tensor (Harvard CMT, with zero trace):                           ####\n" );
        fprintf( fp, "####     X, Y, depth, mrr, mtt, mff, mrt,  mrf,  mtf, exp, newX, newY, event_title       ####\n" );
        fprintf( fp, "####                  mzz  mxx  myy  mxz  -myz  -mxy                                     ####\n" );
        fprintf( fp, "#############################################################################################\n" );

        fprintf( fp, "if( ${mech_panel} == \"y\" ) then\n" );
        fprintf( fp, "\n" );
        fprintf( fp, "gmt psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -T0/0.3p,black -L1p,black -G100 -E255 -N -O -K -Vq  >> ${PS} << EOF\n" );

        fprintf( fp, "0.9 0.95 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.95 (Full) VR=%.1f\n",
                rbest[ifullmt].mzz, rbest[ifullmt].mxx, rbest[ifullmt].myy, rbest[ifullmt].mxz, -rbest[ifullmt].myz, -rbest[ifullmt].mxy, rbest[ifullmt].var_red );

        fprintf( fp, "0.9 0.88 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.88 (Dev) VR=%.1f\n",
                rbest[idevmt].mzz, rbest[idevmt].mxx, rbest[idevmt].myy, rbest[idevmt].mxz, -rbest[idevmt].myz, -rbest[idevmt].mxy, rbest[idevmt].var_red );

        fprintf( fp, "0.9 0.81 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24 0.9 0.81 (DC) VR=%.1f\n",
                rbest[idcmt].mzz, rbest[idcmt].mxx, rbest[idcmt].myy, rbest[idcmt].mxz, -rbest[idcmt].myz, -rbest[idcmt].mxy, rbest[idcmt].var_red );

        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "gmt psmeca -R0/1/0/1 -JX4i/8.5i -Sm0.3i/12p/3p -L1p,black -G100 -E255 -N -O -K -Vq >> ${PS} << EOF\n" );

        fprintf( fp, "0.9 0.74 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24  0.9 0.74 (+Iso) VR=%.1f\n",
                rbest[ipiso].mzz, rbest[ipiso].mxx, rbest[ipiso].myy, rbest[ipiso].mxz, -rbest[ipiso].myz, -rbest[ipiso].mxy, rbest[ipiso].var_red );

        fprintf( fp, "0.9 0.67 1.0 %+.3f %+.3f %+.3f %+.3f %+.3f %+.3f 24  0.9 0.67 (-Iso) VR=%.1f\n",
                rbest[iniso].mzz, rbest[iniso].mxx, rbest[iniso].myy, rbest[iniso].mxz, -rbest[iniso].myz, -rbest[iniso].mxy, rbest[iniso].var_red );

        fprintf( fp, "EOF\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "endif  ### mech_panel yes \n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### Color scale\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "gmt psscale -Dx3.5i/0.5i+w3i/0.2i+e0.1i -Bf0.1a0.2g0.2+l\"normalized Variance Reduction\" -Cvred.cpt  -O -Vq >> ${PS}\n" );
        fprintf( fp, "\n" );

        fprintf( fp, "#### convert and deep clean\n" );
        fprintf( fp, "####\n" );
        fprintf( fp, "gmt psconvert -Tj -E600 -A ${PS}\n" );
        fprintf( fp, "\n" );
        fprintf( fp, "/bin/rm -f ${PS}\n" );
        fprintf( fp, "#/bin/rm -f lune_norm.grd\n" );
        fprintf( fp, "/bin/rm -f lune.out vred.cpt points.xy cint.xy\n" );

        fprintf( fp, "\n" );
        fprintf( fp, "## use xdg-open or eog for some linux systems\n" );
        fprintf( fp, "# eog ${JPG}\n" );
        fprintf( fp, "# xdg-open ${JPG}\n" );

	fprintf( fp, "\n" );
        fprintf( fp, "## use MacOS open -a preview \n" );
        fprintf( fp, "# open ${JPG}\n" );

        fclose(fp);

        chmod( gmt_script_filename, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );

        sprintf( command_line, "csh %s color=y plt_pts=n plt_user_pt=y plt_label_vr=y pltmech=y mech_panel=y", gmt_script_filename );

        if(doplt)
        {
                fprintf( stderr, "%s: %s: %s: =================== system call ===================  \n", progname, __FILE__, __func__ );
                system( command_line );
                fprintf( stderr, "%s: %s: %s: =================== system call ===================  \n", progname, __FILE__, __func__ );
        }
        fprintf( stderr, "\n%s: %s: %s: to plot use the following command:\n\n", progname, __FILE__, __func__ );
        fprintf( stderr, "\t %s\n\n", command_line );

} /*** end of subroutine void mteig_gmt_plot_lune_norm() ****/

