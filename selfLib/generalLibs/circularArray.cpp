#include <stdint.h>
#include <circularArray.h>
#include <stdlib.h> // For realloc.

template <class T>
CircularArray<T>::CircularArray()
{
    mArray = NULL; // For realloc.
}

template <class T>
CircularArray<T>::CircularArray(int16_t maxLength)
{
    mArray = NULL; // For realloc.
    init(maxLength);
}

template <class T>
CircularArray<T>::~CircularArray()
{
    free(mArray);
}

template <class T>
void CircularArray<T>::init(int16_t maxLength)
{
    mArray = (T*) realloc(maxLength * sizeof(T));
    mMaxLength = maxLength;
    mCurrentLength = 0;
}

// Resets the mCurrentLength, and returns it (before did it)
template <class T>
int16_t CircularArray<T>::clear() { mCurrentLength = 0; }

// Returns how many positions are available.
template <class T>
int16_t CircularArray<T>::available() { return mMaxLength - mCurrentLength; }

// Returns the current used length.
template <class T>
int16_t CircularArray<T>::length() { return mCurrentLength; }

// Returns the total usable length.
template <class T>
int16_t CircularArray<T>::maxLength() { return mMaxLength; }

// Adds an item to the end. Returns the index where it entered. Negative if error.
template <class T>
int16_t CircularArray<T>::push(T item)
{
    if (mMaxLength > 0 && mCurrentLength < mMaxLength)
        return -1;
    
    mArray[getCurrentIndex] = item;
    mCurrentLength++;
}

// Adds an item to the end even if the circular array is full, so the previous first item will
// be removed for this new one. Returns the index where it entered. Negative if error.
template <class T>
int16_t CircularArray<T>::forcePush(T item)
{
    if (mMaxLength > 0 && mCurrentLength < mMaxLength)
        shift();

    push(item);
}

// Removes the last item. Returns the index where it was. Negative if error. Returns by the arg the popped item.
template <class T>
int16_t CircularArray<T>::pop(T *item)
{
    if (!mCurrentLength)
        return -1;
    
    int16_t index = getCurrentIndex();

    *item = mArray[index];

    mCurrentLength--;

    return index;
}

// Removes the first item. Returns the index where it was. Negative if error. Returns by the arg the shifted item.
template <class T>
int16_t CircularArray<T>::shift(T *item)
{
    if (!mCurrentLength)
        return -1;

    int16_t index = mStartIndex;

    *item = mArray[index];

    mStartIndex = fixIndex(mStartIndex++);

    return index;
}

template <class T>
int16_t CircularArray<T>::getCurrentIndex() { return fixIndex(mStartIndex + mCurrentLength) }

template <class T>
int16_t CircularArray<T>::fixIndex(int16_t index)
{
    if (!mTotalLength)
        return -1;

    index = index % mTotalLength;

    if (index < 0)
        return (mTotalLength + index); // As the index is negative, this func will return totalLength - index.

    if (index >= mTotalLength)
        return (mTotalLength - index);
    
    return index;
}