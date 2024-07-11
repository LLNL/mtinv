/********************************************************************************************/
/********************************************************************************************
FlinnEngdahl.java - Written by Gene Ichinose 2008
FlinnEngdahl.java - returns Flinn-Engdahl Region name from decimal
lon,lat values given on command line.

Flinn-Engdahl seismic and geographic regionalization databases (1995 revision)
Originally written in Fortran by G.J. Dunphy, and  modified August 18, 1997 by BPresgrave
Translated to C by G.Ichinose from PERL written by
- Bob Simpson January, 2003  <simpson@usgs.gov>
  With fix supplied by George Randall <ger@geophysics.lanl.gov>  2003-02-03
********************************************************************************************/
/********************************************************************************************/

import java.lang.*;
import java.util.*;
import java.io.*;
import java.text.*;

public class FlinnEngdahlTest 
{
	public static void main( String args[] )
	{
		// verbose = true;
		verbose = false;
		float lon = 0;
		float lat = 0;

	/*** Get the command line arguemtns lon and lat ***/
		try {
			lon = Float.parseFloat( args[0] );
			lat = Float.parseFloat( args[1] );
		}
		catch( ArrayIndexOutOfBoundsException e )
		{
			System.out.println("program needs lon lat");
			System.exit(-1);
		}
		catch( NumberFormatException e )
		{
			System.out.println(e);
			System.exit(-1);
		}

	/************************************************/
	/*** Create the FlinnEngdahl object           ***/
	/*** calcuate the grn and grname from lat,lon ***/
	/************************************************/
		FlinnEngdahl fe = new FlinnEngdahl(lon,lat);
		fe.calc();
		System.out.format( "%g, %g, %d, %s\n", 
			lon, lat, 
			fe.get_grn(),
			fe.get_grname()
		);
	}
	
	public static class FlinnEngdahl
	{
		FlinnEngdahl( float mylon, float mylat )
		{
			lon = mylon;
			lat = mylat;
			myQuad = "";
			myQuadID = 0;
			grn = 0;
			grname = "";
		}

		public void calc()
		{
			float lng = lon;
			if( lng <= -180 ) lng += 360;
			if( lng > 180.0 ) lng -= 360;
			int ln = (int)Math.abs( lng );
			int lt = (int)Math.abs( lat );
			if( lat >= 0.0 && lng >= 0.0) { myQuad = "ne"; myQuadID=1; }
			if( lat >= 0.0 && lng <  0.0) { myQuad = "nw"; myQuadID=2; }
			if( lat <  0.0 && lng >= 0.0) { myQuad = "se"; myQuadID=3; }
			if( lat <  0.0 && lng <  0.0) { myQuad = "sw"; myQuadID=4; }

			if(verbose)
			{
		  	  System.out.format( 
		    	   "lon=%f lat=%f ln=%d lt=%d myQuad=%s myQuadID=%d\n", 
		    	    lon, lat,ln,lt, myQuad, myQuadID );
			}

		/*******************************/
		/*** READ THE NAMES.ASC FILE ***/
		/*******************************/
			FlinnEngdahlRegions[] fer = new FlinnEngdahlRegions[758];
			try {
				BufferedReader in = new BufferedReader( 
					 new FileReader( ascpath + "names.asc" ) );
		
				for( int i=1; i<758; i++ )
				{
					fer[i] = new FlinnEngdahlRegions(i);
					fer[i].readFERnames(in);
				}
				in.close();
			}
			catch ( IOException e )
			{
				System.out.println(e.getMessage());
				System.exit(-1);
			}

			if(verbose)
			{
		  		for( FlinnEngdahlRegions e: fer )
					System.out.println( e );
			}

		/***********************************/
		/*** READ THE QUADSINDX.ASC FILE ***/	
		/***********************************/
			Quadrant[] q = new Quadrant[5];
			for( int i=1; i<=4; i++ )
			{
				try {
					BufferedReader in = new BufferedReader(
					  new FileReader( ascpath + "quadsidx.asc" ));
					q[i] = new Quadrant(i);
					q[i].readQuads(in,i);
					in.close();
				}
				catch ( IOException e )
				{
					System.out.println(e.getMessage());
					System.exit(-1);
				}
			}
			if(verbose)
			{
			   for( int i=1; i<=4; i++ ) 
		 	      q[i].print_index();
			}

		/****************************/
		/*** read the sector file ***/
		/****************************/
			try {
				BufferedReader in = new BufferedReader(
					new FileReader( ascpath +
					   q[myQuadID].getsecfilename()) );

				q[myQuadID].readSector(in);
				in.close();
			}
			catch ( IOException e )
			{
				System.out.println(e.getMessage());
				System.exit(-1);
			}

			if(verbose) q[myQuadID].print_sects();

		/*** look up in tables the FE-Gregion id ***/
			int beg = q[myQuadID].get_begin_idx(lt+1);
			int num = q[myQuadID].get_index_idx(lt+1);
			if(verbose) System.out.format( "beg=%d num=%d\n", beg, num );

			grn = q[myQuadID].get_fenum( beg, num, ln );
			grname = fer[grn].get_name();

			if(verbose) {
			  System.out.format( "%g, %g, %d, %s\n",
				lon, lat, grn, grname );
			}
		}
	
