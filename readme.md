## mtinv

### Introduction

The MTINV toolkit (2002-present) is a collection of computer codes and applications written to invert for the moment tensor of a seismic source given the three components of ground motion recorded at regional seismic stations (e.g., Ichinose et al., 2003). The computer codes and workflow are organized to generate moment tensor solutions for a range of source depths and origin times because of the trade-off between these two quantities. The metric used is the variance reduction and variance reduction modulated by the percent double-couple to determine the best-fit moment-tensor solution. We can solve for a deviatoric moment tensor with a constraint added for no isotropic component although this constraint can be lifted for estimating the full moment tensor like mining collapses or explosion sources. 

### Documentation

A software manual is availble mtinv/docs/mtinv4r.pdf, man pages, and command line usage.

### Tutorial

There are 5 hands-on tutorials. See mtinv/mtinv4_sampledata/run_all.csh

### Community

mtinv is an open source project. Questions, discussion, and contributions are welcome. Contributions can be anything from new packages to bugfixes, documentation, or even new core features.

### Contributing

See CONTRIBUTING.md

### Releases

The latest release is v4.0.2

### Code of Conduct

See CODE_OF_CONDUCT.md

### License

BSD 3-Clause License

Copyright (c) 2018, Lawrence Livermore National Security, LLC
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

LLNL-CODE-2000529
