#include "datalink.h"
#include "object.h"

#define FIFO_BUFFER_LEN 1024

static FIFO_BUFFER ring;
uint8_t fifo_buffer[FIFO_BUFFER_LEN];

struct datalink_port_struct_t* datalink_port = NULL;

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

        datalink_port->InputBuffer = fifo_buffer;
        datalink_port->InputBufferSize = FIFO_BUFFER_LEN;
        datalink_port->OutputBuffer = NULL;
        datalink_port->OutputBufferSize = 0;

        datalink_port->FIFOBuffer = &ring;

        FIFO_Init(datalink_port->FIFOBuffer, datalink_port->InputBuffer, datalink_port->InputBufferSize);
				device_peripheral_init();
    }
}

uint16_t Datalink_Create_Frame(
    uint8_t *buffer,            /* where frame is loaded */
    uint16_t buffer_len,        /* amount of space available */
    uint8_t frame_type,         /* type of frame to send - see defines */
    uint8_t *data,              /* any data to be sent - may be null */
    uint16_t data_len)
{                               /* number of bytes of data (up to 504) */
    uint8_t crc8 = 0xFF;        /* used to calculate the crc value */
    uint16_t crc16 = 0xFFFF;    /* used to calculate the crc value */
    uint16_t index = 0;         /* used to load the data portion of the frame */

    if (buffer_len < 6)
        return 0;

    buffer[0] = 0x55;
    buffer[1] = 0xFF;
    buffer[2] = frame_type;
    crc8 = CRC_Calc_Header(buffer[2], crc8);
    buffer[3] = data_len >> 8; /* MSB first */
    crc8 = CRC_Calc_Header(buffer[3], crc8);
    buffer[4] = data_len & 0xFF;
    crc8 = CRC_Calc_Header(buffer[4], crc8);
    buffer[5] = ~crc8;

    index = 6;
    while (data_len && data && (index < buffer_len))
    {
        buffer[index] = *data;
        crc16 = CRC_Calc_Data(buffer[index], crc16);
        data++;
        index++;
        data_len--;
    }

    if (index > 6)
    {
        if ((index + 2) <= buffer_len)
        {
            crc16 = ~crc16;
            buffer[index] = crc16 & 0xFF; /* LSB first */
            index++;
            buffer[index] = crc16 >> 8;
            index++;
        }
        else
            return 0;
    }

    return index;
}

#define BUFFER_SIZE 100

void Datalink_Create_And_Send_Frame(
    uint8_t frame_type,
    uint8_t *data,
    uint16_t data_len)
{
    uint8_t buffer[BUFFER_SIZE];
    uint16_t len = Datalink_Create_Frame(buffer, BUFFER_SIZE, frame_type, data, data_len);

    DMA1_Stream6_Send(buffer,len);
}

void Datalink_Receive_Frame_FSM(
    volatile struct datalink_port_struct_t *datalink_port)
{
    DATALINK_RECEIVE_STATE receive_state = datalink_port->receive_state;
    switch (datalink_port->receive_state)
    {
        case DATALINK_RECEIVE_STATE_IDLE:
            /* Preamble1 */
            if (datalink_port->DataRegister == 0x55)
            {
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_PREAMBLE;
            }
            break;

        case DATALINK_RECEIVE_STATE_PREAMBLE:
            /* Preamble2 */
            if (datalink_port->DataRegister == 0xFF)
            {
                datalink_port->Index = 0;
                datalink_port->HeaderCRC = 0xFF;
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_HEADER;
            }
            else if (datalink_port->DataRegister == 0x55)
            {
            }
            /* NotPreamble */
            else
            {
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
            }
            break;

        case DATALINK_RECEIVE_STATE_HEADER:
            /* FrameType */
            if (datalink_port->Index == 0)
            {
                datalink_port->HeaderCRC =
                    CRC_Calc_Header(datalink_port->DataRegister,
                                    datalink_port->HeaderCRC);
                datalink_port->FrameType = datalink_port->DataRegister;
                datalink_port->Index = 1;
            }
            /* Length1 */
            else if (datalink_port->Index == 1)
            {
                datalink_port->HeaderCRC =
                    CRC_Calc_Header(datalink_port->DataRegister,
                                    datalink_port->HeaderCRC);
                datalink_port->DataLength = datalink_port->DataRegister * 256;
                datalink_port->Index = 2;
            }
            /* Length2 */
            else if (datalink_port->Index == 2)
            {
                datalink_port->HeaderCRC =
                    CRC_Calc_Header(datalink_port->DataRegister,
                                    datalink_port->HeaderCRC);
                datalink_port->DataLength += datalink_port->DataRegister;
                datalink_port->Index = 3;
            }
            /* HeaderCRC */
            else if (datalink_port->Index == 3)
            {
                datalink_port->HeaderCRC =
                    CRC_Calc_Header(datalink_port->DataRegister,
                                    datalink_port->HeaderCRC);
                if (datalink_port->HeaderCRC != 0x55)
                {
                    datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
                }
                else
                {
                    if (datalink_port->DataLength == 0)
                    {
                        datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
                    }
                    else
                    {
                        datalink_port->InputBuffer = malloc(datalink_port->DataLength);
                        if(!datalink_port->InputBuffer)
                        {
                            datalink_port->InputBufferSize = datalink_port->DataLength;
                            datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;

                            Log_Print("Allocating Memory for Input Buffer Failed\n");
                        }
                        else
                        {
                            datalink_port->receive_state = DATALINK_RECEIVE_STATE_DATA;
                        }

                        datalink_port->Index = 0;
                        datalink_port->DataCRC = 0xFFFF;
                    }
                }
            }
            else
            {
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
            }
            break;

        case DATALINK_RECEIVE_STATE_DATA:
            if (datalink_port->Index < datalink_port->DataLength)
            {
                /* DataOctet */
                datalink_port->DataCRC =
                    CRC_Calc_Data(datalink_port->DataRegister,
                                    datalink_port->DataCRC);
                if (datalink_port->Index < datalink_port->InputBufferSize)
                {
                    datalink_port->InputBuffer[datalink_port->Index] =
                        datalink_port->DataRegister;
                }
                datalink_port->Index++;
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_DATA;
            }
            else if (datalink_port->Index == datalink_port->DataLength)
            {
                /* CRC1 */
                datalink_port->DataCRC =
                    CRC_Calc_Data(datalink_port->DataRegister,
                                    datalink_port->DataCRC);
                datalink_port->Index++;
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_DATA;
            }
            else if (datalink_port->Index == (datalink_port->DataLength + 1))
            {
                /* CRC2 */
                datalink_port->DataCRC =
                    CRC_Calc_Data(datalink_port->DataRegister,
                                    datalink_port->DataCRC);
                if (datalink_port->DataCRC == 0xF0B8)
                {
                    if (datalink_port->receive_state ==
                        DATALINK_RECEIVE_STATE_DATA)
                    {
												apdu_dispatch(datalink_port->FrameType, datalink_port->InputBuffer,datalink_port->DataLength);
                    }
                }
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
            }
            else
            {
                datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
            }
            break;

        default:
            datalink_port->receive_state = DATALINK_RECEIVE_STATE_IDLE;
            break;
    }
}

void datalink_frame_handler(FIFO_BUFFER *b, uint32_t data_len)
{
		Log_Print("datalink receive FSM handle %d bytes",data_len);
		uint32_t i = 0;
		while(i < data_len)
		{
			datalink_port->DataRegister = FIFO_Get(b);
			Datalink_Receive_Frame_FSM(datalink_port);
			i++;
		}
}



