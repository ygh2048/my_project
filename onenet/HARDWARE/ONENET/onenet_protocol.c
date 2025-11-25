/**
 * @file onenet_protocol.c
 * @brief OneNET协议实现文件
 * @author Smart Lab System
 * @date 2025-11-05
 */

/* 系统头文件 */
#include "sys.h"
#include "usart.h"
#include "delay.h"

/* OneNET相关 */
#include "esp8266.h"
#include "onenet_protocol.h"
#include "mqttkit.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "relay.h"

/* C库 */
#include <string.h>
#include <stdio.h>

/*================================
*  内存池实现(替代malloc)
*  用于MQTT包在STM32中的内存管理
*================================*/
#define MEM_POOL_SIZE  (4 * 1024)  /* 4KB 用于 MQTT */

typedef struct
{
    uint16_t size;      /* 分配的大小 */
    uint8_t used;       /* 是否已使用 */
    uint8_t reserved;
} mem_block_t;

static uint8_t mem_pool[MEM_POOL_SIZE];
static mem_block_t* first_block = NULL;

/* 前向声明 */
static void mem_pool_stat(void);

void mem_pool_init(void)
{
    first_block = (mem_block_t*)mem_pool;
    first_block->size = MEM_POOL_SIZE - sizeof(mem_block_t);
    first_block->used = 0;
    printf("[MEM_POOL] Initialized: %d bytes\r\n", MEM_POOL_SIZE);
}

void* mem_pool_malloc(size_t size)
{
    mem_block_t* block = first_block;
    int attempts = 0;
    
    if (size == 0)
        return NULL;
    
    /* 第一次扫描：寻找合适的空闲块 */
    while ((uintptr_t)block - (uintptr_t)mem_pool < MEM_POOL_SIZE && attempts < 100)
    {
        if (!block->used && block->size >= size)
        {
            /* 找到合适的块 */
            block->used = 1;
            
            /* 如果块足够大，分割块 */
            if (block->size > size + sizeof(mem_block_t))
            {
                mem_block_t* next_block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + size);
                next_block->size = block->size - size - sizeof(mem_block_t);
                next_block->used = 0;
                block->size = size;
            }
            
            return (void*)((uint8_t*)block + sizeof(mem_block_t));
        }
        
        block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + block->size);
        attempts++;
    }
    
    return NULL;
}

void mem_pool_free(void* ptr)
{
    mem_block_t* block;
    mem_block_t* next_block;
    
    if (ptr == NULL)
        return;
    
    block = (mem_block_t*)((uint8_t*)ptr - sizeof(mem_block_t));
    
    if ((uintptr_t)block < (uintptr_t)mem_pool || 
        (uintptr_t)block >= (uintptr_t)(mem_pool + MEM_POOL_SIZE))
    {
        printf("[MEM_POOL] ERROR: Invalid pointer to free\r\n");
        return;
    }
    
    block->used = 0;
    
    /* 向前合并：如果下一个块是空闲的，合并它 */
    next_block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + block->size);
    if ((uintptr_t)next_block - (uintptr_t)mem_pool < MEM_POOL_SIZE && !next_block->used)
    {
        block->size += sizeof(mem_block_t) + next_block->size;
    }
}

/* 调试函数：显示内存池统计信息 */
static void mem_pool_stat(void)
{
    mem_block_t* block = first_block;
    uint8_t block_count = 0;
    
    while (block_count < 50)
    {
        /* 检查block是否超出内存池范围 */
        if ((uintptr_t)block < (uintptr_t)mem_pool || 
            (uintptr_t)block >= (uintptr_t)(mem_pool + MEM_POOL_SIZE))
        {
            break;
        }
        
        /* 防止size为0或过大导致的死循环 */
        if (block->size == 0 || block->size > MEM_POOL_SIZE)
        {
            break;
        }
        
        block = (mem_block_t*)((uint8_t*)block + sizeof(mem_block_t) + block->size);
        block_count++;
    }
}

/* OneNET平台配置（您的设备信息） */

/*
#define PROID            "55I7rm0QP0"
#define ACCESS_KEY        "r32Dh0EhASMRjSevj0XN5fXJC7mZb72Hc+crYZ1PW3M="
#define DEVICE_NAME        "task"

*/
#define PROID            "22Z1aSgRuA"
#define ACCESS_KEY        "WRF7w2dB/z+TN9eTn+tJPV1joWnxOYFHzXejjV8rn+0="
#define DEVICE_NAME        "monitor"

