/*****************************************************************************

                     DEFINITION TABEL OF COMMON CONSTANTS


MSC Ver.5.1                                      CODED BY A.ANZAI   06/08/1989
*****************************************************************************/

/* FLAG */
#define  SUCCESS       0
#define  ERROR         1
#define  ERROR0        0
#define  ERRORM1       -1
#define  NORMAL        0
#define  TRUE          1
#define  FALSE         0
#define  MODE0         0
#define  MODE1         1
#define  YES           1
#define  NO            0
#define  EQUIVALENT    0
#define  FLAG_ON       '1'
#define  FLAG_OFF      '0'

/* FILE CONTROL */
#define  RDONLY        "r"        /* FILE OPEN MODE FOR 'read' ONLY */
#define  WRONLY        "w"        /* FILE OPEN MODE FOR 'write' ONLY */
#define  APPONLY       "a"        /* FILE OPEN MODE FOR 'append' ONLY */
#define  RDWR          "r+"       /* FILE OPEN MODE FOR 'read & write' */
#define  WRRD          "w+"       /* FILE OPEN MODE FOR 'write & read' */
#define  APPRDWR       "a+"       /* FILE OPEN MODE FOR 'append' */
#define  FCNTL_ERROR   -1         /* RETURN VALUE FOR FILE CONTROL ERROR */
#define  B_RDONLY      "rb"       /* FILE OPEN MODE FOR 'binary+read' ONLY */
#define  B_WRONLY      "wb"       /* FILE OPEN MODE FOR 'binary+write' ONLY */

/* CHARACTER CODE */
#define  TAB           0x09       /* TABULATION */
#define  LF            0x0a       /* LINE FEED */
#define  CR            0x0d       /* CARRIAGE RETURN */
#define  ESC           0x1b       /* ESCAPE CODE */
#define  SPACE         0x20       /* WHITE SPACE */
#define  WQUOTATION    0x22       /* DOUBLE QUOTATION MARK */
#define  EOS           '\0'       /* END OF STRING */

/* FUNCTION MODE */
#define  _TOA_OCTAL    8          /* A BASE FOR CONVERTING A NUMBER TO ASCII */
#define  _TOA_DECIMAL  10         /* A BASE FOR CONVERTING A NUMBER TO ASCII */
#define  _TOA_HEXDEC   16         /* A BASE FOR CONVERTING A NUMBER TO ASCII */

/* SIZE */
#define  FNAME_WOE     9          /* SIZE OF FILE NAME WITHOUT EXTENTION */
#define  FNAME_WE      13         /* SIZE OF FILE NAME WITH EXTENTION */
