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

/************************************************************************/
/*** calculate the travel time direct P-wave through 1D model         ***/
/*** uses modified version of Chapman's WKBJ program (Shearer 1999)   ***/
/*** assumes layer has gradient so requires interpolation/resampling  ***/
/*** so that the gradient is as small as possible                     ***/
/*** this uses receprocity, so shoots ray from receiver down until it ***/
/*** turns up.  Then shots ray from source depth until turns up       ***/
/*** the rest of the ray is simply rayparameters times distance of    ***/
/*** the ray without the up and down going legs G. Ichinose Nov 2010  ***/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../include/mt.h"
                                                                                                                              
extern char progname[128];
 
/***************************************************************************/
/*** calculates the take off angle given the ray parameter and srcz      ***/
/*** if arg of asin(arg) == 1 then the takeoff angle is atan(depth/dist) ***/
/*** otherwords the ray is initially traveling upwards from source       ***/
/***************************************************************************/
                                                                                                                             
float calc_takeoff_angle( float vsrc, float rayparameter, float dist, float depth )
{
	float arg;
	arg = vsrc * rayparameter;
	if( arg < 0.99999 )
		return (180/M_PI) * asin( arg );
	else
		return 90 + (180/M_PI) * atan( depth / dist );
}

/*************************************************************************/
/** Earth flattening transformation for a spherical earth model.        **/
/**  Yields kinematically equivalent flat earth.  Also finds the source **/
/**  depth in flattened model and other useful data for travel time     **/
/**  and ray parameter calculation                                      **/
/*************************************************************************/

void earth_flattening_transformation( VelMod *v )
{	
	int i;
	float radius[MAX_MODEL_LAYERS], Earth_Radius = 6371;
	float cd, vp, vs, rho, dep;
	
/*************************************************************************/
/*** calculate the radius of earth layer                               ***/
/*************************************************************************/

	for( i = 0; i < v->nlay; i++ )
	{
		radius[i] = Earth_Radius - v->ztop[i];	
		/*
		fprintf( stdout, "%d r=%g ztop=%g\n",
			i, radius[i], v->ztop[i] );
		*/
	}

/*************************************************************************/
/*** do the EF transformation                                          ***/
/*************************************************************************/

	for( i = 0; i < v->nlay; i++ )
	{
		cd = Earth_Radius;
		if( radius[i] > 0 ) cd = Earth_Radius / radius[i];

		dep = Earth_Radius * log(cd);

		vp  = v->vp[i]  * cd;
		vs  = v->vs[i]  * cd;
		rho = v->rho[i] * cd;
	
		v->vp[i]  = vp;
		v->vs[i]  = vs;
		v->rho[i] = rho;
	}
}

/**************************************************************************/
/** create vout velocity model with layers interpolated to new thickness **/
/**************************************************************************/

void interpolate_model( VelMod *vin, VelMod *vout, float dlaykm )
{
	int i, j;
	float total_thickness = 680; /* km */

	vout->nlay = (int)rint( total_thickness / dlaykm );
	vout->maxlay = vout->nlay;
	strcpy( vout->modpath, vin->modpath );
	strcpy( vout->modfile, vin->modfile );

	for( i = 0; i < vout->nlay; i++ )
	{
		vout->ztop[i] = (float)i*dlaykm;
		vout->thick[i] = dlaykm;
		for( j = 0; j < vin->nlay - 1; j++ )
		{
			if( vout->ztop[i] >= vin->ztop[j] &&
			    vout->ztop[i] <=  vin->ztop[j+1] )
			{
		/*** layer found ***/

				vout->vp[i]    = vin->vp[j];
				vout->qa[i]    = vin->qa[j];
				vout->vs[i]    = vin->vs[j];
				vout->qb[i]    = vin->qb[j];
				vout->rho[i]   = vin->rho[j];
				vout->sigma[i] = vin->sigma[j];
				break;
			}
			else if( vout->ztop[i] >= vin->ztop[vin->nlay-1] )
			{
		/*** this vout model layer extends beyond bottom of input model ***/

				vout->vp[i]    = vin->vp[vin->nlay-1];
                                vout->qa[i]    = vin->qa[vin->nlay-1];
                                vout->vs[i]    = vin->vs[vin->nlay-1];
                                vout->qb[i]    = vin->qb[vin->nlay-1];
                                vout->rho[i]   = vin->rho[vin->nlay-1];
                                vout->sigma[i] = vin->sigma[vin->nlay-1];
				break;
			}
			else
			{
		/** go to next layer of input model **/
			}
		}
	}
}

/************************************************************************/
/*** calculate the travel time direct P-wave through 1D model         ***/
/*** uses modified version of Chapman's WKBJ program (Shearer 1999)   ***/
/*** assumes layer has gradient so requires interpolation/resampling  ***/
/*** so that the gradient is as small as possible                     ***/
/*** this uses receprocity, so shoots ray from receiver down until it ***/
/*** turns up.  Then shots ray from source depth until turns up       ***/
/*** the rest of the ray is simply rayparameters times distance of    ***/
/*** the ray without the up and down going legs G. Ichinose Nov 2010  ***/
/************************************************************************/

