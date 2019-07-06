#ifndef CIRCULAR_ARRAY_h
#define CIRCULAR_ARRAY_h


#include <stdint.h> // for uint16_t

template <class T>
class CircularArray
{
public:

    // The array isn't created when using this constructor. Call realloc() after.
    CircularArray();
    // If you construct the object with this, there is no need to call init().
    CircularArray(int maxLength);
    // To free the array.
    ~CircularArray();
    // Resets the array.
    void reset();
    // Changes the array length. Returns true if successful, false otherwise.
    // Reset is called if the realloc is successful, as I won't need rearraging right now.
    bool realloc(int maxLength);


    // Returns the item, relative to the first item.
    // Ex real array is [,,,7,8,9,]. getItem(0) returns 7, getItem(2) returns 9, getItem(3) returns 7 again (it cycles!).
    // getItem(-1) retuns 9, getItem(-3) returns 7.
    T    getItem(int index);

    // Returns how many positions are available.
    int  available();
    // Returns the current used length.
    int  length();
    // Returns the total usable length.
    int  maxLength();

    // Adds an item to the end. Won't add if array is full.
    // Return 0 if pushed successfully. Negative if error.
    int  push(T item);

    // Adds an item to the end even if the circular array is full, in this case, removing the first item in the array (shift()).
    // Returns 0 if pushed without shifting. 1 if shifted. Negative if error.
    int  forcePush(T item);

    // Removes the last item. 
    // Returns 0 if popped successfully. Negative if error.
    int  pop(T *item = NULL);

    // Removes the first item.
    // Returns 0 if shifted successfully, Negative if error.
    int  shift(T *item = NULL);


private:

    int getLastIndex();

    // Fixes the given index.
    int fixIndex(int index);

    T   mArray[] = NULL;
    int mStartIndex;
    int mCurrentLength;
    int mMaxLength = 0;
};

#endif