/*  
#define PROID			"9M46J4XhAS"
#define ACCESS_KEY		"HWGFn8Gzd5EXiZ+Rb1GtZ5M16bn3XfWrRWnJO9/x3Wk="
#define DEVICE_NAME		"mytask"
*/

/**
 * @brief       瀹夊叏鐨勫瓧绗︿覆鍒版暣鏁拌浆鎹?涓嶄娇鐢╝toi閬垮厤鏍堟孩鍑?
 * @param       str: 瀛楃涓叉寚閽?
 * @retval      瑙ｆ瀽鍒扮殑鏁存暟鍊?
 */
static int simple_atoi(const char* str)
{
    int value = 0;
    int sign = 1;
    
    if (!str)
        return 0;
    
    /* 璺宠繃绌烘牸 */
    while (*str && (*str == ' ' || *str == '\t'))
        str++;
    
    /* 澶勭悊绗﹀彿 */
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }
    
    /* 瑙ｆ瀽鏁板瓧 */
    while (*str >= '0' && *str <= '9')
    {
        value = value * 10 + (*str - '0');
        str++;
    }
    
    return value * sign;
}

/**
 * @brief       安全的JSON整数解析函数
 * @param       json_str: JSON字符串指针
 * @param       key: 键名(如 "light")
 * @retval      解析到的整数值，失败返回-1
 */
static int safe_json_parse_int(const char* json_str, const char* key)
{
    const char* ptr;
    int value = -1;
    
    if (!json_str || !key)
        return -1;
    
    /* 鏌ユ壘閿?*/
    ptr = strstr(json_str, key);
    if (!ptr)
        return -1;
    
    /* 鏌ユ壘鍐掑彿 */
    ptr = strchr(ptr, ':');
    if (!ptr)
        return -1;
    
    ptr++;  /* 璺宠繃鍐掑彿 */
    
    /* 浣跨敤鑷繁瀹炵幇鐨勬暣鏁拌В鏋愶紝閬垮厤atoi鐨勬爤婧㈠嚭 */
    value = simple_atoi(ptr);
    
    return value;
}

/* 鍏ㄥ眬鍙橀噺 */
char devid[16];
char key[48];

/* 澶栭儴鍙橀噺 */
extern uint8_t esp8266_buf[512];
/*
************************************************************
*	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OTA_UrlEncode
*
*	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	sign锟斤拷要锟斤拷锟斤拷URL锟斤拷锟斤拷
*
*	锟斤拷诓锟斤拷锟斤拷锟?sign锟斤拷锟斤拷锟杰斤拷锟?
*
*	锟斤拷锟截诧拷锟斤拷锟斤拷	0-锟缴癸拷	锟斤拷锟斤拷-失锟斤拷
*
*	说锟斤拷锟斤拷		+			%2B
*				锟秸革拷		%20
*				/			%2F
*				?			%3F
*				%			%25
*				#			%23
*				&			%26
*				=			%3D
************************************************************
*/
static unsigned char OTA_UrlEncode(char *sign)
{

	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == (void *)0 || sign_len < 28)
		return 1;
	
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+':
				strcat(sign + j, "%2B");j += 3;
			break;
			
			case ' ':
				strcat(sign + j, "%20");j += 3;
			break;
			
			case '/':
				strcat(sign + j, "%2F");j += 3;
			break;
			
			case '?':
				strcat(sign + j, "%3F");j += 3;
			break;
			
			case '%':
				strcat(sign + j, "%25");j += 3;
			break;
			
			case '#':
				strcat(sign + j, "%23");j += 3;
			break;
			
			case '&':
				strcat(sign + j, "%26");j += 3;
			break;
			
			case '=':
				strcat(sign + j, "%3D");j += 3;
			break;
			
			default:
				sign[j] = sign_t[i];j++;
			break;
		}
	}
	
	sign[j] = 0;
	
	return 0;

}

