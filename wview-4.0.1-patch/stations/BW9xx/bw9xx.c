/*---------------------------------------------------------------------------
 
  FILENAME:
        bw9xx.c
 
  PURPOSE:
        Provide the BW9xx interface API and utilities.
 
  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        09/06/2008      R.G. Pike       0               Original
 
  NOTES:
        
 
  LICENSE:
  
        This source code is released for free distribution under the terms 
        of the GNU General Public License.
  
----------------------------------------------------------------------------*/

/*  ... System include files
*/

/*  ... Library include files
*/

/*  ... Local include files
*/
#include <bw9xx.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*  ... global memory declarations
*/

/*  ... local memory
*/
#define PERIOD_FACTOR       4

static SIMULATOR_IF_DATA simWorkData;
static void (*ArchiveIndicator) (ARCHIVE_RECORD *newRecord);
static int DataGenerator;
static void storeLoopPkt (LOOP_PKT *dest);
static void *readerLoop(void *);

static float inTemp;
static float outTemp;
static float barometer;
static short humidity;
static short direction;
static float curwindspeed;
static short maxgust;
static short rain;
static short prev_rain;

float MmToInches(short mm)
{
float ret;
ret = (float) mm * (float) .04;
return ret;
}

float CelsiusToFahrenheit(float c)
{
float ret;

ret = (float) 9.0 / (float) 5.0;
ret = ret * c;
ret = ret + 32.0;

return ret;
}

float MillibarsToInches(float mb)
{
return mb * .02953;
}

short KmhToMph(float kmh)
{
float ret;

ret = (float) kmh / (float) 1.61;

return ret;
}


////////////****////****  S T A T I O N   A P I  ****////****////////////
/////  Must be provided by each supported wview station interface  //////

