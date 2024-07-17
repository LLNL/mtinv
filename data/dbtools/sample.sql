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
    7.03e+19, 3.13e+19, -4.02e+20, -1.18e+19, -6.34e+19, 1.17e+20, 
 'FULLMT', 4.38e+21,     3.70,  23,  64,  13,    -0.23, -999.00,   -0.417,    3.3749, 
  60.15, 1.63e-12, 'mtinv4.0.0:6', 'Y', 'N', 'N', 'ichinose1', CURRENT_TIMESTAMP ); 

INSERT into FOCAL_PLANE_STAGE 
 ( fpid, orid, fp_type, decomp_type, strike1, dip1, rake1, strike2, dip2, rake2, 
 eval1, az1, pl1, eval2, az2, pl2, eval3, az3, pl3, auth, devmo, isomo, dcmo, lddate )
VALUES
( (SELECT   max(fpid) FROM fpid_seq), 
  (SELECT max(mtorid) FROM mtorid_seq), 
 'MT', 'FULLMT',   227, 50,  141,   345, 61,   48, 
  0.10, 203, 53,  -0.44, 104, 7,   0.04, 9, 36, 
 'ichinose1', 3.3817e+21, 1.002e+21, 5.60172e+20, CURRENT_TIMESTAMP ); 

INSERT into MT_ORIGIN_STAGE 
 ( lat, lon, depth, time, orid, evid, jdate, algorithm, auth, lddate )
VALUES
 ( 41.3116, 129.052, 7, 1506155357.000000, 
  (SELECT max(mtorid) FROM mtorid_seq), 
  -1, 2017266, 'mtinv4.0.0:6', 'ichinose1', CURRENT_TIMESTAMP ); 


INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'SEHB', 'KS', 344.9, 192, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'SEHB', 'BHZ', 1506155376.162445, 1506155504.162445, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125819, 3.10, 344.9, 192, 11, 0.125, 'Y',  2.07526, 0.740281, -0.25, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SEHB', 'BHZ', 8.5, 999.9, 4.77615e-06, 5.0009, 3.625, 0.137931, 7.25 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SEHB', 'BHZ', 1.5, 7, 9.91174e-06, 101.251, 4, 0.125, 8 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.1, 0.15, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'mdj2', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'SEHB', 'BHR', 1506155376.162445, 1506155504.162445, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125819, 3.10, 344.9, 192, 11, 0.125, 'Y',  4.55248, 0.722021, 9, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SEHB', 'BHR', 8.5, 999.9, 1.49476e-06, 20.1259, 4.125, 0.121212, 8.25 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SEHB', 'BHR', 1.5, 7, 6.80488e-06, 94.3759, 4.25, 0.117647, 8.5 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'SEHB', 'BHT', 1506155376.162445, 1506155504.162445, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125819, 3.10, 344.9, 192, 11, 0.125, 'Y',  1.26565, 0.539744, 49.625, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SEHB', 'BHT', 8.5, 999.9, 3.85026e-06, 5.0009, 4.125, 0.121212, 8.25 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SEHB', 'BHT', 1.5, 7, 4.87309e-06, 104.501, 4.375, 0.114286, 8.75 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'MDJ', 'IC', 370.0, 7, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'MDJ', 'BHZ', 1506155377.553066, 1506155505.553066, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.125, 1, 'N', -1, 0.125819, 3.33, 370.0, 7, 187, 0.125, 'Y',  2.16557, 0.761441, 0.25, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'MDJ', 'BHZ', 8.5, 999.9, 5.3686e-06, 11.7505, 4.125, 0.121212, 8.25 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'MDJ', 'BHZ', 1.5, 7, 1.16261e-05, 102.376, 4, 0.125, 8 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.09, 0.15, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'mdj2', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'MDJ', 'BHR', 1506155377.553066, 1506155505.553066, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.125, 1, 'N', -1, 0.125819, 3.33, 370.0, 7, 187, 0.125, 'Y',  1.82355, 0.626485, -0.625, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'MDJ', 'BHR', 8.5, 999.9, 4.6125e-06, 20.6255, 3.75, 0.133333, 7.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'MDJ', 'BHR', 1.5, 7, 8.41115e-06, 105.126, 3.875, 0.129032, 7.75 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'MDJ', 'BHT', 1506155377.553066, 1506155505.553066, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.125, 1, 'N', -1, 0.125819, 3.33, 370.0, 7, 187, 0.125, 'Y',  6.96144, 0.941117, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'MDJ', 'BHT', 8.5, 999.9, 3.75813e-06, 10.0005, 3.875, 0.129032, 7.75 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'MDJ', 'BHT', 1.5, 7, 2.6162e-05, 90.7505, 4, 0.125, 8 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'SEO2', 'KS', 462.0, 204, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'SEO2', 'BHZ', 1506155382.669395, 1506155541.389397, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.155, 1, 'N', -1, 0.125745, 4.16, 462.0, 204, 23, 0.155, 'Y',  2.70294, 0.674811, -1.085, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SEO2', 'BHZ', 8.5, 999.9, 4.14538e-06, 11.9355, 6.045, 0.082713, 12.09 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SEO2', 'BHZ', 1.5, 7, 1.12047e-05, 132.681, 5.89, 0.0848897, 11.78 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.06, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'mdj2', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'SEO2', 'BHR', 1506155382.669395, 1506155541.389397, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.155, 1, 'N', -1, 0.125745, 4.16, 462.0, 204, 23, 0.155, 'Y',  4.23675, 0.736368, -0.93, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SEO2', 'BHR', 8.5, 999.9, 2.28711e-06, 6.2005, 5.735, 0.087184, 11.47 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SEO2', 'BHR', 1.5, 7, 9.68991e-06, 135.471, 5.735, 0.087184, 11.47 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'SEO2', 'BHT', 1506155382.669395, 1506155541.389397, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -0.155, 1, 'N', -1, 0.125745, 4.16, 462.0, 204, 23, 0.155, 'Y',  15.7525, 0.902984, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'SEO2', 'BHT', 8.5, 999.9, 1.54354e-06, 18.2905, 5.89, 0.0848897, 11.78 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'SEO2', 'BHT', 1.5, 7, 2.43147e-05, 114.701, 5.425, 0.0921659, 10.85 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'INCN', 'IU', 474.2, 207, 'Y', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'INCN', 'BHZ', 1506155383.344185, 1506155542.064186, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125745, 4.27, 474.2, 207, 25, 0.155, 'Y',  2.45555, 0.623706, -0.31, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'INCN', 'BHZ', 8.5, 999.9, 3.34585e-06, 8.68054, 4.65, 0.107527, 9.3 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'INCN', 'BHZ', 1.5, 7, 8.21589e-06, 140.896, 5.58, 0.0896057, 11.16 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.06, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'mdj2', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'INCN', 'BHR', 1506155383.344185, 1506155542.064186, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125745, 4.27, 474.2, 207, 25, 0.155, 'Y',  1.84633, 0.491942, -0.93, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'INCN', 'BHR', 8.5, 999.9, 3.47285e-06, 9.76554, 4.805, 0.104058, 9.61 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'INCN', 'BHR', 1.5, 7, 6.41204e-06, 132.991, 5.89, 0.0848897, 11.78 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'INCN', 'BHT', 1506155383.344185, 1506155542.064186, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125745, 4.27, 474.2, 207, 25, 0.155, 'Y',  3.63617, 0.828951, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'INCN', 'BHT', 8.5, 999.9, 5.64313e-06, 22.7855, 6.51, 0.0768049, 13.02 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'INCN', 'BHT', 1.5, 7, 2.05194e-05, 117.646, 5.11501, 0.0977516, 10.23 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'CHJ2', 'KS', 501.8, 191, 'N', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'CHJ2', 'BHZ', 1506155384.879566, 1506155569.199574, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125745, 4.52, 501.8, 191, 10, 0.18, 'N',  5.02894, 0.901303, -19.44, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CHJ2', 'BHZ', 8.5, 999.9, 2.47719e-06, 30.9605, 4.68, 0.106838, 9.36 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CHJ2', 'BHZ', 1.5, 7, 1.24577e-05, 156.061, 5.04001, 0.0992062, 10.08 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.08, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'mdj2', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CHJ2', 'BHR', 1506155384.879566, 1506155569.199574, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125745, 4.52, 501.8, 191, 10, 0.18, 'N',  4.70803, 0.702096, -9.18, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CHJ2', 'BHR', 8.5, 999.9, 1.53807e-06, 10.4405, 5.22, 0.0957854, 10.44 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CHJ2', 'BHR', 1.5, 7, 7.24129e-06, 143.461, 5.22, 0.0957854, 10.44 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'CHJ2', 'BHT', 1506155384.879566, 1506155569.199574, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, 0, 1, 'N', -1, 0.125745, 4.52, 501.8, 191, 10, 0.18, 'N',  5.78376, 0.815108, -15.66, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'CHJ2', 'BHT', 8.5, 999.9, 1.21293e-06, 24.8405, 5.94, 0.0841751, 11.88 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'CHJ2', 'BHT', 1.5, 7, 7.01531e-06, 138.421, 5.04001, 0.0992062, 10.08 );



