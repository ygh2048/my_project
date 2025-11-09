/**
 * @file mqttkit.c
 * @brief MQTT协议包处理文件
 * @author Smart Lab System
 * @date 2025-11-05
 */

#include "mqttkit.h"

/* C库 */
#include <string.h>
#include <stdio.h>


#define CMD_TOPIC_PREFIX		"$creq"


//==========================================================
//	函数名称：	MQTT_NewBuffer
//
//	函数功能：	创建缓冲区
//
//	入口参数：	mqttPacket结构体
//				size大小
//
//	返回值：	无
//
//	说明：		1.使用动态内存分配缓冲区
//				2.使用全局静态内存分配缓冲区指针
//==========================================================
void MQTT_NewBuffer(MQTT_PACKET_STRUCTURE *mqttPacket, uint32 size)
{
	
	uint32 i = 0;

	if(mqttPacket->_data == NULL)
	{
		mqttPacket->_memFlag = MEM_FLAG_ALLOC;
		
		mqttPacket->_data = (uint8 *)MQTT_MallocBuffer(size);
		if(mqttPacket->_data != NULL)
		{
			mqttPacket->_len = 0;
			
			mqttPacket->_size = size;
			
			for(; i < mqttPacket->_size; i++)
				mqttPacket->_data[i] = 0;
		}
	}
	else
	{
		mqttPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < mqttPacket->_size; i++)
			mqttPacket->_data[i] = 0;
		
		mqttPacket->_len = 0;
		
		if(mqttPacket->_size < size)
			mqttPacket->_data = NULL;
	}

}

//==========================================================
//	函数名称：	MQTT_DeleteBuffer
//
//	函数功能：	删除缓冲区
//
//	入口参数：	mqttPacket结构体
//
//	返回值：	无
//
//	说明：		
//==========================================================
void MQTT_DeleteBuffer(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	if(mqttPacket->_memFlag == MEM_FLAG_ALLOC)
		MQTT_FreeBuffer(mqttPacket->_data);
	
	mqttPacket->_data = NULL;
	mqttPacket->_len = 0;
	mqttPacket->_size = 0;
	mqttPacket->_memFlag = MEM_FLAG_NULL;

}

int32 MQTT_DumpLength(size_t len, uint8 *buf)
{
	
	int32 i = 0;
	
	for(i = 1; i <= 4; ++i)
	{
		*buf = len % 128;
		len >>= 7;
		if(len > 0)
		{
			*buf |= 128;
			++buf;
		}
		else
		{
			return i;
		}
	}

	return -1;
}

int32 MQTT_ReadLength(const uint8 *stream, int32 size, uint32 *len)
{
	
	int32 i;
	const uint8 *in = stream;
	uint32 multiplier = 1;

	*len = 0;
	for(i = 0; i < size; ++i)
	{
		*len += (in[i] & 0x7f) * multiplier;

		if(!(in[i] & 0x80))
		{
			return i + 1;
		}

		multiplier <<= 7;
		if(multiplier >= 2097152)		//128 * *128 * *128
		{
			return -2;					// error, out of range
		}
	}

	return -1;							// not complete

}

//==========================================================
//	函数名称：	MQTT_UnPacketRecv
//
//	函数功能：	MQTT解包接收
//
//	入口参数：	dataPtr数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_UnPacketRecv(uint8 *dataPtr)
{
	
	uint8 status = 255;
	
	if(dataPtr == NULL)
	{
		return status;
	}
	
	uint8 type = dataPtr[0] >> 4;				//获取消息类型
	
	if(type < 1 || type > 14)
	{
		return status;
	}
	
	if(type == MQTT_PKT_PUBLISH)
	{
		uint8 *msgPtr;
		uint32 remain_len = 0;
		
		msgPtr = dataPtr + MQTT_ReadLength(dataPtr + 1, 4, &remain_len) + 1;
		
		if(remain_len < 2 || dataPtr[0] & 0x01)					//retain
			return 255;
		
		if(remain_len < ((uint16)msgPtr[0] << 8 | msgPtr[1]) + 2)
			return 255;
		
		if(strstr((int8 *)msgPtr + 2, CMD_TOPIC_PREFIX) != NULL)	//查找命令主题前缀
			status = MQTT_PKT_CMD;
		else
			status = MQTT_PKT_PUBLISH;
	}
	else
		status = type;
	
	return status;

}