// station-supplied init function
// -- Can Be Asynchronous - event indication required --
//
// MUST:
//   - set the 'stationGeneratesArchives' flag in WVIEWD_WORK:
//     if the station generates archive records (TRUE) or they should be 
//     generated automatically by the daemon from the sensor readings (FALSE)
//   - Initialize the 'stationData' store for station work area
//   - Initialize the interface medium
//   - do initial LOOP acquisition
//   - do any catch-up on archive records if there is a data logger
//   - 'work->runningFlag' can be used for start up synchronization but should 
//     not be modified by the station interface code
//   - indicate init is done by sending the STATION_INIT_COMPLETE_EVENT event to
//     this process (radProcessEventsSend (NULL, STATION_INIT_COMPLETE_EVENT, 0))
//
// OPTIONAL:
//   - Initialize a state machine or any other construct required for the 
//     station interface - these should be stored in the 'stationData' store
//
// 'archiveIndication' - indication callback used to pass back an archive record
//   generated as a result of 'stationGetArchive' being called; should receive a
//   NULL pointer for 'newRecord' if no record available; only used if 
//   'stationGeneratesArchives' flag is set to TRUE by the station interface
//
// Returns: OK or ERROR
//
int stationInit
(
    WVIEWD_WORK     *work,
    void            (*archiveIndication)(ARCHIVE_RECORD *newRecord)
)
{
	pthread_t t;

    int             minutes;


    memset (&simWorkData, 0, sizeof(simWorkData));

    // save the archive indication callback (we should never need it)
    ArchiveIndicator = archiveIndication;

    // set our work data pointer
    work->stationData = &simWorkData;

    // set the Archive Generation flag to indicate the Simulator DOES NOT 
    // generate them
    work->stationGeneratesArchives = FALSE;

    // Set the rain collector type and size:
    work->rainTicksPerInch = 100;
    work->RainCollectorType = 0x1000;

    // initialize the medium abstraction based on user configuration
    // we just set the fd to -1 so it is not used for select - 
    // we won't really open a serial channel...
    work->medium.fd = -1;

    // grab the station configuration now
    if (stationGetConfigValueInt (work, 
                                  STATION_PARM_ELEVATION, 
                                  &simWorkData.elevation) 
        == ERROR)
    {
        radMsgLog (PRI_HIGH, "stationInit: stationGetConfigValueInt ELEV failed!");
        (*(work->medium.exit)) (&work->medium);
        return ERROR;
    }
    if (stationGetConfigValueFloat (work, 
                                    STATION_PARM_LATITUDE, 
                                    &simWorkData.latitude) 
        == ERROR)
    {
        radMsgLog (PRI_HIGH, "stationInit: stationGetConfigValueInt LAT failed!");
        (*(work->medium.exit)) (&work->medium);
        return ERROR;
    }
    if (stationGetConfigValueFloat (work, 
                                    STATION_PARM_LONGITUDE, 
                                    &simWorkData.longitude) 
        == ERROR)
    {
        radMsgLog (PRI_HIGH, "stationInit: stationGetConfigValueInt LONG failed!");
        (*(work->medium.exit)) (&work->medium);
        return ERROR;
    }
    if (stationGetConfigValueInt (work, 
                                  STATION_PARM_ARC_INTERVAL, 
                                  &simWorkData.archiveInterval) 
        == ERROR)
    {
        radMsgLog (PRI_HIGH, "stationInit: stationGetConfigValueInt ARCINT failed!");
        (*(work->medium.exit)) (&work->medium);
        return ERROR;
    }

    // set the work archive interval now
    work->archiveInterval = simWorkData.archiveInterval;

    // sanity check the archive interval against the most recent record
    if (stationVerifyArchiveInterval (work) == ERROR)
    {
        // bad magic!
        radMsgLog (PRI_HIGH, "stationInit: stationVerifyArchiveInterval failed!");
        radMsgLog (PRI_HIGH, "You must either move old /var/wview/archive files out of the way -or-");
        radMsgLog (PRI_HIGH, "fix the wview.conf setting...");
        return ERROR;
    }
    else
    {
        radMsgLog (PRI_STATUS, "station archive interval: %d minutes",
                   work->archiveInterval);
    }


    // initialize the station interface
    // nothing to do here...

	pthread_create(&t, NULL, readerLoop, NULL);


	/* FIXME: It takes at least 60 seconds to gather a full round of
	 *			data from the BW9xx.  For now, sleep for 65 seconds
	 *			to ensure we have data.  Would be better to
	 *			wait until we read the time/date twice, meaning
	 *			we have a full round of data
	 */
	sleep(65);


    // do the initial GetReadings now
    // populate the LOOP structure
    storeLoopPkt (&work->loopPkt);

    // compute the data generation period
    minutes = 360 * PERIOD_FACTOR;
    minutes *= (work->cdataInterval/1000);
    minutes /= 60;

    radMsgLog (PRI_STATUS,
		"Simulator station opened: %d minute data generation period...",
		minutes);

    // we can indicate successful completion here!
    radProcessEventsSend (NULL, STATION_INIT_COMPLETE_EVENT, 0);

    return OK;
}

// station-supplied exit function
//
// Returns: N/A
//
void stationExit (WVIEWD_WORK *work)
{
    return;
}

// station-supplied function to retrieve positional info (lat, long, elev) -
// should populate 'work' fields: latitude, longitude, elevation
// -- Synchronous --
//
// - If station does not store these parameters, they can be retrieved from the
//   wview.conf file (see daemon.c for example conf file use) - user must choose
//   station type "Generic" when running the wviewconfig script
//
// Returns: OK or ERROR
//
int stationGetPosition (WVIEWD_WORK *work)
{
    // just set the values from our internal store - we retrieved them in
    // stationInit
    work->elevation     = (short)simWorkData.elevation;
    if (simWorkData.latitude >= 0)
        work->latitude      = (short)((simWorkData.latitude*10)+0.5);
    else
        work->latitude      = (short)((simWorkData.latitude*10)-0.5);
    if (simWorkData.longitude >= 0)
        work->longitude     = (short)((simWorkData.longitude*10)+0.5);
    else
        work->longitude     = (short)((simWorkData.longitude*10)-0.5);
    
    radMsgLog (PRI_STATUS, "station location: elevation: %d feet",
               work->elevation);

    radMsgLog (PRI_STATUS, "station location: latitude: %3.1f %c  longitude: %3.1f %c",
               (float)abs(work->latitude)/10.0,
               ((work->latitude < 0) ? 'S' : 'N'),
               (float)abs(work->longitude)/10.0,
               ((work->longitude < 0) ? 'W' : 'E'));

    return OK;
}

