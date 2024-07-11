#!/bin/csh
echo "merging pdf files: $* "
gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sOutputFile=merged.pdf $*
echo "written merged.pdf"
