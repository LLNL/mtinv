#!/bin/csh
########################################################
### Autogenerated C-shell script by setupMT.c        ###
########################################################

### 
### format: DIST DT NPTS T0 VRED  ## Net.Sta.Loc 
### 
cat >! distfile << EOF
179.74   0.07 2048 -1.00 18.00 ## net=KN sta=CHM loc=() R=179.74 Az=38.4155 KN.CHM.      nchan=1 nseg=1 (BHZ)
EOF

###
### depth file
###
cat >! depthfile << EOF
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
EOF

###
### Create Velocity Model file
###
cat >! wus.model << EOF
MODEL.01
TEST MODEL
ISOTROPIC
KGS
FLAT EARTH
1-D
CONSTANT VELOCITY
LINE08
LINE09
LINE10
LINE11
H       VP     VS   RHO  QP     QS    ETAP ETAS  FREFP FREFS
    4.00     4.52     2.61     2.39     500.0     250.0   0.0   0.0   1.0   1.0
   28.00     6.21     3.59     2.76     500.0     250.0   0.0   0.0   1.0   1.0
   20.00     7.73     4.34     3.22    1000.0     500.0   0.0   0.0   1.0   1.0
    0.00     7.64     4.29     3.19    1000.0     500.0   0.0   0.0   1.0   1.0
EOF

###
### generate hspec96.dat read by hspec96, options are -ALL -EQEX -EXF 
###
# hprep96 -M wus.model -d distfile -HS 2 -HR 0 -EQEX -NDEC 1
# hprep96 -M wus.model -d distfile -FHS depthfile -HR 0 -EQEX -NDEC 1
hprep96 -M wus.model -d distfile -FHS depthfile -HR 0 -EQEX -NDEC 1 -XF 1

###
### run hspec96 and send output to hspec96.out
###
hspec96 > hspec96.out

###
### read hspec96 output greens functions hspec96.grn and write sac binary files
###
hpulse96 -i -D | f96tosac -B

