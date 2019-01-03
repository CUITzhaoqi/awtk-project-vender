#ifndef _MQTTCLIENT_H__
#define _MQTTCLIENT_H__

#include <stdint.h>
#include <windows.h>
#include "base/types_def.h"

#include "MQTTPacket.h"

enum QoS { 
    QOS0 = 0, 
    QOS1 = 1, 
    QOS2 = 2 
};

enum MQTT_STATUS {
    MQ_NOCONNECT  = 0,
    MQ_CONNECTING  = 1,
    MQ_CONNECTED   = 2,
    MQ_CLOSE       = 3,
};

typedef struct __mqtt_sub_topic{
    uint8_t     topic[64];
    uint8_t     qos;
} __mqtt_sub_topic_t;

typedef struct __mqtt_pub_topic{
    uint8_t     topic[64];
    uint8_t     qos;
    uint16_t    len;
    uint8_t     *payload;
} __mqtt_pub_topic_t;
 
typedef struct __mqtt_handle {
    SOCKET                 socket;
    uint8_t                 mqtt_status;
    uint8_t                 hostname[64];
    uint16_t                port;

    MQTTPacket_connectData  connectData;
    
    SOCKET                  (*fpn_open)(unsigned char *hostname, int port);
    int                     (*fpn_send)(unsigned char *buf, int len);
    int                     (*fpn_recv)(unsigned char *buf, int len);
    int8_t                  (*fpn_close)();
    
    void                    (*fpn_on_connect_success)(void *p_arg);
    void                    (*fpn_on_connect_faild)(void *p_arg);
    void                    (*fpn_on_pub_success)(void *p_arg);
    void                    (*fpn_on_pub_faild)(void *p_arg);
    void                    (*fpn_on_sub_success)(void *p_arg);
    void                    (*fpn_on_sub_faild)(void *p_arg);
    
    void                    (*fpn_on_sub_rec)(void *p_topic, void *p_data, uint16_t len);
} __mqtt_handle_t;


/************************************************************************
** ��������: MQTTClientInit								
** ��������: ��ʼ���ͻ��˲���¼������
** ��ڲ���: int32_t sock:����������
** ���ڲ���: >=0:���ͳɹ� <0:����ʧ��
** ��    ע: 
************************************************************************/
int8_t MQTTClientInit(__mqtt_handle_t *mqtt_handle);

int8_t MQTTSubscribe(__mqtt_handle_t *mqtt_handle, uint8_t *topic, enum QoS pos);

int8_t MQTTPingreq(__mqtt_handle_t *mqtt_handle);

void mqtt_pktype_ctl(__mqtt_handle_t *mqtt_handle, uint8_t packtype, uint8_t *buf, uint16_t buflen);
int32_t MQTTMsgPublish(__mqtt_handle_t *mqtt_handle, uint8_t *topic, uint8_t qos, uint8_t retained, uint8_t* msg, uint32_t msg_len);
#endif


