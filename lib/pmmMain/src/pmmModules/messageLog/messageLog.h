#ifndef PORT_STRING_h
#define PORT_STRING_h

#include <stdint.h> // for uint32_t

#include "pmmTelemetry/pmmTelemetry.h"
#include "pmmSd/pmmSd.h"

#define PORT_MESSAGE_LOG_INDEX_LSB_CRC_PACKET   0
#define PORT_MESSAGE_LOG_INDEX_MSB_CRC_PACKET   1
#define PORT_MESSAGE_LOG_INDEX_STRING_X         2
#define PORT_MESSAGE_LOG_INDEX_OF_Y_MINUS_1     3
// Total header length is equal to...
#define PORT_STRING_HEADER_LENGTH               4

#define PMM_PACKAGE_MESSAGE_LOG_MAX_STRING_LENGTH (PMM_TLM_MAX_PAYLOAD_LENGTH - PORT_STRING_HEADER_LENGTH)




class PmmModuleMessageLog
{

public:
    int init(uint32_t* packageLogIdPtr, uint32_t* packageLogMillisPtr, PmmTelemetry* pmmTelemetry, PmmSd* pmmSd);
    int addString(char *string); // For now, all added strings will be saved on SD and sent by telemetry.
    int addRawString(char *string); // For now, all added strings will be saved on SD and sent by telemetry.

    void receivedPackageString(receivedPacketAllInfoStructType* packetInfo);

    uint8_t getActualNumberOfStrings();
    uint8_t getPackageInTelemetryFormat(uint8_t* arrayToCopy, uint8_t requestedStringId);

    int loadStringFromSd(char stringDestination[], uint16_t requestedStringId);

private:
    uint32_t* mPackageLogIdPtr;
    uint32_t* mPackageLogMillisPtr; // To share the same millis as the Package Log
    PmmTelemetry* mPmmTelemetry;
    PmmSd* mPmmSd;
    char mString[PMM_PACKAGE_MESSAGE_LOG_MAX_STRING_LENGTH];
    uint8_t mActualNumberOfStrings;


};

#endif