//==========================================================
//	函数名称：	MQTT_PacketConnect
//
//	函数功能：	打包连接请求
//
//	入口参数：	user用户名
//				password密码
//				devid设备ID
//				cTime连接超时时间
//				clean_session清除会话标志
//				qos服务质量等级
//				will_topic遗嘱主题
//				will_msg遗嘱消息
//				will_retain遗嘱保留标志
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_PacketConnect(const int8 *user, const int8 *password, const int8 *devid,
						uint16 cTime, uint1 clean_session, uint1 qos,
						const int8 *will_topic, const int8 *will_msg, int32 will_retain,
						MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint8 flags = 0;
	uint8 will_topic_len = 0;
	uint16 total_len = 15;
	int16 len = 0, devid_len = strlen(devid);
	
	if(!devid)
	{
		printf("[ERROR-MQTT] devid is NULL\r\n");
		return 1;
	}
	
	total_len += devid_len + 2;
	
	//清除会话标志
	if(clean_session)
	{
		flags |= MQTT_CONNECT_CLEAN_SESSION;
	}
	
	//遗嘱相关
	if(will_topic)
	{
		flags |= MQTT_CONNECT_WILL_FLAG;
		will_topic_len = strlen(will_topic);
		total_len += 4 + will_topic_len + strlen(will_msg);
	}
	
	//qos设置
	switch((unsigned char)qos)
	{
		case MQTT_QOS_LEVEL0:
			flags |= MQTT_CONNECT_WILL_QOS0;							//取值为0
		break;
		
		case MQTT_QOS_LEVEL1:
			flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS1);	//取值为1
		break;
		
		case MQTT_QOS_LEVEL2:
			flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS2);	//取值为2
		break;
		
		default:
		return 2;
	}
	
	//设置遗嘱保留标志
	if(will_retain)
	{
		flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_RETAIN);
	}
	
	//用户名和密码
	if(!user || !password)
	{
		return 3;
	}
	flags |= MQTT_CONNECT_USER_NAME | MQTT_CONNECT_PASSORD;
	
	total_len += strlen(user) + strlen(password) + 4;
	
	//创建缓冲区
	MQTT_NewBuffer(mqttPacket, total_len);
	if(mqttPacket->_data == NULL)
		return 4;
	
	memset(mqttPacket->_data, 0, total_len);
	
/*************************************MQTT连接请求头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_CONNECT << 4;
	
	//写入剩余长度
	len = MQTT_DumpLength(total_len - 5, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 5;
	}
	else
		mqttPacket->_len += len;
	
/*************************************MQTT连接协议头部***********************************************/

	//写入协议头
	mqttPacket->_data[mqttPacket->_len++] = 0;
	mqttPacket->_data[mqttPacket->_len++] = 4;
	mqttPacket->_data[mqttPacket->_len++] = 'M';
	mqttPacket->_data[mqttPacket->_len++] = 'Q';
	mqttPacket->_data[mqttPacket->_len++] = 'T';
	mqttPacket->_data[mqttPacket->_len++] = 'T';
	
	//协议级别
	mqttPacket->_data[mqttPacket->_len++] = 4;
	
	//连接标志
	mqttPacket->_data[mqttPacket->_len++] = flags;
	
	//保持活动时间
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(cTime);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(cTime);
	 
