/**
 * File:   demo1_app.c
 * Author: AWTK Develop Team
 * Brief:  basic class of all widget
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-02-16 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "base/types_def.h"
#include "tkc/mem.h"
#include "widgets/label.h"
#include "base/timer.h"
#include "widgets/button.h"
#include "widgets/dialog.h"
#include "widgets/image.h"
#include "tkc/utils.h"
#include "widgets/window.h"
#include "tkc/time_now.h"
#include "base/main_loop.h"
#include "base/locale_info.h"
#include "widgets/check_button.h"
#include "widgets/progress_bar.h"
#include "base/image_manager.h"
#include "base/window_manager.h"
#include "base/widget_factory.h"
#include "widgets/view.h"
#include "ui_loader/ui_builder_default.h"
#include "ext_widgets.h"
#include "scroll_view/list_item.h"
#include "scroll_view/scroll_bar.h"
#include "cJSON.h"

#include "mqttclient.h"
#include "transport.h"

#define PORT (u_short)44965  
#define DEST_IP_ADDR "192.168.0.7"
#define DEFAULT_ITEM_HIGH  "70"

__mqtt_handle_t _g_mqtt_handle;

typedef struct _qq_msg_t {
  uint32_t  timestamp;
  char      msgid[32];
  char      msg_payload[1024];
  char      group_name[64];
} qq_msg_t;
ret_t awtk_demo_qq_create_message(qq_msg_t *msg_payload);
static widget_t *qq = NULL;

#define MQTT_BUF_SIZE  2048
static uint8_t mqtt_buf[MQTT_BUF_SIZE];

/**
 * @brief 收到订阅消息回调
 */
static void on_sub_rec(void *p_topic, void *p_data, uint16_t len)
{
    MQTTString* topic = p_topic;
    char rectopic[128] = {0,};
    memcpy(rectopic, topic->lenstring.data, topic->lenstring.len);
    log_debug("+topic:%s,payload:%d\r\n", rectopic, len);
    log_debug("%s", (char *)p_data); 
    log_debug("message arrived %d: %s\r\n", len, p_data);

    cJSON * root = NULL;
    cJSON * item = NULL;
    root = cJSON_Parse((char *)p_data);     
    if (!root)  {
      log_debug("Error before: [%s]\n",cJSON_GetErrorPtr());
    } else {
      qq_msg_t *message = TKMEM_ALLOC(sizeof(qq_msg_t));
      item = cJSON_GetObjectItem(root, "msg_payload");
      if (item != NULL) {
        memcpy(message->msg_payload, item->valuestring, strlen(item->valuestring));
      }
      item = cJSON_GetObjectItem(root, "group_name");
      if (item != NULL) {
        memcpy(message->group_name, item->valuestring, strlen(item->valuestring));
      }
      awtk_demo_qq_create_message(message);
      TKMEM_FREE(message);
      cJSON_Delete(root);
    }
    
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
  log_debug("sub thread started\n");
  int8_t sessionPresent = 0;
  uint16_t count = 0;
  uint16_t buflen = sizeof(mqtt_buf);
  int16_t type, ret;

  MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
  //Get_ChipInfo();
  connectData.clientID.cstring = (char *)"dtu";
  connectData.username.cstring = NULL;
  connectData.password.cstring = NULL;
  connectData.keepAliveInterval = 60;

  memset(&_g_mqtt_handle.connectData, 0, sizeof(MQTTPacket_connectData));
  memcpy(&_g_mqtt_handle.connectData, &connectData, sizeof(MQTTPacket_connectData));

  _g_mqtt_handle.mqtt_status = MQ_NOCONNECT;
  _g_mqtt_handle.fpn_open  = transport_open;
  _g_mqtt_handle.fpn_close = transport_close;
  _g_mqtt_handle.fpn_send  = transport_sendPacketBuffer;
  _g_mqtt_handle.fpn_recv  = transport_getdata;
  _g_mqtt_handle.fpn_on_sub_rec = on_sub_rec;
MQTT_START:
  while(1)
  {
      _g_mqtt_handle.mqtt_status = MQ_CONNECTING;
      //连接服务器
      _g_mqtt_handle.socket = (*_g_mqtt_handle.fpn_open)("132.232.66.149", 1883);
      //如果连接服务器成功
      if (_g_mqtt_handle.socket >= 0) {
          log_debug("MQTT>>Connect Server Success...\n");
          _g_mqtt_handle.mqtt_status = MQ_CONNECTED;
          break;
      } else {
          log_debug("MQTT>>Connect Server Failed...\n");
          _g_mqtt_handle.mqtt_status = MQ_NOCONNECT;
      }

      //休息3秒
      Sleep(3000);
  }

  sessionPresent = MQTTClientInit(&_g_mqtt_handle);

  if(sessionPresent < 0)
  {
      _g_mqtt_handle.mqtt_status = MQ_NOCONNECT;
      goto MQTT_reconnect;
  }

  if (_g_mqtt_handle.fpn_on_connect_success != NULL) {
      (*_g_mqtt_handle.fpn_on_connect_success)(&_g_mqtt_handle);
  }
  if(MQTTPingreq(&_g_mqtt_handle) == 0 ) {
    log_debug("MQTT>>PingSuccess...\r\n");
  } else {
    log_debug("MQTT>>PingFailed...\r\n");
    goto MQTT_reconnect;
  }
  MQTTSubscribe(&_g_mqtt_handle, "test", QOS0);
  while(1) {
	  count++;
    fd_set readfd;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 20 * 1000;
    //推送消息
    FD_ZERO(&readfd);
    FD_SET(_g_mqtt_handle.socket,&readfd);

    select(0, &readfd, NULL, NULL, &tv);
    if(FD_ISSET(_g_mqtt_handle.socket, &readfd) != 0) {
      type = MQTTPacket_read(mqtt_buf, buflen, _g_mqtt_handle.fpn_recv);
      if (type != -1) {
          mqtt_pktype_ctl(&_g_mqtt_handle, type, mqtt_buf, buflen);
          /* 清空缓冲区 */
          memset(mqtt_buf, 0, MQTT_BUF_SIZE);
      }
    }
    if (count == _g_mqtt_handle.connectData.keepAliveInterval * 20) {
      count = 0;
      if(MQTTPingreq(&_g_mqtt_handle) == 0 ) {
        log_debug("MQTT>>PingSuccess...\r\n");
      } else {
        log_debug("MQTT>>PingFailed...\r\n");

        goto MQTT_reconnect;
      }
    }
  }
MQTT_reconnect:
    (*_g_mqtt_handle.fpn_close)();
//重新链接服务器
    goto MQTT_START;  
  return 0;
}

