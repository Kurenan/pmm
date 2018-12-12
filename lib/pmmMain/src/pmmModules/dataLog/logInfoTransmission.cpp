#include <string.h>

#include "crc.h"            // To calculate the CRC
#include "byteSelection.h"  // For unified LSBx functions
#include "pmmModules/dataLog/dataLog.h"
#include "pmmModules/ports.h"



// 02/11/2018, Ok.
// DataLogInfo in Telemetry format, for transmission.
void PmmModuleDataLog::updateLogInfoCombinedPayload()
{
    unsigned variableCounter;
    unsigned stringLength;              // The length withou null char!

    mLogInfoRawPayloadArrayLength = 0;  // Zero the length of the array.


// 1) Add the "Number of variables"
    mDataLogInfoTelemetryRawArray[0] = mNumberVariables;
    mLogInfoRawPayloadArrayLength++;


// 2) Add the "Variable types"

    // 2.1) Adds the variables types in groups of two, as the types are always <= 4 bits, the group makes 8 bits.
    for (variableCounter = 0; variableCounter < mNumberVariables; variableCounter ++)
    {
        if (!variableCounter % 2) // If the counter is even (rest of division by 2 is 0)
            mDataLogInfoTelemetryRawArray[mLogInfoRawPayloadArrayLength] = (mVariableTypeArray[variableCounter] << 4);

        else // Else, the number is odd (rest of division by 2 is 1)
        {
            mDataLogInfoTelemetryRawArray[mLogInfoRawPayloadArrayLength] |= mVariableTypeArray[variableCounter];
            mLogInfoRawPayloadArrayLength++;
        }
    }

    // 2.2) If the the previous loop ended on a odd number (so the last conditional-valid value on the counter was even), increase the length.
    if (variableCounter % 2) 
        mLogInfoRawPayloadArrayLength++;


// 3) Add the Variable strings
    for (variableCounter = 0; variableCounter < mNumberVariables; variableCounter ++)
    {
        // As I couldn't find a way to use strnlen, made it!
        // Again, the stringLength doesn't include the '\0'. The '\0' is manually added in the next lines.
        for (stringLength = 0;
             ((stringLength < (MODULE_DATA_LOG_MAX_STRING_LENGTH - 1)) && mVariableNameArray[variableCounter][stringLength]); // - 1 as the MAX_STRING_LENGTH includes the '\0'.
             stringLength++); 
            
        memcpy(mDataLogInfoTelemetryRawArray + mLogInfoRawPayloadArrayLength, mVariableNameArray[variableCounter], stringLength);
        mLogInfoRawPayloadArrayLength += stringLength;
        mDataLogInfoTelemetryRawArray[mLogInfoRawPayloadArrayLength] = '\0'; // Manually write the null terminating char, in case the string was broken.
        mLogInfoRawPayloadArrayLength ++;
    }

    // Calculate the total number of packets.
    mDataLogInfoPackets = ceil(mLogInfoRawPayloadArrayLength / (float) PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);

    mIsLocked = 1;
}



int PmmModuleDataLog::sendDataLogInfo(uint8_t requestedPacket)
{

    if (requestedPacket >= mDataLogInfoPackets)
        return 2;

    if (!mIsLocked) // So we won't be able to change the variables anymore in this LogData Identifier!
        updateLogInfoCombinedPayload();

    mPacketStruct.payloadLength = 0;

// 1) Adds the DataLogInfo Header to the packet
    // 1.1) The CRC-16 of the packet is added on the end of the function.
    // 1.2) Add the Session Identifier
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_SESSION_ID]   = mSystemSession;
    // 1.3) Add the Packet X
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_PACKET_X]     = requestedPacket;
    // 1.4) Add the Of Y Packets
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_OF_Y_PACKETS] = mDataLogInfoPackets;
    // 1.5) Add the DataLogInfo Identifier

// 2) Adds the DataLogInfo Payload, which was built on updateLogInfoCombinedPayload().
    // 2.1) First, get the number of bytes this payload will have, as the last packet may not occupy all the available length.
    // This packet size is the total raw size minus the (current packet * packetPayloadLength).
    // If it is > maximum payload length, it will be equal to the payload length.
    uint16_t payloadBytesInThisPacket = mLogInfoRawPayloadArrayLength - (requestedPacket * PORT_LOG_INFO_MAX_PAYLOAD_LENGTH);
    if (payloadBytesInThisPacket > PORT_LOG_INFO_MAX_PAYLOAD_LENGTH)
        payloadBytesInThisPacket = PORT_LOG_INFO_MAX_PAYLOAD_LENGTH;

    // 2.2) Add the Payload.
    memcpy(mPacketStruct.payload + mPacketStruct.payloadLength, mDataLogInfoTelemetryRawArray, payloadBytesInThisPacket);
    mPacketStruct.payloadLength += payloadBytesInThisPacket;

// 3) CRC16 of this packet:
    uint16_t crc16ThisPacket = crc16(mPacketStruct.payload + 2, mPacketStruct.payloadLength - 2); // + 2 and - 2 skips the self CRC bytes.

    mPacketStruct.payload[PORT_LOG_INFO_INDEX_CRC_LSB] = LSB0(crc16ThisPacket); // Little endian!
    mPacketStruct.payload[PORT_LOG_INFO_INDEX_CRC_MSB] = LSB1(crc16ThisPacket);

// 5) Add the remaining fields and add it to the queue!
    mPacketStruct.protocol           = PMM_NEO_PROTOCOL_ID;
    mPacketStruct.sourceAddress      = PMM_TELEMETRY_ADDRESS_THIS_SYSTEM;
    mPacketStruct.destinationAddress = PMM_TELEMETRY_ADDRESS_BROADCAST;
    mPacketStruct.port               = PORT_DATA_LOG_ID;
    mPmmTelemetry->addPacketToQueue(&mPacketStruct, PMM_TELEMETRY_QUEUE_PRIORITY_LOW);

    return 0;
}