/*
************************************************************
*	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OTA_Authorization
*
*	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	锟斤拷锟斤拷Authorization
*
*	锟斤拷诓锟斤拷锟斤拷锟?ver锟斤拷锟斤拷锟斤拷锟斤拷姹撅拷牛锟斤拷锟斤拷诟锟绞斤拷锟侥壳帮拷锟街э拷指锟绞?2018-10-31"
*				res锟斤拷锟斤拷品id
*				et锟斤拷锟斤拷锟斤拷时锟戒，UTC锟斤拷值
*				access_key锟斤拷锟斤拷锟斤拷锟斤拷钥
*				dev_name锟斤拷锟借备锟斤拷
*				authorization_buf锟斤拷锟斤拷锟斤拷token锟斤拷指锟斤拷
*				authorization_buf_len锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷(锟街斤拷)
*
*	锟斤拷锟截诧拷锟斤拷锟斤拷	0-锟缴癸拷	锟斤拷锟斤拷-失锟斤拷
*
*	说锟斤拷锟斤拷		锟斤拷前锟斤拷支锟斤拷sha1
************************************************************
*/
#define METHOD		"sha1"
static unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	
	char sign_buf[64];								//锟斤拷锟斤拷签锟斤拷锟斤拷Base64锟斤拷锟斤拷锟斤拷 锟斤拷 URL锟斤拷锟斤拷锟斤拷
	char hmac_sha1_buf[64];							//锟斤拷锟斤拷签锟斤拷
	char access_key_base64[64];						//锟斤拷锟斤拷access_key锟斤拷Base64锟斤拷锟斤拷锟斤拷
	char string_for_signature[72];					//锟斤拷锟斤拷string_for_signature锟斤拷锟斤拷锟斤拷羌锟斤拷艿锟絢ey

//----------------------------------------------------锟斤拷锟斤拷锟较凤拷锟斤拷--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------锟斤拷access_key锟斤拷锟斤拷Base64锟斤拷锟斤拷----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	//printf("access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------锟斤拷锟斤拷string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
	//printf("string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------HMAC璁＄畻-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	unsigned char temp_data[128];  // 涓存椂缂撳啿鍖?閬垮厤杈撳叆鏁版嵁琚慨鏀?
	memcpy(temp_data, string_for_signature, strlen(string_for_signature));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)temp_data, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
//----------------------------------------------------灏嗙鍚嶇粨鏋滆繘琛孊ase64缂栫爜------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));
//----------------------------------------------------锟斤拷Base64锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷URL锟斤拷锟斤拷---------------------------------------------------
	OTA_UrlEncode(sign_buf);
	
//----------------------------------------------------鏋勫缓Token--------------------------------------------------------------------
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
	
	return 0;

}