###
### Rename the output file (e.g., copy B0101ZDD.sac -> NM.CGM3.00.cus.ZDD.SAC)
###
/bin/cp B0101ZDD.sac KN.CHM..wus.1.ZDD.SAC
/bin/cp B0102RDD.sac KN.CHM..wus.1.RDD.SAC
/bin/cp B0103ZDS.sac KN.CHM..wus.1.ZDS.SAC
/bin/cp B0104RDS.sac KN.CHM..wus.1.RDS.SAC
/bin/cp B0105TDS.sac KN.CHM..wus.1.TDS.SAC
/bin/cp B0106ZSS.sac KN.CHM..wus.1.ZSS.SAC
/bin/cp B0107RSS.sac KN.CHM..wus.1.RSS.SAC
/bin/cp B0108TSS.sac KN.CHM..wus.1.TSS.SAC
/bin/cp B0109ZEX.sac KN.CHM..wus.1.ZEX.SAC
/bin/cp B0110REX.sac KN.CHM..wus.1.REX.SAC
/bin/cp B0201ZDD.sac KN.CHM..wus.2.ZDD.SAC
/bin/cp B0202RDD.sac KN.CHM..wus.2.RDD.SAC
/bin/cp B0203ZDS.sac KN.CHM..wus.2.ZDS.SAC
/bin/cp B0204RDS.sac KN.CHM..wus.2.RDS.SAC
/bin/cp B0205TDS.sac KN.CHM..wus.2.TDS.SAC
/bin/cp B0206ZSS.sac KN.CHM..wus.2.ZSS.SAC
/bin/cp B0207RSS.sac KN.CHM..wus.2.RSS.SAC
/bin/cp B0208TSS.sac KN.CHM..wus.2.TSS.SAC
/bin/cp B0209ZEX.sac KN.CHM..wus.2.ZEX.SAC
/bin/cp B0210REX.sac KN.CHM..wus.2.REX.SAC
/bin/cp B0301ZDD.sac KN.CHM..wus.3.ZDD.SAC
/bin/cp B0302RDD.sac KN.CHM..wus.3.RDD.SAC
/bin/cp B0303ZDS.sac KN.CHM..wus.3.ZDS.SAC
/bin/cp B0304RDS.sac KN.CHM..wus.3.RDS.SAC
/bin/cp B0305TDS.sac KN.CHM..wus.3.TDS.SAC
/bin/cp B0306ZSS.sac KN.CHM..wus.3.ZSS.SAC
/bin/cp B0307RSS.sac KN.CHM..wus.3.RSS.SAC
/bin/cp B0308TSS.sac KN.CHM..wus.3.TSS.SAC
/bin/cp B0309ZEX.sac KN.CHM..wus.3.ZEX.SAC
/bin/cp B0310REX.sac KN.CHM..wus.3.REX.SAC
/bin/cp B0401ZDD.sac KN.CHM..wus.4.ZDD.SAC
/bin/cp B0402RDD.sac KN.CHM..wus.4.RDD.SAC
/bin/cp B0403ZDS.sac KN.CHM..wus.4.ZDS.SAC
/bin/cp B0404RDS.sac KN.CHM..wus.4.RDS.SAC
/bin/cp B0405TDS.sac KN.CHM..wus.4.TDS.SAC
/bin/cp B0406ZSS.sac KN.CHM..wus.4.ZSS.SAC
/bin/cp B0407RSS.sac KN.CHM..wus.4.RSS.SAC
/bin/cp B0408TSS.sac KN.CHM..wus.4.TSS.SAC
/bin/cp B0409ZEX.sac KN.CHM..wus.4.ZEX.SAC
/bin/cp B0410REX.sac KN.CHM..wus.4.REX.SAC
/bin/cp B0501ZDD.sac KN.CHM..wus.5.ZDD.SAC
/bin/cp B0502RDD.sac KN.CHM..wus.5.RDD.SAC
/bin/cp B0503ZDS.sac KN.CHM..wus.5.ZDS.SAC
/bin/cp B0504RDS.sac KN.CHM..wus.5.RDS.SAC
/bin/cp B0505TDS.sac KN.CHM..wus.5.TDS.SAC
/bin/cp B0506ZSS.sac KN.CHM..wus.5.ZSS.SAC
/bin/cp B0507RSS.sac KN.CHM..wus.5.RSS.SAC
/bin/cp B0508TSS.sac KN.CHM..wus.5.TSS.SAC
/bin/cp B0509ZEX.sac KN.CHM..wus.5.ZEX.SAC
/bin/cp B0510REX.sac KN.CHM..wus.5.REX.SAC
/bin/cp B0601ZDD.sac KN.CHM..wus.6.ZDD.SAC
/bin/cp B0602RDD.sac KN.CHM..wus.6.RDD.SAC
/bin/cp B0603ZDS.sac KN.CHM..wus.6.ZDS.SAC
/bin/cp B0604RDS.sac KN.CHM..wus.6.RDS.SAC
/bin/cp B0605TDS.sac KN.CHM..wus.6.TDS.SAC
/bin/cp B0606ZSS.sac KN.CHM..wus.6.ZSS.SAC
/bin/cp B0607RSS.sac KN.CHM..wus.6.RSS.SAC
/bin/cp B0608TSS.sac KN.CHM..wus.6.TSS.SAC
/bin/cp B0609ZEX.sac KN.CHM..wus.6.ZEX.SAC
/bin/cp B0610REX.sac KN.CHM..wus.6.REX.SAC
/bin/cp B0701ZDD.sac KN.CHM..wus.7.ZDD.SAC
/bin/cp B0702RDD.sac KN.CHM..wus.7.RDD.SAC
/bin/cp B0703ZDS.sac KN.CHM..wus.7.ZDS.SAC
/bin/cp B0704RDS.sac KN.CHM..wus.7.RDS.SAC
/bin/cp B0705TDS.sac KN.CHM..wus.7.TDS.SAC
/bin/cp B0706ZSS.sac KN.CHM..wus.7.ZSS.SAC
/bin/cp B0707RSS.sac KN.CHM..wus.7.RSS.SAC
/bin/cp B0708TSS.sac KN.CHM..wus.7.TSS.SAC
/bin/cp B0709ZEX.sac KN.CHM..wus.7.ZEX.SAC
/bin/cp B0710REX.sac KN.CHM..wus.7.REX.SAC
/bin/cp B0801ZDD.sac KN.CHM..wus.8.ZDD.SAC
/bin/cp B0802RDD.sac KN.CHM..wus.8.RDD.SAC
/bin/cp B0803ZDS.sac KN.CHM..wus.8.ZDS.SAC
/bin/cp B0804RDS.sac KN.CHM..wus.8.RDS.SAC
/bin/cp B0805TDS.sac KN.CHM..wus.8.TDS.SAC
/bin/cp B0806ZSS.sac KN.CHM..wus.8.ZSS.SAC
/bin/cp B0807RSS.sac KN.CHM..wus.8.RSS.SAC
/bin/cp B0808TSS.sac KN.CHM..wus.8.TSS.SAC
/bin/cp B0809ZEX.sac KN.CHM..wus.8.ZEX.SAC
/bin/cp B0810REX.sac KN.CHM..wus.8.REX.SAC
/bin/cp B0901ZDD.sac KN.CHM..wus.9.ZDD.SAC
/bin/cp B0902RDD.sac KN.CHM..wus.9.RDD.SAC
/bin/cp B0903ZDS.sac KN.CHM..wus.9.ZDS.SAC
/bin/cp B0904RDS.sac KN.CHM..wus.9.RDS.SAC
/bin/cp B0905TDS.sac KN.CHM..wus.9.TDS.SAC
/bin/cp B0906ZSS.sac KN.CHM..wus.9.ZSS.SAC
/bin/cp B0907RSS.sac KN.CHM..wus.9.RSS.SAC
/bin/cp B0908TSS.sac KN.CHM..wus.9.TSS.SAC
/bin/cp B0909ZEX.sac KN.CHM..wus.9.ZEX.SAC
/bin/cp B0910REX.sac KN.CHM..wus.9.REX.SAC
/bin/cp B1001ZDD.sac KN.CHM..wus.10.ZDD.SAC
/bin/cp B1002RDD.sac KN.CHM..wus.10.RDD.SAC
/bin/cp B1003ZDS.sac KN.CHM..wus.10.ZDS.SAC
/bin/cp B1004RDS.sac KN.CHM..wus.10.RDS.SAC
/bin/cp B1005TDS.sac KN.CHM..wus.10.TDS.SAC
/bin/cp B1006ZSS.sac KN.CHM..wus.10.ZSS.SAC
/bin/cp B1007RSS.sac KN.CHM..wus.10.RSS.SAC
/bin/cp B1008TSS.sac KN.CHM..wus.10.TSS.SAC
/bin/cp B1009ZEX.sac KN.CHM..wus.10.ZEX.SAC
/bin/cp B1010REX.sac KN.CHM..wus.10.REX.SAC
/bin/cp B1101ZDD.sac KN.CHM..wus.11.ZDD.SAC
/bin/cp B1102RDD.sac KN.CHM..wus.11.RDD.SAC
/bin/cp B1103ZDS.sac KN.CHM..wus.11.ZDS.SAC
/bin/cp B1104RDS.sac KN.CHM..wus.11.RDS.SAC
/bin/cp B1105TDS.sac KN.CHM..wus.11.TDS.SAC
/bin/cp B1106ZSS.sac KN.CHM..wus.11.ZSS.SAC
/bin/cp B1107RSS.sac KN.CHM..wus.11.RSS.SAC
/bin/cp B1108TSS.sac KN.CHM..wus.11.TSS.SAC
/bin/cp B1109ZEX.sac KN.CHM..wus.11.ZEX.SAC
/bin/cp B1110REX.sac KN.CHM..wus.11.REX.SAC
/bin/cp B1201ZDD.sac KN.CHM..wus.12.ZDD.SAC
/bin/cp B1202RDD.sac KN.CHM..wus.12.RDD.SAC
/bin/cp B1203ZDS.sac KN.CHM..wus.12.ZDS.SAC
/bin/cp B1204RDS.sac KN.CHM..wus.12.RDS.SAC
/bin/cp B1205TDS.sac KN.CHM..wus.12.TDS.SAC
/bin/cp B1206ZSS.sac KN.CHM..wus.12.ZSS.SAC
/bin/cp B1207RSS.sac KN.CHM..wus.12.RSS.SAC
/bin/cp B1208TSS.sac KN.CHM..wus.12.TSS.SAC
/bin/cp B1209ZEX.sac KN.CHM..wus.12.ZEX.SAC
/bin/cp B1210REX.sac KN.CHM..wus.12.REX.SAC
/bin/cp B1301ZDD.sac KN.CHM..wus.13.ZDD.SAC
/bin/cp B1302RDD.sac KN.CHM..wus.13.RDD.SAC
/bin/cp B1303ZDS.sac KN.CHM..wus.13.ZDS.SAC
/bin/cp B1304RDS.sac KN.CHM..wus.13.RDS.SAC
/bin/cp B1305TDS.sac KN.CHM..wus.13.TDS.SAC
/bin/cp B1306ZSS.sac KN.CHM..wus.13.ZSS.SAC
/bin/cp B1307RSS.sac KN.CHM..wus.13.RSS.SAC
/bin/cp B1308TSS.sac KN.CHM..wus.13.TSS.SAC
/bin/cp B1309ZEX.sac KN.CHM..wus.13.ZEX.SAC
/bin/cp B1310REX.sac KN.CHM..wus.13.REX.SAC
/bin/cp B1401ZDD.sac KN.CHM..wus.14.ZDD.SAC
/bin/cp B1402RDD.sac KN.CHM..wus.14.RDD.SAC
/bin/cp B1403ZDS.sac KN.CHM..wus.14.ZDS.SAC
/bin/cp B1404RDS.sac KN.CHM..wus.14.RDS.SAC
/bin/cp B1405TDS.sac KN.CHM..wus.14.TDS.SAC
/bin/cp B1406ZSS.sac KN.CHM..wus.14.ZSS.SAC
/bin/cp B1407RSS.sac KN.CHM..wus.14.RSS.SAC
/bin/cp B1408TSS.sac KN.CHM..wus.14.TSS.SAC
/bin/cp B1409ZEX.sac KN.CHM..wus.14.ZEX.SAC
/bin/cp B1410REX.sac KN.CHM..wus.14.REX.SAC
/bin/cp B1501ZDD.sac KN.CHM..wus.15.ZDD.SAC
/bin/cp B1502RDD.sac KN.CHM..wus.15.RDD.SAC
/bin/cp B1503ZDS.sac KN.CHM..wus.15.ZDS.SAC
/bin/cp B1504RDS.sac KN.CHM..wus.15.RDS.SAC
/bin/cp B1505TDS.sac KN.CHM..wus.15.TDS.SAC
/bin/cp B1506ZSS.sac KN.CHM..wus.15.ZSS.SAC
/bin/cp B1507RSS.sac KN.CHM..wus.15.RSS.SAC
/bin/cp B1508TSS.sac KN.CHM..wus.15.TSS.SAC
/bin/cp B1509ZEX.sac KN.CHM..wus.15.ZEX.SAC
/bin/cp B1510REX.sac KN.CHM..wus.15.REX.SAC
/bin/cp B1601ZDD.sac KN.CHM..wus.16.ZDD.SAC
/bin/cp B1602RDD.sac KN.CHM..wus.16.RDD.SAC
/bin/cp B1603ZDS.sac KN.CHM..wus.16.ZDS.SAC
/bin/cp B1604RDS.sac KN.CHM..wus.16.RDS.SAC
/bin/cp B1605TDS.sac KN.CHM..wus.16.TDS.SAC
/bin/cp B1606ZSS.sac KN.CHM..wus.16.ZSS.SAC
/bin/cp B1607RSS.sac KN.CHM..wus.16.RSS.SAC
/bin/cp B1608TSS.sac KN.CHM..wus.16.TSS.SAC
/bin/cp B1609ZEX.sac KN.CHM..wus.16.ZEX.SAC
/bin/cp B1610REX.sac KN.CHM..wus.16.REX.SAC
/bin/cp B1701ZDD.sac KN.CHM..wus.17.ZDD.SAC
/bin/cp B1702RDD.sac KN.CHM..wus.17.RDD.SAC
/bin/cp B1703ZDS.sac KN.CHM..wus.17.ZDS.SAC
/bin/cp B1704RDS.sac KN.CHM..wus.17.RDS.SAC
/bin/cp B1705TDS.sac KN.CHM..wus.17.TDS.SAC
/bin/cp B1706ZSS.sac KN.CHM..wus.17.ZSS.SAC
/bin/cp B1707RSS.sac KN.CHM..wus.17.RSS.SAC
/bin/cp B1708TSS.sac KN.CHM..wus.17.TSS.SAC
/bin/cp B1709ZEX.sac KN.CHM..wus.17.ZEX.SAC
/bin/cp B1710REX.sac KN.CHM..wus.17.REX.SAC
/bin/cp B1801ZDD.sac KN.CHM..wus.18.ZDD.SAC
/bin/cp B1802RDD.sac KN.CHM..wus.18.RDD.SAC
/bin/cp B1803ZDS.sac KN.CHM..wus.18.ZDS.SAC
/bin/cp B1804RDS.sac KN.CHM..wus.18.RDS.SAC
/bin/cp B1805TDS.sac KN.CHM..wus.18.TDS.SAC
/bin/cp B1806ZSS.sac KN.CHM..wus.18.ZSS.SAC
/bin/cp B1807RSS.sac KN.CHM..wus.18.RSS.SAC
/bin/cp B1808TSS.sac KN.CHM..wus.18.TSS.SAC
/bin/cp B1809ZEX.sac KN.CHM..wus.18.ZEX.SAC
/bin/cp B1810REX.sac KN.CHM..wus.18.REX.SAC
/bin/cp B1901ZDD.sac KN.CHM..wus.19.ZDD.SAC
/bin/cp B1902RDD.sac KN.CHM..wus.19.RDD.SAC
/bin/cp B1903ZDS.sac KN.CHM..wus.19.ZDS.SAC
/bin/cp B1904RDS.sac KN.CHM..wus.19.RDS.SAC
/bin/cp B1905TDS.sac KN.CHM..wus.19.TDS.SAC
/bin/cp B1906ZSS.sac KN.CHM..wus.19.ZSS.SAC
/bin/cp B1907RSS.sac KN.CHM..wus.19.RSS.SAC
/bin/cp B1908TSS.sac KN.CHM..wus.19.TSS.SAC
/bin/cp B1909ZEX.sac KN.CHM..wus.19.ZEX.SAC
/bin/cp B1910REX.sac KN.CHM..wus.19.REX.SAC
/bin/cp B2001ZDD.sac KN.CHM..wus.20.ZDD.SAC
/bin/cp B2002RDD.sac KN.CHM..wus.20.RDD.SAC
/bin/cp B2003ZDS.sac KN.CHM..wus.20.ZDS.SAC
/bin/cp B2004RDS.sac KN.CHM..wus.20.RDS.SAC
/bin/cp B2005TDS.sac KN.CHM..wus.20.TDS.SAC
/bin/cp B2006ZSS.sac KN.CHM..wus.20.ZSS.SAC
/bin/cp B2007RSS.sac KN.CHM..wus.20.RSS.SAC
/bin/cp B2008TSS.sac KN.CHM..wus.20.TSS.SAC
/bin/cp B2009ZEX.sac KN.CHM..wus.20.ZEX.SAC
/bin/cp B2010REX.sac KN.CHM..wus.20.REX.SAC
/bin/cp B2101ZDD.sac KN.CHM..wus.21.ZDD.SAC
/bin/cp B2102RDD.sac KN.CHM..wus.21.RDD.SAC
/bin/cp B2103ZDS.sac KN.CHM..wus.21.ZDS.SAC
/bin/cp B2104RDS.sac KN.CHM..wus.21.RDS.SAC
/bin/cp B2105TDS.sac KN.CHM..wus.21.TDS.SAC
/bin/cp B2106ZSS.sac KN.CHM..wus.21.ZSS.SAC
/bin/cp B2107RSS.sac KN.CHM..wus.21.RSS.SAC
/bin/cp B2108TSS.sac KN.CHM..wus.21.TSS.SAC
/bin/cp B2109ZEX.sac KN.CHM..wus.21.ZEX.SAC
/bin/cp B2110REX.sac KN.CHM..wus.21.REX.SAC
/bin/cp B2201ZDD.sac KN.CHM..wus.22.ZDD.SAC
/bin/cp B2202RDD.sac KN.CHM..wus.22.RDD.SAC
/bin/cp B2203ZDS.sac KN.CHM..wus.22.ZDS.SAC
/bin/cp B2204RDS.sac KN.CHM..wus.22.RDS.SAC
/bin/cp B2205TDS.sac KN.CHM..wus.22.TDS.SAC
/bin/cp B2206ZSS.sac KN.CHM..wus.22.ZSS.SAC
/bin/cp B2207RSS.sac KN.CHM..wus.22.RSS.SAC
/bin/cp B2208TSS.sac KN.CHM..wus.22.TSS.SAC
/bin/cp B2209ZEX.sac KN.CHM..wus.22.ZEX.SAC
/bin/cp B2210REX.sac KN.CHM..wus.22.REX.SAC
/bin/cp B2301ZDD.sac KN.CHM..wus.23.ZDD.SAC
/bin/cp B2302RDD.sac KN.CHM..wus.23.RDD.SAC
/bin/cp B2303ZDS.sac KN.CHM..wus.23.ZDS.SAC
/bin/cp B2304RDS.sac KN.CHM..wus.23.RDS.SAC
/bin/cp B2305TDS.sac KN.CHM..wus.23.TDS.SAC
/bin/cp B2306ZSS.sac KN.CHM..wus.23.ZSS.SAC
/bin/cp B2307RSS.sac KN.CHM..wus.23.RSS.SAC
/bin/cp B2308TSS.sac KN.CHM..wus.23.TSS.SAC
/bin/cp B2309ZEX.sac KN.CHM..wus.23.ZEX.SAC
/bin/cp B2310REX.sac KN.CHM..wus.23.REX.SAC
/bin/cp B2401ZDD.sac KN.CHM..wus.24.ZDD.SAC
/bin/cp B2402RDD.sac KN.CHM..wus.24.RDD.SAC
/bin/cp B2403ZDS.sac KN.CHM..wus.24.ZDS.SAC
/bin/cp B2404RDS.sac KN.CHM..wus.24.RDS.SAC
/bin/cp B2405TDS.sac KN.CHM..wus.24.TDS.SAC
/bin/cp B2406ZSS.sac KN.CHM..wus.24.ZSS.SAC
/bin/cp B2407RSS.sac KN.CHM..wus.24.RSS.SAC
/bin/cp B2408TSS.sac KN.CHM..wus.24.TSS.SAC
/bin/cp B2409ZEX.sac KN.CHM..wus.24.ZEX.SAC
/bin/cp B2410REX.sac KN.CHM..wus.24.REX.SAC
/bin/cp B2501ZDD.sac KN.CHM..wus.25.ZDD.SAC
/bin/cp B2502RDD.sac KN.CHM..wus.25.RDD.SAC
/bin/cp B2503ZDS.sac KN.CHM..wus.25.ZDS.SAC
/bin/cp B2504RDS.sac KN.CHM..wus.25.RDS.SAC
/bin/cp B2505TDS.sac KN.CHM..wus.25.TDS.SAC
/bin/cp B2506ZSS.sac KN.CHM..wus.25.ZSS.SAC
/bin/cp B2507RSS.sac KN.CHM..wus.25.RSS.SAC
/bin/cp B2508TSS.sac KN.CHM..wus.25.TSS.SAC
/bin/cp B2509ZEX.sac KN.CHM..wus.25.ZEX.SAC
/bin/cp B2510REX.sac KN.CHM..wus.25.REX.SAC
