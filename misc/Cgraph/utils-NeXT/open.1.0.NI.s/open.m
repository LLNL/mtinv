
/* Author: Christian Limpach (chris@nice.ch) */

/*
 * /usr/local/lib/cvs/open/open.m,v 1.2 1994/05/09 21:37:42 chris Exp
 *
 * open.m,v
 * Revision 1.2  1994/05/09  21:37:42  chris
 * fixed NXHost which was broken because of [Application new]
 *
 * Revision 1.1  1994/05/05  22:32:05  chris
 * replaces /usr/bin/open and adds some nice features
 *
 */

#ifdef RCSID
static char rcsid[] = "open.m,v 1.2 1994/05/09 21:37:42 chris Exp";
#endif

#import "open.h"

@implementation AppInfo

- initFor:(const char *)openApp onHost:(const char *)hostName
{
  [super init];
  if((Port=NXPortFromName(openApp, hostName)) != PORT_NULL)
  {
    Name=str_copy(openApp);
    Host=str_copy(hostName);
  }
  return self;
}

- (port_t)port
{
  return Port;
}

- (BOOL)isEntryFor:(const char *)openApp:(const char *)hostName
{
  if(!strcmp(Name, openApp) &&
     (Host==hostName || (Host!=NULL && hostName!=NULL &&
			 !strcmp(Host, hostName))))
    return YES;
  else
    return NO;
}

@end

@implementation AppList

- (port_t)getPortFor:(const char *)anApp onHost:(const char *)aHost
{
  int index;
  
  for(index=0; index<[self count]; index++)
    if([[self objectAt:index] isEntryFor:anApp:aHost])
      break;
  if(index!=[self count])
    return [[self objectAt:index] port];
  else {
    id info;
    
    info=[[AppInfo alloc] initFor:anApp onHost:aHost];
    if ([info port]!=PORT_NULL)
      [self addObject:info];
    return [info port];
  }
}

@end

