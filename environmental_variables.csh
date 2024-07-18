#!/bin/csh -x

#### Put these in your .cshrc or .tcshrc or .bashrc
#### of couse change your home directory path

setenv MTINV_VERSION_LABEL v4.0.2
setenv MTINV_PATH          /Users/ichinose1/Work/mtinv.${MTINV_VERSION_LABEL}
setenv MT_DATABASE_FILE    /Users/ichinose1/Work/mtinv.${MTINV_VERSION_LABEL}/data/mt.db
setenv MTINV_GMT_GRID_FILE /Users/ichinose1/Work/topogmt/etopo5.grd
setenv MTINV_GMT_INT_FILE  /Users/ichinose1/Work/topogmt/etopo5.int
setenv MTINV_GMT_CPT_FILE  /Users/ichinose1/Work/topogmt/etopo5.cpt

