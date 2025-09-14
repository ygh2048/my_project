#include "car_use.h"


SPEED_CONTROL transform(int x,int y)
{
    float a11 = 0.5;    float a12 = 1;
    float a21 =-0.5;    float a22 = 1;

    float b1 = 0;
    float b2 = 0;

    SPEED_CONTROL sc;

    sc.left_speed = a11 * x + a12 * y + b1;
    sc.right_speed = a21 * x + a22 * y+ b2;

    //sc.left_speed = 0.5 * x + y;
    //sc.right_speed = -0.5 *x + y;

    return sc;

}


void CAR::CAR_init(void)
{
    pinMode(IN3,OUTPUT);
    pinMode(IN4,OUTPUT);
    pinMode(IN5,OUTPUT);
    pinMode(IN6,OUTPUT);

    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW);
    digitalWrite(IN5,LOW);
    digitalWrite(IN6,LOW);
    
    Serial.begin(115200);
    Serial.println("CAR init success");
}


void CAR::move(int speed,int PIN1,int PIN2)
{
    speed = map(speed,-100,100,-255,255);
    delay(10);
    if(speed > 12)
    {
        digitalWrite(PIN2,LOW);
        analogWrite(PIN1,speed);
    }
    else if(speed < -12)
    {
        digitalWrite(PIN1,LOW);
        analogWrite(PIN2,-speed);
    }
    else
    {
        digitalWrite(PIN1,LOW);
        digitalWrite(PIN2,LOW);
    }
}
//channels[2]油门    下184 中1040 上1784 THR上
//channels[3]左手 横 左624 中1016 右1424 RUD下
//channels[0]右手 横 左608 中1000 右1404 AIL上  MIX 下
//channels[1]右手 竖 下536 中939  上1336 ELE下
//channels[4]CH.5   上200 中1000 下1800 
//channels[5]CH.6   上200 下1800
SPEED_CONTROL CAR::rc_control(void)
{
    car_control.speed_rate = map(channels[2],170,1810,0,100);
    car_control.toforward = map(channels[1],20,1410,-100,100);
    car_control.toright = map(channels[0],550,1450,-100,100);
    car_control.camera_mode = channels[5] == 200 ? false : true;
    car_control.mode = channels[4] == 200 ? 0 : 1;

    car_control.toforward = car_control.toforward * car_control.speed_rate *0.01;
    car_control.toright = car_control.toright * car_control.speed_rate *0.01;

    return transform(car_control.toright,car_control.toforward);
}

SPEED_CONTROL CAR::app_control(void)
{
    int rel_x,rel_y;
    rel_x = car_control.joy_x - 128;
    rel_y = car_control.joy_y - 128;

    rel_x = map(rel_x,-128,128,-100,100);
    rel_y = map(rel_y,-128,128,-100,100);

    car_control.toright = rel_x * car_control.speed_rate *0.01;
    car_control.toforward = rel_y * car_control.speed_rate *0.01;

    return transform(car_control.toright,car_control.toforward);

}

bool CAR::car_work_handle(BLINKER_CAMERA bc,int16_t channel[18])
{
    car_control.speed = bc.speed_slinder;
    car_control.joy_x = bc.joy_x;
    car_control.joy_y = bc.joy_y;
    car_control.mode =  bc.mode;

    car_control.camera_mode = bc.camera_mode;

    car_control.speed_rate = car_control.speed/2.55;

    for(int i=0;i<6;i++)
    {
        channels[i] = channel[i];
    }
//赋值完成
    if(car_control.mode == 1){
        speed_control = app_control();

    }
    else if(car_control.mode == 0){
        speed_control = rc_control();
    }
    else
    {
        speed_control.left_speed = 0;
        speed_control.right_speed = 0;
    }

    move(int(speed_control.left_speed),IN3,IN4);
    move(int(speed_control.right_speed),IN5,IN6); 

    return car_control.camera_mode;
}


void CAR::car_error_handle(void)
{
    delay(1000);

}

void CAR::debug()
{
    delay(500);
    Serial.print("speed_rate:");
    Serial.print(car_control.speed_rate);
    Serial.print(" ");
    Serial.print("speed_left:");
    Serial.print(speed_control.left_speed);
    Serial.print(" ");
    Serial.print("speed_right:");
    Serial.print(speed_control.right_speed);
    Serial.println();
}