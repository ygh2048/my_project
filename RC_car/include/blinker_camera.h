#ifndef BLINKER_CAMERA_H
#define BLINKER_CAMERA_H

#include "stdint.h"

typedef struct
{
    /* data */
    uint8_t joy_x;
    uint8_t joy_y;
    int speed_slinder;
    int mode;
    bool camera_mode;
}BLINKER_CAMERA;


#endif
