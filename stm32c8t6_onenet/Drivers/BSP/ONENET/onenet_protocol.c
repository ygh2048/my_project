/* 系统头文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

/* OneNET相关 */
#include "./BSP/ESP8266/esp8266.h"
#include "./BSP/ONENET/onenet_protocol.h"
#include "./BSP/ONENET/mqttkit.h"
#include "./BSP/ONENET/base64.h"
#include "./BSP/ONENET/hmac_sha1.h"
#include "./BSP/RELAY/relay.h"

/* C库 */
#include <string.h>
#include <stdio.h>

/*================================
*  内存池实现 (替代malloc)
*  用于MQTT库在STM32上的内存管理
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
#define PROID			"9M46J4XhAS"
#define ACCESS_KEY		"HWGFn8Gzd5EXiZ+Rb1GtZ5M16bn3XfWrRWnJO9/x3Wk="
#define DEVICE_NAME		"mytask"
/*  
#define PROID			"9M46J4XhAS"
#define ACCESS_KEY		"HWGFn8Gzd5EXiZ+Rb1GtZ5M16bn3XfWrRWnJO9/x3Wk="
#define DEVICE_NAME		"mytask"
*/

/**
 * @brief       安全的字符串到整数转换(不使用atoi避免栈溢出)
 * @param       str: 字符串指针
 * @retval      解析到的整数值
 */
static int simple_atoi(const char* str)
{
    int value = 0;
    int sign = 1;
    
    if (!str)
        return 0;
    
    /* 跳过空格 */
    while (*str && (*str == ' ' || *str == '\t'))
        str++;
    
    /* 处理符号 */
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }
    
    /* 解析数字 */
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
 * @param       key: 键名(如"light")
 * @retval      解析到的整数值，失败返回-1
 */
static int safe_json_parse_int(const char* json_str, const char* key)
{
    const char* ptr;
    int value = -1;
    
    if (!json_str || !key)
        return -1;
    
    /* 查找键 */
    ptr = strstr(json_str, key);
    if (!ptr)
        return -1;
    
    /* 查找冒号 */
    ptr = strchr(ptr, ':');
    if (!ptr)
        return -1;
    
    ptr++;  /* 跳过冒号 */
    
    /* 使用自己实现的整数解析，避免atoi的栈溢出 */
    value = simple_atoi(ptr);
    
    return value;
}

/* 全局变量 */
char devid[16];
char key[48];

/* 外部变量 */
extern uint8_t esp8266_buf[512];
extern system_status_t sys_status;  /* 系统状态（来自main.c） */
extern void set_remote_light_timeout(void);  /* 设置远程光控制优先级超时 */

/*
************************************************************
*	�������ƣ�	OTA_UrlEncode
*
*	�������ܣ�	sign��Ҫ����URL����
*
*	��ڲ�����	sign�����ܽ��
*
*	���ز�����	0-�ɹ�	����-ʧ��
*
*	˵����		+			%2B
*				�ո�		%20
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
*	�������ƣ�	OTA_Authorization
*
*	�������ܣ�	����Authorization
*
*	��ڲ�����	ver��������汾�ţ����ڸ�ʽ��Ŀǰ��֧�ָ�ʽ"2018-10-31"
*				res����Ʒid
*				et������ʱ�䣬UTC��ֵ
*				access_key��������Կ
*				dev_name���豸��
*				authorization_buf������token��ָ��
*				authorization_buf_len������������(�ֽ�)
*
*	���ز�����	0-�ɹ�	����-ʧ��
*
*	˵����		��ǰ��֧��sha1
************************************************************
*/
#define METHOD		"sha1"
static unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	
	char sign_buf[64];								//����ǩ����Base64������ �� URL������
	char hmac_sha1_buf[64];							//����ǩ��
	char access_key_base64[64];						//����access_key��Base64������
	char string_for_signature[72];					//����string_for_signature������Ǽ��ܵ�key

