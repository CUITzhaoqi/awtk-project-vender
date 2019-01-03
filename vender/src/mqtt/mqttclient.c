/*****************************Copyright(c)*************************************
** 							  By zhaoqi.
**
**                            http://blog.zhaoqi.info
**
**------File Info--------------------------------------------------------------
** File Name:              mqttclient.c
** Latest modified Date:   2018-6-6
** Latest Version:         V1.0
** Description:            mqtt锟酵伙拷锟剿凤拷�?
**
**-----------------------------------------------------------------------------
** Created By:             zhaoqi
** Created date:           2018-6-6
** Version:                V1.0
** Descriptions:           mqtt锟酵伙拷锟剿凤拷�?
**
**-----------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
******************************************************************************/

#include "mqttclient.h"



/**
 * @brief 锟斤拷�?�锟斤拷锟酵伙拷锟剿诧拷锟斤拷录锟斤拷锟斤拷锟斤�?
 */
int8_t MQTTClientInit(__mqtt_handle_t *mqtt_handle)
{
//    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    uint8_t buf[128];
    uint32_t buflen = sizeof(buf);
    uint16_t len;
    uint8_t sessionPresent, connack_rc;
    uint16_t time_out = 100;

    //锟斤拷锟斤拷MQTT锟酵伙拷锟斤拷锟斤拷锟接诧拷锟斤�?
//    connectData.willFlag = 0;
//    //MQTT锟芥�?
//    connectData.MQTTVersion = 4;
//    //锟酵伙拷锟斤拷ID--锟斤拷锟斤拷�?一
//    connectData.clientID.cstring = "MIMITOTO";
//    //锟斤拷锟斤拷锟斤�?
//    connectData.keepAliveInterval = 60;
//    //锟矫伙拷锟斤�?
//    connectData.username.cstring = "zq";
//    //锟矫伙拷锟斤拷锟斤拷
//    connectData.password.cstring = "zhaoqi";
//    //锟斤拷锟斤拷�??
//    connectData.cleansession = 1;

    //锟斤拷锟�?伙拷锟斤拷锟斤拷锟斤拷息
    len = MQTTSerialize_connect(buf, buflen, &(mqtt_handle->connectData));
    //锟斤拷锟斤拷TCP锟斤拷锟斤拷

    if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
        return -1;
    }

    fd_set readfd;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    //锟斤拷锟斤拷锟斤拷息
    FD_ZERO(&readfd);
    FD_SET(mqtt_handle->socket,&readfd);

    select(0, &readfd, NULL, NULL, &tv);
    if(FD_ISSET(mqtt_handle->socket, &readfd) == 0) {
        return -2;
    }
    if(MQTTPacket_read(buf, buflen, mqtt_handle->fpn_recv) != CONNACK)
        return -4;
    //锟斤拷锟斤拷锟斤拷踊锟接︼拷�??
    if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
        return -5;

    if (sessionPresent == 1) {
        return 1;//锟斤拷锟斤拷要锟斤拷锟铰讹拷锟斤�?--锟斤拷锟斤拷锟斤拷锟窖撅拷锟斤拷住锟剿�??伙拷锟剿碉拷状�?
    } else {
        return 0;//锟斤拷�?�锟斤拷锟铰讹拷锟斤�?
    }
}

/**
 * @brief 锟酵伙拷锟剿讹拷锟斤拷锟斤拷锟斤�?
 */
int8_t MQTTSubscribe(__mqtt_handle_t *mqtt_handle, uint8_t *topic, enum QoS pos)
{
    static uint16_t PacketID = 0;
    uint16_t packetidbk = 0;
    int conutbk = 0;
    uint8_t buf[200];
    uint16_t buflen = sizeof(buf);
    MQTTString topicString = MQTTString_initializer;
    uint16_t len;
    int req_qos, qosbk;
    uint8_t time_out = 200;


    //锟斤拷锟斤拷锟斤拷锟斤拷
    topicString.cstring = (char *)topic;
    //锟斤拷锟斤拷锟斤拷锟斤拷
    req_qos = pos;

    //锟斤拷锟�?伙拷锟斤拷锟斤拷锟斤拷息
    len = MQTTSerialize_subscribe(buf, buflen, 0, PacketID++, 1, &topicString, &req_qos);
    //锟斤拷锟斤拷TCP锟斤拷锟斤拷
    if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
        return -1;
    }

    fd_set readfd;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    //锟斤拷锟斤拷锟斤拷息
    FD_ZERO(&readfd);
    FD_SET(mqtt_handle->socket,&readfd);

    select(mqtt_handle->socket+1,&readfd,NULL,NULL,&tv);
    if(FD_ISSET(mqtt_handle->socket,&readfd) == 0) {
        return -2;
    }

    //锟饺达拷锟斤拷锟侥凤拷锟斤拷--�?锟秸碉拷锟斤拷锟侥凤拷锟斤拷
    if(MQTTPacket_read(buf, buflen, mqtt_handle->fpn_recv) != SUBACK) {
        return -4;
    }


    //锟斤拷锟侥伙拷应锟斤�?
    if(MQTTDeserialize_suback(&packetidbk, 1, &conutbk, &qosbk, buf, buflen) != 1) {
        return -5;
    }


    //锟斤拷夥碉拷锟斤拷锟斤拷莸锟斤拷锟饺凤拷�??
    if((qosbk == 0x80) || (packetidbk != (PacketID - 1))) {
        return -6;
    }
    //锟斤拷锟侥成癸拷
    return 0;
}