//==========================================================
//	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OneNet_DevLink
//
//	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	锟斤拷onenet锟斤拷锟斤拷锟斤拷锟斤拷
//
//	锟斤拷诓锟斤拷锟斤拷锟?锟斤拷
//
//	锟斤拷锟截诧拷锟斤拷锟斤拷	1-锟缴癸拷	0-失锟斤拷
//
//	说锟斤拷锟斤拷		锟斤拷onenet平台锟斤拷锟斤拷锟斤拷锟斤拷
//==========================================================
_Bool OneNet_DevLink(void)
{
	static unsigned char mqtt_buf[256];  // 闈欐€佺紦鍐插尯,閬垮厤malloc澶辫触
	MQTT_PACKET_STRUCTURE mqttPacket = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};

	unsigned char *dataPtr;
	
	char authorization_buf[160];
	
	_Bool status = 1;
	
	OneNET_Authorization("2018-10-31", PROID, 1956499200, ACCESS_KEY, DEVICE_NAME,
								authorization_buf, sizeof(authorization_buf), 0);
	
	if(MQTT_PacketConnect(PROID, authorization_buf, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		if(esp8266_send_data(mqttPacket._data, mqttPacket._len) != 0)	//涓婁紶骞冲彴
		{
			printf("[ERROR] Failed to send MQTT CONNECT packet!\r\n");
			return 1;  /* 鍙戦€佸け璐?鐩存帴杩斿洖 */
		}
		
		dataPtr = esp8266_get_ipd(1000);								//绛夊緟骞冲彴鍝嶅簲 (10绉?
		
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:
						status = 0;
						break;
					
					case 1:printf("[OneNET] ERROR: Unacceptable protocol version\r\n");break;
					case 2:printf("[OneNET] ERROR: Identifier rejected\r\n");break;
					case 3:printf("[OneNET] ERROR: Server unavailable\r\n");break;
					case 4:printf("[OneNET] ERROR: Bad username or password\r\n");break;
					case 5:printf("[OneNET] ERROR: Not authorized\r\n");break;
					
					default:printf("[OneNET] ERROR: Connection failed\r\n");break;
				}
			}
			else
			{
				printf("[ERROR] Wrong packet type\r\n");
			}
		}
		else
		{
			printf("[ERROR] CONNACK timeout\r\n");
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//鍒犻櫎
	}
	else
		printf("[ERROR] MQTT Connect Failed\r\n");
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf)
{
	char text[64];
	
	memset(text, 0, sizeof(text));
	
	/* OneJSON格式：{"id":123,"dp":{"属性名":[{"v":值}],...}} */
	strcpy(buf, "{\"id\":123,\"dp\":{");
	
	/* 电压(mV) - 需要乘以1000转换 */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"mv\":[{\"v\":%ld}],", (long)(sys_status.voltage * 1000));
	strcat(buf, text);
	
	/* 鐢垫祦(mA) - 闇€瑕佷箻浠?0鏄剧ず涓烘暣鏁?0.1mA绮惧害) */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"ma\":[{\"v\":%ld}],", (long)(sys_status.current * 10));
	strcat(buf, text);
	
	/* 鍔熺巼(mW) - 闇€瑕佷箻浠?0鏄剧ず涓烘暣鏁?0.1mW绮惧害) */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"mp\":[{\"v\":%ld}],", (long)(sys_status.power * 10));
	strcat(buf, text);
	
	/* 璐ㄩ噺(g) */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"weight\":[{\"v\":%d}],", sys_status.weight);
	strcat(buf, text);
	
	/* 鐓ф槑寮€鍏?*/
	memset(text, 0, sizeof(text));
	sprintf(text, "\"light\":[{\"v\":%d}],", sys_status.light_status);
	strcat(buf, text);
	
	/* 鎬荤數婧愬紑鍏?*/
	memset(text, 0, sizeof(text));
	sprintf(text, "\"open_close\":[{\"v\":%d}],", sys_status.power_status);
	strcat(buf, text);
	
	/* 鐗╁搧鐘舵€?褰掕繕/鏈綊杩? - 鏍规嵁閲嶉噺鍒ゆ柇 */
	memset(text, 0, sizeof(text));
	const char *object_status = (sys_status.weight > APP_WEIGHT_THRESHOLD_GRAMS) ? "returned" : "not_returned";
	sprintf(text, "\"object\":[{\"v\":\"%s\"}]", object_status);
	strcat(buf, text);
	
	strcat(buf, "}}");
	
	return strlen(buf);
}

//==========================================================
//	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OneNet_SendData
//
//	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	锟较达拷锟斤拷锟捷碉拷平台
//
//	锟斤拷诓锟斤拷锟斤拷锟?type锟斤拷锟斤拷锟斤拷锟斤拷锟捷的革拷式
//
//	锟斤拷锟截诧拷锟斤拷锟斤拷	锟斤拷
//
//	说锟斤拷锟斤拷		
//==========================================================
void OneNet_SendData(void)
{
	static unsigned char mqtt_buf[512];
	MQTT_PACKET_STRUCTURE mqttPacket = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROID, DEVICE_NAME, body_len, NULL, &mqttPacket) == 0)
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			esp8266_send_data(mqttPacket._data, mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);
		}
	}
	
}

//==========================================================
//	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OneNET_Publish
//
//	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	锟斤拷锟斤拷锟斤拷息
//
//	锟斤拷诓锟斤拷锟斤拷锟?topic锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
//				msg锟斤拷锟斤拷息锟斤拷锟斤拷
//
//	锟斤拷锟截诧拷锟斤拷锟斤拷	锟斤拷
//
//	说锟斤拷锟斤拷		
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{
	static unsigned char mqtt_buf[512];  // 闈欐€佺紦鍐插尯
	MQTT_PACKET_STRUCTURE mqtt_packet = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		esp8266_send_data(mqtt_packet._data, mqtt_packet._len);
		
		MQTT_DeleteBuffer(&mqtt_packet);
	}

}