// station-supplied function to indicate a time sync should be performed if the
// station maintains time, otherwise may be safely ignored
// -- Can Be Asynchronous --
//
// Returns: OK or ERROR
//
int stationSyncTime (WVIEWD_WORK *work)
{
    // Simulator does not keep time...
    return OK;
}

// station-supplied function to indicate sensor readings should be performed -
// should populate 'work' struct: loopPkt (see datadefs.h for minimum field reqs)
// -- Can Be Asynchronous --
//
// - indicate readings are complete by sending the STATION_LOOP_COMPLETE_EVENT
//   event to this process (radProcessEventsSend (NULL, STATION_LOOP_COMPLETE_EVENT, 0))
//
// Returns: OK or ERROR
//
int stationGetReadings (WVIEWD_WORK *work)
{
    // populate the LOOP structure (with dummy data)
    storeLoopPkt (&work->loopPkt);

    // indicate we are done
    radProcessEventsSend (NULL, STATION_LOOP_COMPLETE_EVENT, 0);
    
    return OK;
}

// station-supplied function to indicate an archive record should be generated -
// MUST populate an ARCHIVE_RECORD struct and indicate it to 'archiveIndication'
// function passed into 'stationInit'
// -- Asynchronous - callback indication required --
//
// Returns: OK or ERROR
//
// Note: 'archiveIndication' should receive a NULL pointer for the newRecord if
//       no record is available
// Note: This function will only be invoked by the wview daemon if the 
//       'stationInit' function set the 'stationGeneratesArchives' to TRUE
//
int stationGetArchive (WVIEWD_WORK *work)
{
    // just indicate a NULL record, Simulator does not generate them (and this 
    // function should never be called!)
    (*ArchiveIndicator) (NULL);
    return OK;
}

// station-supplied function to indicate data is available on the station 
// interface medium (serial or ethernet) -
// It is the responsibility of the station interface to read the data from the 
// medium and process appropriately. The data does not have to be read within
// the context of this function, but may be used to stimulate a state machine.
// -- Synchronous --
//
// Returns: N/A
//
void stationDataIndicate (WVIEWD_WORK *work)
{
    // Simulator station is synchronous...
    return;
}

// station-supplied function to indicate the interface timer has expired -
// It is the responsibility of the station interface to start/stop the interface
// timer as needed for the particular station requirements.
// The station interface timer is specified by the 'ifTimer' member of the
// WVIEWD_WORK structure. No other timers in that structure should be manipulated
// in any way by the station interface code.
// -- Synchronous --
//
// Returns: N/A
//
void stationIFTimerExpiry (WVIEWD_WORK *work)
{
    // Simulator station is synchronous...
    return;
}

////////////****////  S T A T I O N   A P I   E N D  ////****////////////


//  ... ----- static (local) methods ----- ...

