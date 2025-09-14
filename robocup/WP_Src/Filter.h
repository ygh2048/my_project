#ifndef _FILTER_H_
#define _FILTER_H_

#define N2 3

typedef struct
{
 float Input_Butter[3];
 float Output_Butter[3];
}Butter_BufferData;


typedef struct
{
 float Output_Butter[3];
}Notch_Filter_BufferData;


typedef struct
{
  float a[3];
  float b[3];
}Butter_Parameter;



//巴特沃斯滤波器参数
typedef struct
{
    int N;          //巴特沃斯滤波器最小实现阶数
    int length;     //滤波器系统函数系数数组的长度
    float fc;       //巴特沃斯滤波器截止频率
    float cosW0;    //中心频率，带通带阻时用到
    float fs;       //采样频率
    int filterType; //需要设计的数字滤波器类型 
	  float pass[2];
	  float stop[2];
	  float rp ;//通带衰减，典型值2dB
	  float rs ;//阻带衰减，典型值20dB
	  float sbvalue[11];
	  float num[11];
	  float den[11];
	  float input[11];
	  float output[11];
	  bool isFOK;
}ButterFilterStruct;


void Tes(void);

extern Butter_Parameter Bandstop_Filter_Parameter_30_98,Bandstop_Filter_Parameter_30_94,Butter_5HZ_Parameter_RC;

extern Butter_Parameter Butter_5HZ_Parameter_Acce,Butter_2HZ_Parameter_Acce,Butterworth_Parameter_Baro,Butterworth_Parameter_Baro_Acc;

extern Butter_BufferData Butterworth_Buffer_Baro,Butterworth_Buffer_Baro_Acc;


float set_lpf_alpha(int16_t cutoff_frequency, float time_step);
void Acce_Control_Filter(void);
float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter);


void Set_Cutoff_Frequency(float sample_frequent, float cutoff_frequent,Butter_Parameter *LPF);
void Butterworth_Parameter_Init(void);
void Test_Filter(void);
float BPF_Butterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter);


void pascalTriangle(int N,int symbol,int *vector);
bool filter(ButterFilterStruct butterValue,float *numerator,float *denominator,float *xVector,int length,float *yVector);
bool IIR_High_Order_Filter(ButterFilterStruct *butterValue,float data);
void IIR_High_Order_Init(void);


extern ButterFilterStruct Gyro_X_Butter_Filter,Gyro_Y_Butter_Filter,Gyro_Z_Butter_Filter;
extern ButterFilterStruct Accel_X_Butter_Filter,Accel_Y_Butter_Filter,Accel_Z_Butter_Filter;
extern ButterFilterStruct Accel_X_INS,Accel_Y_INS,Accel_Z_INS;
extern float Data_X_MAG[N2];
extern float Data_Y_MAG[N2];
extern float Data_Z_MAG[N2];
extern Butter_Parameter Bandstop_Filter_Parameter_30_98;
extern Butter_Parameter Bandstop_Filter_Parameter_30_94;
float GildeAverageValueFilter_MAG(float NewValue,float *Data);
#endif