void main(int argc, char *argv[]) {
  port_t appPort;
  int ok, arg=1;
  char *host=NULL;
  const char *openApp=WORKSPACE, *appName;
  id appSpeaker = [[Speaker alloc] init];
  id appList = [[AppList alloc] init];
  int openMode=OPEN, fileMode=DISK, openedAFile=0;
  int oline=0, waitForFileChange=0, noStat=0;
  char *myArgv[argc];
  int i;

  // get a copy of the arg pointers as [Application new] removes
  // -NXHost options
  for(i=0; i<argc; i++)
    myArgv[i]=argv[i];
  
  [Application new];
  appName=[NXApp appName];
					       
  if(strcmp(appName, "open"))
    if(!strcmp(appName, "appopen") || !strcmp(appName, "run"))
    {
      if(argc<2)
      {
	printf("Usage: %s Application file...\n", appName);
	exit(1);
      }
      openApp=myArgv[arg++];
    } else if(!strcmp(appName, "unhide")) {
      if(argc>1)
	openApp=myArgv[arg++];
      openMode=UNHIDE;
    } else
      openApp=appName;
  
  while(1)
  {
    int argIsFile=0;
    
    if(arg>=argc)
      /* auto detect stdin */
      if(openedAFile)
	break;
      else {
	if((openApp!=WORKSPACE && openMode==OPEN) || openMode==UNHIDE)
	{
	  if(openMode==OPEN)
	    openMode=APP;
	  fileMode=NONE;
	} else
	  fileMode=STDIN;
	argIsFile++;
      }
    else if(myArgv[arg][0]=='-') {
      if(!strcmp(&myArgv[arg][1], "NXHost"))
      {
	if(arg!=(argc-1))
	  host=myArgv[++arg];
	else argIsFile++;
      } else if(!strcmp(&myArgv[arg][1], "wait")) {
	waitForFileChange++;
      } else if(!strcmp(&myArgv[arg][1], "nostat")) {
	noStat++;
      } else if(!strcmp(&myArgv[arg][1], "unhide") ||
		!strcmp(&myArgv[arg][1], "activate") ||
		!strcmp(&myArgv[arg][1], "makeKey")) {
	openMode=UNHIDE;
	fileMode=NONE;
	argIsFile++;
      } else if(!strcmp(&myArgv[arg][1], "temp")) {
	fileMode=TEMP;
      } else if(myArgv[arg][1]=='\000') {
	if(arg==(argc-1) && !openedAFile)
	  fileMode=STDIN;
	argIsFile++;
      } else if(myArgv[arg][2]=='\000') {
	switch(myArgv[arg][1]) {
	case 'a': /* next arg is new application to open with */
	  if(arg!=(argc-1))
	    openApp=myArgv[++arg];
	  else argIsFile++;
	  break;
	case 'o': /* open next arg without checking switches */
	  openMode=OPEN;
	  if(arg!=(argc-1))
	    arg++;
	  argIsFile++;
	  break;
	case 'p': /* go printmode */
	  openMode=PRINT;
	  break;
	default: /* not a switch -> open it */
	  argIsFile++;
	  break;
	}
      } else argIsFile++; /* not a longopt -> open it */
    } else if(myArgv[arg][0]=='+') {
      int i=0;
      
      oline=0;
      while(myArgv[arg][++i]!='\0')
	if(isdigit(myArgv[arg][i]))
	  oline=10*oline+(myArgv[arg][i]-48);
	else {
	  argIsFile++;
	  break;
	}
      if(!argIsFile)
      {
	if(arg!=(argc-1))
	{
	  openMode=OPENLINE;
	  arg++;
	}
	argIsFile++;
      }
    } else argIsFile++; /* not an opt at all -> open it */

    if(argIsFile)
    {
      char fname[MAXPATHLEN+1+(arg>=argc?0:strlen(myArgv[arg]))+1];
      struct stat buf;
      
      switch(fileMode) {
      case STDIN: {
	char buffer[STDINBUFSIZE+1];
	int len;
	FILE *file;
	
	if((len=fread(buffer, 1, STDINBUFSIZE, stdin))>=0)
	{
	  buffer[len]='\0';
	  if(len>=5 && !strncmp(buffer, "{\\rtf", 5))
	    strcpy(fname, "/tmp/openfiletmpXXXXXX.rtf");
	  else if(len>=11 && !strncmp(buffer, "%!PS-Adobe-", 11))
	    if(strstr(buffer, "EPSF-")<strchr(buffer, '\012'))
	      strcpy(fname, "/tmp/openfiletmpXXXXXX.eps");
	    else
	      strcpy(fname, "/tmp/openfiletmpXXXXXX.ps");
	  else
	    strcpy(fname, "/tmp/openfiletmpXXXXXX");
	  
	  NXGetTempFilename(fname, 16);
	  if(!(file=fopen(fname, "wb")))
	  {
	    printf("%s: couldn't create temp file\n", appName);
	    exit(1);
	  }
	  while(len==STDINBUFSIZE)
	  {
	    fwrite(buffer, STDINBUFSIZE, 1, file);
	    len=fread(buffer, 1, STDINBUFSIZE, stdin);
	  }
	  fwrite(buffer, 1, len, file);
	  fclose(file);
	}
	break;
      }
	case NONE:
	  break;
	case DISK:
	case TEMP:
	default:
	  if(myArgv[arg][0]=='/')
	    strcpy(fname, myArgv[arg]);
	  else {
	    getwd(fname);
	    strcat(fname, "/");
	    strcat(fname, myArgv[arg]);
	  }
	  if(fname[strlen(fname)-1]=='/')
	    strcat(fname, ".");
	  break;
	}

      if(!noStat && stat(fname, &buf))
	printf("%s: can't stat file: %s\n", appName, fname);
      else {
	char *connName;

	if((openMode==PRINT || openMode==OPENLINE) &&
	   !strcmp(openApp, WORKSPACE))
	  [[Application workspace] getInfoForFile:fname
	   application:&connName type:&NXPlainFileType];
	else
	  connName=str_copy(openApp);
	if((appPort=[appList getPortFor:connName onHost:host]) !=
	   PORT_NULL)
	{
	  [appSpeaker setSendPort:appPort];
	  switch(openMode) {
	  case UNHIDE:
	    [appSpeaker performRemoteMethod:"unhide"];
	    break;
	  case APP:
	    break;
	    
	  case OPENLINE:
	    [appSpeaker selectorRPC:"openFile:onHost:atTrueLine:"
	     paramTypes:"cci", fname, host?host:"localhost", oline];
	    break;
	  case PRINT:
	    if([appSpeaker msgPrint:fname ok:&ok] || !ok)
	      printf("%s: unable to print file: %s\n", appName, fname);
	    break;
	  case OPEN:
	  default:
	    if(fileMode==STDIN || fileMode==TEMP)
	    {
	      if([appSpeaker openTempFile:fname ok:&ok] || !ok)
		printf("%s: unable to open temp file: %s\n", appName,
		       fname);
	    } else {
	      if([appSpeaker openFile:fname ok:&ok] || !ok)
		printf("%s: unable to open file: %s\n", appName,
		       fname);
	    }
	    break;
	  }
	  if(!noStat && waitForFileChange)
	  {
	    time_t last=buf.st_mtime;
	    
	    while(buf.st_mtime==last && !sleep(1))
	      stat(fname, &buf);
	  }
	} else {
	  printf("%s: can't open connection to %s on %s.\n",
		 appName , openApp, host?host:"local host");
	}
	str_free(connName);
      }
      waitForFileChange=0;
      noStat=0;
      fileMode=DISK;
      openMode=OPEN;
      openedAFile++;
    }
    arg++;
  }
  
  [appSpeaker free];
  exit(0);
}
