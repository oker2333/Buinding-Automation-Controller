#include "apdu.h"
#include "object.h"

bool apdu_handler(
    uint8_t* apdu,
    uint16_t apdu_len,
    void* value)
{
    if(!apdu || !apdu_len)
        return false;

    uint32_t ret = 0;

    switch(apdu[1])
    {
        case READ_SINGLE:
            ret = ReadSingleValueHandler(&apdu[0], apdu_len);
        break;

        case READ_SINGLE_ACK:
            ret = ReadSingleValueAckHandler(value, &apdu[0], apdu_len);
        break;

        case WRITE_SINGLE:
            ret = WriteSingleValueHandler(&apdu[0], apdu_len);
        break;

        case WRITE_SINGLE_ACK:
            ret = WriteSingleValueAckHandler(&apdu[0], apdu_len);
        break;
#if 0
        case READ_MULTIPLE: 
        break;

        case READ_MULTIPLE_ACK:
        break;

        case WRITE_MULTIPLE:
        break;

        case WRITE_MULTIPLE_ACK:
        break;

        case SUBSCRIBE_COV:
        break;

        case COV_NOTIFY:
        break;
#endif
    }
    return ret;
}











