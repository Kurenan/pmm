// Main pmm code, by Henrique Bruno Fantauzzi de Almeida; Minerva Rockets - UFRJ; Brazil.

#include "pmmConsts.h"

#if PMM_SYSTEM_ROUTINE == PMM_ROUTINE_ROCKET_AVIONIC

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

#include "pmmRoutines/rocketAvionic/recovery/recovery.h"
#include "pmmRoutines/rocketAvionic/rocketAvionic.h"

RoutineRocketAvionic::RoutineRocketAvionic() {}

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
        case SubRoutines::AwaitingGps:
            sR_AwaitingGps();   break;
        case SubRoutines::Ready:
            sR_Ready();         break;
        case SubRoutines::Flying:
            sR_Flying();        break;
        case SubRoutines::OrderedDrogue:
            sR_OrderedDrogue(); break;
        case SubRoutines::OrderedMain:
            sR_OrderedMain();   break;
        case SubRoutines::Landed:
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


// The trigger can be readden as
// [truePercentToTrigger]% of [CheckType] [AreRelationThan] [checkValue] units [perTimeUnit]
// Ex: "90% of the FirstDerivatives AreGreatherThan 10 units/second"
// Be careful that the constructor will alloc (mMinTotalMillis / minMillisPerMeasure) * 8 bytes.
// Be sure that this class suits your needs.
class CustomVariableChecker
{
public:
    typedef struct {
        float    value;
        uint32_t microsBetween;
    } Measure;

    enum class CheckType {Values, FirstDerivatives, SecondDerivatives};
    enum class Relation  {AreLesserThan, AreGreaterThan};
    enum class Time      {DontApply, Second, Millisecond, Microsecond};

    CustomVariableChecker(uint32_t minMicrosBetween, uint32_t maxMicrosBetween, uint32_t mMinTotalMicrosToTrigger,

                          float truePercentToTrigger, CheckType checkType, Relation relation, float checkValue, Time perTimeUnit);

    bool addMeasureAndCheck(float measure);

private:
    Measure *mArray;
    uint32_t lastMicros;

    uint32_t mMinMicrosBetween;
    uint32_t mMaxMicrosBetween;

    uint16_t mStartIndex;
    uint16_t mLength;


};

CustomVariableChecker::CustomVariableChecker(uint32_t minMicrosBetween, uint32_t maxMicrosBetween,
                                             uint32_t mMinTotalMicrosToTrigger, float positivesPercentToTrigger,
                                             CheckType checkType, Relation relation, float checkValue, Time perTimeUnit)
{
    mMinMicrosBetween = minMicrosBetween;
    mMaxMicrosBetween = maxMicrosBetween;

    mArray = (Measure*) malloc(minMicrosBetween * mMinTotalMicrosToTrigger * sizeof(Measure));

    mStartIndex = 0;
    mLength     = 0;


}

bool CustomVariableChecker::addMeasureAndCheck(float measure)
{
    uint32_t nowMicros = micros();

    if (mLength > 0)
    {
        uint32_t microsBetween = nowMicros - mLastMicros;

        // If the time between measures is invalid, ignore the measure.
        if ((microsBetween < mMinMicrosBetween) || (microsBetween > mMaxMicrosBetween))
            return 1;

        mArray[mStartIndex] = {measure, microsBetween};


    }

    mLastMicros = nowMicros;

}

#endif