/*************************************连接请求有效载荷**********************************************/

	//写入设备ID
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(devid_len);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(devid_len);
	
	strncat((int8 *)mqttPacket->_data + mqttPacket->_len, devid, devid_len);
	mqttPacket->_len += devid_len;
	
	//写入遗嘱相关信息
	if(flags & MQTT_CONNECT_WILL_FLAG)
	{
		unsigned short mLen = 0;
		
		if(!will_msg)
			will_msg = "";
		
		mLen = strlen(will_topic);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(mLen);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(mLen);
		strncat((int8 *)mqttPacket->_data + mqttPacket->_len, will_topic, mLen);
		mqttPacket->_len += mLen;
		
		mLen = strlen(will_msg);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(mLen);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(mLen);
		strncat((int8 *)mqttPacket->_data + mqttPacket->_len, will_msg, mLen);
		mqttPacket->_len += mLen;
	}
	
	//写入用户名
	if(flags & MQTT_CONNECT_USER_NAME)
	{
		unsigned short user_len = strlen(user);
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(user_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(user_len);
		strncat((int8 *)mqttPacket->_data + mqttPacket->_len, user, user_len);
		mqttPacket->_len += user_len;
	}

	//写入密码
	if(flags & MQTT_CONNECT_PASSORD)
	{
		unsigned short psw_len = strlen(password);
		
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(psw_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(psw_len);
		strncat((int8 *)mqttPacket->_data + mqttPacket->_len, password, psw_len);
		mqttPacket->_len += psw_len;
	}

	return 0;

}

//==========================================================
//	函数名称：	MQTT_PacketDisConnect
//
//	函数功能：	打包断开连接请求
//
//	入口参数：	mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_PacketDisConnect(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 2);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************MQTT断开连接请求头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_DISCONNECT << 4;
	
	//写入剩余长度
	mqttPacket->_data[mqttPacket->_len++] = 0;
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketConnectAck
//
//	函数功能：	解包连接确认
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_UnPacketConnectAck(uint8 *rev_data)
{
	if(rev_data[1] != 2)
	{
		printf("[ERROR-MQTT] CONNACK length error: %d (expected 2)\r\n", rev_data[1]);
		return 1;
	}
	
	if(rev_data[2] == 0 || rev_data[2] == 1)
	{
		return rev_data[3];
	}
	else
	{
		printf("[ERROR-MQTT] CONNACK flags error: %d\r\n", rev_data[2]);
		return 255;
	}

}

//==========================================================
//	函数名称：	MQTT_PacketSaveData
//
//	函数功能：	打包保存数据请求
//
//	入口参数：	pro_id产品ID
//				dev_name设备名称
//				send_len数据长度
//				type_bin_head二进制头部信息
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_PacketSaveData(const int8 *pro_id, const char *dev_name,
							int16 send_len, int8 *type_bin_head, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	char topic_buf[48];
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/dp/post/json", pro_id, dev_name);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic_buf, NULL, send_len + 0, MQTT_QOS_LEVEL1, 0, 1, mqttPacket) == 0)
	{
//		mqttPacket->_data[mqttPacket->_len++] = type;					//写入数据类型
//		
//		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(send_len);
//		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(send_len);
	}
	else
		return 1;
	
	return 0;

}//==========================================================
//	函数名称：	MQTT_PacketSaveBinData
//
//	函数功能：	打包保存二进制数据请求
//
//	入口参数：	name文件名
//				file_len文件长度
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_PacketSaveBinData(const int8 *name, int16 file_len, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	uint1 result = 1;
	int8 *bin_head = NULL;
	uint8 bin_head_len = 0;
	int8 *payload = NULL;
	int32 payload_size = 0;
	
	bin_head = (int8 *)MQTT_MallocBuffer(13 + strlen(name));
	if(bin_head == NULL)
		return result;
	
	sprintf(bin_head, "{\"ds_id\":\"%s\"}", name);
	
	bin_head_len = strlen(bin_head);
	payload_size = 7 + bin_head_len + file_len;
	
	payload = (int8 *)MQTT_MallocBuffer(payload_size - file_len);
	if(payload == NULL)
	{
		MQTT_FreeBuffer(bin_head);
		
		return result;
	}
	
	payload[0] = 2;						//数据类型为2
		
	payload[1] = MOSQ_MSB(bin_head_len);
	payload[2] = MOSQ_LSB(bin_head_len);
	
	memcpy(payload + 3, bin_head, bin_head_len);
	
	payload[bin_head_len + 3] = (file_len >> 24) & 0xFF;
	payload[bin_head_len + 4] = (file_len >> 16) & 0xFF;
	payload[bin_head_len + 5] = (file_len >> 8) & 0xFF;
	payload[bin_head_len + 6] = file_len & 0xFF;
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, "$sys/7bUd924I1v/TEST1/dp/post/json", payload, payload_size, MQTT_QOS_LEVEL1, 0, 1, mqttPacket) == 0)
		result = 0;
	
	MQTT_FreeBuffer(bin_head);
	MQTT_FreeBuffer(payload);
	
	return result;

}

