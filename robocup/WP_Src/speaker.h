#ifndef __SPEAKER_H__
#define __SPEAKER_H__


void ConfigureUART_Speaker(unsigned long bound);
void Speaker_Send(uint8_t *pui8Buffer, uint32_t ui32Count);
void speaker_notify_run(uint16_t *mode);
void speaker_gh(float x,float y,float z,int16_t q0,int16_t q1,int16_t q2,int16_t q3);

extern uint16_t speaker_mode;

#endif