		public int get_grn()
		{
			return grn;
		}

		public String get_grname()
		{
			return grname;
                }

		private String myQuad;
		private int myQuadID;
		private float lat, lon;
		private int grn;
		private String grname;
	}

	public static class Quadrant 
	{
		Quadrant( int myquad )
		{
			iquad = myquad;
			if( myquad == 1 ) quad = "ne";
			if( myquad == 2 ) quad = "nw";
			if( myquad == 3 ) quad = "se";
			if( myquad == 4 ) quad = "sw";
			quadrant_file = quad + "sect.asc";
			index = new int[93];
			begin = new int[93];
			end   = new int[93];
			subpos= new int[93];
			pos   = new int[93];
			nsects = 0;
			lons   = new int[2000];
			fenums = new int[2000];
		}

		public int get_fenum( int beg, int num, int ln )
		{
			int jsec=0;
			for( jsec=beg; jsec < beg+num; jsec++ )
			{
			   if(verbose)
			   {
			     System.out.format("jsec=%d lons=%d fenums=%d\n", 
			  	jsec, lons[jsec], fenums[jsec] );
			   }
			   if( lons[jsec] > ln ) break;
			}
			return fenums[jsec-1];
		}

		public int get_begin_idx(int lat)
		{
			return begin[lat];
		}

		public int get_index_idx(int lat)
		{
			return index[lat];
		}

		public String getsecfilename() 
		{
			return quadrant_file;
		}

		public void print_index()
		{
			for( int k=1; k<=91; k++ )
			{
			  if(verbose)
			  {
			     System.out.format(
			      "quad=%d %s k=%d index=%d subpos=%d pos=%d begin=%d end=%d\n", 
				iquad, quad, k, index[k], subpos[k], pos[k], begin[k], end[k] );
			  }
			}
		}

		public void print_sects()
		{
			for( int k=1; k<=nsects; k++ )
			{
			  if(verbose)
			  {
			   System.out.format(
			     "k=%d lons=%d fenums=%d\n", 
			       k, lons[k], fenums[k] );
			  }
			}
		}

		public void readSector( BufferedReader in ) throws IOException
		{
			String s;
			StringBuffer sb = new StringBuffer();
			while( (s = in.readLine()) != null )
				sb.append(s);
			StringTokenizer st =  new StringTokenizer(sb.toString());
			nsects=0;
			while( st.hasMoreTokens() )
			{
				lons[nsects]  = Integer.parseInt(st.nextToken());
				fenums[nsects] = Integer.parseInt(st.nextToken());
				nsects++;
			}
		}

		public void readQuads( BufferedReader in, int iquad ) throws IOException
		{
			String s;
			int ibeg = (iquad-1)*91 + 1;
			int iend = (ibeg-1) + 91;
			if(verbose)
			   System.out.println( 
				"iquad=" + iquad + " ibeg=" + ibeg + " iend=" + iend );
			StringBuffer sb = new StringBuffer();
			while( (s = in.readLine()) != null )
				sb.append(s);
			int k=1;
			int j=1;
			int start = 0;
			int stop  = -1;
			StringTokenizer st =  new StringTokenizer(sb.toString());
			while( st.hasMoreTokens() )
			{
				String tmpstr=st.nextToken();
				if( k <= iend && k >= ibeg )
				{
					index[j]  = Integer.parseInt(tmpstr);
					subpos[j] = j;
					pos[j]    = k;
					start = stop + 1;
					stop  += index[j];
					begin[j]  = start;
					end[j]    = stop;
					j++;
				}
				k++;
			}
		}

		private int iquad;
		private String quad;
		private String quadrant_file;

		private int[] index;
		private int[] begin;
		private int[] end;
		private int[] subpos;
		private int[] pos;

		private String sector_file;
		private int nsects;
		private int[] lons;
		private int[] fenums;
	}

	public static class FlinnEngdahlRegions
	{
		FlinnEngdahlRegions(int id)
		{
			index = id;
			name = "";
		}

		public String get_name()
		{
			return name;
		}

		public String toString()
		{
			return  "id=" + index + 
				" name=" + name + 
				" strlen=" + name.length();
		}
	
		public void readFERnames( BufferedReader in ) throws IOException
		{
			name = in.readLine();
		}

		private int index;
		private String name;
	}

	private static boolean verbose;
	private static final String ascpath="/Users/ichinose1/Work/mtinv.v3.0.5/misc/Flinn_Engdahl/src/";
}
