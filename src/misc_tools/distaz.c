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
#include <math.h>
#include <errno.h>
#define OK      0
#define ERR     1

/*
 *      DISTAZ
 *
 *      distaz(olat,olon,tlat,tlon,del,az,baz)
 *              float olat,olon,tlat,tlon;
 *              float *del, *az, *baz;
 *
 *   This subroutine will compute the distance, azimuth, and back
 * azimuth (in degrees), given the latitude and longitude (in degrees)
 * of an origin point and a target point.  (E+,W-; N+,S-)
 */

int distaz( double olat, double olon, double tlat, double tlon,
	double *del, double *az, double *baz )
{
	double pi,twopi,rd;
        double clat,clon,clar,clor,stho,ctho,ctlat,ctlon;
        double ctlar,ctlor,sth,cth,dph,sdph,cdph,delr;
        double azr,bazr;

/*** these are defined in math.h ***/
        /* double acos(),sin(),cos(),atan2(); */

	pi = acos(-1.0);
	twopi = 2.*pi;
	rd = 180/pi;

        if (olat < -90. || olat > 90.) return(ERR);
        if (olon < -180. || olon > 180.) return(ERR);
        if (tlat < -90. || tlat > 90.) return(ERR);
        if (tlon < -180. || tlon > 180.) return(ERR);

        clat = 90. - olat;
        clon = olon;
        if (clon < 0.) clon += 360.;
        clar = clat / rd;
        clor = clon / rd;
        stho = sin(clar);
        ctho = cos(clar);
        ctlat = 90. - tlat;
        ctlon = tlon;
        if (clon < 0.) ctlon += 360.;
        ctlar = ctlat / rd;
        ctlor = ctlon / rd;
        sth = sin(ctlar);
        cth = cos(ctlar);
        dph = ctlor - clor;
        sdph = sin(dph);
        cdph = cos(dph);
        delr = acos(stho * sth * cdph + ctho * cth);
        *del = 111.11 * delr * rd;

/* computer forward azimuth */
        if (sth == 0.) azr = 0.;
        else azr = atan2(sdph,stho*cth/sth-ctho*cdph);
        if (azr < 0.) azr += twopi;
        *az = azr*rd;

/******
        if (errno) {
                fprintf (stderr, "errno = %d\ny = %d\tx=%d\n", errno, sdph,stho*cth/sth-ctho*cdph);
                return(ERR);
        }
******/

/* compute back azimuth */

        if (stho == 0.) bazr = 0.;
        else bazr = atan2(-sdph,sth*ctho/stho-cth*cdph);
/*****
        if (errno) {
                fprintf (stderr, "errno = %d\ny = %d\tx=%d\n", errno, sdph,stho*cth/sth-ctho*cdph);
                return(ERR);
        }
****/
        if (bazr < 0.) bazr += twopi;
        *baz = bazr * rd;

        return(OK);
}
