#!/bin/csh
########################################################
### Autogenerated C-shell script by setupMT.c        ###
########################################################

### 
### format: DIST DT NPTS T0 VRED  ## Net.Sta.Loc 
### 
cat >! distfile << EOF
174.019   0.07 2048 -1.00 18.00 ## net=N4 sta=V48A loc=(00) R=174.019 Az=265.556 N4.V48A.00   nchan=1 nseg=1 (HHZ)
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
cat >! cus.model << EOF
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
    1.00     5.00     2.89     2.50     581.0     258.0   0.0   0.0   1.0   1.0
    9.00     6.10     3.52     2.73     625.0     275.5   0.0   0.0   1.0   1.0
   10.00     6.40     3.70     2.82     671.1     275.5   0.0   0.0   1.0   1.0
   20.00     6.70     3.87     2.90     671.1     297.6   0.0   0.0   1.0   1.0
    0.00     8.15     4.70     3.36     515.4     232.0   0.0   0.0   1.0   1.0
EOF

###
### generate hspec96.dat read by hspec96, options are -ALL -EQEX -EXF 
###
# hprep96 -M cus.model -d distfile -HS 2 -HR 0 -EQEX -NDEC 1
# hprep96 -M cus.model -d distfile -FHS depthfile -HR 0 -EQEX -NDEC 1
hprep96 -M cus.model -d distfile -FHS depthfile -HR 0 -EQEX -NDEC 1 -XF 1

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
/bin/cp B0101ZDD.sac N4.V48A.00.cus.1.ZDD.SAC
/bin/cp B0102RDD.sac N4.V48A.00.cus.1.RDD.SAC
/bin/cp B0103ZDS.sac N4.V48A.00.cus.1.ZDS.SAC
/bin/cp B0104RDS.sac N4.V48A.00.cus.1.RDS.SAC
/bin/cp B0105TDS.sac N4.V48A.00.cus.1.TDS.SAC
/bin/cp B0106ZSS.sac N4.V48A.00.cus.1.ZSS.SAC
/bin/cp B0107RSS.sac N4.V48A.00.cus.1.RSS.SAC
/bin/cp B0108TSS.sac N4.V48A.00.cus.1.TSS.SAC
/bin/cp B0109ZEX.sac N4.V48A.00.cus.1.ZEX.SAC
/bin/cp B0110REX.sac N4.V48A.00.cus.1.REX.SAC
/bin/cp B0201ZDD.sac N4.V48A.00.cus.2.ZDD.SAC
/bin/cp B0202RDD.sac N4.V48A.00.cus.2.RDD.SAC
/bin/cp B0203ZDS.sac N4.V48A.00.cus.2.ZDS.SAC
/bin/cp B0204RDS.sac N4.V48A.00.cus.2.RDS.SAC
/bin/cp B0205TDS.sac N4.V48A.00.cus.2.TDS.SAC
/bin/cp B0206ZSS.sac N4.V48A.00.cus.2.ZSS.SAC
/bin/cp B0207RSS.sac N4.V48A.00.cus.2.RSS.SAC
/bin/cp B0208TSS.sac N4.V48A.00.cus.2.TSS.SAC
/bin/cp B0209ZEX.sac N4.V48A.00.cus.2.ZEX.SAC
/bin/cp B0210REX.sac N4.V48A.00.cus.2.REX.SAC
/bin/cp B0301ZDD.sac N4.V48A.00.cus.3.ZDD.SAC
/bin/cp B0302RDD.sac N4.V48A.00.cus.3.RDD.SAC
/bin/cp B0303ZDS.sac N4.V48A.00.cus.3.ZDS.SAC
/bin/cp B0304RDS.sac N4.V48A.00.cus.3.RDS.SAC
/bin/cp B0305TDS.sac N4.V48A.00.cus.3.TDS.SAC
/bin/cp B0306ZSS.sac N4.V48A.00.cus.3.ZSS.SAC
/bin/cp B0307RSS.sac N4.V48A.00.cus.3.RSS.SAC
/bin/cp B0308TSS.sac N4.V48A.00.cus.3.TSS.SAC
/bin/cp B0309ZEX.sac N4.V48A.00.cus.3.ZEX.SAC
/bin/cp B0310REX.sac N4.V48A.00.cus.3.REX.SAC
/bin/cp B0401ZDD.sac N4.V48A.00.cus.4.ZDD.SAC
/bin/cp B0402RDD.sac N4.V48A.00.cus.4.RDD.SAC
/bin/cp B0403ZDS.sac N4.V48A.00.cus.4.ZDS.SAC
/bin/cp B0404RDS.sac N4.V48A.00.cus.4.RDS.SAC
/bin/cp B0405TDS.sac N4.V48A.00.cus.4.TDS.SAC
/bin/cp B0406ZSS.sac N4.V48A.00.cus.4.ZSS.SAC
/bin/cp B0407RSS.sac N4.V48A.00.cus.4.RSS.SAC
/bin/cp B0408TSS.sac N4.V48A.00.cus.4.TSS.SAC
/bin/cp B0409ZEX.sac N4.V48A.00.cus.4.ZEX.SAC
/bin/cp B0410REX.sac N4.V48A.00.cus.4.REX.SAC
/bin/cp B0501ZDD.sac N4.V48A.00.cus.5.ZDD.SAC
/bin/cp B0502RDD.sac N4.V48A.00.cus.5.RDD.SAC
/bin/cp B0503ZDS.sac N4.V48A.00.cus.5.ZDS.SAC
/bin/cp B0504RDS.sac N4.V48A.00.cus.5.RDS.SAC
/bin/cp B0505TDS.sac N4.V48A.00.cus.5.TDS.SAC
/bin/cp B0506ZSS.sac N4.V48A.00.cus.5.ZSS.SAC
/bin/cp B0507RSS.sac N4.V48A.00.cus.5.RSS.SAC
/bin/cp B0508TSS.sac N4.V48A.00.cus.5.TSS.SAC
/bin/cp B0509ZEX.sac N4.V48A.00.cus.5.ZEX.SAC
/bin/cp B0510REX.sac N4.V48A.00.cus.5.REX.SAC
/bin/cp B0601ZDD.sac N4.V48A.00.cus.6.ZDD.SAC
/bin/cp B0602RDD.sac N4.V48A.00.cus.6.RDD.SAC
/bin/cp B0603ZDS.sac N4.V48A.00.cus.6.ZDS.SAC
/bin/cp B0604RDS.sac N4.V48A.00.cus.6.RDS.SAC
/bin/cp B0605TDS.sac N4.V48A.00.cus.6.TDS.SAC
/bin/cp B0606ZSS.sac N4.V48A.00.cus.6.ZSS.SAC
/bin/cp B0607RSS.sac N4.V48A.00.cus.6.RSS.SAC
/bin/cp B0608TSS.sac N4.V48A.00.cus.6.TSS.SAC
/bin/cp B0609ZEX.sac N4.V48A.00.cus.6.ZEX.SAC
/bin/cp B0610REX.sac N4.V48A.00.cus.6.REX.SAC
/bin/cp B0701ZDD.sac N4.V48A.00.cus.7.ZDD.SAC
/bin/cp B0702RDD.sac N4.V48A.00.cus.7.RDD.SAC
/bin/cp B0703ZDS.sac N4.V48A.00.cus.7.ZDS.SAC
/bin/cp B0704RDS.sac N4.V48A.00.cus.7.RDS.SAC
/bin/cp B0705TDS.sac N4.V48A.00.cus.7.TDS.SAC
/bin/cp B0706ZSS.sac N4.V48A.00.cus.7.ZSS.SAC
/bin/cp B0707RSS.sac N4.V48A.00.cus.7.RSS.SAC
/bin/cp B0708TSS.sac N4.V48A.00.cus.7.TSS.SAC
/bin/cp B0709ZEX.sac N4.V48A.00.cus.7.ZEX.SAC
/bin/cp B0710REX.sac N4.V48A.00.cus.7.REX.SAC
/bin/cp B0801ZDD.sac N4.V48A.00.cus.8.ZDD.SAC
/bin/cp B0802RDD.sac N4.V48A.00.cus.8.RDD.SAC
/bin/cp B0803ZDS.sac N4.V48A.00.cus.8.ZDS.SAC
/bin/cp B0804RDS.sac N4.V48A.00.cus.8.RDS.SAC
/bin/cp B0805TDS.sac N4.V48A.00.cus.8.TDS.SAC
/bin/cp B0806ZSS.sac N4.V48A.00.cus.8.ZSS.SAC
/bin/cp B0807RSS.sac N4.V48A.00.cus.8.RSS.SAC
/bin/cp B0808TSS.sac N4.V48A.00.cus.8.TSS.SAC
/bin/cp B0809ZEX.sac N4.V48A.00.cus.8.ZEX.SAC
/bin/cp B0810REX.sac N4.V48A.00.cus.8.REX.SAC
/bin/cp B0901ZDD.sac N4.V48A.00.cus.9.ZDD.SAC
/bin/cp B0902RDD.sac N4.V48A.00.cus.9.RDD.SAC
/bin/cp B0903ZDS.sac N4.V48A.00.cus.9.ZDS.SAC
/bin/cp B0904RDS.sac N4.V48A.00.cus.9.RDS.SAC
/bin/cp B0905TDS.sac N4.V48A.00.cus.9.TDS.SAC
/bin/cp B0906ZSS.sac N4.V48A.00.cus.9.ZSS.SAC
/bin/cp B0907RSS.sac N4.V48A.00.cus.9.RSS.SAC
/bin/cp B0908TSS.sac N4.V48A.00.cus.9.TSS.SAC
/bin/cp B0909ZEX.sac N4.V48A.00.cus.9.ZEX.SAC
/bin/cp B0910REX.sac N4.V48A.00.cus.9.REX.SAC
/bin/cp B1001ZDD.sac N4.V48A.00.cus.10.ZDD.SAC
/bin/cp B1002RDD.sac N4.V48A.00.cus.10.RDD.SAC
/bin/cp B1003ZDS.sac N4.V48A.00.cus.10.ZDS.SAC
/bin/cp B1004RDS.sac N4.V48A.00.cus.10.RDS.SAC
/bin/cp B1005TDS.sac N4.V48A.00.cus.10.TDS.SAC
/bin/cp B1006ZSS.sac N4.V48A.00.cus.10.ZSS.SAC
/bin/cp B1007RSS.sac N4.V48A.00.cus.10.RSS.SAC
/bin/cp B1008TSS.sac N4.V48A.00.cus.10.TSS.SAC
/bin/cp B1009ZEX.sac N4.V48A.00.cus.10.ZEX.SAC
/bin/cp B1010REX.sac N4.V48A.00.cus.10.REX.SAC
/bin/cp B1101ZDD.sac N4.V48A.00.cus.11.ZDD.SAC
/bin/cp B1102RDD.sac N4.V48A.00.cus.11.RDD.SAC
/bin/cp B1103ZDS.sac N4.V48A.00.cus.11.ZDS.SAC
/bin/cp B1104RDS.sac N4.V48A.00.cus.11.RDS.SAC
/bin/cp B1105TDS.sac N4.V48A.00.cus.11.TDS.SAC
/bin/cp B1106ZSS.sac N4.V48A.00.cus.11.ZSS.SAC
/bin/cp B1107RSS.sac N4.V48A.00.cus.11.RSS.SAC
/bin/cp B1108TSS.sac N4.V48A.00.cus.11.TSS.SAC
/bin/cp B1109ZEX.sac N4.V48A.00.cus.11.ZEX.SAC
/bin/cp B1110REX.sac N4.V48A.00.cus.11.REX.SAC
/bin/cp B1201ZDD.sac N4.V48A.00.cus.12.ZDD.SAC
/bin/cp B1202RDD.sac N4.V48A.00.cus.12.RDD.SAC
/bin/cp B1203ZDS.sac N4.V48A.00.cus.12.ZDS.SAC
/bin/cp B1204RDS.sac N4.V48A.00.cus.12.RDS.SAC
/bin/cp B1205TDS.sac N4.V48A.00.cus.12.TDS.SAC
/bin/cp B1206ZSS.sac N4.V48A.00.cus.12.ZSS.SAC
/bin/cp B1207RSS.sac N4.V48A.00.cus.12.RSS.SAC
/bin/cp B1208TSS.sac N4.V48A.00.cus.12.TSS.SAC
/bin/cp B1209ZEX.sac N4.V48A.00.cus.12.ZEX.SAC
/bin/cp B1210REX.sac N4.V48A.00.cus.12.REX.SAC
/bin/cp B1301ZDD.sac N4.V48A.00.cus.13.ZDD.SAC
/bin/cp B1302RDD.sac N4.V48A.00.cus.13.RDD.SAC
/bin/cp B1303ZDS.sac N4.V48A.00.cus.13.ZDS.SAC
/bin/cp B1304RDS.sac N4.V48A.00.cus.13.RDS.SAC
/bin/cp B1305TDS.sac N4.V48A.00.cus.13.TDS.SAC
/bin/cp B1306ZSS.sac N4.V48A.00.cus.13.ZSS.SAC
/bin/cp B1307RSS.sac N4.V48A.00.cus.13.RSS.SAC
/bin/cp B1308TSS.sac N4.V48A.00.cus.13.TSS.SAC
/bin/cp B1309ZEX.sac N4.V48A.00.cus.13.ZEX.SAC
/bin/cp B1310REX.sac N4.V48A.00.cus.13.REX.SAC
/bin/cp B1401ZDD.sac N4.V48A.00.cus.14.ZDD.SAC
/bin/cp B1402RDD.sac N4.V48A.00.cus.14.RDD.SAC
/bin/cp B1403ZDS.sac N4.V48A.00.cus.14.ZDS.SAC
/bin/cp B1404RDS.sac N4.V48A.00.cus.14.RDS.SAC
/bin/cp B1405TDS.sac N4.V48A.00.cus.14.TDS.SAC
/bin/cp B1406ZSS.sac N4.V48A.00.cus.14.ZSS.SAC
/bin/cp B1407RSS.sac N4.V48A.00.cus.14.RSS.SAC
/bin/cp B1408TSS.sac N4.V48A.00.cus.14.TSS.SAC
/bin/cp B1409ZEX.sac N4.V48A.00.cus.14.ZEX.SAC
/bin/cp B1410REX.sac N4.V48A.00.cus.14.REX.SAC
/bin/cp B1501ZDD.sac N4.V48A.00.cus.15.ZDD.SAC
/bin/cp B1502RDD.sac N4.V48A.00.cus.15.RDD.SAC
/bin/cp B1503ZDS.sac N4.V48A.00.cus.15.ZDS.SAC
/bin/cp B1504RDS.sac N4.V48A.00.cus.15.RDS.SAC
/bin/cp B1505TDS.sac N4.V48A.00.cus.15.TDS.SAC
/bin/cp B1506ZSS.sac N4.V48A.00.cus.15.ZSS.SAC
/bin/cp B1507RSS.sac N4.V48A.00.cus.15.RSS.SAC
/bin/cp B1508TSS.sac N4.V48A.00.cus.15.TSS.SAC
/bin/cp B1509ZEX.sac N4.V48A.00.cus.15.ZEX.SAC
/bin/cp B1510REX.sac N4.V48A.00.cus.15.REX.SAC
/bin/cp B1601ZDD.sac N4.V48A.00.cus.16.ZDD.SAC
/bin/cp B1602RDD.sac N4.V48A.00.cus.16.RDD.SAC
/bin/cp B1603ZDS.sac N4.V48A.00.cus.16.ZDS.SAC
/bin/cp B1604RDS.sac N4.V48A.00.cus.16.RDS.SAC
/bin/cp B1605TDS.sac N4.V48A.00.cus.16.TDS.SAC
/bin/cp B1606ZSS.sac N4.V48A.00.cus.16.ZSS.SAC
/bin/cp B1607RSS.sac N4.V48A.00.cus.16.RSS.SAC
/bin/cp B1608TSS.sac N4.V48A.00.cus.16.TSS.SAC
/bin/cp B1609ZEX.sac N4.V48A.00.cus.16.ZEX.SAC
/bin/cp B1610REX.sac N4.V48A.00.cus.16.REX.SAC
/bin/cp B1701ZDD.sac N4.V48A.00.cus.17.ZDD.SAC
/bin/cp B1702RDD.sac N4.V48A.00.cus.17.RDD.SAC
/bin/cp B1703ZDS.sac N4.V48A.00.cus.17.ZDS.SAC
/bin/cp B1704RDS.sac N4.V48A.00.cus.17.RDS.SAC
/bin/cp B1705TDS.sac N4.V48A.00.cus.17.TDS.SAC
/bin/cp B1706ZSS.sac N4.V48A.00.cus.17.ZSS.SAC
/bin/cp B1707RSS.sac N4.V48A.00.cus.17.RSS.SAC
/bin/cp B1708TSS.sac N4.V48A.00.cus.17.TSS.SAC
/bin/cp B1709ZEX.sac N4.V48A.00.cus.17.ZEX.SAC
/bin/cp B1710REX.sac N4.V48A.00.cus.17.REX.SAC
/bin/cp B1801ZDD.sac N4.V48A.00.cus.18.ZDD.SAC
/bin/cp B1802RDD.sac N4.V48A.00.cus.18.RDD.SAC
/bin/cp B1803ZDS.sac N4.V48A.00.cus.18.ZDS.SAC
/bin/cp B1804RDS.sac N4.V48A.00.cus.18.RDS.SAC
/bin/cp B1805TDS.sac N4.V48A.00.cus.18.TDS.SAC
/bin/cp B1806ZSS.sac N4.V48A.00.cus.18.ZSS.SAC
/bin/cp B1807RSS.sac N4.V48A.00.cus.18.RSS.SAC
/bin/cp B1808TSS.sac N4.V48A.00.cus.18.TSS.SAC
/bin/cp B1809ZEX.sac N4.V48A.00.cus.18.ZEX.SAC
/bin/cp B1810REX.sac N4.V48A.00.cus.18.REX.SAC
/bin/cp B1901ZDD.sac N4.V48A.00.cus.19.ZDD.SAC
/bin/cp B1902RDD.sac N4.V48A.00.cus.19.RDD.SAC
/bin/cp B1903ZDS.sac N4.V48A.00.cus.19.ZDS.SAC
/bin/cp B1904RDS.sac N4.V48A.00.cus.19.RDS.SAC
/bin/cp B1905TDS.sac N4.V48A.00.cus.19.TDS.SAC
/bin/cp B1906ZSS.sac N4.V48A.00.cus.19.ZSS.SAC
/bin/cp B1907RSS.sac N4.V48A.00.cus.19.RSS.SAC
/bin/cp B1908TSS.sac N4.V48A.00.cus.19.TSS.SAC
/bin/cp B1909ZEX.sac N4.V48A.00.cus.19.ZEX.SAC
/bin/cp B1910REX.sac N4.V48A.00.cus.19.REX.SAC
/bin/cp B2001ZDD.sac N4.V48A.00.cus.20.ZDD.SAC
/bin/cp B2002RDD.sac N4.V48A.00.cus.20.RDD.SAC
/bin/cp B2003ZDS.sac N4.V48A.00.cus.20.ZDS.SAC
/bin/cp B2004RDS.sac N4.V48A.00.cus.20.RDS.SAC
/bin/cp B2005TDS.sac N4.V48A.00.cus.20.TDS.SAC
/bin/cp B2006ZSS.sac N4.V48A.00.cus.20.ZSS.SAC
/bin/cp B2007RSS.sac N4.V48A.00.cus.20.RSS.SAC
/bin/cp B2008TSS.sac N4.V48A.00.cus.20.TSS.SAC
/bin/cp B2009ZEX.sac N4.V48A.00.cus.20.ZEX.SAC
/bin/cp B2010REX.sac N4.V48A.00.cus.20.REX.SAC
/bin/cp B2101ZDD.sac N4.V48A.00.cus.21.ZDD.SAC
/bin/cp B2102RDD.sac N4.V48A.00.cus.21.RDD.SAC
/bin/cp B2103ZDS.sac N4.V48A.00.cus.21.ZDS.SAC
/bin/cp B2104RDS.sac N4.V48A.00.cus.21.RDS.SAC
/bin/cp B2105TDS.sac N4.V48A.00.cus.21.TDS.SAC
/bin/cp B2106ZSS.sac N4.V48A.00.cus.21.ZSS.SAC
/bin/cp B2107RSS.sac N4.V48A.00.cus.21.RSS.SAC
/bin/cp B2108TSS.sac N4.V48A.00.cus.21.TSS.SAC
/bin/cp B2109ZEX.sac N4.V48A.00.cus.21.ZEX.SAC
/bin/cp B2110REX.sac N4.V48A.00.cus.21.REX.SAC
/bin/cp B2201ZDD.sac N4.V48A.00.cus.22.ZDD.SAC
/bin/cp B2202RDD.sac N4.V48A.00.cus.22.RDD.SAC
/bin/cp B2203ZDS.sac N4.V48A.00.cus.22.ZDS.SAC
/bin/cp B2204RDS.sac N4.V48A.00.cus.22.RDS.SAC
/bin/cp B2205TDS.sac N4.V48A.00.cus.22.TDS.SAC
/bin/cp B2206ZSS.sac N4.V48A.00.cus.22.ZSS.SAC
/bin/cp B2207RSS.sac N4.V48A.00.cus.22.RSS.SAC
/bin/cp B2208TSS.sac N4.V48A.00.cus.22.TSS.SAC
/bin/cp B2209ZEX.sac N4.V48A.00.cus.22.ZEX.SAC
/bin/cp B2210REX.sac N4.V48A.00.cus.22.REX.SAC
/bin/cp B2301ZDD.sac N4.V48A.00.cus.23.ZDD.SAC
/bin/cp B2302RDD.sac N4.V48A.00.cus.23.RDD.SAC
/bin/cp B2303ZDS.sac N4.V48A.00.cus.23.ZDS.SAC
/bin/cp B2304RDS.sac N4.V48A.00.cus.23.RDS.SAC
/bin/cp B2305TDS.sac N4.V48A.00.cus.23.TDS.SAC
/bin/cp B2306ZSS.sac N4.V48A.00.cus.23.ZSS.SAC
/bin/cp B2307RSS.sac N4.V48A.00.cus.23.RSS.SAC
/bin/cp B2308TSS.sac N4.V48A.00.cus.23.TSS.SAC
/bin/cp B2309ZEX.sac N4.V48A.00.cus.23.ZEX.SAC
/bin/cp B2310REX.sac N4.V48A.00.cus.23.REX.SAC
/bin/cp B2401ZDD.sac N4.V48A.00.cus.24.ZDD.SAC
/bin/cp B2402RDD.sac N4.V48A.00.cus.24.RDD.SAC
/bin/cp B2403ZDS.sac N4.V48A.00.cus.24.ZDS.SAC
/bin/cp B2404RDS.sac N4.V48A.00.cus.24.RDS.SAC
/bin/cp B2405TDS.sac N4.V48A.00.cus.24.TDS.SAC
/bin/cp B2406ZSS.sac N4.V48A.00.cus.24.ZSS.SAC
/bin/cp B2407RSS.sac N4.V48A.00.cus.24.RSS.SAC
/bin/cp B2408TSS.sac N4.V48A.00.cus.24.TSS.SAC
/bin/cp B2409ZEX.sac N4.V48A.00.cus.24.ZEX.SAC
/bin/cp B2410REX.sac N4.V48A.00.cus.24.REX.SAC
/bin/cp B2501ZDD.sac N4.V48A.00.cus.25.ZDD.SAC
/bin/cp B2502RDD.sac N4.V48A.00.cus.25.RDD.SAC
/bin/cp B2503ZDS.sac N4.V48A.00.cus.25.ZDS.SAC
/bin/cp B2504RDS.sac N4.V48A.00.cus.25.RDS.SAC
/bin/cp B2505TDS.sac N4.V48A.00.cus.25.TDS.SAC
/bin/cp B2506ZSS.sac N4.V48A.00.cus.25.ZSS.SAC
/bin/cp B2507RSS.sac N4.V48A.00.cus.25.RSS.SAC
/bin/cp B2508TSS.sac N4.V48A.00.cus.25.TSS.SAC
/bin/cp B2509ZEX.sac N4.V48A.00.cus.25.ZEX.SAC
/bin/cp B2510REX.sac N4.V48A.00.cus.25.REX.SAC
