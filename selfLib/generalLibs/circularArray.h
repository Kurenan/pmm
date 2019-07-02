#ifndef CIRCULAR_ARRAY_h
#define CIRCULAR_ARRAY_h


#include <stdint.h> // for uint16_t

// This won't create the array. You must provide it.
template <class T>
class CircularArray
{
public:

    // You will need to call init.
    CircularArray();

    // If you construct the object with this, there is no need to call init().
    CircularArray(int16_t maxLength);

    // To free the array.
    ~CircularArray();

    // You can rearrange the size of the array with this. For now, it won't work properly. This is only to init after
    // using the constructor without args.
    void init(int16_t maxLength);

    // Resets the mCurrentLength, and returns it (before did it)
    int16_t clear();

    // Returns how many positions are available.
    int16_t available();

    // Returns the current used length.
    int16_t length();

    // Returns the total usable length.
    int16_t maxLength();

    // Adds an item to the end. Returns the index where it entered. Negative if error.
    int16_t push(T item);

    // Adds an item to the end even if the circular array is full, so the previous first item will
    // be removed for this new one. Returns the index where it entered. Negative if error.
    int16_t forcePush(T item);

    // Removes the last item. Returns the index where it was. Negative if error. Returns by the arg the popped item.
    int16_t pop(T *item);

    // Removes the first item. Returns the index where it was. Negative if error. Returns by the arg the shifted item.
    int16_t shift(T *item);


    int16_t getCurrentIndex();

    // Fixes the given index.
    int16_t fixIndex(int16_t index);

private:

    T mArray[];
    int16_t mStartIndex;
    int16_t mCurrentLength;
    int16_t mMaxLength;

};

#endif