#define NRAYS 1000  /** number of rays to shoot ***/

void calc_1D_tt( float distkm, float dep, float *ray_parameter, 
                 float *ttime, float *raybot, int *isrclayer, VelMod *vm, int verbose )
{
	float h, utop, ubot, dx, dt;
	float p[NRAYS],dp,pmin,pmax;
	int best_ray, iray;
	int ilay, irtr, isrc, ilaybot, BAD_RAY=0;
	int layerxt( float p, float h, float utop, float ubot, float *dx, float *dt );
	float tmin, tleg_down, tleg_up, rbot[NRAYS], xxd[NRAYS], xxu[NRAYS], tt[NRAYS];
	
/*************************************************************************/
/*** find the source layer                                             ***/
/*************************************************************************/

	isrc = vm->nlay - 1;
	for( ilay = 0; ilay < vm->nlay - 1; ilay++ )
	{
		if( dep >= vm->ztop[ilay] && dep < vm->ztop[ilay+1] ) 
		{
			isrc = ilay;
			break;
		}
	}
	*isrclayer = isrc;
	if(verbose)fprintf( stdout, "dep=%g isrc=%d\n", dep, isrc );

/***************************************************/
/*** the range of ray parametrs for ray shooting ***/
/***************************************************/

	pmin = 1/vm->vp[vm->nlay - 1];
	pmax = 1/vm->vp[0];
	dp = (pmax - pmin)/NRAYS;
	if(verbose)fprintf( stdout, "pmax=%g pmin=%g dp=%g \n", pmax, pmin, dp );

/*************************************************************************/
/*** begin shooting rays from receiver down first                      ***/
/*************************************************************************/

	for( iray = 0; iray < NRAYS; iray++ )
	{  
	  xxd[iray] = 0;
	  xxu[iray] = 0;
	  tt[iray] = 0;  
	  tleg_down = 0;
	  tleg_up   = 0;
	  p[iray] = pmin + (float)iray*dp;
	  dx = 0;
	  dt = 0;

	  if(verbose)
	    fprintf( stdout, "iray=%d p=%g -------------\n", iray, p[iray] );

	/*******************************************************************/
        /*** ray traveling down from receiver to turning point           ***/
	/*******************************************************************/

          for( ilay = 0; ilay < vm->nlay; ilay++ )
          {
                h = vm->thick[ilay];
 
                if( ilay == vm->nlay -1 )
                {
                        utop = 1/vm->vp[ilay];
                        ubot = 1/vm->vp[ilay];
                }
                else
                {
                        utop = 1/vm->vp[ilay];
                        ubot = 1/vm->vp[ilay+1];
                }
                irtr = layerxt( p[iray], h, utop, ubot, &dx, &dt );

                if(verbose) 
		{
		  fprintf( stdout, 
    "UP   iray=%d ilay=%d irtr=%d z=%g vp=%g p=%g h=%g utop=%g ubot=%g dx=%g dt=%g x=%g t=%g\n",
                     iray, ilay, irtr, 
		    vm->ztop[ilay], vm->vp[ilay], p[iray], h, utop, ubot, dx, dt, xxu[iray], tleg_up );
		}

					/*********************************/
                if( irtr == 0 ) break; /** ray turned above this layer ***/
					/*********************************/
                xxu[iray] += dx;
                tleg_up += dt;

					/*********************************/
                if( irtr == 2 ) break; /** ray turned within layer **/	
					/*********************************/
		BAD_RAY = 0;

                if( xxu[iray] > distkm )
		{
			BAD_RAY = 1;
			if(verbose) 
			  fprintf( stdout, "iray=%d ray leaves model ilay=%d\n", iray, ilay );

				/***********************/
			break; /*** ray leaves model ***/
				/***********************/
		}
          }
	  
	  ilaybot = ilay;
	  rbot[iray] =  vm->ztop[ilay];
	  if( BAD_RAY == 0 )
	  { 
	    if(verbose)
	    {
	      fprintf( stdout, 
		"UP   iray=%d p=%g rbot=%g tt=%g tleg_down=%g tleg_up=%g xxd=%g xxu=%g  -------------\n",
                iray, p[iray], rbot[iray], tt[iray], tleg_down, tleg_up, xxd[iray], xxu[iray] );
	    }
	  }

	/*********************************************/
	/*** the ray turned before the src-rec dist ***/
	/*** ray traveling down from source depth ***/
	/********************************************/

	  for( ilay = isrc; ilay <= ilaybot; ilay++ )
	  {
		if( ilay == isrc ) 
			h = vm->thick[ilay] - ( dep - vm->ztop[ilay] );
		else
			h = vm->thick[ilay];

		if( ilay == vm->nlay -1 )
		{
			utop = 1/vm->vp[ilay];
			ubot = 1/vm->vp[ilay];
		}
		else
		{
			utop = 1/vm->vp[ilay];
			ubot = 1/vm->vp[ilay+1];
		}
		irtr = layerxt( p[iray], h, utop, ubot, &dx, &dt );
	
		if(verbose)
		{
		  fprintf( stdout, 
		    "DOWN iray=%d ilay=%d irtr=%d z=%g vp=%g p=%g h=%g utop=%g ubot=%g dx=%g dt=%g x=%g t=%g\n",
		      iray, ilay, irtr, 
		      vm->ztop[ilay], vm->vp[ilay], p[iray], h, utop, ubot, dx, dt, xxd[iray], tleg_down );
		}

		if( irtr == 0 ) break; /** ray turned above this layer ***/
		xxd[iray] += dx;
		tleg_down += dt;
		if( irtr == 2 ) break; /** ray turned within layer **/
		if( xxd[iray] > distkm/2 ) break; /*** ray reflection ***/
	  }

	  if( BAD_RAY == 0 )
	  {
	    if(verbose)
	    {
	      fprintf( stdout, 
		"DOWN iray=%d p=%g rbot=%g tt=%g tleg_down=%g tleg_up=%g xxd=%g xxu=%g  -------------\n",
                  iray, p[iray], rbot[iray], tt[iray], tleg_down, tleg_up, xxd[iray], xxu[iray] );
	    }
	  }

	/*************************************************************/
	/*** if the ray is good then compute the total travel time ***/
	/*** including the refaction portion                       ***/
	/*************************************************************/

	  if( BAD_RAY == 1 )
	    tt[iray] = 999999;
	  else
	    tt[iray] = p[iray] * ( distkm - (xxu[iray] + xxd[iray]) ) + ( tleg_down + tleg_up );

	  if( BAD_RAY == 0 )
	  {
	    if(verbose)
	    {
	      fprintf( stdout, 
	"     iray=%d p=%g rbot=%g tt=%g tleg_down=%g tleg_up=%g xxd=%g xxu=%g R=%g tt=%g -------------\n", 
		iray, p[iray], rbot[iray], tt[iray], tleg_down, tleg_up, 
		xxd[iray], xxu[iray], 
		( xxd[iray] + xxu[iray] ),
		tt[iray] );
	    }
	  }    
	  BAD_RAY = 0; /** RESET from bad ray to good ray ***/
	}

/***************************************************/
/*** check the shortest upgoing ray              ***/
/***************************************************/

	*ttime = sqrt(dep*dep + distkm*distkm) / vm->vp[0];
	*ray_parameter = 1/vm->vp[0];
	*raybot = vm->ztop[ilay+1]/2;
	tmin = *ttime;
	if(verbose) 
	  fprintf( stdout, "direct ray: ttime=%g rayp=%g raybot=%g\n",
		*ttime, *ray_parameter, *raybot );

/****************************************************/
/*** search for minimum travel time  FERMAT'S LAW ***/
/****************************************************/

	for( iray = 0; iray < NRAYS; iray++ )
	{
		if( tt[iray] < tmin ) 
		{
			tmin = tt[iray];
			best_ray = iray;
			*ttime = tmin;
			*ray_parameter = p[best_ray];
			*raybot = rbot[best_ray];
		}
	}
}

