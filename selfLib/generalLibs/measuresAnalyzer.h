// By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma)
// Code to analyse measures, to know if a percentage of the measurements pass a condition,
// which can be the values itself, the first or second derivative; that are lesser or greater than a given value.
// This was done in order to detect a rocket liftoff or its descending, using barometer data.
// After thinking about it in almost 2 years, and after several methods, I think this is the best I have came up with.

#ifndef MEASURES_ANALYZER_h
#define MEASURES_ANALYZER_h

#include <stdint.h> // For uint32_t.
#include <circularArray.h>

//
// Be careful that the constructor will malloc(mMinTotalMillis / minMillisPerMeasure) * 8 bytes.

// Type of measure is float. I could use a template, but most hobbyists don't know it. C++17 gives the possibility of
//   default template without using <>, but platformIO still uses C++14. And I don't want to use special flags.
class MeasuresAnalyzer
{
public:
    typedef struct {
        float    value;
        uint32_t microsDifToNext;
    } Measure;

    enum class CheckType {Values, FirstDerivative};
    enum class Relation  {AreLesserThan, AreGreaterThan};
    enum class Time      {DontApply, Second, Millisecond, Microsecond};

    // 
    MeasuresAnalyzer(uint32_t minMicrosBetween, uint32_t maxMicrosBetween, uint32_t microsWindow, int numberConditions);
                     
    ~MeasuresAnalyzer();


    int  addMeasure(float measure);

    // The condition can be readden as
    // [minPercent]% of [checkType] [relation] [checkValue] units [perTimeUnit]
    // Ex: "90% of the FirstDerivatives AreGreatherThan 10 units/second"
    // Returns the condition index. If error, negative value is returned.
    int  addCondition(float minPercent, CheckType checkType, Relation relation, float checkValue, Time perTimeUnit);
    bool checkCondition(int conditionIndex);

    // Do both functions above in one.
    bool addMeasureAndCheck(float measure);
    void reset();

private:
    void calculateChecks();
    int16_t getCurrentIndex();

    int      mCanCheckCondition;
    int      mCanFirstDerivative;

    CircularArray<Measure> mCircularArray;

    uint32_t mLastMicros;
    uint32_t mMicrosWindow;
    uint32_t mCurrentTotalMicros;

    uint32_t mMinMicrosBetween;
    uint32_t mMaxMicrosBetween;

    static constexpr uint8_t ADDITIONAL_LENGTH = 2;
};

#endif