//==========================================================
//	函数名称：	MQTT_UnPacketCmd
//
//	函数功能：	解包命令请求
//
//	入口参数：	rev_data接收数据指针
//				cmdid命令ID指针
//				req请求数据指针
//				req_len请求数据长度指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_UnPacketCmd(uint8 *rev_data, int8 **cmdid, int8 **req, uint16 *req_len)
{

	int8 *dataPtr = strchr((int8 *)rev_data + 6, '/');	//跳过6字节找到'/'
	
	uint32 remain_len = 0;
	
	if(dataPtr == NULL)									//未找到'/'
		return 1;
	dataPtr++;											//指向'/'后一个字节
	
	MQTT_ReadLength(rev_data + 1, 4, &remain_len);		//获取剩余长度
	
	*cmdid = (int8 *)MQTT_MallocBuffer(37);				//cmdid长度为36字节加1字节结束符
	if(*cmdid == NULL)
		return 2;
	
	memset(*cmdid, 0, 37);								//全零初始化
	memcpy(*cmdid, (const int8 *)dataPtr, 36);			//拷贝cmdid
	dataPtr += 36;
	
	*req_len = remain_len - 44;							//请求数据长度 = 剩余长度(remain_len) - 2 - 5($creq) - 1(\) - cmdid长度
	*req = (int8 *)MQTT_MallocBuffer(*req_len + 1);		//申请请求数据缓冲区
	if(*req == NULL)
	{
		MQTT_FreeBuffer(*cmdid);
		return 3;
	}
	
	memset(*req, 0, *req_len + 1);						//清零
	memcpy(*req, (const int8 *)dataPtr, *req_len);		//拷贝请求数据
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_PacketCmdResp
//
//	函数功能：	打包命令响应
//
//	入口参数：	cmdid命令ID
//				req请求数据
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_PacketCmdResp(const int8 *cmdid, const int8 *req, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint16 cmdid_len = strlen(cmdid);
	uint16 req_len = strlen(req);
	_Bool status = 0;
	
	int8 *payload = MQTT_MallocBuffer(cmdid_len + 7);
	if(payload == NULL)
		return 1;
	
	memset(payload, 0, cmdid_len + 7);
	memcpy(payload, "$crsp/", 6);
	strncat(payload, cmdid, cmdid_len);

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, payload, req, strlen(req), MQTT_QOS_LEVEL0, 0, 1, mqttPacket) == 0)
		status = 0;
	else
		status = 1;
	
	MQTT_FreeBuffer(payload);
	
	return status;

}