//==========================================================
//	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OneNET_Subscribe
//
//	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	锟斤拷锟斤拷
//
//	锟斤拷诓锟斤拷锟斤拷锟?锟斤拷
//
//	锟斤拷锟截诧拷锟斤拷锟斤拷	锟斤拷
//
//	说锟斤拷锟斤拷		
//==========================================================
void OneNET_Subscribe(void)
{
	static unsigned char mqtt_buf[256];  // 闈欐€佺紦鍐插尯
	MQTT_PACKET_STRUCTURE mqtt_packet = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};
	static char topic_buf[80];           // 鏀逛负 static锛岄伩鍏嶆爤鍙橀噺瑕嗙洊
	static char topic_accepted[80];      // 鏀逛负 static
	static char topic_rejected[80];      // 鏀逛负 static
	const char *topics[1];               // 姣忔鍙紶閫掍竴涓富棰樻寚閽?
	
	/* 璁㈤槄涓婚1: 鍛戒护涓嬪彂 (浣跨敤閫氶厤绗﹁闃呮墍鏈夊懡浠? */
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/cmd/#", PROID, DEVICE_NAME);
	topics[0] = topic_buf;
	
	// 璁㈤槄鍛戒护涓婚 - 閲嶆柊鍒濆鍖栫粨鏋勪綋
	mqtt_packet._data = mqtt_buf;
	mqtt_packet._len = 0;
	mqtt_packet._size = sizeof(mqtt_buf);
	mqtt_packet._memFlag = MEM_FLAG_STATIC;
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, 1, &mqtt_packet) == 0)
	{
		esp8266_send_data(mqtt_packet._data, mqtt_packet._len);
		MQTT_DeleteBuffer(&mqtt_packet);
	}
	delay_ms(100);
	
	/* 璁㈤槄涓婚2: 鏁版嵁涓婁紶鎴愬姛鍝嶅簲 */
	snprintf(topic_accepted, sizeof(topic_accepted), "$sys/%s/%s/dp/post/json/accepted", PROID, DEVICE_NAME);
	topics[0] = topic_accepted;
	
	// 璁㈤槄鏁版嵁涓婁紶accepted涓婚 - 閲嶆柊鍒濆鍖栫粨鏋勪綋
	mqtt_packet._data = mqtt_buf;
	mqtt_packet._len = 0;
	mqtt_packet._size = sizeof(mqtt_buf);
	mqtt_packet._memFlag = MEM_FLAG_STATIC;
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID + 1, MQTT_QOS_LEVEL0, topics, 1, &mqtt_packet) == 0)
	{
		esp8266_send_data(mqtt_packet._data, mqtt_packet._len);
		MQTT_DeleteBuffer(&mqtt_packet);
	}
	delay_ms(100);
	
	/* 璁㈤槄涓婚3: 鏁版嵁涓婁紶澶辫触鍝嶅簲 */
	snprintf(topic_rejected, sizeof(topic_rejected), "$sys/%s/%s/dp/post/json/rejected", PROID, DEVICE_NAME);
	topics[0] = topic_rejected;
	
	// 璁㈤槄鏁版嵁涓婁紶rejected涓婚 - 閲嶆柊鍒濆鍖栫粨鏋勪綋
	mqtt_packet._data = mqtt_buf;
	mqtt_packet._len = 0;
	mqtt_packet._size = sizeof(mqtt_buf);
	mqtt_packet._memFlag = MEM_FLAG_STATIC;
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID + 2, MQTT_QOS_LEVEL0, topics, 1, &mqtt_packet) == 0)
	{
		esp8266_send_data(mqtt_packet._data, mqtt_packet._len);
		MQTT_DeleteBuffer(&mqtt_packet);
	}

}

