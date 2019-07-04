#include <Arduino.h> // For malloc()
#include <circularArray.h>
#include <measuresAnalyzer.h>


MeasuresAnalyzer::MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxMicrosBetween, uint32_t microsWindow)
{
    mMinMicrosBetween = minMicrosBetween;
    mMaxMicrosBetween = maxMicrosBetween;
    mMicrosWindow     = microsWindow;

    mCircularArray.realloc(minMicrosBetween * microsWindow + ADDITIONAL_LENGTH); // Create the circular array, as it initialized without size.
}

void MeasuresAnalyzer::addMeasure(float measure)
{
}

bool MeasuresAnalyzer::addMeasureAndCheck(float measure)
{
    uint32_t nowMicros = micros();

    if (mCircularArray->length > 0)
    {
        uint32_t microsBetween = nowMicros - mLastMicros;

        // If the time between measures is invalid, ignore the measure.
        if ((microsBetween < mMinMicrosBetween) || (microsBetween > mMaxMicrosBetween))
            return 1;

        mArray[getCurrentIndex()] = {measure, microsBetween};

        mCurrentTotalMicros += microsBetween;

        while (mCurrentTotalMicros > mMicrosWindow)
        {
            mCurrentTotalMicros -= mArray[mStartIndex].microsDifToNext;

        }
    }

    mLastMicros = nowMicros;
    return false;
}

void MeasuresAnalyzer::calculateChecks()
{
}