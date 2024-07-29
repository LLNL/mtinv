#!/bin/csh -x

# /***********************************************************************************/
# /*** Copyright 2024 Gene A. Ichinose (LLNL)                                      ***/
# /***                                                                             ***/
# /*** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” ***/
# /*** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   ***/
# /*** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ***/
# /*** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   ***/
# /*** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         ***/
# /*** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        ***/
# /*** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    ***/
# /*** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ***/
# /*** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     ***/
# /*** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      ***/
# /*** THE POSSIBILITY OF SUCH DAMAGE.                                             ***/
# /***                                                                             ***/
# /*** Prepared by LLNL under Contract DE-AC52-07NA27344.                          ***/
# /***********************************************************************************/

### ------------------------------------------------------- ###
### WARNING, this script will drop all tables and seq_id's! ###
### ------------------------------------------------------- ###

### /Users/ichinose1/Work/mtinv/data/dbtools
### setenv MTINV_DATABASE_FILE /Users/ichinose1/Work/mtinv.v4.0.1/data/mt.db

###
### to clean out the database completely or start new
###
/bin/rm -f ../mt.db

###
### drop tables, and create new schema and sequence ids
### if new then soft errors from dropping nonexisting tables
###
sqlite3 ../mt.db < create.sql

###
### reload the georegion table
###
sqlite3 ../mt.db < create_georegion.sql

###
### now work a MT, get a solution and use the run2.csh to 
### insert a new MT solution, here is just an example
###
sqlite3 ../mt.db < sample.sql

###
### run mtinv/bin/updateMTdb to link the georegion, otherwise the next query wont work
###
updateMTdb

###
### use the provided script mtinv/bin/list_MTdb.csh to list the solution
###
list_MTdb.csh
