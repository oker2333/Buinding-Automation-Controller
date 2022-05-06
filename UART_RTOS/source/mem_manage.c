#include "mem_manage.h"

uint64_t bytes_allocated = 0;
Manage_Uinit_t uints[MANAGE_ARRAY] = {0};

void setMemoryUint(uint32_t address, uint32_t bytes)
{
		uint32_t index = 0;
		while(index < MANAGE_ARRAY)
		{
			if(!uints[index].occupied)
			{
				uints[index].address = address;
				uints[index].bytes = bytes;
				break;
			}
			index++;
		}
		bytes_allocated += bytes;
}

void freeMemoryUinit(uint32_t address)
{
	uint32_t index = 0;
    while(index < MANAGE_ARRAY)
	{
		if(address == uints[index].address)
		{
			bytes_allocated -= uints[index].bytes;
			uints[index].occupied = 0;
			break;
		}
		index++;
	}
}




