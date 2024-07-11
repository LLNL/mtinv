#!/bin/csh

driver sacf=test.sac

sac << EOF
read test.sac
rtrend
lh depmin depmax depmen
write append .sac
quit
EOF

cat >! sac.mac << EOF
bd x
qdp off
read rtrend.sac test.sac.sac
color on inc on
p2
pause
EOF

sac sac.mac