/**
 * @brief 锟斤拷锟斤拷锟斤拷取MQTT锟斤拷锟斤拷
 */
int32_t ReadPacketTimeout(__mqtt_handle_t *mqtt_handle, uint8_t *buf, int32_t buflen, uint32_t timeout)
{

    fd_set readfd;
    struct timeval tv;
    tv.tv_sec = timeout/1000;
    tv.tv_usec = (timeout%1000) * 1000;
    //锟斤拷锟斤拷锟斤拷息
    FD_ZERO(&readfd);
    FD_SET(mqtt_handle->socket,&readfd);

    select(mqtt_handle->socket+1,&readfd,NULL,NULL,&tv);
    if(FD_ISSET(mqtt_handle->socket,&readfd) == 0) {
        return -1;
    }

    return MQTTPacket_read(buf, buflen, mqtt_handle->fpn_recv);
    //锟斤拷取TCP/IP锟铰硷拷

}

/**
 * @brief 锟饺达拷锟截讹拷锟斤拷锟斤拷锟捷�?�?
 */
int32_t WaitForPacket(__mqtt_handle_t *mqtt_handle, uint8_t packettype, uint8_t times)
{
    int32_t type;
    uint8_t buf[256];
    uint8_t n = 0;
    int32_t buflen = sizeof(buf);
    do
    {
        //锟斤拷取锟斤拷锟捷帮�?
        type = ReadPacketTimeout(mqtt_handle, buf, buflen, 2);
        if(type != -1) {
            mqtt_pktype_ctl(mqtt_handle,type, buf, buflen);
        }
        n++;
    } while((type != packettype) && (n < times));
    //锟秸碉拷锟斤拷锟斤拷锟侥�?�?
    if (type == packettype) {
        return 0;
    }
    return -1;
}

/**
 * @brief 锟矫伙拷锟斤拷锟斤拷锟斤拷息
 */
int32_t MQTTMsgPublish(__mqtt_handle_t *mqtt_handle, 
                        uint8_t *topic, 
                        uint8_t qos, 
                        uint8_t retained, 
                        uint8_t* msg,
                        uint32_t msg_len)
{
    static uint8_t buf[3072] = {0,};
    static uint16_t qospackid = 0;
    uint32_t buflen = sizeof(buf), len;
    MQTTString topicString = MQTTString_initializer;
    uint16_t packid = 0, packetidbk;

    /* 锟斤拷锟斤拷锟斤拷锟?*/
    topicString.cstring = (char *)topic;

    /* 锟斤拷锟斤拷锟斤拷莅锟絀D */
    if ((qos == QOS1) || (qos == QOS2)) {
        //packid = GetNextPackID();
        packid = qospackid++;
    } else {
        qos = QOS0;
        retained = 0;
        packid = 0;
    }

    /* 锟斤拷锟斤拷锟斤拷息 */
    len = MQTTSerialize_publish(buf, buflen, 0, qos, retained, packid, topicString, (unsigned char*)msg, msg_len);
    if (len <= 0) {
        return -1;
    }
    if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
        return -2;
    }

    /* 锟斤拷锟斤拷锟饺硷拷0锟斤拷锟斤拷锟斤拷�?�锟斤拷锟斤�? */
    if (qos == QOS0) {
        return 0;
    }

    /* 锟饺硷拷1 */
    if (qos == QOS1) {
        /* 锟饺达拷PUBACK */
        if(WaitForPacket(mqtt_handle, PUBACK, 5) < 0)
            return -3;
        return 1;

    }

    /* 锟饺硷拷2 */
    if (qos == QOS2) {
        /* 锟饺达拷PUBREC */
        if(WaitForPacket(mqtt_handle, PUBREC, 5) < 0)
            return -3;
        /* 锟斤拷锟斤拷PUBREL */
        len = MQTTSerialize_pubrel(buf, buflen, 0, packetidbk);
        if(len == 0)
            return -4;
        if((*mqtt_handle->fpn_send)(buf, len) < 0)
            return -6;
        /* 锟饺达拷PUBCOMP */
        if(WaitForPacket(mqtt_handle, PUBREC, 5) < 0)
            return -7;
        return 2;
    }
    
    /* 锟饺硷拷锟斤拷锟斤拷 */
    return -8;
}

