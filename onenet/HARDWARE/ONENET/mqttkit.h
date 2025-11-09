#ifndef _MQTTKIT_H_
#define _MQTTKIT_H_

#include "common.h"
#include "mem_pool.h"


//=============================閰嶇疆==============================
//===========濡傛灉鎻愪緵RTOS鍒欎娇鐢ㄧ郴缁熺殑鍐呭瓨鍒嗛厤锛屽惁鍒欎娇鐢–搴?==========
//RTOS
#include <stdlib.h>

#define MQTT_MallocBuffer	mem_pool_malloc

#define MQTT_FreeBuffer		mem_pool_free
//==========================================================


#define MOSQ_MSB(A)         (uint8)((A & 0xFF00) >> 8)
#define MOSQ_LSB(A)         (uint8)(A & 0x00FF)


/*--------------------------------锟节达拷锟斤拷浞斤拷锟斤拷锟街?-------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8	*_data;		//协锟斤拷锟斤拷锟斤拷
	
	uint32	_len;		//写锟斤拷锟斤拷锟斤拷莩锟斤拷锟?
	
	uint32	_size;		//锟斤拷锟斤拷锟杰达拷小
	
	uint8	_memFlag;	//锟节达拷使锟矫的凤拷锟斤拷锟斤拷0-未锟斤拷锟斤拷	1-使锟矫的讹拷态锟斤拷锟斤拷		2-使锟矫的固讹拷锟节达拷
	
} MQTT_PACKET_STRUCTURE;


/*--------------------------------锟教讹拷头锟斤拷锟斤拷息锟斤拷锟斤拷--------------------------------*/
enum MqttPacketType
{
	
    MQTT_PKT_CONNECT = 1, /**< 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_CONNACK,     /**< 锟斤拷锟斤拷确锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_PUBLISH,     /**< 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_PUBACK,      /**< 锟斤拷锟斤拷确锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_PUBREC,      /**< 锟斤拷锟斤拷锟斤拷锟斤拷锟窖斤拷锟斤拷锟斤拷锟捷帮拷锟斤拷Qos 2时锟斤拷锟截革拷MQTT_PKT_PUBLISH */
    MQTT_PKT_PUBREL,      /**< 锟斤拷锟斤拷锟斤拷锟斤拷锟酵凤拷锟斤拷锟捷帮拷锟斤拷 Qos 2时锟斤拷锟截革拷MQTT_PKT_PUBREC */
    MQTT_PKT_PUBCOMP,     /**< 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷莅锟斤拷锟?Qos 2时锟斤拷锟截革拷MQTT_PKT_PUBREL */
    MQTT_PKT_SUBSCRIBE,   /**< 锟斤拷锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_SUBACK,      /**< 锟斤拷锟斤拷确锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_UNSUBSCRIBE, /**< 取锟斤拷锟斤拷锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_UNSUBACK,    /**< 取锟斤拷锟斤拷锟斤拷确锟斤拷锟斤拷锟捷帮拷 */
    MQTT_PKT_PINGREQ,     /**< ping 锟斤拷锟捷帮拷 */
    MQTT_PKT_PINGRESP,    /**< ping 锟斤拷应锟斤拷锟捷帮拷 */
    MQTT_PKT_DISCONNECT,  /**< 锟较匡拷锟斤拷锟斤拷锟斤拷锟捷帮拷 */
	
	//锟斤拷锟斤拷
	
	MQTT_PKT_CMD  		 /**< 锟斤拷锟斤拷锟铰凤拷锟斤拷锟捷帮拷 */
	
};


/*--------------------------------MQTT QOS锟饺硷拷--------------------------------*/
enum MqttQosLevel
{
	
    MQTT_QOS_LEVEL0,  /**< 锟斤拷喾拷锟揭伙拷锟?*/
    MQTT_QOS_LEVEL1,  /**< 锟斤拷锟劫凤拷锟斤拷一锟斤拷  */
    MQTT_QOS_LEVEL2   /**< 只锟斤拷锟斤拷一锟斤拷 */
	
};


/*--------------------------------MQTT 锟斤拷锟斤拷锟斤拷锟斤拷锟街疚伙拷锟斤拷诓锟绞癸拷锟?-------------------------------*/
enum MqttConnectFlag
{
	
