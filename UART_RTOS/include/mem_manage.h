#ifndef MEM_MANAGE_H
#define MEM_MANAGE_H

#include <stdint.h>

typedef struct {
		uint32_t occupied;
		uint32_t address;
		uint32_t bytes;
}Manage_Uinit_t;

#define MANAGE_ARRAY 1024

void setMemoryUint(uint32_t address, uint32_t bytes);
void freeMemoryUinit(uint32_t address);

#endif