ret_t awtk_demo_qq_create_message(qq_msg_t *msg_payload)
{
  widget_t *msg = widget_lookup(qq, "msg", TRUE);

  widget_t *qq_pages = widget_lookup(qq, "qq_pages", TRUE);
  if (qq_pages == NULL) {
    log_debug("---------qq_pages NULL---------------------------------------\r\n");
    return RET_FAIL;
  }

  widget_t *qq_msg = widget_lookup(qq_pages, "qq_msg", TRUE);
  if (qq_msg == NULL) {
    log_debug("---------qq_msg NULL---------------------------------------\r\n");
    return RET_FAIL;
  }
 
  widget_t *qq_view = widget_lookup(qq_msg, "qq_view", TRUE);
  
  if (qq_view != NULL) {
    log_debug("---------qq_view NOT NULL---------------------------------------\r\n");

    int32_t child_count = widget_count_children(qq_view);

	  widget_t *new_list_item = list_item_create(qq_view, 0, 0, 32, 32);
    
    if (new_list_item != NULL) {
      log_debug("---------new_list_item NOT NULL---------------------------------------\r\n");
      
      /* 对新的 list_item 进行设置 */
      widget_use_style(new_list_item, "odd_clickable");
      char new_item_offset[16] = {0,};
      sprintf(new_item_offset, "%d", child_count * atoi(DEFAULT_ITEM_HIGH));
      widget_set_self_layout_params(new_list_item, "0", new_item_offset, "100%", "70");
      widget_layout(new_list_item);

      /* 创建一个新的view */
      widget_t *new_view1 = view_create(new_list_item, 0, 0, 0, 0);
      widget_set_self_layout_params(new_view1, "5", "5", "5%", "50");
      widget_layout(new_view1);

      widget_t *new_img1 = image_create(new_view1, 0, 0, 32, 32);
      widget_set_self_layout_params(new_img1, "5", "0", "100%", "50");
      widget_layout(new_img1);
      image_set_draw_type(new_img1, IMAGE_DRAW_ICON);
      image_set_image(new_img1, "a");

      widget_t *new_view2 = view_create(new_list_item, 0, 0, 0, 0);
      widget_set_self_layout_params(new_view2, "c", "0", "85%", "50");
      widget_layout(new_view2);

      widget_t *new_lab1 = label_create(new_view2, 0,0,0,0);
      widget_use_style(new_lab1, "qq_main_black");
      widget_set_self_layout_params(new_lab1, "r", "m:9", "100%", "50");
      widget_layout(new_lab1);
      widget_set_text_utf8(new_lab1, msg_payload->group_name);

      widget_t *new_lab2 = label_create(new_view2, 0,0,0,0);
      widget_use_style(new_lab2, "qq_black");
      widget_set_self_layout_params(new_lab2, "r", "m:17", "100%", "20");
      widget_layout(new_lab2);
      widget_set_text_utf8(new_lab2, msg_payload->msg_payload);

      widget_t *new_view3 = view_create(new_list_item, 0, 0, 0, 0);
      widget_set_self_layout_params(new_view3, "r", "0", "10%", "50");
      widget_layout(new_view3);

      widget_t *new_lab3 = label_create(new_view3, 0,0,0,0);
      widget_use_style(new_lab3, "qq_black");
      widget_set_self_layout_params(new_lab3, "r:10", "m:-13", "60", "20");
      widget_layout(new_lab3);
      widget_set_text_utf8(new_lab3, "12:34");

      widget_t *new_img2 = image_create(new_view3, 0, 0, 32, 32);
      widget_set_self_layout_params(new_img2, "r:10", "m:11", "60", "20");
      widget_layout(new_img2);
      image_set_draw_type(new_img2, IMAGE_DRAW_ICON);
      image_set_image(new_img2, "red_circle");

      widget_t *new_lab4 = label_create(new_view3, 0,0,0,0);
      widget_use_style(new_lab4, "qq_num_index");
      widget_set_self_layout_params(new_lab4, "r:10", "m:11", "60", "20");
      widget_layout(new_lab4);
      widget_set_text_utf8(new_lab4, "1");

      // widget_t *qq_scroll_bar = widget_lookup(qq, "qq_scroll_bar", TRUE);
      // value_t v;
      // widget_get_prop(qq_scroll_bar, WIDGET_PROP_MAX, &v);
      // uint16_t value = v.value.u16;
      // value = value + atoi(DEFAULT_ITEM_HIGH);
      // v.value.u16 = value;
      // widget_set_prop(qq_scroll_bar, WIDGET_PROP_MAX, &v);
      // scroll_bar_scroll_delta(qq_scroll_bar, atoi(DEFAULT_ITEM_HIGH));
    }
    
  }
}

