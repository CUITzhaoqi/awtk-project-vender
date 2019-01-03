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
#include "qr/qr.h"
#include "cJSON.h"

ret_t demo_qr_timer_cb(const timer_info_t* timer)
{
  widget_t* win = WIDGET(timer->ctx);
  widget_t *l_back_time = widget_lookup(win, "back_time", TRUE);
  static uint8_t timer_count = 0;
  timer_count ++;
  if(timer_count == 60) {
    widget_destroy(win);
    return RET_REMOVE;
  } else if(timer_count == 50) {
    widget_use_style(l_back_time, "count_red");
  }
  char back_time[32] = {0,};
  tk_snprintf(back_time, sizeof(back_time), "%d", 60 - timer_count);
  widget_set_text_utf8(l_back_time, back_time);
  
  return RET_REPEAT;
}


ret_t awtk_demo_sale_select_on(void* ctx, event_t* e)
{
  log_debug("fun is:%s line is:%d\r\n", __func__, __LINE__);
  widget_t *sale_ok = window_open("sale_ok");

  widget_t *qr_view = widget_lookup(sale_ok, "qr_view", TRUE);

  widget_t *new_qr_code = qr_create(qr_view, 0, 0, 0, 0);
  widget_use_style(new_qr_code, "qr_border");
  qr_set_pixsize(new_qr_code, 3);
  qr_set_text(new_qr_code, "致远电子");
  widget_set_self_layout_params(new_qr_code, "0", "0", "100%", "100%");
  widget_set_name(new_qr_code, "new_qr_code");

  widget_t *l_back_time = widget_lookup(sale_ok, "back_time", TRUE);
  widget_set_text_utf8(l_back_time, "60");
  timer_add(demo_qr_timer_cb, sale_ok, 1000);

  return RET_OK;
}

ret_t awtk_demo_sale_image_clk_on(void* ctx, event_t* e)
{
  image_t* image = IMAGE((widget_t *)ctx);

  log_debug("fun is:%s line is:%d\r\n", __func__, __LINE__);

  return RET_OK;
}

ret_t awtk_demo_sale(void* ctx, event_t* e) 
{
  widget_factory_register(widget_factory(), WIDGET_TYPE_QR, qr_create);

  widget_t *sale = window_open((const char *)ctx);

  widget_t *b_sale_ok = widget_lookup(sale, "sale_ok", TRUE);
  if (b_sale_ok != NULL) {
    widget_on(b_sale_ok, EVT_CLICK, awtk_demo_sale_select_on, (void *)b_sale_ok);
  }

  widget_t *i_kele = widget_lookup(sale, "i_kele", TRUE);
  if (i_kele != NULL) {
    widget_on(i_kele, EVT_POINTER_UP, awtk_demo_sale_image_clk_on, (void *)i_kele);
  }

  widget_t *l_kele = widget_lookup(sale, "l_kele", TRUE);
  if (l_kele != NULL) {
    widget_set_text_utf8(l_kele, "阔乐");
  }

  /* 拿到 view*/
  widget_t *v_sale_layout = widget_lookup(sale, "v_sale_layout", TRUE);
  if (v_sale_layout != NULL) {
    widget_t *new_sale_view = view_create(v_sale_layout, 0, 0, 0, 0);
    
    /* 创建图片 */
    widget_t *new_sale_image = image_create(new_sale_view, 0, 0, 0, 0);
    /* 设置图片的绘制类型 */
    image_set_draw_type(new_sale_image, IMAGE_DRAW_ICON);
    /* 设置图片是否可以点击 */
    image_set_selectable(new_sale_image, TRUE);
    /* 设置图片的style */
    widget_use_style(new_sale_image, "selectable");
    /* 设置图片 */
    image_set_image(new_sale_image, "kele");
    // x="c" y="0" w="100%" h="100%"
    /* 设置图片的布局参数 */
    widget_set_self_layout_params(new_sale_image, "c", "0", "100%", "100%");
    /* 设置图片的name, 方便以后可以拿到这个空间 */
    widget_set_name(new_sale_image, "new_test_lable");

    /* 创建图片 */
    widget_t *new_sale_lable = label_create(new_sale_view, 0, 0, 0, 0);
    widget_set_text_utf8(new_sale_lable, "可乐");
    //x="c" y="m:55" w="100%" h="4%"
    widget_set_self_layout_params(new_sale_lable, "c", "m:55", "100%", "4%");


  }

  return RET_OK;
}