/**************************************************************************************
c LAYERXT calculates dx and dt for ray in layer with linear velocity gradient
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
**************************************************************************************/

int layerxt( float p, float h, float utop, float ubot, float *dx, float *dt )
{	
	int irtr;
	float dtau,tau1,tau2,x1,x2,u1,u2,v1,v2,b,eta1,eta2;

/***************************************************/
/*** ray turned above layer                      ***/
/***************************************************/

	if( p >= utop )
	{
		*dx = 0;
		*dt = 0;
		irtr = 0;
		return irtr;
	}
	else if( h == 0 )
	{
		*dx = 0;
		*dt = 0;
		irtr = -1;
		return irtr;
	}

	u1 = utop;
	u2 = ubot;
	v1 = 1/u1;
	v2 = 1/u2;
	b = (v2-v1)/h;  /** slope of velocity gradient **/

	eta1 = sqrt( (u1*u1) - (p*p) );

/*********************************************************/
/*** constant velocity layer, ray passed through layer ***/
/*********************************************************/

	if( b == 0 )
	{
		*dx = h * p/eta1;
		*dt = h * (u1*u1)/eta1;
		irtr = 1;
		return irtr;
	}

/***********************************************************/
/*** ray turned within layer, 1 segment counted in dx,dt ***/
/***********************************************************/

	x1 = eta1 / ( u1 * b * p );
	tau1 = (log((u1+eta1)/p)-eta1/u1)/b;
	if( p >= ubot ) 
	{
		*dx = x1;
		dtau = tau1;
		*dt = dtau + p * (*dx);
		irtr = 2;
		return irtr;
	}

/***************************************************/
/**** ray passed through layer                  ****/
/***************************************************/

	irtr = 1;
	eta2 = sqrt( (u2*u2) - (p*p) );
	x2 = eta2/( u2 * b * p );
	tau2 = (log((u2+eta2)/p)-eta2/u2)/b;
	*dx = x1 - x2;
	dtau = tau1 - tau2;
	*dt = dtau + p * (*dx);
	return irtr;
}
