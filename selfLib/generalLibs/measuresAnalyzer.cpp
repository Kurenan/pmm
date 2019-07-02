#include <Arduino.h> // For malloc()
#include <circularArray.h>
#include <measuresAnalyzer.h>


MeasuresAnalyzer::MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxMicrosBetween,
                                   uint32_t microsWindow, float positivesPercentToTrigger,
                                   CheckType checkType, Relation relation, float checkValue, Time perTimeUnit)
{
    mMinMicrosBetween = minMicrosBetween;
    mMaxMicrosBetween = maxMicrosBetween;
    mMicrosWindow     = microsWindow;

    mCircularArray    = new CircularArray<Measure>(minMicrosBetween * microsWindow);
}

MeasuresAnalyzer::~MeasuresAnalyzer()
{
    delete mCircularArray;
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



uint16_t MeasuresAnalyzer::getCurrentIndex()
{
    uint16_t currentIndex = mStartIndex + mCurrentLength;
    return getIndex(currentIndex, mTotalLength);
}

// Removes the first measure from the array.
bool MeasuresAnalyzer::shiftMeasure()
{
    if (!mCurrentLength)
        return false;

    mStartIndex = getIndex(mStartIndex + 1);
    mCurrentLength--;
}

// Will return 0xFFFF if 
