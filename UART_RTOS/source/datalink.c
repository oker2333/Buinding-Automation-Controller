#include "datalink.h"

#define FIFO_BUFFER_LEN 1024

static FIFO_BUFFER ring;
uint8_t fifo_buffer[FIFO_BUFFER_LEN];

void Datalink_Init(void)
{
    static struct datalink_port_struct_t datalink_port_s;
    datalink_port = &datalink_port_s;

    if (datalink_port)
    {
        datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
        datalink_port->DataRegister = 0;
        datalink_port->DataCRC = 0;
        datalink_port->DataLength = 0;
        datalink_port->EventCount = 0;
        datalink_port->FrameType = FRAME_TYPE_UNDEFINE;
        datalink_port->FrameCount = 0;
        datalink_port->HeaderCRC = 0;
        datalink_port->Index = 0;
        datalink_port->ReceivedValidFrame = false;

        datalink_port->InputBuffer = NULL;
        datalink_port->InputBufferSize = 0;
        datalink_port->OutputBuffer = fifo_buffer;
        datalink_port->OutputBufferSize = FIFO_BUFFER_LEN;

        datalink_port->FIFOBuffer = &ring;

        FIFO_Init(datalink_port->FIFOBuffer, datalink_port->OutputBuffer, FIFO_BUFFER_LEN);
    }
}
