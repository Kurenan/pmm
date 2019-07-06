#include <stdint.h>
#include <stdlib.h> // For realloc.
#include <circularArray.h>

template <class T> CircularArray<T>::CircularArray ()              { reset(); }
template <class T> CircularArray<T>::CircularArray (int maxLength) { realloc(maxLength); }
template <class T> CircularArray<T>::~CircularArray()              { free(mArray); }
// Resets the array.
template <class T> void CircularArray<T>::reset()    { mStartIndex = mCurrentLength = 0; }
// Changes the array length. Returns true if successful, false otherwise.
// Reset is called if the realloc is successful, as I won't need rearraging right now.
template <class T> bool CircularArray<T>::realloc(int maxLength)
{
    if (!(T*) realloc(mArray, maxLength * sizeof(T))) return false;

    mMaxLength = maxLength;
    reset();
    return true;
}


// Returns how many positions are available. 0 means is full.
template <class T> int CircularArray<T>::available() { return mMaxLength - mCurrentLength; }
// Returns the current used length.
template <class T> int CircularArray<T>::length()    { return mCurrentLength; }
// Returns the total usable length.
template <class T> int CircularArray<T>::maxLength() { return mMaxLength; }


// Adds an item to the end, if there is available space.
template <class T> int CircularArray<T>::push(T item)
{
    if (!mMaxLength) return -1;
    if (mCurrentLength >= mMaxLength) return -2;
    
    mArray[getLastItemIndex()] = item;
    mCurrentLength++;
    return 0;
}
// Adds an item to the end even if the circular array is full, so the previous first item will
// be removed for this new one.
template <class T> int CircularArray<T>::forcePush(T item)
{
    if (!mMaxLength) return -1;
    if (mMaxLength > 0 && mCurrentLength < mMaxLength)
        shift();

    push(item);
}

// Removes the last item. Returns the index where it was. Negative if error. Returns by the arg the popped item.
template <class T> int CircularArray<T>::pop(T *item)
{
    if (!mCurrentLength) return -1;
    if (item)
        *item = mArray[getLastItemIndex()];

    mCurrentLength--;

    return 0;
}

// Removes the first item. Returns the index where it was. Negative if error. Returns by the arg the shifted item.
template <class T> int CircularArray<T>::shift(T *item)
{
    if (!mCurrentLength) return -1;
    if (item)
        *item = mArray[mStartIndex];

    mStartIndex++
    return 0;
}

// Returns the item, relative to the first item.
// Ex real array is [,,,7,8,9,]. getItem(0) returns 7, getItem(2) returns 9, getItem(3) returns 7 again (it cycles!).
// getItem(-1) retuns 9, getItem(-3) returns 7.
template <class T> T CircularArray<T>::getItem(int index)
{
    return fixIndex(mStartIndex + index)
}


// Returns the index of the last push()ed item.
template <class T> int CircularArray<T>::getLastIndex()
{
    return fixIndex(mStartIndex + mCurrentLength)
}

// Translates negative indexes, and cicles values that are beyond the current array length.
template <class T> int CircularArray<T>::fixIndex(int index)
{
    if (!mTotalLength) return -1;

    index %= mTotalLength;

    if (index < 0) // 
        return (mTotalLength + index);

    if (index >= mTotalLength)
        return (mTotalLength - index);
    
    return index;
}