/**
 * @brief 锟酵伙拷锟斤拷PING锟斤拷锟斤拷
 */
int8_t MQTTPingreq(__mqtt_handle_t *mqtt_handle)
{
    uint16_t len;
    uint8_t buf[256];
    uint16_t buflen = sizeof(buf);
    uint8_t time_out = 200;

    
    len = MQTTSerialize_pingreq(buf, buflen);
    if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
        return -1;
    }

    fd_set readfd;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    FD_ZERO(&readfd);
    FD_SET(mqtt_handle->socket,&readfd);

    select(mqtt_handle->socket+1,&readfd,NULL,NULL,&tv);
    if(FD_ISSET(mqtt_handle->socket,&readfd) == 0) {
        return -2;
    }

    if(MQTTPacket_read(buf, buflen, mqtt_handle->fpn_recv) != PINGRESP) {
        return -3;
    }
    return 0;
}

/**
 * @brief 锟斤拷锟捷帮拷锟斤拷锟酵斤拷锟叫达拷锟斤�?
 */
void mqtt_pktype_ctl(__mqtt_handle_t *mqtt_handle, 
                        uint8_t packtype, 
                        uint8_t *buf, 
                        uint16_t buflen)
{
    uint8_t dup = 0;
    uint32_t qos = 0;
    uint8_t retained = 0;
    uint16_t id = 0;
    uint8_t *payload = NULL;
    uint16_t payloadlen = 0;
    uint8_t type = 0;
    int32_t rc;
    MQTTString receivedTopic;
    int32_t len;
    switch (packtype) {
        
    case PUBLISH:
        /* 锟斤拷锟斤拷PUBLISH锟斤拷息 */
        if(MQTTDeserialize_publish(&dup, (int*)&qos, &retained, &id, &receivedTopic, (unsigned char **)&payload, (int *)&payloadlen, buf, buflen) != 1)
            return;

        /* 锟斤拷锟斤拷锟斤拷息 */
        /* 锟斤拷息锟斤拷锟斤拷锟斤拷同锟斤拷锟斤拷锟斤拷同 */
        if (qos == QOS0) {
            /* QOS0-锟斤拷锟斤拷要ACK
               直锟接达拷锟斤拷锟斤拷锟斤拷      
            */
            if (mqtt_handle->fpn_on_sub_rec != NULL) {
                (*mqtt_handle->fpn_on_sub_rec)(&receivedTopic, payload, payloadlen);
            }
            return ;
        }
        
        /* 锟斤拷锟斤拷PUBACK锟斤拷息 */
        if (qos == QOS1) {
            len = MQTTSerialize_puback(buf, buflen, id);
            if (len == 0) {
                return;
            }       

            /* 锟斤拷锟酵凤拷锟斤拷 */
            if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
                return ;
            }
            return;
        }

        /* 锟斤拷锟斤拷锟斤拷锟斤拷2,�?锟斤拷�?�锟斤拷锟斤拷PUBREC锟酵匡拷锟斤拷锟斤拷 */
        if (qos == QOS2) {
            len = MQTTSerialize_ack(buf, buflen, PUBREC, 0, id);
            if(len == 0)
                return;
            /* 锟斤拷锟酵凤拷锟斤拷 */
            if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
                return ;
            }
        }
        break;
        
    case  PUBREL:
        /* 锟斤拷锟斤拷锟斤拷锟斤拷锟捷ｏ拷锟斤拷锟斤拷锟絀D锟斤拷同锟脚匡拷锟斤�? */
        rc = MQTTDeserialize_ack(&type, &dup, &id, buf, buflen);
        if ((rc != 1) || (type != PUBREL) ) {
            return ;
        } 
        /* 锟斤拷锟�?伙拷PUBCMP锟斤拷息 */
        len = MQTTSerialize_pubcomp(buf, buflen, id);
        if(len == 0) {
            return;
        }      
        /* 锟斤拷锟酵凤拷锟斤拷--PUBCOMP */
        if ((*mqtt_handle->fpn_send)(buf, len) < 0) {
            return ;
        }
        break;
    case   PUBACK:  /* 锟饺硷拷1锟酵伙拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷后，凤拷锟斤拷锟斤拷锟斤拷锟斤拷 */
        break;
    
    case   PUBREC:  /* 锟饺硷拷2锟酵伙拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷后，凤拷锟斤拷锟斤拷锟斤拷锟斤拷 */
        break;
    
    case   PUBCOMP: /* 锟饺硷拷2锟酵伙拷锟斤拷锟斤拷锟斤拷PUBREL锟�?�，凤拷锟斤拷锟斤拷锟斤拷锟斤拷 */
        break;
    
    default:
        break;
    }
}