    MQTT_CONNECT_CLEAN_SESSION  = 0x02,
    MQTT_CONNECT_WILL_FLAG      = 0x04,
    MQTT_CONNECT_WILL_QOS0      = 0x00,
    MQTT_CONNECT_WILL_QOS1      = 0x08,
    MQTT_CONNECT_WILL_QOS2      = 0x10,
    MQTT_CONNECT_WILL_RETAIN    = 0x20,
    MQTT_CONNECT_PASSORD        = 0x40,
    MQTT_CONNECT_USER_NAME      = 0x80
	
};


/*--------------------------------锟斤拷息锟斤拷packet ID锟斤拷锟斤拷锟皆讹拷锟斤拷--------------------------------*/
#define MQTT_PUBLISH_ID			10

#define MQTT_SUBSCRIBE_ID		20

#define MQTT_UNSUBSCRIBE_ID		30


/*--------------------------------删锟斤拷--------------------------------*/
void MQTT_DeleteBuffer(MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟?-------------------------------*/
uint8 MQTT_UnPacketRecv(uint8 *dataPtr);

/*--------------------------------锟斤拷录锟斤拷锟?-------------------------------*/
uint8 MQTT_PacketConnect(const int8 *user, const int8 *password, const int8 *devid,
						uint16 cTime, uint1 clean_session, uint1 qos,
						const int8 *will_topic, const int8 *will_msg, int32 will_retain,
						MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟较匡拷锟斤拷锟斤拷锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketDisConnect(MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷应锟斤拷锟?-------------------------------*/
uint8 MQTT_UnPacketConnectAck(uint8 *rev_data);

/*--------------------------------锟斤拷锟捷碉拷锟较达拷锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketSaveData(const int8 *pro_id, const char *dev_name,
								int16 send_len, int8 *type_bin_head, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷锟侥硷拷锟较达拷锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketSaveBinData(const int8 *name, int16 file_len, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟铰凤拷锟斤拷锟?-------------------------------*/
uint8 MQTT_UnPacketCmd(uint8 *rev_data, int8 **cmdid, int8 **req, uint16 *req_len);

/*--------------------------------锟斤拷锟斤拷馗锟斤拷锟斤拷--------------------------------*/
uint1 MQTT_PacketCmdResp(const int8 *cmdid, const int8 *req, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟?-------------------------------*/
uint8 MQTT_PacketSubscribe(uint16 pkt_id, enum MqttQosLevel qos, const int8 *topics[], uint8 topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷锟斤拷馗锟斤拷锟斤拷--------------------------------*/
uint8 MQTT_UnPacketSubscribe(uint8 *rev_data);

/*--------------------------------取锟斤拷锟斤拷锟斤拷锟斤拷锟?-------------------------------*/
uint8 MQTT_PacketUnSubscribe(uint16 pkt_id, const int8 *topics[], uint8 topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------取锟斤拷锟斤拷锟侥回革拷锟斤拷锟?-------------------------------*/
uint1 MQTT_UnPacketUnSubscribe(uint8 *rev_data);

/*--------------------------------锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟?-------------------------------*/
uint8 MQTT_PacketPublish(uint16 pkt_id, const int8 *topic,
						const int8 *payload, uint32 payload_len,
						enum MqttQosLevel qos, int32 retain, int32 own,
						MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟截革拷锟斤拷锟?-------------------------------*/
uint8 MQTT_UnPacketPublish(uint8 *rev_data, int8 **topic, uint16 *topic_len, int8 **payload, uint16 *payload_len, uint8 *qos, uint16 *pkt_id);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Ack锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketPublishAck(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Ack锟斤拷锟?-------------------------------*/
uint1 MQTT_UnPacketPublishAck(uint8 *rev_data);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Rec锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketPublishRec(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Rec锟斤拷锟?-------------------------------*/
uint1 MQTT_UnPacketPublishRec(uint8 *rev_data);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Rel锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketPublishRel(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Rel锟斤拷锟?-------------------------------*/
uint1 MQTT_UnPacketPublishRel(uint8 *rev_data, uint16 pkt_id);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Comp锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketPublishComp(uint16 pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------锟斤拷锟斤拷锟斤拷息锟斤拷Comp锟斤拷锟?-------------------------------*/
uint1 MQTT_UnPacketPublishComp(uint8 *rev_data);

/*--------------------------------锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟?-------------------------------*/
uint1 MQTT_PacketPing(MQTT_PACKET_STRUCTURE *mqttPacket);


#endif

