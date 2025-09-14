#ifndef SBUS_H
#define SBUS_H

//channels[2]油门    下184 中1040 上1784 THR上
//channels[3]左手 横 左624 中1016 右1424 RUD下
//channels[0]右手 横 左608 中1000 右1404 AIL上  MIX 下
//channels[1]右手 竖 下536 中939  上1336 ELE下
//channels[4]CH.5   上200 中1000 下1800 
//channels[5]CH.6   上200 下1800

#include <stdio.h>
#include <Arduino.h>

#define SBUS_SIGNAL_OK          0x00
#define SBUS_SIGNAL_LOST        0x01
#define SBUS_SIGNAL_FAILSAFE    0x03

//#define ALL_CHANNELS

#define  SBUS_RX   12
#define  SBUS_TX   13
class SBUS
{
private:
    /* data */
    uint8_t sbus_data[25] = {
  0x00,0x00,0x00,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
    uint8_t  failsafe_status = SBUS_SIGNAL_FAILSAFE;
    int sbus_passthrough = 1;
    uint8_t byte_in_sbus;
    uint8_t bit_in_sbus;
    uint8_t ch;
    uint8_t bit_in_channel;
    uint8_t inBuffer[25];
    uint8_t inData;
    int toChannels = 0;
    uint32_t baud = 100000;

    unsigned int long time_last=0;
    int bufferIndex=0;
    int feedState;

public:
    int16_t channels[18]  = {
    1023,1023,180,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0,0};

public:
    SBUS(/* args */)=default;
    ~SBUS()=default;
    void sbus_init(void);
    void update_channels(void);
    void feedLine(void);
    void read_channel(int16_t *channel_);
    int16_t * read_channel(void);
    bool sbus_handle(void);
    void sbus_debug(void);
};



#endif
