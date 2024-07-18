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
    -1.16e+14, 2.33e+13, 9.26e+13, -9.04e+13, -4.84e+14, -6.01e+14, 
 'DEVMT', 7.97e+21,     3.87,   0,   7,  93,     0.00, -999.00,   -0.035,    0.0000, 
  65.66, 3.27e-10, 'mtinv4.0.1:5', 'Y', 'N', 'N', 'ichinose1', CURRENT_TIMESTAMP ); 

INSERT into FOCAL_PLANE_STAGE 
 ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, strike2, dip2, rake2, 
   eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, 
   auth, devmo, isomo, dcmo, lddate ) 
VALUES 
( (SELECT   max(fpid) FROM fpid_seq), 
  (SELECT max(mtorid) FROM mtorid_seq), 
 'MT', 'MAJORDC',    83,  7,   32,   321, 86,   96, 
  7.69, 238, 49,  -7.69, 45, 41,   0.00, 141, 6, 
 'ichinose1', 7.97495e+21, 0, 7.41327e+21, CURRENT_TIMESTAMP ); 


INSERT into MT_ORIGIN_STAGE 
 ( lat, lon, depth, time, orid, evid, jdate, algorithm, auth, lddate )
VALUES
 ( 41.7392, 73.377, 6, 1261461278.000000, 
  (SELECT max(mtorid) FROM mtorid_seq), 
  -1, 2009356, 'mtinv4.0.1:5', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'ARLS', 'KR', 79.9, 80, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'ARLS', 'BHZ', 1261461279.437874, 1261461330.637875, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.160954, 0.72, 79.9, 80, 261, 0.05, 'Y',  34.9812, 0.986659, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'ARLS', 'BHZ', 8.5, 999.9, 9.11798e-06, 3.95, 3.95, 0.126582, 7.9 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'ARLS', 'BHZ', 1.5, 7, 0.000318957, 25.65, 5.7, 0.0877193, 11.4 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'ARLS', 'BHR', 1261461279.437874, 1261461330.637875, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.160954, 0.72, 79.9, 80, 261, 0.05, 'Y',  8.48345, 0.966719, -0.05, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'ARLS', 'BHR', 8.5, 999.9, 2.95593e-05, 4.95, 4.1, 0.121951, 8.2 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'ARLS', 'BHR', 1.5, 7, 0.000250765, 28.45, 5.9, 0.0847458, 11.8 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'ARLS', 'BHT', 1261461279.437874, 1261461330.637875, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.160954, 0.72, 79.9, 80, 261, 0.05, 'Y',  6.34608, 0.795988, -2.75, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'ARLS', 'BHT', 8.5, 999.9, 9.44989e-06, 2.8, 2.8, 0.178571, 5.6 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'ARLS', 'BHT', 1.5, 7, 5.99698e-05, 25.15, 6.15, 0.0813008, 12.3 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'EKS2', 'KN', 107.6, 18, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'EKS2', 'BHZ', 1261461280.980019, 1261461332.180019, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 1.35, 1, 'N', -1, 0.160954, 0.97, 107.6, 18, 198, 0.05, 'Y',  25.6845, 0.987766, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'EKS2', 'BHZ', 8.5, 999.9, 8.31786e-06, 6.7, 5.05, 0.0990099, 10.1 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'EKS2', 'BHZ', 1.5, 7, 0.00021364, 30.8, 5.85, 0.0854701, 11.7 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'EKS2', 'BHR', 1261461280.980019, 1261461332.180019, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 1.35, 1, 'N', -1, 0.160954, 0.97, 107.6, 18, 198, 0.05, 'Y',  14.496, 0.876017, 0.2, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'EKS2', 'BHR', 8.5, 999.9, 8.19394e-06, 4.4, 3.05, 0.163934, 6.1 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'EKS2', 'BHR', 1.5, 7, 0.000118779, 33.55, 6.15, 0.0813008, 12.3 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'EKS2', 'BHT', 1261461280.980019, 1261461332.180019, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 1.35, 1, 'N', -1, 0.160954, 0.97, 107.6, 18, 198, 0.05, 'Y',  22.9362, 0.774214, -5.25, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'EKS2', 'BHT', 8.5, 999.9, 7.72199e-06, 2.85, 2.85, 0.175439, 5.7 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'EKS2', 'BHT', 1.5, 7, 0.000177113, 31.15, 5.8, 0.0862069, 11.6 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'UCH', 'KN', 108.4, 60, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'UCH', 'BHZ', 1261461281.022743, 1261461332.222744, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.160954, 0.98, 108.4, 60, 240, 0.05, 'Y',  67.9686, 0.98501, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'UCH', 'BHZ', 8.5, 999.9, 3.50401e-06, 4.55, 2.2, 0.227273, 4.4 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'UCH', 'BHZ', 1.5, 7, 0.000238163, 32.7, 5.9, 0.0847457, 11.8 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'UCH', 'BHR', 1261461281.022743, 1261461332.222744, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.160954, 0.98, 108.4, 60, 240, 0.05, 'Y',  26.3857, 0.936038, 0.25, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'UCH', 'BHR', 8.5, 999.9, 9.21729e-06, 5.5, 5.5, 0.0909091, 11 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'UCH', 'BHR', 1.5, 7, 0.000243205, 35.65, 5.9, 0.0847457, 11.8 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'UCH', 'BHT', 1261461281.022743, 1261461332.222744, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.160954, 0.98, 108.4, 60, 240, 0.05, 'Y',  13.8063, 0.941641, 2.2, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'UCH', 'BHT', 8.5, 999.9, 7.86158e-06, 6.75, 6.2, 0.0806452, 12.4 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'UCH', 'BHT', 1.5, 7, 0.000108539, 27.5, 5.85, 0.0854701, 11.7 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'KBK', 'KN', 164.6, 51, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'KBK', 'BHZ', 1261461284.145935, 1261461355.825935, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.14, 1, 'N', -1, 0.16085, 1.48, 164.6, 51, 232, 0.07, 'Y',  35.8255, 0.970438, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'KBK', 'BHZ', 8.5, 999.9, 5.89466e-06, 10.22, 4.9, 0.102041, 9.8 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'KBK', 'BHZ', 1.5, 7, 0.000211179, 47.04, 5.74, 0.087108, 11.48 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'KBK', 'BHR', 1261461284.145935, 1261461355.825935, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.14, 1, 'N', -1, 0.16085, 1.48, 164.6, 51, 232, 0.07, 'Y',  12.6192, 0.898752, -0.07, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'KBK', 'BHR', 8.5, 999.9, 1.4257e-05, 3.57, 3.57, 0.140056, 7.14 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'KBK', 'BHR', 1.5, 7, 0.000179912, 49.91, 6.09, 0.0821018, 12.18 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'KBK', 'BHT', 1261461284.145935, 1261461355.825935, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.14, 1, 'N', -1, 0.16085, 1.48, 164.6, 51, 232, 0.07, 'Y',  5.64707, 0.770669, 1.19, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'KBK', 'BHT', 8.5, 999.9, 1.79183e-05, 3.15, 7.07, 0.0707214, 14.14 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'KBK', 'BHT', 1.5, 7, 0.000101186, 41.16, 7.56, 0.0661376, 15.12 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'CHM', 'KN', 179.7, 38, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'CHM', 'BHZ', 1261461284.985558, 1261461356.665558, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.07, 1, 'N', -1, 0.128726, 1.62, 179.7, 38, 219, 0.07, 'Y',  59.1322, 0.978724, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CHM', 'BHZ', 8.5, 999.9, 2.58004e-06, 4.2, 3.57, 0.140056, 7.14 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CHM', 'BHZ', 1.5, 7, 0.000152563, 56.7, 6.02, 0.0830565, 12.04 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CHM', 'BHR', 1261461284.985558, 1261461356.665558, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.07, 1, 'N', -1, 0.128726, 1.62, 179.7, 38, 219, 0.07, 'Y',  7.37698, 0.878609, 0.14, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CHM', 'BHR', 8.5, 999.9, 1.88169e-05, 10.5, 8.33, 0.060024, 16.66 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CHM', 'BHR', 1.5, 7, 0.000138811, 53.69, 6.02, 0.0830565, 12.04 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CHM', 'BHT', 1261461284.985558, 1261461356.665558, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.07, 1, 'N', -1, 0.128726, 1.62, 179.7, 38, 219, 0.07, 'Y',  11.2026, 0.955266, 3.57, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CHM', 'BHT', 8.5, 999.9, 1.03579e-05, 11.13, 8.54, 0.058548, 17.08 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CHM', 'BHT', 1.5, 7, 0.000116035, 49.91, 6.23, 0.0802568, 12.46 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'USP', 'KN', 193.1, 28, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'USP', 'BHZ', 1261461285.725018, 1261461367.645016, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.08, 1, 'N', -1, 0.128726, 1.74, 193.1, 28, 209, 0.08, 'Y',  16.9477, 0.980193, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'USP', 'BHZ', 8.5, 999.9, 8.11492e-06, 7.2, 4.8, 0.104167, 9.6 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'USP', 'BHZ', 1.5, 7, 0.000137529, 60, 6, 0.0833333, 12 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'USP', 'BHR', 1261461285.725018, 1261461367.645016, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.08, 1, 'N', -1, 0.128726, 1.74, 193.1, 28, 209, 0.08, 'Y',  30.5563, 0.754435, 0.08, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'USP', 'BHR', 8.5, 999.9, 5.17605e-06, 12, 8.88, 0.0563063, 17.76 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'USP', 'BHR', 1.5, 7, 0.000158161, 56.88, 6.48, 0.0771605, 12.96 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'USP', 'BHT', 1261461285.725018, 1261461367.645016, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.08, 1, 'N', -1, 0.128726, 1.74, 193.1, 28, 209, 0.08, 'Y',  16.1515, 0.800045, -7.04, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'USP', 'BHT', 8.5, 999.9, 8.43393e-06, 4.24, 4.24, 0.117925, 8.48 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'USP', 'BHT', 1.5, 7, 0.000136221, 58.24, 6.32, 0.0791139, 12.64 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'TKM2', 'KN', 224.7, 53, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'TKM2', 'BHZ', 1261461287.481018, 1261461389.881020, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.128726, 2.02, 224.7, 53, 235, 0.1, 'Y',  85.6528, 0.946303, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TKM2', 'BHZ', 8.5, 999.9, 1.87291e-06, 6.8, 4.3, 0.116279, 8.6 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TKM2', 'BHZ', 1.5, 7, 0.00016042, 68.3, 5.9, 0.0847457, 11.8 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TKM2', 'BHR', 1261461287.481018, 1261461389.881020, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.128726, 2.02, 224.7, 53, 235, 0.1, 'Y',  26.951, 0.928383, 0.1, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TKM2', 'BHR', 8.5, 999.9, 4.327e-06, 10.2, 5.9, 0.0847458, 11.8 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TKM2', 'BHR', 1.5, 7, 0.000116617, 65.2, 5.89999, 0.0847458, 11.8 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TKM2', 'BHT', 1261461287.481018, 1261461389.881020, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.128726, 2.02, 224.7, 53, 235, 0.1, 'Y',  11.9411, 0.910097, 1.8, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TKM2', 'BHT', 8.5, 999.9, 6.62957e-06, 6.4, 7.5, 0.0666667, 15 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TKM2', 'BHT', 1.5, 7, 7.91645e-05, 60, 5.9, 0.0847458, 11.8 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'ULHL', 'KN', 243.2, 76, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'ULHL', 'BHZ', 1261461288.508602, 1261461401.148602, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.44, 1, 'N', -1, 0.128726, 2.19, 243.2, 76, 258, 0.11, 'Y',  53.7417, 0.959513, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'ULHL', 'BHZ', 8.5, 999.9, 3.64991e-06, 14.85, 7.15, 0.0699301, 14.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'ULHL', 'BHZ', 1.5, 7, 0.000196153, 73.59, 5.82999, 0.0857634, 11.66 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.033, 0.1, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'wus', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'ULHL', 'BHR', 1261461288.508602, 1261461401.148602, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.44, 1, 'N', -1, 0.128726, 2.19, 243.2, 76, 258, 0.11, 'Y',  16.5912, 0.91702, 0.33, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'ULHL', 'BHR', 8.5, 999.9, 9.52343e-06, 11.44, 10.23, 0.0488759, 20.46 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'ULHL', 'BHR', 1.5, 7, 0.000158006, 70.4, 5.72, 0.0874126, 11.44 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'ULHL', 'BHT', 1261461288.508602, 1261461401.148602, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.44, 1, 'N', -1, 0.128726, 2.19, 243.2, 76, 258, 0.11, 'Y',  4.20454, 0.7563, -0.22, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'ULHL', 'BHT', 8.5, 999.9, 1.16437e-05, 14.63, 8.25, 0.0606061, 16.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'ULHL', 'BHT', 1.5, 7, 4.89566e-05, 59.84, 6.05, 0.0826446, 12.1 );


.quit
