// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma)
// Code to analyse measures, to know if a percentage of the measurements pass a condition,
// which can be the values itself, the first or second derivative; that are lesser or greater than a given value.
// This was done in order to detect a rocket liftoff or its descending, using barometer data.
// After thinking about it in almost 2 years, and after several methods, I think this is the best I have came up with.

#ifndef MEASURES_ANALYZER_h
#define MEASURES_ANALYZER_h

#include <stdint.h> // For uint32_t.
#include <circularArray.h>

// The trigger can be readden as
// [truePercentToTrigger]% of [CheckType] [AreRelationThan] [checkValue] units [perTimeUnit]
// Ex: "90% of the FirstDerivatives AreGreatherThan 10 units/second"
// Be careful that the constructor will alloc (mMinTotalMillis / minMillisPerMeasure) * 8 bytes.
// Be sure that this class suits your needs.
class MeasuresAnalyzer
{
public:
    typedef struct {
        float    value;
        uint32_t microsDifToNext;
    } Measure;

    enum class CheckType {Values, FirstDerivatives, SecondDerivatives};
    enum class Relation  {AreLesserThan, AreGreaterThan};
    enum class Time      {DontApply, Second, Millisecond, Microsecond};

    MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxMicrosBetween, uint32_t microsWindow,
                     float truePercentToTrigger, CheckType checkType, Relation relation, float checkValue, Time perTimeUnit);
    ~MeasuresAnalyzer();

    bool addMeasureAndCheck(float measure);
    void reset();

private:
    int16_t getCurrentIndex();
    bool     shiftMeasure();
    int16_t getIndex(uint16_t index, uint16_t maxLength);

    CircularArray<Measure> *mCircularArray;

    Measure *mArray;
    uint32_t mLastMicros;
    uint32_t mMicrosWindow;
    uint32_t mCurrentTotalMicros;

    uint32_t mMinMicrosBetween;
    uint32_t mMaxMicrosBetween;
};

#endif