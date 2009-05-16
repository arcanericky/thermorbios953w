#ifndef INC_bw9xxh
#define INC_bw9xxh
/*---------------------------------------------------------------------------
 
  FILENAME:
        bw9xx.h
 
  PURPOSE:
        Provide the station simulator interface API and utilities.
 
  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        09/08/2008      R.G. Pike       0               Original
 
  NOTES:
        
 
  LICENSE:
        This source code is released for free distribution under the terms 
        of the GNU General Public License.
  
----------------------------------------------------------------------------*/

/*  ... System include files
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <math.h>

/*  ... Library include files
*/
#include <sysdefs.h>
#include <radtimeUtils.h>
#include <radmsgLog.h>
#include <radsysutils.h>

/*  ... Local include files
*/
#include <datadefs.h>
#include <config.h>
#include <computedData.h>
#include <daemon.h>
#include <station.h>
#include <serial.h>
#include <ethernet.h>


// define simulator interface data here
typedef struct
{
    int             elevation;
    float           latitude;
    float           longitude;
    int             archiveInterval;
    WV_ACCUM_ID     rainRateAccumulator;    // to compute rain rate
} BW9XX_IF_DATA;

#endif

