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
    1.10e+15, -9.09e+14, -1.95e+14, -8.98e+14, 4.40e+14, 4.22e+14, 
 'DEVMT', 1.49e+22,     4.05,   0,   0, 100,     0.00, -999.00,   -0.000,    0.0000, 
  86.46, 3.49e-10, 'mtinv4.0.1:5', 'Y', 'N', 'N', 'ichinose1', CURRENT_TIMESTAMP ); 

INSERT into FOCAL_PLANE_STAGE 
 ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, strike2, dip2, rake2, 
   eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, 
   auth, devmo, isomo, dcmo, lddate ) 
VALUES 
( (SELECT   max(fpid) FROM fpid_seq), 
  (SELECT max(mtorid) FROM mtorid_seq), 
 'MT', 'MAJORDC',    26, 67, -170,   292, 80,  -24, 
  1.49, 341, 9,  -1.49, 247, 23,   0.00, 91, 65, 
 'ichinose1', 1.48964e+22, 1.1259e+15, 1.48878e+22, CURRENT_TIMESTAMP ); 


INSERT into MT_ORIGIN_STAGE 
 ( lat, lon, depth, time, orid, evid, jdate, algorithm, auth, lddate )
VALUES
 ( 36.9077, -90.543, 14, 1637203984.000000, 
  (SELECT max(mtorid) FROM mtorid_seq), 
  -1, 2021322, 'mtinv4.0.1:5', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'CGM3', 'NM', 89.6, 61, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'CGM3', 'BHZ', 1637203988.976908, 1637204040.176908, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.5, 1, 'N', -1, 0.155986, 0.81, 89.6, 61, 241, 0.05, 'Y',  9.74762, 0.994092, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CGM3', 'BHZ', 8.5, 999.9, 4.13372e-05, 5.551, 3.1, 0.16129, 6.2 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CGM3', 'BHZ', 1.5, 7, 0.000402939, 23.201, 4.15, 0.120482, 8.3 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'CGM3', 'BHR', 1637203988.976908, 1637204040.176908, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.5, 1, 'N', -1, 0.155986, 0.81, 89.6, 61, 241, 0.05, 'Y',  7.72792, 0.966223, 0.1, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CGM3', 'BHR', 8.5, 999.9, 3.8305e-05, 4.401, 2.85, 0.175439, 5.7 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CGM3', 'BHR', 1.5, 7, 0.000296017, 20.501, 4.25, 0.117647, 8.5 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CGM3', 'BHT', 1637203988.976908, 1637204040.176908, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.5, 1, 'N', -1, 0.155986, 0.81, 89.6, 61, 241, 0.05, 'Y',  10.7733, 0.973767, 0.05, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CGM3', 'BHT', 8.5, 999.9, 1.69376e-05, 5.1, 3.2, 0.15625, 6.4 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CGM3', 'BHT', 1.5, 7, 0.000182474, 21.2, 4.25, 0.117647, 8.5 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'PENM', 'NM', 96.1, 122, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'PENM', 'BHZ', 1637203989.338575, 1637204040.538576, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.2, 1, 'N', -1, 0.155986, 0.86, 96.1, 122, 302, 0.05, 'Y',  10.4277, 0.961657, -0.25, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'PENM', 'BHZ', 8.5, 999.9, 9.96076e-06, 5.25, 3.25, 0.153846, 6.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'PENM', 'BHZ', 1.5, 7, 0.000103867, 27, 4.25, 0.117647, 8.5 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'PENM', 'BHR', 1637203989.338575, 1637204040.538576, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.2, 1, 'N', -1, 0.155986, 0.86, 96.1, 122, 302, 0.05, 'Y',  4.10101, 0.882185, 0.6, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'PENM', 'BHR', 8.5, 999.9, 2.42105e-05, 5.95, 2.65, 0.188679, 5.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'PENM', 'BHR', 1.5, 7, 9.92876e-05, 20.35, 3.65, 0.136986, 7.3 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'PENM', 'BHT', 1637203989.338575, 1637204040.538576, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.2, 1, 'N', -1, 0.155986, 0.86, 96.1, 122, 302, 0.05, 'Y',  16.5638, 0.990727, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'PENM', 'BHT', 8.5, 999.9, 4.40971e-05, 4.2, 3.15, 0.15873, 6.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'PENM', 'BHT', 1.5, 7, 0.000730417, 22.45, 3.95, 0.126582, 7.9 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'HENM', 'NM', 97.6, 102, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'HENM', 'BHZ', 1637203989.423479, 1637204040.623480, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.155986, 0.88, 97.6, 102, 283, 0.05, 'Y',  7.64624, 0.961015, 0.45, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'HENM', 'BHZ', 8.5, 999.9, 2.31196e-05, 6.05, 4.1, 0.121951, 8.2 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'HENM', 'BHZ', 1.5, 7, 0.000176778, 24.8, 4.45, 0.11236, 8.9 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'HENM', 'BHR', 1637203989.423479, 1637204040.623480, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.155986, 0.88, 97.6, 102, 283, 0.05, 'Y',  20.8989, 0.96689, 0.5, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'HENM', 'BHR', 8.5, 999.9, 9.45271e-06, 4.3, 3.35, 0.149254, 6.7 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'HENM', 'BHR', 1.5, 7, 0.000197551, 26.6, 4.35, 0.114943, 8.7 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'HENM', 'BHT', 1637203989.423479, 1637204040.623480, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.3, 1, 'N', -1, 0.155986, 0.88, 97.6, 102, 283, 0.05, 'Y',  16.3209, 0.9785, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'HENM', 'BHT', 8.5, 999.9, 3.41275e-05, 5.1, 3.25, 0.153846, 6.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'HENM', 'BHT', 1.5, 7, 0.00055699, 22.9, 4.05, 0.123457, 8.1 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'GNAR', 'NM', 114.8, 156, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'GNAR', 'BHZ', 1637203990.376423, 1637204041.576424, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.6, 1, 'N', -1, 0.155986, 1.03, 114.8, 156, 336, 0.05, 'Y',  8.87019, 0.992078, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'GNAR', 'BHZ', 8.5, 999.9, 3.12154e-05, 3, 4.15, 0.120482, 8.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'GNAR', 'BHZ', 1.5, 7, 0.000276886, 28.3, 4.15, 0.120482, 8.3 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'GNAR', 'BHR', 1637203990.376423, 1637204041.576424, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.6, 1, 'N', -1, 0.155986, 1.03, 114.8, 156, 336, 0.05, 'Y',  12.6337, 0.968078, 0.5, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'GNAR', 'BHR', 8.5, 999.9, 3.02883e-05, 2.15, 2.65, 0.188679, 5.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'GNAR', 'BHR', 1.5, 7, 0.000382652, 29.85, 4.2, 0.119048, 8.4 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'GNAR', 'BHT', 1637203990.376423, 1637204041.576424, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.6, 1, 'N', -1, 0.155986, 1.03, 114.8, 156, 336, 0.05, 'Y',  2.28497, 0.850703, -6.95, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'GNAR', 'BHT', 8.5, 999.9, 3.80902e-05, 4, 3.15, 0.15873, 6.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'GNAR', 'BHT', 1.5, 7, 8.7035e-05, 34.35, 3.9, 0.128205, 7.8 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'CCM', 'IU', 141.8, 334, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'CCM', 'BHZ', 1637203991.875662, 1637204063.555662, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.49, 1, 'N', -1, 0.148756, 1.28, 141.8, 334, 154, 0.07, 'Y',  26.5621, 0.984361, -0.07, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CCM', 'BHZ', 8.5, 999.9, 8.69771e-06, 5.46054, 3.15, 0.15873, 6.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CCM', 'BHZ', 1.5, 7, 0.00023103, 38.1505, 4.06, 0.123153, 8.12 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'CCM', 'BHR', 1637203991.875662, 1637204063.555662, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.49, 1, 'N', -1, 0.148756, 1.28, 141.8, 334, 154, 0.07, 'Y',  13.217, 0.98503, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CCM', 'BHR', 8.5, 999.9, 1.64833e-05, 6.44054, 4.97, 0.100604, 9.94 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CCM', 'BHR', 1.5, 7, 0.00021786, 35.8405, 4.27, 0.117096, 8.54 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CCM', 'BHT', 1637203991.875662, 1637204063.555662, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.49, 1, 'N', -1, 0.148756, 1.28, 141.8, 334, 154, 0.07, 'Y',  8.63307, 0.924987, -0.21, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CCM', 'BHT', 8.5, 999.9, 9.66653e-06, 7.28054, 3.64, 0.137363, 7.28 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CCM', 'BHT', 1.5, 7, 8.34519e-05, 32.2705, 4.06, 0.123153, 8.12 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'CCM', 'IU', 141.8, 334, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'CCM', 'BHZ', 1637203991.875662, 1637204063.555662, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.49, 1, 'N', -1, 0.148756, 1.28, 141.8, 334, 154, 0.07, 'Y',  26.5345, 0.984259, -0.07, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CCM', 'BHZ', 8.5, 999.9, 8.825e-06, 5.46054, 3.15, 0.15873, 6.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CCM', 'BHZ', 1.5, 7, 0.000234167, 38.1505, 3.99, 0.125313, 7.98 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'CCM', 'BHR', 1637203991.875662, 1637204063.555662, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.49, 1, 'N', -1, 0.148756, 1.28, 141.8, 334, 154, 0.07, 'Y',  13.1824, 0.984842, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CCM', 'BHR', 8.5, 999.9, 1.66215e-05, 6.44054, 4.97, 0.100604, 9.94 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CCM', 'BHR', 1.5, 7, 0.000219111, 35.8405, 4.27, 0.117096, 8.54 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CCM', 'BHT', 1637203991.875662, 1637204063.555662, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.49, 1, 'N', -1, 0.148756, 1.28, 141.8, 334, 154, 0.07, 'Y',  8.62218, 0.925306, -0.14, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CCM', 'BHT', 8.5, 999.9, 9.79488e-06, 7.28054, 3.64, 0.137363, 7.28 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CCM', 'BHT', 1.5, 7, 8.44533e-05, 32.2705, 3.99, 0.125313, 7.98 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'SIUC', 'NM', 147.5, 52, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'SIUC', 'BHZ', 1637203992.195951, 1637204063.875950, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.49, 1, 'N', -1, 0.148756, 1.33, 147.5, 52, 233, 0.07, 'Y',  12.9735, 0.952254, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SIUC', 'BHZ', 8.5, 999.9, 2.89887e-05, 7, 2.17, 0.230415, 4.34 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SIUC', 'BHZ', 1.5, 7, 0.000376084, 36.68, 4.48, 0.111607, 8.96 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'SIUC', 'BHR', 1637203992.195951, 1637204063.875950, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.49, 1, 'N', -1, 0.148756, 1.33, 147.5, 52, 233, 0.07, 'Y',  6.98677, 0.813691, 0.49, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SIUC', 'BHR', 8.5, 999.9, 3.18043e-05, 5.81, 3.15, 0.15873, 6.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SIUC', 'BHR', 1.5, 7, 0.000222209, 38.15, 4.69, 0.10661, 9.38 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'SIUC', 'BHT', 1637203992.195951, 1637204063.875950, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0.49, 1, 'N', -1, 0.148756, 1.33, 147.5, 52, 233, 0.07, 'Y',  12.0918, 0.949167, -0.28, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SIUC', 'BHT', 8.5, 999.9, 2.46011e-05, 3.57, 5.6, 0.0892857, 11.2 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SIUC', 'BHT', 1.5, 7, 0.000297473, 33.04, 4.2, 0.119048, 8.4 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'SLM', 'NM', 193.9, 8, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'SLM', 'BHZ', 1637203994.773388, 1637204076.693386, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.24, 1, 'N', -1, 0.121914, 1.74, 193.9, 8, 188, 0.08, 'Y',  11.7748, 0.936905, 0.32, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SLM', 'BHZ', 8.5, 999.9, 1.86791e-05, 4.96, 2.64, 0.189394, 5.28 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SLM', 'BHZ', 1.5, 7, 0.000219942, 49.76, 4.24, 0.117924, 8.48 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.075, 0.15, 2 );

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
    'FULL-D', 'SLM', 'BHR', 1637203994.773388, 1637204076.693386, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.24, 1, 'N', -1, 0.121914, 1.74, 193.9, 8, 188, 0.08, 'Y',  24.4164, 0.945124, 0.32, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SLM', 'BHR', 8.5, 999.9, 7.13471e-06, 3.52, 3.12, 0.160256, 6.24 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SLM', 'BHR', 1.5, 7, 0.000174204, 51.84, 4.32, 0.115741, 8.64 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'SLM', 'BHT', 1637203994.773388, 1637204076.693386, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.24, 1, 'N', -1, 0.121914, 1.74, 193.9, 8, 188, 0.08, 'Y',  14.4728, 0.973842, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SLM', 'BHT', 8.5, 999.9, 2.13861e-05, 7.44, 3.6, 0.138889, 7.2 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SLM', 'BHT', 1.5, 7, 0.000309517, 43.92, 4.16, 0.120192, 8.32 );


.quit