//==========================================================
//	函数名称：	MQTT_PacketSubscribe
//
//	函数功能：	打包订阅请求
//
//	入口参数：	pkt_id包ID
//				qos服务质量等级
//				topics主题列表
//				topics_cnt主题数量
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_PacketSubscribe(uint16 pkt_id, enum MqttQosLevel qos, const int8 *topics[], uint8 topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint32 topic_len = 0, remain_len = 0;
	int16 len = 0;
	uint8 i = 0;
	
	if(pkt_id == 0)
		return 1;
	
	//计算主题总长度
	for(; i < topics_cnt; i++)
	{
		if(topics[i] == NULL)
			return 2;
		
		topic_len += strlen(topics[i]);
	}
	
	//2 bytes packet id + topic filter(2 bytes topic + topic length + 1 byte reserve)------
	remain_len = 2 + 3 * topics_cnt + topic_len;
	
	//创建缓冲区
	MQTT_NewBuffer(mqttPacket, remain_len + 5);
	if(mqttPacket->_data == NULL)
		return 3;
	
/*************************************MQTT订阅请求头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_SUBSCRIBE << 4 | 0x02;
	
	//写入剩余长度
	len = MQTT_DumpLength(remain_len, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 4;
	}
	else
		mqttPacket->_len += len;
	
/*************************************有效载荷***********************************************/

	//写入包ID
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	
	//写入主题名称
	for(i = 0; i < topics_cnt; i++)
	{
		topic_len = strlen(topics[i]);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
		
		strncat((int8 *)mqttPacket->_data + mqttPacket->_len, topics[i], topic_len);
		mqttPacket->_len += topic_len;
		
		mqttPacket->_data[mqttPacket->_len++] = qos & 0xFF;
	}

	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketSubscrebe
//
//	函数功能：	解包订阅响应
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_UnPacketSubscribe(uint8 *rev_data)
{
	
	uint8 result = 255;
	uint16 received_id = (rev_data[2] << 8) | rev_data[3];

	// 检查是否为合法的订阅响应包ID
	if(received_id == MQTT_SUBSCRIBE_ID || 
	   received_id == MQTT_SUBSCRIBE_ID + 1 || 
	   received_id == MQTT_SUBSCRIBE_ID + 2)
	{
		switch(rev_data[4])
		{
			case 0x00:
			case 0x01:
			case 0x02:
				//MQTT Subscribe OK
				result = 0;
			break;
			
			case 0x80:
				//MQTT Subscribe Failed
				result = 1;
			break;
			
			default:
				//MQTT Subscribe UnKnown Err
				result = 2;
			break;
		}
	}
	
	return result;

}

//==========================================================
//	函数名称：	MQTT_PacketUnSubscribe
//
//	函数功能：	打包取消订阅请求
//
//	入口参数：	pkt_id包ID
//				qos服务质量等级
//				topics主题列表
//				topics_cnt主题数量
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_PacketUnSubscribe(uint16 pkt_id, const int8 *topics[], uint8 topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket)
{
	
	uint32 topic_len = 0, remain_len = 0;
	int16 len = 0;
	uint8 i = 0;
	
	if(pkt_id == 0)
		return 1;
	
	//计算主题总长度
	for(; i < topics_cnt; i++)
	{
		if(topics[i] == NULL)
			return 2;
		
		topic_len += strlen(topics[i]);
	}
	
	//2 bytes packet id, 2 bytes topic length + topic + 1 byte reserve---------------------
	remain_len = 2 + (topics_cnt << 1) + topic_len;
	
	//创建缓冲区
	MQTT_NewBuffer(mqttPacket, remain_len + 5);
	if(mqttPacket->_data == NULL)
		return 3;
	
/*************************************MQTT取消订阅请求头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_UNSUBSCRIBE << 4 | 0x02;
	
	//写入剩余长度
	len = MQTT_DumpLength(remain_len, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 4;
	}
	else
		mqttPacket->_len += len;
	
/*************************************有效载荷***********************************************/

	//写入包ID
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	
	//写入主题名称
	for(i = 0; i < topics_cnt; i++)
	{
		topic_len = strlen(topics[i]);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
		
		strncat((int8 *)mqttPacket->_data + mqttPacket->_len, topics[i], topic_len);
		mqttPacket->_len += topic_len;
	}

	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketUnSubscribe
//
//	函数功能：	解包取消订阅响应
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_UnPacketUnSubscribe(uint8 *rev_data)
{
	
	uint1 result = 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_UNSUBSCRIBE_ID) && rev_data[3] == MOSQ_LSB(MQTT_UNSUBSCRIBE_ID))
	{
		result = 0;
	}
	
	return result;

}

