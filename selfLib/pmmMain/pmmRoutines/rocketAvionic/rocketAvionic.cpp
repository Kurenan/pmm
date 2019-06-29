// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#include "pmmHealthSignals/healthSignals.h"

#include "pmmEeprom/eeprom.h"

#include "pmmTelemetry/telemetry.h"
#include "pmmModules/portsReception.h"
#include "pmmSd/sd.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"

// Modules
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"

#include "pmmDebug.h"   // For debug prints



void RoutineRocketAvionic::init()
{
    mGpsIsFirstAltitude = mGpsIsFirstCoord = mGpsIsFirstDate = true;
    mSessionId          = 0; // Later, use the EEPROM.
    mMainLoopCounter    = 0;
    mMillis             = millis();




    mPmmTelemetry.init();
    mPmmSd.init(mSessionId);

    mPmmGps.init();

    mPmmImu.init();

    mPmmModuleDataLog.init(&mPmmTelemetry, &mPmmSd, mSessionId, 0, &mMainLoopCounter, &mMillis);
        mPmmModuleDataLog.getDataLogGroupCore()->addGps(mPmmGps.getGpsStructPtr());
        mPmmModuleDataLog.getDataLogGroupCore()->addImu(mPmmImu.getImuStructPtr());



    mPmmModuleMessageLog.init(&mMainLoopCounter, &mPmmTelemetry, &mPmmSd); // PmmModuleMessageLog
    mPmmPortsReception.init(&mPmmModuleDataLog, &mPmmModuleMessageLog);    // PmmPortsReception

    mMillis = millis(); // Again!
}

void RoutineRocketAvionic::update()
{
    switch(mSubRoutine)
    {
        case AwaitingGps:
            sR_AwaitingGps();   break;
        case Ready:
            sR_Ready();         break;
        case Flying:
            sR_Flying();        break;
        case OrderedDrogue:
            sR_OrderedDrogue(); break;
        case OrderedMain:
            sR_OrderedMain();   break;
        case Landed:
            sR_Landed();        break;
    }
    mMainLoopCounter++;
    mMillis = millis();

    PMM_DEBUG_PRINTF("\n\n");
    delay(500);
}

void RoutineRocketAvionic::sR_AwaitingGps()
{
    mPmmImu.update();
    if (mPmmGps.update() == PmmGps::UpdateRtn::GotFix)
    {
        if (mGpsIsFirstCoord && mPmmGps.getFixPtr()->valid.location) {
            mPmmImu.setDeclination(mPmmGps.getGpsStructPtr()->latitude, mPmmGps.getGpsStructPtr()->longitude);
            mGpsIsFirstCoord = false; }

        // if (mGpsIsFirstAltitude && mPmmGps.getFixPtr()->valid.altitude) {
        //     // calibrate barometer to get real altitude
        //     mGpsIsFirstAltitude = false; }

        // if (mGpsIsFirstDate && mPmmGps.getFixPtr()->valid.date) {
        //     // save the date as message.
        //     mGpsIsFirstDate = false; }
    }
    mPmmModuleDataLog.update();
    mPmmModuleDataLog.debugPrintLogContent(); // There is on the start #if PMM_DEBUG && PMM_DATA_LOG_DEBUG

    if(mPmmTelemetry.updateReception())
        mPmmPortsReception.receivedPacket(mPmmTelemetry.getReceivedPacketAllInfoStructPtr());
    mPmmTelemetry.updateTransmission();
}

void RoutineRocketAvionic::sR_Ready()
{
}

void RoutineRocketAvionic::sR_Flying()
{
}

void RoutineRocketAvionic::sR_OrderedDrogue()
{
}

void RoutineRocketAvionic::sR_OrderedMain()
{
}

void RoutineRocketAvionic::sR_Landed()
{
}
