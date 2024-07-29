/***********************************************************************************/
/*** Copyright 2024 Gene A. Ichinose (LLNL)                                      ***/
/***                                                                             ***/
/*** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” ***/
/*** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   ***/
/*** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ***/
/*** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   ***/
/*** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         ***/
/*** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        ***/
/*** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    ***/
/*** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     ***/
/*** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     ***/
/*** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF      ***/
/*** THE POSSIBILITY OF SUCH DAMAGE.                                             ***/
/***                                                                             ***/
/*** Prepared by LLNL under Contract DE-AC52-07NA27344.                          ***/
/***********************************************************************************/

MTINV Version 4.0.2
Wed Jul 17 17:56:54 PDT 2024

Updated(see bottom)
Tue Sep 19 00:10:53 PDT 2023
Sun Feb  4 21:08:22 PST 2024
Wed Jul 17 17:56:50 PDT 2024 last MTINV version 4.0.1

Dependencies:
Requires: GCC and GFortran compilers
Optional: sqlite3, GMT version +4.5.x, GMT +5.x.x, GMT -6.3.x

Tested Systems: 
	1. MacOS Ventura intel x86_64  gcc 11.2   20211101
	2. MacOS Ventura Apple-m2      gcc 13.0.1 20230202 (experimental)
	3. Redhat Linux intel x86_64   gcc 4.8.5  20150623 (Red Hat 4.8.5-44)
	4. Linux Mint                  gcc 11.4.0

Instructions: Type make clean; make all

Executables in ./bin

total 7784
-rwxr-xr-x   1 ichinose1  50064   68272 Jul  2 13:27 FlinnEngdahl*
-rwxr--r--   1 ichinose1  50064     444 Jul  2 13:26 clean_exec_in_directory.csh*
-rwxr-xr-x   1 ichinose1  50064  159632 Jul  2 13:27 compare_grn_gmt*
-rwxr-xr-x   1 ichinose1  50064  212128 Jul  2 13:27 glib2inv*
-rwxr-xr-x   1 ichinose1  50064  123392 Jul  2 13:27 grn2Mxy*
-rwxr-xr-x   1 ichinose1  50064   67728 Jul  2 13:27 grn2db*
-rwxr-xr-x   1 ichinose1  50064  159088 Jul  2 13:27 grnlib2sac*
-rwxr-xr-x   1 ichinose1  50064  124944 Jul  2 13:27 hspec96_to_grnlib*
-rwxr--r--   1 ichinose1  50064    1281 Jul  2 13:26 list_MTdb.csh*
-rwxr-xr-x   1 ichinose1  50064  102896 Jul  2 13:27 makepar*
-rwxr-xr-x   1 ichinose1  50064  245792 Jul  2 13:27 mkgrnlib*
-rwxr-xr-x   1 ichinose1  50064  121952 Jul  2 13:27 mtbestfit*
-rwxr-xr-x   1 ichinose1  50064  612368 Jul  2 13:27 mteig*
-rwxr-xr-x   1 ichinose1  50064  592032 Jul  2 13:27 mtinv*
-rwxr--r--@  1 ichinose1  50064    4131 Jul  2 13:26 mtscreen.py*
-rwxr-xr-x   1 ichinose1  50064   86112 Jul  2 13:27 multithread_mkgrnlib*
-rwxr-xr-x   1 ichinose1  50064   86752 Jul  2 13:27 pltmod*
-rwxr--r--   1 ichinose1  50064    4563 Jul  2 13:26 print_MTdb.csh*
-rwxr--r--   1 ichinose1  50064    2396 Jul  2 13:26 remove_MTdb.csh*
-rwxr--r--   1 ichinose1  50064     440 Jul  2 13:26 rename_SACPZs.csh*
-rwxr-xr-x   1 ichinose1  50064   67712 Jul  2 13:27 renamesac*
-rwxr-xr-x   1 ichinose1  50064  106176 Jul  2 13:27 sac2gmtmap*
-rwxr-xr-x   1 ichinose1  50064   87968 Jul  2 13:34 sac2xy*
-rwxr-xr-x   1 ichinose1  50064  263536 Jul  2 13:27 sacdata2inv*
-rwxr-xr-x   1 ichinose1  50064   89008 Jul  2 13:27 sacmerge*
drwxr--r--  14 ichinose1  50064     448 Jul  2 13:26 scripts_original/
-rwxr-xr-x   1 ichinose1  50064  158704 Jul  2 13:27 setupMT*
-rwxr-xr-x   1 ichinose1  50064   67856 Jul  2 13:27 stats*
-rwxr-xr-x   1 ichinose1  50064  141968 Jul  2 13:27 sw4_to_grnlib*
-rwxr--r--   1 ichinose1  50064    2216 Jul  2 13:26 unpack.csh*
-rwxr-xr-x   1 ichinose1  50064   69152 Jul  2 13:27 updateMTdb*
-rwxr-xr-x   1 ichinose1  50064   88320 Jul  2 13:27 whatshere*


Do not forget to set ./mtinv.4.x.x/bin directory to your executable PATH 
shell variables.  See file environmental_variables.csh for additional 
required and optional environmental variables.

Bugfixes
Tue Sep 19 00:12:47 PDT 2023
1. sacdata2inv - memory leak fix with linux mint gcc compilers
2. sacdata2inv - renamed fmul() to scale_data(), wierd problem where linux mint gcc compiler failed to reconize the subroutine
3. added GMT version 4.5.x support to mteig plotting
4. upgraded  GMT version 4.5.x support to mtinv plotting waveform plot
5. various sprintf and strcpy issues when compiling with linux mint gcc compilers

New features:
Sun Feb  4 21:08:44 PST 2024
1. updates to setupMT -hspeec96
2. hspec96_to_grnlib.c

Update:
Tue Jul  2 14:25:50 PDT 2024
Major deletes of obsolete, deprecated code. Cleaned out for review&release
Wed Jul 17 17:57:15 PDT 2024
2 memory allocation errors were fixed that caused random segfaults, added sqlite3 SQL to create new database tables and sequence IDs
These changes lead to minor version 4.0.2 to track debug logistics

geneichinose@yahoo.com
ichinose1@llnl.gov
-- Gene Ichinose
