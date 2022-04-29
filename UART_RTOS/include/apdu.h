#ifndef APDU_H
#define APDU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

bool apdu_handler(
    uint8_t* apdu,
    uint16_t apdu_len,
    void* value);



#endif
