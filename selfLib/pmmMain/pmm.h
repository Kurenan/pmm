/* pmm.h
 * By Henrique Bruno Fantauzzi de Almeida (aka SrBrahma) - Minerva Rockets, UFRJ, Rio de Janeiro - Brazil */

#ifndef PMM_h
#define PMM_h

#include <pmmConsts.h>


class Pmm
{

public:

    Pmm();

    void init(bool skipDebugDelay = false);
    void update();

};

#endif
