#include "object.h"
#include "datalink.h"
#include "debug.h"
#include "os.h"
#include "apdu.h"

static Object_Property peripherals[PERPHERAL_NUM];


static Apdu_Data apdu_data[MAX_RECEIVE_APDU] = {0};

bool parseResponse(uint8_t sequence, uint8_t** result, uint16_t* result_len)
{
    int i = 0;
    while (i < MAX_RECEIVE_APDU)
    {
        if (apdu_data[i].occupied && (apdu_data[i].sequence == sequence))
        {
            *result = apdu_data[i].buffer;
            *result_len = apdu_data[i].buffer_len;
            return true;
        }
        i++;
    }
    return false;
}

void freeResponse(uint8_t sequence)
{
    int i = 0;
    while (i++ < MAX_RECEIVE_APDU)
    {
        if (apdu_data[i].sequence == sequence)
        {
            free(apdu_data[i].buffer);
            apdu_data[i].occupied = 0;
        }
    }
}

void setResponse(uint8_t sequence, uint8_t *result, uint16_t result_len)
{
    int i = 0;
    while (i++ < MAX_RECEIVE_APDU)
    {
        if (!apdu_data[i].occupied)
        {
            apdu_data[i].sequence = sequence;
            apdu_data[i].buffer = result;
            apdu_data[i].buffer_len = result_len;
            apdu_data[i].occupied = 1;
        }
    }
}

void apdu_struct_analysis(uint8_t *apdu, uint16_t apdu_len)
{
    uint8_t sequence = apdu[0];
    setResponse(sequence, &apdu[0], apdu_len);
}

static uint8_t sequence = 0;

uint8_t sequence_allocate(void)
{
		return sequence++;
}

const Object_Property_List properties_set[PERPHERAL_NUM];

void device_peripheral_init(void)
{
    for (int index = 0; index < PERPHERAL_NUM; index++)
    {
        peripherals[index].array_num = properties_set[index].array_num;
        peripherals[index].data_type = properties_set[index].data_type;
        peripherals[index].obj_id = index;
        peripherals[index].obj_type = properties_set[index].obj_type;
    }

    for (int index = 0; index < PERPHERAL_NUM; index++)
    {
        switch (peripherals[index].data_type)
        {
        case UINT8_T:
            peripherals[index].data_size = sizeof(uint8_t);
            peripherals[index].value_array = malloc(sizeof(uint8_t) * peripherals[index].array_num);
            break;

        case UINT16_T:
            peripherals[index].data_size = sizeof(uint16_t);
            peripherals[index].value_array = malloc(sizeof(uint16_t) * peripherals[index].array_num);
            break;

        case UINT32_T:
            peripherals[index].data_size = sizeof(uint32_t);
            peripherals[index].value_array = malloc(sizeof(uint32_t) * peripherals[index].array_num);
            break;

        case UINT64_T:
            peripherals[index].data_size = sizeof(uint64_t);
            peripherals[index].value_array = malloc(sizeof(uint64_t) * peripherals[index].array_num);
            break;

        case INT8_T:
            peripherals[index].data_size = sizeof(int8_t);
            peripherals[index].value_array = malloc(sizeof(int8_t) * peripherals[index].array_num);
            break;

        case INT16_T:
            peripherals[index].data_size = sizeof(int16_t);
            peripherals[index].value_array = malloc(sizeof(int16_t) * peripherals[index].array_num);
            break;

        case INT32_T:
            peripherals[index].data_size = sizeof(int32_t);
            peripherals[index].value_array = malloc(sizeof(int32_t) * peripherals[index].array_num);
            break;

        case INT64_T:
            peripherals[index].data_size = sizeof(int64_t);
            peripherals[index].value_array = malloc(sizeof(int64_t) * peripherals[index].array_num);
            break;

        case FLOAT_T:
            peripherals[index].data_size = sizeof(float);
            peripherals[index].value_array = malloc(sizeof(float) * peripherals[index].array_num);
            break;

        case DOUBLE_T:
            peripherals[index].data_size = sizeof(double);
            peripherals[index].value_array = malloc(sizeof(double) * peripherals[index].array_num);
            break;
        }
    }
}

//超时重传机制
uint32_t wait_timeout_retransmission(Function_Code function_code, uint8_t sequence, uint8_t *buffer, uint16_t buffer_len, uint8_t **result, uint16_t *result_len)
{
		OS_ERR err;
    int i = 0;
    uint32_t ret = 0;

    while(i++ < DATA_TRANSMIT_RETRIES)
    {
        Datalink_Create_And_Send_Frame(function_code, buffer, buffer_len);
        
        OSTimeDlyHMSM(0u, 0u, 0u, DATA_TRANSMIT_TIMEOUT, 0u, &err);  //延时，等待响应帧
				
				ret = parseResponse(sequence, result, result_len);
        if(ret)
				{
					break;
				}

        Log_Print("Wait Ack Timeout and Retry %d Now\n", i);
    }
    return ret;
}