//----------------------------------------------------�����Ϸ���--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------��access_key����Base64����----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	//printf("access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------����string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
	//printf("string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------HMAC计算-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	unsigned char temp_data[128];  // 临时缓冲区,避免输入数据被修改
	memcpy(temp_data, string_for_signature, strlen(string_for_signature));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)temp_data, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
//----------------------------------------------------将签名结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));
//----------------------------------------------------��Base64����������URL����---------------------------------------------------
	OTA_UrlEncode(sign_buf);
	
//----------------------------------------------------构建Token--------------------------------------------------------------------
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
	
	return 0;

}

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	static unsigned char mqtt_buf[256];  // 静态缓冲区,避免malloc失败
	MQTT_PACKET_STRUCTURE mqttPacket = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};

	unsigned char *dataPtr;
	
	char authorization_buf[160];
	
	_Bool status = 1;
	
	OneNET_Authorization("2018-10-31", PROID, 1956499200, ACCESS_KEY, DEVICE_NAME,
								authorization_buf, sizeof(authorization_buf), 0);
	
	if(MQTT_PacketConnect(PROID, authorization_buf, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		if(esp8266_send_data(mqttPacket._data, mqttPacket._len) != 0)	//上传平台
		{
			printf("[ERROR] Failed to send MQTT CONNECT packet!\r\n");
			return 1;  /* 发送失败,直接返回 */
		}
		
		dataPtr = esp8266_get_ipd(1000);								//等待平台响应 (10秒)
		
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
		
		MQTT_DeleteBuffer(&mqttPacket);								//删除
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
	
	/* 电流(mA) - 需要乘以10显示为整数(0.1mA精度) */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"ma\":[{\"v\":%ld}],", (long)(sys_status.current * 10));
	strcat(buf, text);
	
	/* 功率(mW) - 需要乘以10显示为整数(0.1mW精度) */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"mp\":[{\"v\":%ld}],", (long)(sys_status.power * 10));
	strcat(buf, text);
	
	/* 质量(g) */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"weight\":[{\"v\":%d}],", sys_status.weight);
	strcat(buf, text);
	
	/* 照明开关 */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"light\":[{\"v\":%d}],", sys_status.light_status);
	strcat(buf, text);
	
	/* 总电源开关 */
	memset(text, 0, sizeof(text));
	sprintf(text, "\"open_close\":[{\"v\":%d}],", sys_status.power_status);
	strcat(buf, text);
	
	/* 物品状态(归还/未归还) - 根据重量判断 */
	memset(text, 0, sizeof(text));
	const char *object_status = (sys_status.weight > 500) ? "returned" : "not_returned";
	sprintf(text, "\"object\":[{\"v\":\"%s\"}]", object_status);
	strcat(buf, text);
	
	strcat(buf, "}}");
	
	return strlen(buf);
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
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
//	�������ƣ�	OneNET_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNET_Publish(const char *topic, const char *msg)
{
	static unsigned char mqtt_buf[512];  // 静态缓冲区
	MQTT_PACKET_STRUCTURE mqtt_packet = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		esp8266_send_data(mqtt_packet._data, mqtt_packet._len);
		
		MQTT_DeleteBuffer(&mqtt_packet);
	}

}

