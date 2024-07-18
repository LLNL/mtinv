INSERT into   mtid_seq ( lddate ) VALUES ( current_timestamp );
INSERT into   fpid_seq ( lddate ) VALUES ( current_timestamp );
INSERT into mtorid_seq ( lddate ) VALUES ( current_timestamp );

INSERT into MOMENT_STAGE
  ( mtid, fpid, orid, mxx, myy, mzz, mxy, mxz, myz, mt_type, mo, momag, 
  piso, pclvd, pdc, kiso, kclvd, epsilon, f_factor, var_red, residual, 
  algorithm, epi_fixed, time_fixed, depth_fixed, auth, lddate ) 
VALUES ( 
 (SELECT   max(mtid) FROM mtid_seq), 
 (SELECT   max(fpid) FROM fpid_seq), 
 (SELECT max(mtorid) FROM mtorid_seq), 
    -7.51e+14, -9.12e+14, -2.13e+15, 3.44e+13, 8.44e+14, 2.66e+14, 
 'FULLMT', 2.56e+22,     4.21,  49,  27,  24,    -0.49, -999.00,   -0.268,    1.0264, 
  90.48, 6.57e-11, 'mtinv4.0.1:6', 'Y', 'N', 'N', 'ichinose1', CURRENT_TIMESTAMP ); 

INSERT into FOCAL_PLANE_STAGE 
 ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, strike2, dip2, rake2, 
 eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, auth, devmo, isomo, dcmo, lddate )
VALUES
( (SELECT   max(fpid) FROM fpid_seq), 
  (SELECT max(mtorid) FROM mtorid_seq), 
 'MT', 'FULLMT',   104, 19,  -93,   287, 71,  -89, 
 -0.31, 16, 26,  -2.56, 198, 64,  -0.92, 106, 1, 
 'ichinose1', 1.29864e+22, 1.26519e+22, 6.03132e+21, CURRENT_TIMESTAMP ); 

INSERT into MT_ORIGIN_STAGE 
 ( lat, lon, depth, time, orid, evid, jdate, algorithm, auth, lddate )
