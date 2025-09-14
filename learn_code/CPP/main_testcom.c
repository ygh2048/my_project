#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// 定义类型和常量
#define INT_LENGTH 3



uint8_t GHLink_Head[2]={0xFF,0xFC};//数据帧头
uint8_t GHLink_End[2] ={0xA1,0xA2};//数据帧尾
// CRC8校验表（多项式0x07）
const uint8_t crc8_table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

/**
 * @description: CRC8校验计算函数
 * @param {uint8_t} *data 数据指针
 * @param {uint16_t} len 数据长度
 * @return {uint8_t} CRC校验值
 */
uint8_t calculate_crc8(uint8_t *data, uint16_t len)
{
    uint8_t crc = 0;
    for(uint16_t i = 0; i < len; i++)
    {
        crc = crc8_table[crc ^ data[i]];
    }
    return crc;
}



int array_data[INT_LENGTH];

// 模拟串口发送函数
void USART1_Send_ArrayU8(uint8_t *data, uint8_t length) {
    printf("发送数据: ");
    for(int i = 0; i < length; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// 类型转换联合体
union {
    uint8_t u8data[4];
    int intdata;
} mytransform_int;

// u8数据转int数据
void u8_to_int(uint8_t *Byte, uint8_t Subscript, int *intValue) {
    mytransform_int.u8data[0] = Byte[Subscript];
    mytransform_int.u8data[1] = Byte[Subscript + 1];
    mytransform_int.u8data[2] = Byte[Subscript + 2];
    mytransform_int.u8data[3] = Byte[Subscript + 3];
    *intValue = mytransform_int.intdata;
}

// int数据转u8数据
void int_to_u8(int *intValue, uint8_t Subscript, uint8_t *Byte) {
    mytransform_int.intdata = *intValue;
    // 检测大小端模式 (PC通常是小端模式)
    uint16_t test = 0x0001;
    if(*(uint8_t *)&test == 0x01) { // 小端模式
        Byte[Subscript]     = mytransform_int.u8data[0];
        Byte[Subscript + 1] = mytransform_int.u8data[1];
        Byte[Subscript + 2] = mytransform_int.u8data[2];
        Byte[Subscript + 3] = mytransform_int.u8data[3];
    } else { // 大端模式
        Byte[Subscript]     = mytransform_int.u8data[3];
        Byte[Subscript + 1] = mytransform_int.u8data[2];
        Byte[Subscript + 2] = mytransform_int.u8data[1];
        Byte[Subscript + 3] = mytransform_int.u8data[0];
    }
}

// 数据打包函数
void gh_link_pack(void)
{
	uint8_t data_to_send[INT_LENGTH*4+5]; // 增加1字节CRC空间
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;
	sum = INT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
        // 调用int_to_u8函数，修复参数顺序错误
        int_to_u8(&array_data[i], cnt, data_to_send); // 参数顺序修正
        cnt += 4;		
	}
	
	// 计算CRC（从帧头后开始到数据结束）
	uint8_t crc = calculate_crc8(&data_to_send[2], cnt-2);
	data_to_send[cnt++] = crc;
	
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}

void gh_link_data(uint8_t *data_buf,uint8_t num)
{
    if(!(data_buf[0]==GHLink_Head[0]&&data_buf[1]==GHLink_Head[1]))         return;//判断帧头
	if(!(data_buf[num-2]==GHLink_End[0]&&data_buf[num-1]==GHLink_End[1])) return;//帧尾校验

	// 检查数据长度是否正确（帧头2 + 数据FLOAT_LENGTH*4 + CRC1 + 帧尾2）
	if(num != INT_LENGTH*4 + 5)
	{
		return;//长度错误
	}
	
	// 验证CRC（从帧头后开始到CRC前）
	uint8_t received_crc = data_buf[2 + INT_LENGTH*4]; // CRC字节位置
	uint8_t calculated_crc = calculate_crc8(&data_buf[2], INT_LENGTH*4);
	
	if(received_crc != calculated_crc)
	{
		return;//CRC校验失败
	}
	// CRC校验通过，解析数据
    if(num == INT_LENGTH * 4 + 5) {
        for(int i = 0; i < INT_LENGTH; i++) {
            u8_to_int(data_buf, 2+i*4, &array_data[i]);
            printf("解析数据[%d]: %d\n", i, array_data[i]);
        }
    }
}

void gh_link_unpack_process(uint8_t data)
{
    static uint8_t buf[100];
	static uint8_t data_cnt = 0;
	static uint8_t state = 0;
	
	if(state==0&&data==GHLink_Head[0])//判断帧头1
	{
		state=1;
		buf[0]=data;
	}
	else if(state==1&&data==GHLink_Head[1])//判断帧头2
	{
		state=2;
		buf[1]=data;
		data_cnt=0;
	}
	else if(state==2)//数据接收
	{
		buf[2+data_cnt++]=data;
		// 接收数据 + CRC（1字节）
		if(data_cnt== INT_LENGTH*4 + 1)
		{
			state=3;
		}
	}
	else if(state==3&&data==GHLink_End[0])//帧尾0
	{
		state = 4;
		buf[2+data_cnt++]=data;
	}
	else if(state==4&&data==GHLink_End[1])//帧尾1
	{
		state = 0;
		buf[2+data_cnt]=data;
		gh_link_data(buf,data_cnt+3);//数据解析
	}
	else state = 0;
}


// 模拟接收数据
void simulate_receive_data(uint8_t *data, int length) {
    printf("模拟接收数据: ");
    for(int i = 0; i < length; i++) {
        printf("%02X ", data[i]);
        gh_link_unpack_process(data[i]);
    }
    printf("\n");
}



// 测试数据发送
void test_data_send(int array_data[]) {
    uint8_t data_to_send[INT_LENGTH*4+4];
    uint8_t i=0, cnt=0;
    uint8_t sum = INT_LENGTH;
    
    data_to_send[cnt++] = GHLink_Head[0];
    data_to_send[cnt++] = GHLink_Head[1];

    for(i=0; i<sum; i++) {
        int_to_u8(&array_data[i], cnt, data_to_send);
        cnt += 4;
    }
    
    data_to_send[cnt++] = GHLink_End[0];
    data_to_send[cnt++] = GHLink_End[1];

    USART1_Send_ArrayU8(data_to_send, cnt);
}

// 清空数组
void clear_array_data(void) {
    memset(array_data, 0, INT_LENGTH * sizeof(int));
}

// 从十六进制字符串解析数据
void parse_hex_string(const char *hex_str, uint8_t *data, int *length) {
    *length = 0;
    int value;
    while(*hex_str) {
        if(sscanf(hex_str, "%2x", &value) == 1) {
            data[(*length)++] = (uint8_t)value;
            hex_str += 2;
        } else {
            hex_str++;
        }
    }
}

// 主函数
int main() {
    printf("串口通信测试程序\n");
    
    // 测试发送数据
    printf("\n1. 测试发送数据:\n");
    for(int i = 0; i < INT_LENGTH; i++) {
        array_data[i] = i * 11;
    }

    gh_link_pack();

    // 测试接收数据
    printf("\n2. 测试接收数据:\n");
    char input[256];

    clear_array_data();
    printf("请输入十六进制数据(例如: AABB 00000000 0000000A 00000014 0000001E 0D0A): ");
    fgets(input, sizeof(input), stdin);
    
    uint8_t received_data[100];
    int data_length;
    parse_hex_string(input, received_data, &data_length);
    
    simulate_receive_data(received_data, data_length);
    
    // 显示解析后的数据
    printf("\n解析后的数组数据:\n");
    for(int i = 0; i < INT_LENGTH; i++) {
        printf("array_data[%d] = %d\n", i, array_data[i]);
    }
    
    return 0;
}