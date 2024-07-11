
/* Author: Christian Limpach (chris@nice.ch) */

/*
 * /usr/local/lib/cvs/open/open.h,v 1.2 1994/05/09 21:37:39 chris Exp
 *
 * open.h,v
 * Revision 1.2  1994/05/09  21:37:39  chris
 * fixed NXHost which was broken because of [Application new]
 *
 * Revision 1.1  1994/05/05  22:32:02  chris
 * replaces /usr/bin/open and adds some nice features
 *
 */

#import <appkit/appkit.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define str_copy(str)	((str == NULL) ? NULL : NXCopyStringBuffer(str))
#define str_free(str)	{if (str != NULL) free(str);}

#define WORKSPACE "Workspace"
#define STDINBUFSIZE 1024


/* openMode modes... */
#define OPEN 1
#define APP 2
#define PRINT 3
#define OPENLINE 4
#define UNHIDE 5

/* fileMode mode... */
#define DISK 1
#define STDIN 2
#define NONE 3
#define TEMP 4

@interface AppInfo:Object
{
  char *Name;
  char *Host;
  port_t Port;
}

- initFor:(const char *)appName onHost:(const char *)hostName;
- (port_t)port;
- (BOOL)isEntryFor:(const char *)appName:(const char *)hostName;

@end

@interface AppList:List
{
}

- (port_t)getPortFor:(const char *)anApp onHost:(const char *)aHost;

@end
