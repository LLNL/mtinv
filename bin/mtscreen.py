#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
"""
The screening approach is discussed in:
Ford, S. R., Kraft G. & Ichinose, G. (2020). Seismic Moment Tensor Event
Screening, GJI, doi: .

usage: mtscreen.py [-h] [-mxx MXX] [-mxy MXY] [-mxz MXZ] [-myy MYY] [-myz MYZ] [-mzz MZZ]

Calculate the screening statistic for a given seismic moment tensor. The
moment tensor elements can be absolute or normalized and in any accepted
unit (e.g., N-m, dyne-cm, Ak).

arguments:
  -h        Show this help message and exit
  -mxx MXX  Force couple in x direction along x axis (default: 6.5)
  -mxy MXY  Force couple in x direction along y axis (default: -1.0)
  -mxz MXZ  Force couple in x direction along z axis (default: 0.2)
  -myy MYY  Force couple in y direction along y axis (default: 6.6)
  -myz MYZ  Force couple in y direction along z axis (default: 0.5)
  -mzz MZZ  Force couple in z direction along z axis (default: 7.0)
            
output: angle and screening suggestion (in = explosion; out = not explosion)

version: 0.1.0

examples:

% python3 mtscreen.py -mxx=6.5 -mxy=-1.0 -mxz=0.2 -myy=6.6 -myz=0.5 -mzz=7.0
Screened In (consistent with explosion) [𝛿=9.5° ≤ 30.4°]
%
% ./mtscreen.py -mxx -4.15 -mxy -0.01 -mxz 0.47 -myy -4.21 -myz 0.1 -mzz -5.15
Screened out (inconsistent with explosion) [𝛿=169.0° > 30.4°]
"""

import argparse
from math import sqrt, acos, pi
from numpy import dot
from numpy.linalg import norm

# Constants
A = 30.4 # screening angle (degrees) [𝛿_M from Ford et al. (2020)]

def main():
    parser = argparse.ArgumentParser(
        description=('Calculate the screening statistic for a given '
        'seismic moment tensor. The moment tensor elements can be absolute '
        'or normalized and in any accepted unit (e.g., N-m, dyne-cm, Ak). '
        'For background see Ford et al. (2020)'),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-mxx', type=float, default=6.5,
        help='Force couple in x direction along x axis')
    parser.add_argument('-mxy', type=float, default=-1.0,
        help='Force couple in x direction along y axis')
    parser.add_argument('-mxz', type=float, default=0.2,
        help='Force couple in x direction along z axis')
    parser.add_argument('-myy', type=float, default=6.6,
        help='Force couple in y direction along y axis')
    parser.add_argument('-myz', type=float, default=0.5,
        help='Force couple in y direction along z axis')
    parser.add_argument('-mzz', type=float, default=7.0,
        help='Force couple in z direction along z axis')
    parser.add_argument('-v','--version', action='version', version='0.1.0')
    args = parser.parse_args()
    # To symmetric matrix
    M = m2M(args.mxx,args.mxy,args.mxz,args.myy,args.myz,args.mzz)
    # Normalize symmetric matrix
    Mnorm = M2Mnorm(M)
    # To 5-sphere unit vector
    s = finv(Mnorm)
    # Calculate angular distance from explosion population mean vector
    d = angdistdeg(s)
    
    if d > A:
        print('Screened out (inconsistent with explosion) [𝛿={0:.1f}° > 30.4°]'
        .format(d))
    else:
        print('Screened in (consistent with explosion) [𝛿={0:.1f}° ≤ 30.4°]'
        .format(d))

def m2M(x1,x2,x3,x4,x5,x6):
    """
    From unique elements (m) to symmetric matrix (M)
    """
    M = [[x1, x2, x3], [x2, x4, x5], [x3, x5, x6]]
    return M
    
def M2Mnorm(M):
    """
    From symmetric matrix (M) to normalized symmetric matrix (Mnorm)
    """
    Mnorm = M/norm(M)
    return Mnorm

def finv(Mnorm):
    """
    From normalized symmetric matrix (Mnorm) to 5-sphere unit vector (s)
    Inverse of Eqn. 4 from Ford et al. (2020), GJI
    """
    s = [Mnorm[0,0], Mnorm[1,1], Mnorm[2,2],
         sqrt(2)*Mnorm[1,2], sqrt(2)*Mnorm[0,2], sqrt(2)*Mnorm[0,1]]
    return s

def angdistdeg(s):
    """
    Angular distance (degrees) between unit vector (s) and mean vector (mu)
    Eqn. 5 from Ford et al. (2020), GJI
    """
    mu = [0.45026, 0.523815, 0.71339, 0.0271901, 0.024514, -0.112334]
    d = (180 * acos(dot(s,mu))) / pi
    return d

if __name__ == '__main__':
    main()
