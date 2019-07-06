#include <Arduino.h> // For malloc()
#include <circularArray.h>
#include <generalUnitsOps.h>
#include <measuresAnalyzer.h>


MeasuresAnalyzer::MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxMicrosBetween, uint32_t microsWindow)
{
    mMinMicrosBetween = minMicrosBetween;
    mMaxMicrosBetween = maxMicrosBetween;
    mMicrosWindow     = microsWindow;

    mCircularArray.realloc(minMicrosBetween * microsWindow + ADDITIONAL_LENGTH); // Create the circular array, as it initialized without size.
}

int MeasuresAnalyzer::addMeasure(float measure)
{
    uint32_t nowMicros     = micros();
    uint32_t microsBetween = timeDifference(nowMicros, mLastMicros);

    if (microsBetween < mMinMicrosBetween)
        return 1;

    mCircularArray.forcePush({measure, nowMicros});

    mLastMicros = nowMicros;

    calculateChecks();
    return 0;
}

void MeasuresAnalyzer::calculateChecks()
{
    for (int i = 0; i < mCurrentConditions; i++)
    {
        
    }
}