//==========================================================
//	锟斤拷锟斤拷锟斤拷锟狡ｏ拷	OneNet_RevPro
//
//	锟斤拷锟斤拷锟斤拷锟杰ｏ拷	平台锟斤拷锟斤拷锟斤拷锟捷硷拷锟?
//
//	锟斤拷诓锟斤拷锟斤拷锟?dataPtr锟斤拷平台锟斤拷锟截碉拷锟斤拷锟斤拷
//
//	锟斤拷锟截诧拷锟斤拷锟斤拷	锟斤拷
//
//	说锟斤拷锟斤拷		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char type = 0;
	
	short result = -1;  /* 鍒濆鍖栦负 -1 琛ㄧず鏈鐞?*/
	
	if(cmd == NULL)
	{
		return;
	}
	
	type = MQTT_UnPacketRecv(cmd);
	
	switch(type)
	{
	case MQTT_PKT_PUBLISH:  /* 鏀跺埌Publish娑堟伅 */
		
		result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
		
		if(result == 0)
		{
			char *data_ptr = NULL;
			int value;
			
			/* 澶勭悊灞炴€у啓鍏?(property/set) */
			if(strstr(cmdid_topic, "property/set"))
			{
				char reply_topic[128];
				
				/* 鏋勯€犲簲绛斾富棰?*/
				snprintf(reply_topic, sizeof(reply_topic), "$sys/%s/%s/thing/property/set_reply",
						 PROID, DEVICE_NAME);
				
				/* 瑙ｆ瀽light灞炴€?*/
				value = safe_json_parse_int(req_payload, "light");
				if(value >= 0)
				{
					sys_status.light_status = (value != 0) ? 1 : 0;
					/* 绔嬪嵆鎺у埗缁х數鍣?*/
					if(sys_status.light_status)
						relay_light_on();
					else
						relay_light_off();
					/* 璁剧疆杩滅▼鎺у埗浼樺厛绾?*/
					set_remote_light_timeout();
				}
				
				/* 瑙ｆ瀽open_close灞炴€?*/
				value = safe_json_parse_int(req_payload, "open_close");
				if(value >= 0)
				{
					sys_status.power_status = (value != 0) ? 1 : 0;
					/* 绔嬪嵆鎺у埗缁х數鍣?*/
					if(sys_status.power_status)
						relay_power_on();
					else
						relay_power_off();
				}
				
				/* 鍙戦€佸睘鎬ц缃簲绛?*/
				delay_ms(500);
				OneNET_Publish(reply_topic, "{\"errno\":0,\"error\":\"success\"}");
			}
			/* 澶勭悊鍛戒护涓嬪彂 */
			else if(strstr(cmdid_topic, "cmd/request/"))
			{
				char topic_buf[80], cmdid[40];
				
				/* 鎻愬彇cmdid */
				data_ptr = strstr(cmdid_topic, "request/");
				if(data_ptr)
				{
					data_ptr = strchr(data_ptr, '/');
					if(data_ptr)
					{
						data_ptr++;
						memcpy(cmdid, data_ptr, 36);
						cmdid[36] = 0;
						
						/* 鏋勯€犲搷搆斾富棰?*/
						snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/cmd/response/%s",
								 PROID, DEVICE_NAME, cmdid);
						
						/* 鍙戦€佸懡浠ゅ搷搆?*/
						OneNET_Publish(topic_buf, "{\"errno\":0,\"error\":\"succ\"}");
						
						/* 鎵ц鍛戒护 */
						value = safe_json_parse_int(req_payload, "light");
						if(value >= 0)
						{
							sys_status.light_status = (value != 0) ? 1 : 0;
							/* 绔嬪嵆鎺у埗缁х數鍣?*/
							if(sys_status.light_status)
								relay_light_on();
							else
								relay_light_off();
							/* 璁剧疆杩滅▼鎺у埗浼樺厛绾?*/
							set_remote_light_timeout();
						}
						
						value = safe_json_parse_int(req_payload, "open_close");
						if(value >= 0)
						{
							sys_status.power_status = (value != 0) ? 1 : 0;
							/* 绔嬪嵆鎺у埗缁х數鍣?*/
							if(sys_status.power_status)
								relay_power_on();
							else
								relay_power_off();
						}
					}
				}
			}
		}	case MQTT_PKT_PUBACK:  /* 鏀跺埌Publish娑堟伅鐨勫钩鍙板洖澶岮ck */
		
		MQTT_UnPacketPublishAck(cmd);
		
	break;
	
	case MQTT_PKT_SUBACK:
	
		MQTT_UnPacketSubscribe(cmd);
	
	break;
	
	default:
		result = -1;
	break;
	}
	
	esp8266_clear();  /* 娓呯┖缂撳瓨 */	/* 鍙湁褰撴垚鍔熻В鏋愪簡娑堟伅鏃舵墠閲婃斁鍐呭瓨 */
	if(result == 0 && (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH))
	{
		if(cmdid_topic != NULL)
		{
			MQTT_FreeBuffer(cmdid_topic);
		}
		if(req_payload != NULL)
		{
			MQTT_FreeBuffer(req_payload);
		}
	}
	
	printf("[DEBUG] [OneNet_RevPro] Exit\r\n");
}