//==========================================================
//	�������ƣ�	OneNET_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNET_Subscribe(void)
{
	static unsigned char mqtt_buf[256];  // 静态缓冲区
	MQTT_PACKET_STRUCTURE mqtt_packet = {mqtt_buf, 0, sizeof(mqtt_buf), MEM_FLAG_STATIC};
	static char topic_buf[80];           // 改为 static，避免栈变量覆盖
	static char topic_accepted[80];      // 改为 static
	static char topic_rejected[80];      // 改为 static
	const char *topics[1];               // 每次只传递一个主题指针
	
	/* 订阅主题1: 命令下发 (使用通配符订阅所有命令) */
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/cmd/#", PROID, DEVICE_NAME);
	topics[0] = topic_buf;
	
	// 订阅命令主题 - 重新初始化结构体
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
	
	/* 订阅主题2: 数据上传成功响应 */
	snprintf(topic_accepted, sizeof(topic_accepted), "$sys/%s/%s/dp/post/json/accepted", PROID, DEVICE_NAME);
	topics[0] = topic_accepted;
	
	// 订阅数据上传accepted主题 - 重新初始化结构体
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
	
	/* 订阅主题3: 数据上传失败响应 */
	snprintf(topic_rejected, sizeof(topic_rejected), "$sys/%s/%s/dp/post/json/rejected", PROID, DEVICE_NAME);
	topics[0] = topic_rejected;
	
	// 订阅数据上传rejected主题 - 重新初始化结构体
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
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
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
	
	short result = -1;  /* 初始化为 -1 表示未处理 */
	
	if(cmd == NULL)
	{
		return;
	}
	
	type = MQTT_UnPacketRecv(cmd);
	
	switch(type)
	{
	case MQTT_PKT_PUBLISH:  /* 收到Publish消息 */
		
		result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
		
		if(result == 0)
		{
			char *data_ptr = NULL;
			int value;
			
			/* 处理属性写入 (property/set) */
			if(strstr(cmdid_topic, "property/set"))
			{
				char reply_topic[128];
				
				/* 构造应答主题 */
				snprintf(reply_topic, sizeof(reply_topic), "$sys/%s/%s/thing/property/set_reply",
						 PROID, DEVICE_NAME);
				
				/* 解析light属性 */
				value = safe_json_parse_int(req_payload, "light");
				if(value >= 0)
				{
					sys_status.light_status = (value != 0) ? 1 : 0;
					/* 立即控制继电器 */
					if(sys_status.light_status)
						relay_light_on();
					else
						relay_light_off();
					/* 设置远程控制优先级 */
					set_remote_light_timeout();
				}
				
				/* 解析open_close属性 */
				value = safe_json_parse_int(req_payload, "open_close");
				if(value >= 0)
				{
					sys_status.power_status = (value != 0) ? 1 : 0;
					/* 立即控制继电器 */
					if(sys_status.power_status)
						relay_power_on();
					else
						relay_power_off();
				}
				
				/* 发送属性设置应答 */
				delay_ms(500);
				OneNET_Publish(reply_topic, "{\"errno\":0,\"error\":\"success\"}");
			}
			/* 处理命令下发 */
			else if(strstr(cmdid_topic, "cmd/request/"))
			{
				char topic_buf[80], cmdid[40];
				
				/* 提取cmdid */
				data_ptr = strstr(cmdid_topic, "request/");
				if(data_ptr)
				{
					data_ptr = strchr(data_ptr, '/');
					if(data_ptr)
					{
						data_ptr++;
						memcpy(cmdid, data_ptr, 36);
						cmdid[36] = 0;
						
						/* 构造响应主题 */
						snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/cmd/response/%s",
								 PROID, DEVICE_NAME, cmdid);
						
						/* 发送命令响应 */
						OneNET_Publish(topic_buf, "{\"errno\":0,\"error\":\"succ\"}");
						
						/* 执行命令 */
						value = safe_json_parse_int(req_payload, "light");
						if(value >= 0)
						{
							sys_status.light_status = (value != 0) ? 1 : 0;
							/* 立即控制继电器 */
							if(sys_status.light_status)
								relay_light_on();
							else
								relay_light_off();
							/* 设置远程控制优先级 */
							set_remote_light_timeout();
						}
						
						value = safe_json_parse_int(req_payload, "open_close");
						if(value >= 0)
						{
							sys_status.power_status = (value != 0) ? 1 : 0;
							/* 立即控制继电器 */
							if(sys_status.power_status)
								relay_power_on();
							else
								relay_power_off();
						}
					}
				}
			}
		}	case MQTT_PKT_PUBACK:  /* 收到Publish消息的平台回复Ack */
		
		MQTT_UnPacketPublishAck(cmd);
		
	break;
	
	case MQTT_PKT_SUBACK:
	
		MQTT_UnPacketSubscribe(cmd);
	
	break;
	
	default:
		result = -1;
	break;
	}
	
	esp8266_clear();  /* 清空缓存 */	/* 只有当成功解析了消息时才释放内存 */
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


