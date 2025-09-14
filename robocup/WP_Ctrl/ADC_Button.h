#ifndef _ADC_BUTTON_
#define _ADC_BUTTON_


	


void ADC_Button_Init(void);
void ADC_Button_Read(void);
void ADC_Button_Scan(void);


extern uint8_t Key_Value;
extern button_state   ADC_Button1,ADC_Button2,ADC_Button3,ADC_Button4;
extern uint16_t ADC_PPM_Databuf[10];


extern uint8_t adc_ppm_update_flag;


#endif


