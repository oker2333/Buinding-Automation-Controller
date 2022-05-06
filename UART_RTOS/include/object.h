#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define SUCCEED 1
#define FAIL 0

#define MAX_PDU 64u

#define MAX_RECEIVE_APDU 3u

#define DATA_TRANSMIT_RETRIES 3u
#define DATA_TRANSMIT_TIMEOUT 200u

typedef enum {
    MOTOR,
    RADAR,
    PERPHERAL_NUM
} Object_Identifier;

typedef enum {
    READ_SINGLE,
    READ_SINGLE_ACK,
    WRITE_SINGLE,
    WRITE_SINGLE_ACK,
    READ_MULTIPLE,
    READ_MULTIPLE_ACK,
    WRITE_MULTIPLE,
    WRITE_MULTIPLE_ACK,
    SUBSCRIBE_COV,
    COV_NOTIFY,
    MAX_REQUEST_SERVICE
} Function_Code;

typedef enum {
    ANALOG_INPUT,
    ANALOG_OUTPUT,
    DIGITAL_INPUT,
    DIGITAL_OUTPUT
} Object_Type;

typedef enum {
    UINT8_T,
    UINT16_T,
    UINT32_T,
    UINT64_T,
    INT8_T,
    INT16_T,
    INT32_T,
    INT64_T,
    FLOAT_T,
    DOUBLE_T
} Data_Type;

typedef struct 
{
    Object_Type obj_type;
    Data_Type data_type;
    uint8_t array_num;
}Object_Property_List;

typedef struct {
    Object_Identifier obj_id;
    Object_Type obj_type;
    Data_Type data_type;
    void* value_array;
    uint8_t array_num;
    uint8_t data_size;
}Object_Property;

typedef struct
{
    uint8_t occupied;
    uint8_t *buffer;
    uint16_t buffer_len;
    int32_t sequence;
} Apdu_Data;

void apdu_dispatch(uint8_t FrameType, uint8_t *apdu, uint16_t apdu_len);
void device_peripheral_init(void);

bool ReadSingleValueHandler(uint8_t *apdu_data, uint16_t apdu_data_len);
bool ReadSingleValueAckHandler(void *value, uint8_t *apdu_data, uint16_t apdu_data_len);
bool WriteSingleValueHandler(uint8_t *apdu_data, uint16_t apdu_data_len);
bool WriteSingleValueAckHandler(uint8_t *apdu_data, uint16_t apdu_data_len);

#endif