INSERT into mtdataid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_DATA_STAGE
 ( mtdataid, mtid, sta, net, distkm, azimuth, used, quality, auth, lddate ) 
VALUES
 ( ( SELECT max(mtdataid) FROM mtdataid_seq ), 
   ( SELECT     max(mtid) FROM mtid_seq ), 
   'TJN', 'KG', 567.4, 195, 'N', '-', 'ichinose1', CURRENT_TIMESTAMP ); 


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
    'FULL-D', 'TJN', 'BHZ', 1506155388.519844, 1506155583.079842, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -7.22, 1, 'N', -1, 0.125745, 5.11, 567.4, 195, 14, 0.19, 'N',  3.10971, 0.611657, -2.66, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TJN', 'BHZ', 8.5, 999.9, 1.49363e-06, 11.0205, 4.75, 0.105263, 9.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TJN', 'BHZ', 1.5, 7, 4.64477e-06, 168.151, 4.94, 0.101215, 9.88 );

INSERT into MT_FILTER_STAGE
  ( mtfilterid, filtertype, filterdesign, npole, lcorner, hcorner, npass ) 
VALUES
  ( (SELECT max(mtfilterid) FROM mtfilterid_seq), 'BP', 'BUTTERWORTH', 3, 0.08, 0.12, 2 );

INSERT into MT_EARTHMODEL_STAGE
 ( mtvmodid, dimension, nlon, nlat, ndep, modelname, auth, lddate ) 
VALUES
  ( ( SELECT max(mtvmodid) FROM mtvmodid_seq),   '1-D', 1, 1, 4, 'mdj2', 'ichinose1', CURRENT_TIMESTAMP );

INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TJN', 'BHR', 1506155388.519844, 1506155583.079842, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -7.22, 1, 'N', -1, 0.125745, 5.11, 567.4, 195, 14, 0.19, 'N',  4.13026, 0.796356, -2.85, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TJN', 'BHR', 8.5, 999.9, 1.21284e-06, 33.2505, 4.56, 0.109649, 9.12 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TJN', 'BHR', 1.5, 7, 5.00935e-06, 160.741, 5.12999, 0.0974661, 10.26 );



INSERT into mtwfsegid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );

INSERT into MT_WAVEFORM_SEGMENT_STAGE
  ( mtwfsegid, mtdataid, wfsegtype, sta, chan, start_time, end_time, mtfilterid, 
  mtvmodid, redvel, tshift, ampweight, distnorm, R0, RAYPARAM, distdeg, 
  distkm, az, baz, interpsamprate, used, snr, xcor, lagtime, algor, lddate ) 
VALUES
 ( (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   (SELECT  max(mtdataid) FROM mtdataid_seq), 
    'FULL-D', 'TJN', 'BHT', 1506155388.519844, 1506155583.079842, 
  (SELECT max(mtfilterid) FROM mtfilterid_seq), 
  (SELECT max(mtvmodid) FROM mtvmodid_seq), 
  18, -7.22, 1, 'N', -1, 0.125745, 5.11, 567.4, 195, 14, 0.19, 'N',  3.74772, 0.911866, 0, 'FK',  CURRENT_TIMESTAMP ); 


INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'NOISE', 'TJN', 'BHT', 8.5, 999.9, 2.22703e-06, 12.9205, 4.75, 0.105263, 9.5 );

INSERT into phaid_seq ( lddate ) VALUES ( CURRENT_TIMESTAMP );
INSERT into MT_PHASE_DATA_STAGE
 ( phaid, mtwfsegid, phasetype, sta, chan, gvlo, gvhi, amp, time, dur, freq, per )
VALUES
 ( (SELECT     max(phaid) FROM     phaid_seq), 
   (SELECT max(mtwfsegid) FROM mtwfsegid_seq), 
   'SIGNAL', 'TJN', 'BHT', 1.5, 7, 8.3463e-06, 146.681, 4.93999, 0.101215, 9.87997 );


.quit
