#ifndef PMM_UNITS_h
#define PMM_UNITS_h

#include <stdint.h>

// Converts the latitude or longitude in uint32_t type to float type.
inline float coord32ToFloat(uint32_t latOrLon)
{
    return latOrLon * 1.0e-7; // That's how it happens on Location.h, of NeoGps lib.
}


#endif