VALUES
 ( 35.8767, -84.898, 2, 1628855855.000000, 
  (SELECT max(mtorid) FROM mtorid_seq), 
  -1, 2021225, 'mtinv4.0.1:6', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'W50A', 'N4', 84.0, 207, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'W50A', 'BHZ', 1628855859.664772, 1628855910.864773, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.35, 1, 'N', -1, 0.163849, 0.76, 84.0, 207, 26, 0.05, 'Y',  8.8893, 0.998709, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'W50A', 'BHZ', 8.5, 999.9, 2.39351e-05, 5.2, 2.7, 0.185185, 5.4 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'W50A', 'BHZ', 1.5, 7, 0.000212766, 21.05, 4.75, 0.105263, 9.5 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'W50A', 'BHR', 1628855859.664772, 1628855910.864773, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.35, 1, 'N', -1, 0.163849, 0.76, 84.0, 207, 26, 0.05, 'Y',  11.6985, 0.996092, 0.05, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'W50A', 'BHR', 8.5, 999.9, 1.31821e-05, 2.3, 2.9, 0.172414, 5.8 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'W50A', 'BHR', 1.5, 7, 0.000154211, 23.45, 4.8, 0.104167, 9.6 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'W50A', 'BHT', 1628855859.664772, 1628855910.864773, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.35, 1, 'N', -1, 0.163849, 0.76, 84.0, 207, 26, 0.05, 'Y',  3.15847, 0.746931, 1.55, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'W50A', 'BHT', 8.5, 999.9, 4.14526e-06, 3.5, 3.5, 0.142857, 7 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'W50A', 'BHT', 1.5, 7, 1.30927e-05, 13.95, 5.05, 0.0990099, 10.1 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'TKL', 'IM', 104.2, 103, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TKL', 'BHZ', 1628855860.789204, 1628855911.989204, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.2, 1, 'N', -1, 0.163849, 0.94, 104.2, 103, 284, 0.05, 'Y',  28.5614, 0.994118, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TKL', 'BHZ', 8.5, 999.9, 7.00518e-06, 2.9, 3.25, 0.153846, 6.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TKL', 'BHZ', 1.5, 7, 0.000200078, 29.35, 4.85, 0.103093, 9.7 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TKL', 'BHR', 1628855860.789204, 1628855911.989204, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.2, 1, 'N', -1, 0.163849, 0.94, 104.2, 103, 284, 0.05, 'Y',  11.869, 0.959211, 0.1, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TKL', 'BHR', 8.5, 999.9, 1.08607e-05, 4.3, 2.15, 0.232558, 4.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TKL', 'BHR', 1.5, 7, 0.000128906, 27.05, 4.7, 0.106383, 9.4 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TKL', 'BHT', 1628855860.789204, 1628855911.989204, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.2, 1, 'N', -1, 0.163849, 0.94, 104.2, 103, 284, 0.05, 'Y',  9.32647, 0.850943, 2.9, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TKL', 'BHT', 8.5, 999.9, 1.6295e-06, 2.5, 2.95, 0.169492, 5.9 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TKL', 'BHT', 1.5, 7, 1.51975e-05, 19, 4.85, 0.103093, 9.7 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'U49A', 'N4', 106.0, 312, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'U49A', 'BHZ', 1628855860.891407, 1628855912.091408, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.65, 1, 'N', -1, 0.163849, 0.95, 106.0, 312, 132, 0.05, 'Y',  20.662, 0.9978, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'U49A', 'BHZ', 8.5, 999.9, 9.0218e-06, 6.4, 3.6, 0.138889, 7.2 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'U49A', 'BHZ', 1.5, 7, 0.000186408, 29.7, 4.7, 0.106383, 9.4 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'U49A', 'BHR', 1628855860.891407, 1628855912.091408, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.65, 1, 'N', -1, 0.163849, 0.95, 106.0, 312, 132, 0.05, 'Y',  6.87597, 0.949381, 0.05, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'U49A', 'BHR', 8.5, 999.9, 1.84383e-05, 4.05, 2.55, 0.196078, 5.1 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'U49A', 'BHR', 1.5, 7, 0.000126781, 27.5, 4.65, 0.107527, 9.3 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'U49A', 'BHT', 1628855860.891407, 1628855912.091408, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.65, 1, 'N', -1, 0.163849, 0.95, 106.0, 312, 132, 0.05, 'Y',  6.17193, 0.908459, -9.6, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'U49A', 'BHT', 8.5, 999.9, 2.05619e-06, 6.601, 4.7, 0.106383, 9.4 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'U49A', 'BHT', 1.5, 7, 1.26907e-05, 28.551, 5.4, 0.0925926, 10.8 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'W52A', 'N4', 123.6, 134, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'W52A', 'BHZ', 1628855861.867104, 1628855923.307102, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.42, 1, 'N', -1, 0.155986, 1.11, 123.6, 134, 315, 0.06, 'Y',  11.595, 0.997162, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'W52A', 'BHZ', 8.5, 999.9, 1.7238e-05, 5.52, 3.9, 0.128205, 7.8 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'W52A', 'BHZ', 1.5, 7, 0.000199875, 34.8, 4.8, 0.104167, 9.59999 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'W52A', 'BHR', 1628855861.867104, 1628855923.307102, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.42, 1, 'N', -1, 0.155986, 1.11, 123.6, 134, 315, 0.06, 'Y',  12.9324, 0.996092, -0.3, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'W52A', 'BHR', 8.5, 999.9, 1.05508e-05, 7.68, 4.5, 0.111111, 9 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'W52A', 'BHR', 1.5, 7, 0.000136447, 32.7, 4.86, 0.102881, 9.71999 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'W52A', 'BHT', 1628855861.867104, 1628855923.307102, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.42, 1, 'N', -1, 0.155986, 1.11, 123.6, 134, 315, 0.06, 'Y',  1.6083, 0.637019, 19.56, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'W52A', 'BHT', 8.5, 999.9, 5.59655e-06, 4.8, 2.88, 0.173611, 5.76 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'W52A', 'BHT', 1.5, 7, 9.00092e-06, 10.8, 4.44, 0.112613, 8.88 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'T50A', 'N4', 127.2, 2, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'T50A', 'BHZ', 1628855862.066002, 1628855923.506001, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.12, 1, 'N', -1, 0.155986, 1.14, 127.2, 2, 182, 0.06, 'Y',  22.2076, 0.973135, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'T50A', 'BHZ', 8.5, 999.9, 7.50177e-06, 3.24, 2.58, 0.193798, 5.16 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'T50A', 'BHZ', 1.5, 7, 0.000166597, 28.68, 4.74, 0.105485, 9.48 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'T50A', 'BHR', 1628855862.066002, 1628855923.506001, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.12, 1, 'N', -1, 0.155986, 1.14, 127.2, 2, 182, 0.06, 'Y',  13.9158, 0.958634, 0.12, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'T50A', 'BHR', 8.5, 999.9, 8.46072e-06, 5.28, 2.64, 0.189394, 5.28 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'T50A', 'BHR', 1.5, 7, 0.000117738, 31.08, 4.8, 0.104167, 9.6 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'T50A', 'BHT', 1628855862.066002, 1628855923.506001, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.12, 1, 'N', -1, 0.155986, 1.14, 127.2, 2, 182, 0.06, 'Y',  1.29933, 0.449252, -0.6, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'T50A', 'BHT', 8.5, 999.9, 8.64788e-06, 3.48, 4.44, 0.112613, 8.88 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'T50A', 'BHT', 1.5, 7, 1.12365e-05, 23.94, 5.22, 0.0957854, 10.44 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'TZTN', 'US', 141.8, 58, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TZTN', 'BHZ', 1628855862.880424, 1628855934.560425, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.35, 1, 'N', -1, 0.155986, 1.28, 141.8, 58, 239, 0.07, 'Y',  7.60868, 0.994354, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TZTN', 'BHZ', 8.5, 999.9, 2.68924e-05, 7, 4.55, 0.10989, 9.1 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TZTN', 'BHZ', 1.5, 7, 0.000204615, 37.73, 4.83, 0.10352, 9.66 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TZTN', 'BHR', 1628855862.880424, 1628855934.560425, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.35, 1, 'N', -1, 0.155986, 1.28, 141.8, 58, 239, 0.07, 'Y',  14.7071, 0.984083, -0.14, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TZTN', 'BHR', 8.5, 999.9, 7.95552e-06, 6.02, 2.8, 0.178571, 5.6 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TZTN', 'BHR', 1.5, 7, 0.000117003, 35.42, 4.83, 0.10352, 9.66 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TZTN', 'BHT', 1628855862.880424, 1628855934.560425, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.35, 1, 'N', -1, 0.155986, 1.28, 141.8, 58, 239, 0.07, 'Y',  3.66039, 0.897379, -1.05, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TZTN', 'BHT', 8.5, 999.9, 6.1772e-06, 8.82, 5.53, 0.0904159, 11.06 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TZTN', 'BHT', 1.5, 7, 2.26109e-05, 37.87, 4.76, 0.105042, 9.52 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'X51A', 'N4', 145.7, 179, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'X51A', 'BHZ', 1628855863.094494, 1628855934.774494, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.14, 1, 'N', -1, 0.155986, 1.31, 145.7, 179, 359, 0.07, 'Y',  8.57555, 0.991464, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'X51A', 'BHZ', 8.5, 999.9, 1.87097e-05, 5.04, 3.99, 0.125313, 7.98 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'X51A', 'BHZ', 1.5, 7, 0.000160446, 40.67, 4.76, 0.105042, 9.52 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'X51A', 'BHR', 1628855863.094494, 1628855934.774494, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.14, 1, 'N', -1, 0.155986, 1.31, 145.7, 179, 359, 0.07, 'Y',  11.6366, 0.988745, -0.14, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'X51A', 'BHR', 8.5, 999.9, 1.20863e-05, 7.84, 4.62, 0.108225, 9.24 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'X51A', 'BHR', 1.5, 7, 0.000140644, 38.43, 4.76, 0.105042, 9.52 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'X51A', 'BHT', 1628855863.094494, 1628855934.774494, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.14, 1, 'N', -1, 0.155986, 1.31, 145.7, 179, 359, 0.07, 'Y',  2.53154, 0.646022, -14.98, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'X51A', 'BHT', 8.5, 999.9, 5.55546e-06, 5.88, 3.15, 0.15873, 6.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'X51A', 'BHT', 1.5, 7, 1.40639e-05, 40.11, 4.48, 0.111607, 8.96 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'V48A', 'N4', 174.0, 266, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into mtfilterid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into   mtvmodid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'V48A', 'BHZ', 1628855864.667744, 1628855936.347744, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.07, 1, 'N', -1, 0.155986, 1.57, 174.0, 266, 84, 0.07, 'Y',  47.8155, 0.988194, -0.21, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'V48A', 'BHZ', 8.5, 999.9, 3.28626e-06, 10.5, 8.47, 0.0590319, 16.94 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'V48A', 'BHZ', 1.5, 7, 0.000157134, 46.76, 4.83, 0.10352, 9.66 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 5, 'cus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'V48A', 'BHR', 1628855864.667744, 1628855936.347744, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.07, 1, 'N', -1, 0.155986, 1.57, 174.0, 266, 84, 0.07, 'Y',  8.72511, 0.995225, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'V48A', 'BHR', 8.5, 999.9, 1.08362e-05, 7.49, 3.29, 0.151976, 6.58 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'V48A', 'BHR', 1.5, 7, 9.45474e-05, 44.31, 4.76, 0.105042, 9.52 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'V48A', 'BHT', 1628855864.667744, 1628855936.347744, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.07, 1, 'N', -1, 0.155986, 1.57, 174.0, 266, 84, 0.07, 'Y',  2.99636, 0.534572, 14.91, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'V48A', 'BHT', 8.5, 999.9, 2.82199e-06, 10.78, 8.4, 0.0595238, 16.8 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'V48A', 'BHT', 1.5, 7, 8.45571e-06, 21.28, 5.11, 0.0978473, 10.22 );


.quit
