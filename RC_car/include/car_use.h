#ifndef CAR_USE_H
#define CAR_USE_H

#include <Arduino.h>
#include "blinker_camera.h"

#define IN3 4
#define IN4 2
#define IN5 14
#define IN6 15

typedef struct
{
    int speed=0;    //blinker滑块值
    float speed_rate = 0.0;       //速度档位,油门

    float toforward = 0;

    float toright = 0;

    int mode = 0;   //0:rc 1:app
    bool camera_mode = true;

    int joy_x;
    int joy_y;

}CAR_CONTROL;

typedef struct
{
    float speed_rate;
    float left_speed;
    float right_speed;
}SPEED_CONTROL;

class CAR
{
private:
    /* data */
    CAR_CONTROL car_control;
    SPEED_CONTROL speed_control;
    int16_t channels[6]  = {
    1023,1023,180,1023,1023,200};
public:
    CAR(/* args */)=default;
    ~CAR()=default;
    void CAR_init(void);
    void move(int speed,int PIN1,int PIN2);
    SPEED_CONTROL rc_control(void);
    SPEED_CONTROL app_control(void);
    bool car_work_handle(BLINKER_CAMERA bc,int16_t channel[18]);
    void car_error_handle(void);
    void debug(void);
};

#endif
