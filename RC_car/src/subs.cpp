#include "sbus.h"


void SBUS::sbus_init(void)
{
    Serial2.begin(100000,SERIAL_8E2,SBUS_RX,SBUS_TX);//初始化串口  
    feedState = 0;

    Serial.println("SBUS init success");
}

/*****************************************************
** 函数名称：feedLine()
** 功能：  通过串口接收解析Futaba(SBUS)数据  
** 参数：  none
** 返回值：none
******************************************************/
void SBUS::feedLine(void)
{
    if( Serial2.available()>24 )
    {
            while(Serial2.available()>0)
            {
                    inData = Serial2.read();
                    switch (feedState)
                    {
                    case 0:
                        if(inData != 0x0f)
                        {
                            while( Serial2.available()>0 ) 
                            {
                                inData = Serial2.read();
                             }
                            return; 
                         }
                         else
                         {
                              bufferIndex = 0;
                              inBuffer[bufferIndex] = inData;
                              inBuffer[24] = 0xff;
                              feedState = 1;
                          }
                          break;
                     case 1:
                         bufferIndex ++;
                         inBuffer[bufferIndex] = inData;
                         if (bufferIndex < 24 && Serial2.available() == 0)
                         {
                              feedState = 0;
                          }
                          if (bufferIndex == 24)
                          {
                              feedState = 0;
                              if (inBuffer[0]==0x0f && inBuffer[24] == 0x00)
                              {
                                  memcpy(sbus_data,inBuffer,25);
                                  toChannels = 1;
                              }
                          }
                         break;
                     }
                    //  if (inData == 0x0f)
                    //  {
                    //      bufferIndex = 0;
                    //      inBuffer[bufferIndex] = inData;
                    //      inBuffer[24] = 0xff;
                    //  }
                    // else
                    // {
                    //       bufferIndex ++;
                    //       inBuffer[bufferIndex] = inData;
                    // }
                    // if(inBuffer[0]==0x0f & inBuffer[24] == 0x00)
                    // {
                    //       memcpy(sbus_data,inBuffer,25);
                    //       toChannels = 1;
                    //       return;
                    // }
            }
     }
}

/*****************************************************
 * 函数名称：update_channels()
 * 功能：  解析Futaba(SBUS)数据，获取通道数据，丢弃其他数据
 * 参数：  none
 * 返回值：none
 * ***************************************************/
void SBUS::update_channels(void)
{
  channels[0]  = ((sbus_data[1]|sbus_data[2]<< 8) & 0x07FF);
  channels[1]  = ((sbus_data[2]>>3|sbus_data[3]<<5) & 0x07FF);
  channels[2]  = ((sbus_data[3]>>6|sbus_data[4]<<2|sbus_data[5]<<10) & 0x07FF);
  channels[3]  = ((sbus_data[5]>>1|sbus_data[6]<<7) & 0x07FF);
  channels[4]  = ((sbus_data[6]>>4|sbus_data[7]<<4) & 0x07FF);
  channels[5]  = ((sbus_data[7]>>7|sbus_data[8]<<1|sbus_data[9]<<9) & 0x07FF);
  channels[6]  = ((sbus_data[9]>>2|sbus_data[10]<<6) & 0x07FF);
  channels[7]  = ((sbus_data[10]>>5|sbus_data[11]<<3) & 0x07FF); // & the other 8 + 2 channels if you need them
  #ifdef ALL_CHANNELS
  channels[8]  = ((sbus_data[12]|sbus_data[13]<< 8) & 0x07FF);
  channels[9]  = ((sbus_data[13]>>3|sbus_data[14]<<5) & 0x07FF);
  channels[10] = ((sbus_data[14]>>6|sbus_data[15]<<2|sbus_data[16]<<10) & 0x07FF);
  channels[11] = ((sbus_data[16]>>1|sbus_data[17]<<7) & 0x07FF);
  channels[12] = ((sbus_data[17]>>4|sbus_data[18]<<4) & 0x07FF);
  channels[13] = ((sbus_data[18]>>7|sbus_data[19]<<1|sbus_data[20]<<9) & 0x07FF);
  channels[14] = ((sbus_data[20]>>2|sbus_data[21]<<6) & 0x07FF);
  channels[15] = ((sbus_data[21]>>5|sbus_data[22]<<3) & 0x07FF);
  #endif
  // Failsafe
  failsafe_status = SBUS_SIGNAL_OK;
  if (sbus_data[23] & (1<<2))
  {
    failsafe_status = SBUS_SIGNAL_LOST;
  }
  if (sbus_data[23] & (1<<3))
  {
    failsafe_status = SBUS_SIGNAL_FAILSAFE;
  }

}

void SBUS::read_channel(int16_t *channel_)
{
    Serial.print(channel_[0]);
    Serial.print(" ");
    Serial.print(channel_[1]);
    Serial.print(" ");
    Serial.print(channel_[2]);
    Serial.print(" ");
    Serial.print(channel_[3]);
    Serial.print(" ");
    Serial.print(channel_[4]);
    Serial.print(" ");
    Serial.print(channel_[5]);
    Serial.print(" ");
    Serial.print(channel_[6]);
    Serial.print(" ");
}


int16_t * SBUS::read_channel(void)
{
    // Serial.print(channels[0]);
    // Serial.print(" ");
    // Serial.print(channels[1]);
    // Serial.print(" ");
    // Serial.print(channels[2]);
    // Serial.print(" ");
    // Serial.print(channels[3]);
    // Serial.print(" ");
    // Serial.print(channels[4]);
    // Serial.print(" ");
    // Serial.print(channels[5]);
    // Serial.print(" ");
    // Serial.print(channels[6]);
    // Serial.print(" ");
    return channels;
}

bool SBUS::sbus_handle(void)
{
    feedLine();
    if(millis()-time_last >= 150)
    {
        update_channels();
        read_channel();
        time_last = millis();
        return true;
    }
    else
        return false;
}

void SBUS::sbus_debug(void)
{
//    Serial.print("channels[0]:");
    Serial.print(channels[0]);
    Serial.print(" ");
//    Serial.print("channels[1]:");
    Serial.print(channels[1]);
    Serial.print(" ");
//    Serial.print("channels[2]:");
    Serial.print(channels[2]);
    Serial.print(" ");
//    Serial.print("channels[3]:");
    Serial.print(channels[3]);
    Serial.print(" ");
//    Serial.print("channels[4]:");
    Serial.print(channels[4]);
    Serial.print(" ");
//    Serial.print("channels[5]:");
    Serial.print(channels[5]);
    Serial.print(" ");
//    Serial.print("channels[6]:");
    Serial.print(channels[6]);
    Serial.print(" ");
    Serial.print("failsafe_status:");
    Serial.print(failsafe_status);
    Serial.print(" ");
    Serial.println();
}