static void storeLoopPkt (LOOP_PKT *dest)
{
	dest->barometer = MillibarsToInches(barometer);
 	dest->outTemp = CelsiusToFahrenheit(outTemp);
 	dest->inTemp = CelsiusToFahrenheit(inTemp);
	dest->outHumidity = humidity;
	dest->windDir = direction;
    dest->windSpeed = KmhToMph(curwindspeed);
    dest->windGust = KmhToMph(maxgust);

	radMsgLog (PRI_STATUS, "Current rain: %d, Prev Rain %d", rain, prev_rain);
    dest->sampleRain = MmToInches(rain - prev_rain);
	prev_rain = rain;

    // calculate station pressure by giving a negative elevation 
    dest->stationPressure = wvutilsConvertSPToSLP(dest->barometer, 
		dest->outTemp, (float)(-simWorkData.elevation));

    // calculate altimeter
    dest->altimeter = wvutilsConvertSPToAltimeter(dest->stationPressure,
		(float)simWorkData.elevation);

    // clear the ones we don't support
    dest->inHumidity                    = 0;
    dest->sampleET                      = 0;
    dest->radiation                     = 0;
    dest->UV                            = 0;
    dest->rainRate						= 0;
    dest->windGustDir					= 0;

    // now calculate a few
    dest->dewpoint = wvutilsCalculateDewpoint (dest->outTemp, 
		(float)dest->outHumidity);
    dest->windchill = wvutilsCalculateWindChill (dest->outTemp, 
		(float)dest->windSpeed);
    dest->heatindex = wvutilsCalculateHeatIndex (dest->outTemp, 
		(float)dest->outHumidity);

    // bump our data generator index
    DataGenerator++;
    DataGenerator %= (360*PERIOD_FACTOR);
    
    return;
}

void *readerLoop(void *notused)
{
char path[] = "/tmp/wsd";
char buf[100];
struct sockaddr_un sun;
int fd;
int ret;

notused = notused;

fd = socket(PF_UNIX, SOCK_STREAM, 0);
if (fd == -1)
	{
	perror("socket");
	return NULL;
	}

memset(&sun, 0, sizeof (sun));
sun.sun_family = AF_UNIX;
memcpy(sun.sun_path, path, strlen(path) + 1);

ret = connect(fd, (struct sockaddr *) &sun, sizeof (sun));
if (ret == -1)
	{
	perror("connect");
	return NULL;
	}

memset(buf, 0, sizeof(buf));

/* FIXME: This read and the read in the loop below are the cheap
 *			way out.  Network read should either be double
 *			buffered, or socket changed to a stream and read
 *			until a newline, or read a byte at a time until
 *			a newline is hit.  This gets us by for now, change later.
 */
ret = read(fd, buf, sizeof(buf));
while (ret > 0)
	{
	char *Barometer = "DATA: Current Pressure: ";
	char *Humidity = "DATA: Current Humidity: ";
	char *InsideCurTempText = "DATA: Current Inside Temperature: ";
	char *OutsideCurTempText = "DATA: Current Outside Temperature: ";
	char *WindDirection = "DATA: Wind Direction: ";
	char *CurWindSpeed = "DATA: Current Wind Speed: ";
	char *MaxWindGust = "DATA: Maximum Wind Gust: ";
	char *CurRain = "DATA: Current Rain: ";
	/* radMsgLog (PRI_STATUS, buf); */

	if (strstr(buf, InsideCurTempText))
		{
		char *s = buf + strlen(InsideCurTempText);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			inTemp = atof(s);
			}
		}
	else if (strstr(buf, OutsideCurTempText))
		{
		char *s = buf + strlen(OutsideCurTempText);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			outTemp = atof(s);
			}
		}
	else if (strstr(buf, Barometer))
		{
		char *s = buf + strlen(Barometer);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			barometer = atof(s);
			}
		}
	else if (strstr(buf, Humidity))
		{
		char *s = buf + strlen(Humidity);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			humidity = atoi(s);
			}
		}
	else if (strstr(buf, WindDirection))
		{
		char *s = buf + strlen(WindDirection);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			direction = atoi(s);
			}
		}
	else if (strstr(buf, CurWindSpeed))
		{
		char *s = buf + strlen(CurWindSpeed);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			curwindspeed = atof(s);
			}
		}
	else if (strstr(buf, MaxWindGust))
		{
		char *s = buf + strlen(MaxWindGust);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			maxgust = atoi(s);
			}
		}
	else if (strstr(buf, CurRain))
		{
		char *s = buf + strlen(CurRain);
		if (*s != '-')
			{
			*(strchr(s, ' ')) = '\0';
			rain = atoi(s);
			}
		}

	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, sizeof(buf));
	}

close(fd);

}