//阻塞式读/写
bool ReadSingleValue(Object_Identifier obj_id, uint8_t array_index, void *value)
{
    if(obj_id >= PERPHERAL_NUM)
    {
        Log_Print("ReadSingleValue obj_id %d is not correct\n",obj_id);
        return false;
    }
    if(array_index >= peripherals[obj_id].array_num)
    {
        Log_Print("ReadSingleValue array_index %d is not correct\n",array_index);
        return false;
    }
    uint32_t ret = 0;
    uint16_t data_len = 0;
    uint8_t sequence = sequence_allocate();
    uint8_t Request_Transmit_Buffer[MAX_PDU];
    Data_Type data_type = peripherals[obj_id].data_type;

    uint8_t *input_buffer = NULL;
    uint16_t input_buffer_len = 0;

    Request_Transmit_Buffer[data_len] = sequence;
    data_len++;
    Request_Transmit_Buffer[data_len] = READ_SINGLE;
    data_len++;
    Request_Transmit_Buffer[data_len] = obj_id;
    data_len++;
    Request_Transmit_Buffer[data_len] = array_index;
    data_len++;

    Log_Print("Read Single Value obj_id = %d,array_index = %d\n",obj_id, array_index);

    ret = wait_timeout_retransmission(READ_SINGLE, sequence, Request_Transmit_Buffer, data_len, &input_buffer, &input_buffer_len);
    if (ret)
        ret = apdu_handler(input_buffer, input_buffer_len, value);

    freeResponse(sequence);
    return ret;
}

bool WriteSingleValue(Object_Identifier obj_id, uint8_t array_index, void *value)
{
    if(obj_id >= PERPHERAL_NUM)
    {
        Log_Print("WriteSingleValue obj_id %d is not correct\n",obj_id);
        return false;
    }
    if(array_index >= peripherals[obj_id].array_num)
    {
        Log_Print("WriteSingleValue array_index %d is not correct\n",array_index);
        return false;
    }

    uint32_t ret = 0;
    uint16_t data_len = 0;
    uint8_t sequence = sequence_allocate();
    uint8_t data_size = peripherals[obj_id].data_size;
    uint8_t Request_Transmit_Buffer[MAX_PDU];

    uint8_t *input_buffer = NULL;
    uint16_t input_buffer_len = 0;

    Request_Transmit_Buffer[data_len] = sequence;
    data_len++;
    Request_Transmit_Buffer[data_len] = WRITE_SINGLE;
    data_len++;
    Request_Transmit_Buffer[data_len] = obj_id;
    data_len++;

    Request_Transmit_Buffer[data_len] = array_index;
    data_len++;
    memcpy(&Request_Transmit_Buffer[data_len], value, data_size);
    data_len += data_size;

    Log_Print("Write Single Value obj_id = %d,array_index = %d\n",obj_id, array_index);

    ret = wait_timeout_retransmission(WRITE_SINGLE, sequence, Request_Transmit_Buffer, data_len, &input_buffer, &input_buffer_len);
    if (ret)
        ret = apdu_handler(input_buffer, input_buffer_len, NULL);

    freeResponse(sequence);
    return ret;
}

//处理服务
bool ReadSingleValueHandler(uint8_t *apdu_data, uint16_t apdu_data_len)
{
    int ret = 0;
    uint8_t sequence = apdu_data[0];
    Function_Code function_code = apdu_data[1];
    Object_Identifier obj_id = apdu_data[2];
    uint8_t index = apdu_data[3];

    uint8_t data_size = peripherals[obj_id].data_size;
    uint8_t *ptr = (uint8_t *)peripherals[obj_id].value_array;

    uint8_t apdu_buffer[MAX_PDU];
    uint16_t len = 0;

    apdu_buffer[len++] = sequence;
    apdu_buffer[len++] = READ_SINGLE_ACK;
    apdu_buffer[len++] = obj_id;
    apdu_buffer[len++] = index;

    memcpy(&apdu_buffer[len], &ptr[index * data_size], data_size);
    len += data_size;

    Datalink_Create_And_Send_Frame(READ_SINGLE_ACK, apdu_buffer, len);
////////////////////////////
    return false;
}

bool WriteSingleValueHandler(uint8_t *apdu_data, uint16_t apdu_data_len)
{
    int ret = 0;
    uint8_t sequence = apdu_data[0];
    Function_Code function_code = apdu_data[1];
    Object_Identifier obj_id = apdu_data[2];
    uint8_t index = apdu_data[3];

    uint8_t data_size = peripherals[obj_id].data_size;
    uint8_t *ptr = (uint8_t *)peripherals[obj_id].value_array;

    uint8_t apdu_buffer[MAX_PDU];
    uint16_t len = 0;

    apdu_buffer[len++] = sequence;
    apdu_buffer[len++] = WRITE_SINGLE_ACK;
    apdu_buffer[len++] = obj_id;
    apdu_buffer[len++] = index;
    apdu_buffer[len++] = SUCCEED;

    Datalink_Create_And_Send_Frame(WRITE_SINGLE_ACK, apdu_buffer, len);

    return false;
}

bool ReadSingleValueAckHandler(void *value, uint8_t *apdu_data, uint16_t apdu_data_len)
{
    Object_Identifier obj_id = apdu_data[2];
    if (obj_id >= PERPHERAL_NUM)
    {
        return false;
    }

    uint8_t data_size = peripherals[obj_id].data_size;
    if (data_size != (apdu_data_len - 4))
    {
        return false;
    }

    Mem_Copy(value, &apdu_data[4], data_size);
    return true;
}

bool WriteSingleValueAckHandler(uint8_t *apdu_data, uint16_t apdu_data_len)
{
    Object_Identifier obj_id = apdu_data[2];
    if (obj_id >= PERPHERAL_NUM)
    {
        return false;
    }

    return apdu_data[4];
}


