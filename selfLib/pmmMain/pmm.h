/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include <pmmConsts.h>

#include "pmmHealthSignals/healthSignals.h"


#include "pmmTelemetry/telemetry.h"
#include "pmmImu/imu.h"
#include "pmmGps/gps.h"
#include "pmmSd/sd.h"

// Modules
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/messageLog/messageLog.h"
#include "pmmModules/portsReception.h"



class Pmm
{

public:

    Pmm();

    int  init(bool skipDebugDelay = false);
    void update();

    PmmTelemetry*           getTelemetryPtr();
    PmmImu*                 getPmmImuPtr();
    PmmGps*                 getGpsPtr();
    PmmSd*                  getSdPtr();

    PmmModuleDataLog*       getModuleDataLog();
    PmmModuleMessageLog*    getModuleMessageLog();

private:

    int setSystemMode(pmmSystemState systemMode);

    uint8_t  mSessionId;
    uint32_t mMainLoopCounter, mMillis;

    pmmSystemState mSystemState;

    // Main objects
    PmmTelemetry mPmmTelemetry;
    PmmImu       mPmmImu;
    PmmGps       mPmmGps;
    PmmSd        mPmmSd;

    // Modules
    PmmModuleMessageLog mPmmModuleMessageLog;
    PmmPortsReception   mPmmPortsReception;
    PmmModuleDataLog    mPmmModuleDataLog;

};

#endif