//==========================================================
//	函数名称：	MQTT_PacketPublish
//
//	函数功能：	打包发布消息
//
//	入口参数：	pkt_id包ID
//				topic主题
//				payload有效载荷
//				payload_len有效载荷长度
//				qos服务质量等级
//				retain保留标志
//				own标志
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_PacketPublish(uint16 pkt_id, const int8 *topic,
						const int8 *payload, uint32 payload_len,
						enum MqttQosLevel qos, int32 retain, int32 own,
						MQTT_PACKET_STRUCTURE *mqttPacket)
{

	uint32 total_len = 0, topic_len = 0;
	uint32 data_len = 0;
	int32 len = 0;
	uint8 flags = 0;
	
	//pkt_id检查
	if(pkt_id == 0)
		return 1;
	
	//$dp为系统保留主题，不能包含#和+号
	for(topic_len = 0; topic[topic_len] != '\0'; ++topic_len)
	{
		if((topic[topic_len] == '#') || (topic[topic_len] == '+'))
			return 2;
	}
	
	//Publish报文固定报头
	flags |= MQTT_PKT_PUBLISH << 4;
	
	//retain标志
	if(retain)
		flags |= 0x01;
	
	//计算总长度
	total_len = topic_len + payload_len + 2;
	
	//qos设置
	switch(qos)
	{
		case MQTT_QOS_LEVEL0:
			flags |= MQTT_CONNECT_WILL_QOS0;	//取值为0
		break;
		
		case MQTT_QOS_LEVEL1:
			flags |= 0x02;						//取值为1
			total_len += 2;
		break;
		
		case MQTT_QOS_LEVEL2:
			flags |= 0x04;						//取值为2
			total_len += 2;
		break;
		
		default:
		return 3;
	}
	
	//创建缓冲区
	if(payload != NULL)
	{
		if(payload[0] == 2)
		{
			uint32 data_len_t = 0;
			
			while(payload[data_len_t++] != '}');
			data_len_t -= 3;
			data_len = data_len_t + 7;
			data_len_t = payload_len - data_len;
			
			MQTT_NewBuffer(mqttPacket, total_len + 3 - data_len_t);
			
			if(mqttPacket->_data == NULL)
				return 4;
			
			memset(mqttPacket->_data, 0, total_len + 3 - data_len_t);
		}
		else
		{
			MQTT_NewBuffer(mqttPacket, total_len + 5);
			
			if(mqttPacket->_data == NULL)
				return 4;
			
			memset(mqttPacket->_data, 0, total_len + 5);
		}
	}
	else
	{
		MQTT_NewBuffer(mqttPacket, total_len + 5);
		
		if(mqttPacket->_data == NULL)
			return 4;
		
		memset(mqttPacket->_data, 0, total_len + 5);
	}
	
/*************************************MQTT发布消息头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = flags;
	
	//写入剩余长度
	len = MQTT_DumpLength(total_len, mqttPacket->_data + mqttPacket->_len);
	if(len < 0)
	{
		MQTT_DeleteBuffer(mqttPacket);
		return 5;
	}
	else
		mqttPacket->_len += len;
	
/*************************************MQTT发布主题和有效载荷***********************************************/

	//写入主题
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(topic_len);
	mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(topic_len);
	
	strncat((int8 *)mqttPacket->_data + mqttPacket->_len, topic, topic_len);
	mqttPacket->_len += topic_len;
	if(qos != MQTT_QOS_LEVEL0)
	{
		//写入包ID
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_MSB(pkt_id);
		mqttPacket->_data[mqttPacket->_len++] = MOSQ_LSB(pkt_id);
	}
	
	//写入有效载荷
	if(payload != NULL)
	{
		if(payload[0] == 2)
		{
			memcpy((int8 *)mqttPacket->_data + mqttPacket->_len, payload, data_len);
			mqttPacket->_len += data_len;
		}
		else
		{
			memcpy((int8 *)mqttPacket->_data + mqttPacket->_len, payload, payload_len);
			mqttPacket->_len += payload_len;
		}
	}
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketPublish
//
//	函数功能：	解包发布消息
//
//	入口参数：	rev_data接收数据指针
//				topic主题指针
//				topic_len主题长度指针
//				payload有效载荷指针
//				payload_len有效载荷长度指针
//				qos服务质量等级指针
//				pkt_id包ID指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint8 MQTT_UnPacketPublish(uint8 *rev_data, int8 **topic, uint16 *topic_len, int8 **payload, uint16 *payload_len, uint8 *qos, uint16 *pkt_id)
{
	
	const int8 flags = rev_data[0] & 0x0F;
	uint8 *msgPtr;
	uint32 remain_len = 0;

	const int8 dup = flags & 0x08;

	*qos = (flags & 0x06) >> 1;
	
	msgPtr = rev_data + MQTT_ReadLength(rev_data + 1, 4, &remain_len) + 1;
	
	if(remain_len < 2 || flags & 0x01)							//retain
		return 255;
	
	*topic_len = (uint16)msgPtr[0] << 8 | msgPtr[1];
	if(remain_len < *topic_len + 2)
		return 255;
	
	if(strstr((int8 *)msgPtr + 2, CMD_TOPIC_PREFIX) != NULL)	//查找命令主题前缀
		return MQTT_PKT_CMD;
	
	switch(*qos)
	{
		case MQTT_QOS_LEVEL0:									// qos0 have no packet identifier
			
			if(0 != dup)
				return 255;

			*topic = MQTT_MallocBuffer(*topic_len + 1);			//为topic申请缓冲区
			if(*topic == NULL)
				return 255;
			
			memset(*topic, 0, *topic_len + 1);
			memcpy(*topic, (int8 *)msgPtr + 2, *topic_len);		//拷贝topic
			
			*payload_len = remain_len - 2 - *topic_len;			//为payload申请缓冲区
			*payload = MQTT_MallocBuffer(*payload_len + 1);
			if(*payload == NULL)								//申请失败
			{
				MQTT_FreeBuffer(*topic);						//释放已分配的topic缓冲区
				return 255;
			}
			
			memset(*payload, 0, *payload_len + 1);
			memcpy(*payload, (int8 *)msgPtr + 2 + *topic_len, *payload_len);
			
		break;

		case MQTT_QOS_LEVEL1:
		case MQTT_QOS_LEVEL2:
			
			if(*topic_len + 2 > remain_len)
				return 255;
			
			*pkt_id = (uint16)msgPtr[*topic_len + 2] << 8 | msgPtr[*topic_len + 3];
			if(pkt_id == 0)
				return 255;
			
			*topic = MQTT_MallocBuffer(*topic_len + 1);			//为topic申请缓冲区
			if(*topic == NULL)
				return 255;
			
			memset(*topic, 0, *topic_len + 1);
			memcpy(*topic, (int8 *)msgPtr + 2, *topic_len);		//拷贝topic
			
			*payload_len = remain_len - 4 - *topic_len;
			*payload = MQTT_MallocBuffer(*payload_len + 1);		//为payload申请缓冲区
			if(*payload == NULL)								//申请失败
			{
				MQTT_FreeBuffer(*topic);						//释放已分配的topic缓冲区
				return 255;
			}
			
			memset(*payload, 0, *payload_len + 1);
			memcpy(*payload, (int8 *)msgPtr + 4 + *topic_len, *payload_len);
			
		break;

		default:
			return 255;
	}
	
	if(strchr((int8 *)topic, '+') || strchr((int8 *)topic, '#'))
		return 255;

	return 0;

}

//==========================================================
//	函数名称：	MQTT_PacketPublishAck
//
//	函数功能：	打包发布确认
//
//	入口参数：	pkt_id包ID
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		当QoS为1时，发布的消息需要Ack确认
//==========================================================
uint1 MQTT_PacketPublishAck(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************MQTT发布确认头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBACK << 4;
	
	//写入剩余长度
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************MQTT发布确认有效载荷***********************************************/

	//写入包ID
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketPublishAck
//
//	函数功能：	解包发布确认
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_UnPacketPublishAck(uint8 *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}

//==========================================================
//	函数名称：	MQTT_PacketPublishRec
//
//	函数功能：	打包发布接收
//
//	入口参数：	pkt_id包ID
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		当QoS为2时，发布的消息需要接收确认
//==========================================================
uint1 MQTT_PacketPublishRec(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************MQTT发布接收头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREC << 4;
	
	//写入剩余长度
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************MQTT发布接收有效载荷***********************************************/

	//写入包ID
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketPublishRec
//
//	函数功能：	解包发布接收
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_UnPacketPublishRec(uint8 *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}

//==========================================================
//	函数名称：	MQTT_PacketPublishRel
//
//	函数功能：	打包发布释放
//
//	入口参数：	pkt_id包ID
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		当QoS为2时，发布的消息需要释放
//==========================================================
uint1 MQTT_PacketPublishRel(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************MQTT发布释放头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBREL << 4 | 0x02;
	
	//写入剩余长度
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************MQTT发布释放有效载荷***********************************************/

	//写入包ID
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketPublishRel
//
//	函数功能：	解包发布释放
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_UnPacketPublishRel(uint8 *rev_data, uint16 pkt_id)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(pkt_id) && rev_data[3] == MOSQ_LSB(pkt_id))
		return 0;
	else
		return 1;

}

//==========================================================
//	函数名称：	MQTT_PacketPublishComp
//
//	函数功能：	打包发布完成
//
//	入口参数：	pkt_id包ID
//				mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		当QoS为2时，发布的消息需要完成
//==========================================================
uint1 MQTT_PacketPublishComp(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 4);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************MQTT发布完成头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PUBCOMP << 4;
	
	//写入剩余长度
	mqttPacket->_data[mqttPacket->_len++] = 2;
	
/*************************************MQTT发布完成有效载荷***********************************************/

	//写入包ID
	mqttPacket->_data[mqttPacket->_len++] = pkt_id >> 8;
	mqttPacket->_data[mqttPacket->_len++] = pkt_id & 0xff;
	
	return 0;

}

//==========================================================
//	函数名称：	MQTT_UnPacketPublishComp
//
//	函数功能：	解包发布完成
//
//	入口参数：	rev_data接收数据指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_UnPacketPublishComp(uint8 *rev_data)
{

	if(rev_data[1] != 2)
		return 1;

	if(rev_data[2] == MOSQ_MSB(MQTT_PUBLISH_ID) && rev_data[3] == MOSQ_LSB(MQTT_PUBLISH_ID))
		return 0;
	else
		return 1;

}

//==========================================================
//	函数名称：	MQTT_PacketPing
//
//	函数功能：	打包心跳请求
//
//	入口参数：	mqttPacket指针
//
//	返回值：	状态码
//
//	说明：		
//==========================================================
uint1 MQTT_PacketPing(MQTT_PACKET_STRUCTURE *mqttPacket)
{

	MQTT_NewBuffer(mqttPacket, 2);
	if(mqttPacket->_data == NULL)
		return 1;
	
/*************************************MQTT心跳请求头部***********************************************/

	//写入固定报头
	mqttPacket->_data[mqttPacket->_len++] = MQTT_PKT_PINGREQ << 4;
	
	//写入剩余长度
	mqttPacket->_data[mqttPacket->_len++] = 0;
	
	return 0;

}

