/* pmmGps.cpp
 * Some commentaries were taken from the neoGps lib.
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#include <Arduino.h>
#include <NMEAGPS.h>
#include "pmmConsts.h"
#include "pmmDebug.h"

#include "pmmGps/gps.h"

//------------------------------------------------------------
#if PMM_DEBUG && PMM_DEBUG_MORE && PMM_GPS_DEBUG_MORE
    #include <Streamers.h>
    // example from "trace_all(DEBUG_PORT, mGps, mFix);"
    // Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,
    // 3,2018-04-25 16:32:02.840,-228587517,-432291933,4830,5200,,,5,0,356,
    // 3,2018-04-25 16:32:05.840,-228588867,-432293117,4330,1700,630,3,26,0,1661,
#endif

PmmGps::PmmGps(){}

int PmmGps::init()
{

    PMM_GPS_PORT.begin(9600);
    #if PMM_GPS_GET_SPEEDS
        mTempLastReadMillis = 0;
        mLastReadMillis = 0;
    #endif

    if (PMM_GPS_PORT)
    {
        gpsDebugMorePrintf("Initialized successfully!\n");
        mGpsIsWorking = 1;
    }

    else
    {
        advPrintf("Initialization failed!\n");
        mGpsIsWorking = 0;
    }

    return 0;
}

int PmmGps::update()
{
    if (mGpsIsWorking)
    {
        int hadUpdate = 0;
        while (mGps.available(PMM_GPS_PORT))
        {
            mFix = mGps.read();

            if (!hadUpdate)
                hadUpdate = 1;
        }

        if (hadUpdate)
        {
            #ifdef GPS_FIX_LOCATION
                mPmmGpsStruct.latitude        = mFix.latitude();
                mPmmGpsStruct.longitude       = mFix.longitude();
            #endif

            #ifdef GPS_FIX_ALTITUDE
                mPmmGpsStruct.altitude        = mFix.altitude();
            #endif

            #ifdef GPS_FIX_SATELLITES
                mPmmGpsStruct.satellites      = mFix.satellites;
            #endif

            #ifdef GPS_FIX_HEADING
                mPmmGpsStruct.headingDegree   = mFix.heading();
            #endif

            #ifdef GPS_FIX_SPEED
                mPmmGpsStruct.upSpeed         = mFix.velocity_down    / (-100.0);  // As NeoGps outputs this in cm/s, we divide by 100 to get in m/s.
                mFix.calculateNorthAndEastVelocityFromSpeedAndHeading();
                mPmmGpsStruct.horizontalSpeed = mFix.speed_metersph() / 3600.0  ;  // In m/s
                #ifdef GPS_FIX_HEADING
                    mPmmGpsStruct.northSpeed  = mFix.velocity_north   / 100.0   ;  // As NeoGps outputs this in cm/s, we divide by 100 to get in m/s.
                    mPmmGpsStruct.eastSpeed   = mFix.velocity_east    / 100.0   ;  // As NeoGps outputs this in cm/s, we divide by 100 to get in m/s.
                #endif
            #endif

            #if PMM_DEBUG && PMM_DEBUG_MORE && PMM_GPS_DEBUG_MORE
                gpsDebugMorePrintf("GPS updated. trace_all(): ")
                trace_all(Serial, mGps, mFix);
            #endif
        }

        return 1; // Had update!
    }
    else
        return 0;
}

pmmGpsStructType* PmmGps::getGpsStructPtr()
{
    return &mPmmGpsStruct;
}

pmmGpsStructType  PmmGps::getGpsStruct()
{
    return mPmmGpsStruct;
}

gps_fix*          PmmGps::getFixPtr()
{
    return &mFix;
}