ret_t awtk_demo_qq_init(void* ctx, event_t* e) 
{
  qq = window_open((const char *)ctx);
  log_debug("------------------------------------------------\r\n");
  /* 为底部标题栏设置国际化 */
  widget_t *msg = widget_lookup(qq, "msg", TRUE);
  widget_set_tr_text(msg, "Message");

  widget_t *contact = widget_lookup(qq, "contact", TRUE);
  widget_set_tr_text(contact, "Contact");

  widget_t *discovery = widget_lookup(qq, "discovery", TRUE);
  widget_set_tr_text(discovery, "Discovery");

  widget_t *me = widget_lookup(qq, "me", TRUE);
  widget_set_tr_text(me, "Me");

  widget_t *device = widget_lookup(qq, "device", TRUE);
  widget_set_tr_text(device, "Device");

  qq_msg_t meaasge = {
    .timestamp = 123456,
    .msg_payload = "您有新消息啦",
    .group_name = "AWTK使用交流",
  };

  awtk_demo_qq_create_message(&meaasge);

  // cJSON * root =  cJSON_CreateObject();
  // cJSON * item =  cJSON_CreateObject();
  // cJSON * next =  cJSON_CreateObject();

  // cJSON_AddItemToObject(root, "rc", cJSON_CreateNumber(0));//根节点下添加
  // cJSON_AddItemToObject(root, "operation", cJSON_CreateString("CALL"));
  // cJSON_AddItemToObject(root, "service", cJSON_CreateString("telephone"));
  // cJSON_AddItemToObject(root, "text", cJSON_CreateString("打电话给张三"));
  // cJSON_AddItemToObject(root, "semantic", item);//root节点下添加semantic节点
  // cJSON_AddItemToObject(item, "slots", next);//semantic节点下添加item节点
  // cJSON_AddItemToObject(next, "name", cJSON_CreateString("张三"));//添加name节点

  // log_debug("%s\n", cJSON_Print(root));

  /* 创建MQTT 线程 */
#if defined(WIN32)
  DWORD threadID;
  HANDLE hThread;
  hThread = CreateThread(NULL,0,ThreadProc,NULL,0,&threadID); 
#endif

  return RET_OK;
}
