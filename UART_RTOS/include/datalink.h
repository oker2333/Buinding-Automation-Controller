#ifndef DATALINK_H
#define DATALINK_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "fifo.h"

static struct datalink_port_struct_t* datalink_port = NULL;

/* The initial CRC16 checksum value */
#define CRC16_INITIAL_VALUE 0xFFFF

#define FRAME_TYPE_UNDEFINE 0XFF
#define FRAME_TYPE_TEST_REQUEST 0
#define FRAME_TYPE_TEST_RESPONSE 1
#define FRAME_TYPE_DATA_REQUEST 2
#define FRAME_TYPE_DATA_RESPONSE 3

/* receive FSM states */
typedef enum
{
    DATALINK_RECEIVE_STATE_IDLE = 0,
    DATALINK_RECEIVE_STATE_PREAMBLE = 1,
    DATALINK_RECEIVE_STATE_HEADER = 2,
    DATALINK_RECEIVE_STATE_DATA = 3,
} DATALINK_RECEIVE_STATE;

struct datalink_port_struct_t
{
    DATALINK_RECEIVE_STATE receive_state;
    bool ReceivedValidFrame;
    uint8_t DataRegister;
    uint16_t DataCRC;
    uint16_t DataLength;
    uint8_t EventCount;
    uint8_t FrameType;
    uint8_t FrameCount;
    uint8_t HeaderCRC;
    uint32_t Index;
    uint8_t *InputBuffer;
    uint16_t InputBufferSize;

    uint8_t *OutputBuffer;
    uint16_t OutputBufferSize;

    FIFO_BUFFER *FIFOBuffer;

    /*Platform-specific port data */
    void *UserData;
};

uint16_t Datalink_Create_Frame(
    uint8_t *buffer,     /* where frame is loaded */
    uint16_t buffer_len, /* amount of space available */
    uint8_t frame_type,  /* type of frame to send - see defines */
    uint8_t *data,       /* any data to be sent - may be null */
    uint16_t data_len);

bool Datalink_Create_And_Send_Frame(
    uint8_t frame_type,
    uint8_t *data,
    uint16_t data_len);

void Datalink_Receive_Frame_FSM(
    volatile struct datalink_port_struct_t *datalink_port);

void Datalink_Send_Frame_FSM(
    volatile struct datalink_port_struct_t *datalink_port);

void Datalink_Init(void);

#endif
