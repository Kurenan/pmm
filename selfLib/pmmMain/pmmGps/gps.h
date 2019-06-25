/* pmmGps.h
 *
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_GPS_h
#define PMM_GPS_h

#include <NMEAGPS.h>
#include "pmmGps/gpsType.h"
#include "pmmConsts.h"


// Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,


class PmmGps
{

public:

    PmmGps();

    enum class InitRtn {Ok, Error};
    InitRtn init();

    enum class UpdateRtn {OkNoData, GotFix, NotWorking}; // Warning [FirstCoord]: It wasn't tested that the GPS always gives a valid first position.
    // If you notice it happened, make some sort of filter for the first results, until the update can return the FirstCoord.
    UpdateRtn update();

    // Sleep mode https://forum.arduino.cc/index.php?topic=497410.15

    pmmGpsStructType* getGpsStructPtr();
    pmmGpsStructType  getGpsStruct   ();
    gps_fix*          getFixPtr      ();
    gps_fix           getFix         ();

private:

    unsigned mGpsIsWorking;

    NMEAGPS mNMEAGPS; // This object parses received characters into the gps.fix() data structure

    gps_fix mFix;

    pmmGpsStructType mPmmGpsStruct;

    #if (defined GPS_FIX_SPEED && defined GPS_FIX_ALTITUDE) // https://stackoverflow.com/a/38474505
        unsigned long mLastReadMillis;
        unsigned long mTempLastReadMillis;
        float mLastAltitude;
    #endif

};

#endif
