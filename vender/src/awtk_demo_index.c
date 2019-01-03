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

#include "awtk.h"
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
#include "ui_loader/ui_builder_default.h"
#include "ext_widgets.h"

#if defined(WIN32)
#include <windows.h>
#endif

extern ret_t demo_loading_init();
extern ret_t awtk_demo_qq_init(void* ctx, event_t* e);
extern ret_t awtk_demo_qq_login(void* ctx, event_t* e);
extern ret_t awtk_demo_sale(void* ctx, event_t* e);


static void open_window(const char* name, widget_t* to_close);

static ret_t on_open_window(void* ctx, event_t* e) {
  window_open((const char*)ctx);
  (void)e;

  return RET_OK;
}


static ret_t install_one(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);

  log_debug("%s\r\n", widget->name);
  if (widget->name != NULL) {
    const char* name = widget->name;
    if (strstr(name, "open:qq_login") != NULL) {
      log_debug("install %s\r\n", name);
      widget_on(widget, EVT_CLICK, awtk_demo_qq_login, (void*)(name + 5));
    } else if (strstr(name, "open:sale") != NULL) {
      log_debug("install %s\r\n", name);
      widget_on(widget, EVT_CLICK, awtk_demo_sale, (void*)(name + 5));
    }
  }
  (void)ctx;

  return RET_OK;
}


static void install_click_hander(widget_t* widget) {
  widget_foreach(widget, install_one, widget);
}


static void open_window(const char* name, widget_t* to_close) {
  widget_t* win = to_close ? window_open_and_close(name, to_close) : window_open(name);

  install_click_hander(win);

  if (tk_str_eq(widget_get_type(win), WIDGET_TYPE_DIALOG)) {
    dialog_modal(win);
  }
}

static ret_t awtk_get_currtime(timer_info_t* timer)
{
  char text[64];
  widget_t* win = WIDGET(timer->ctx);
  widget_t* dis_date = widget_lookup(win, "dis_date", TRUE);

  timer->duration = 1000;

#if defined(WIN32)
  SYSTEMTIME sys;
  GetLocalTime(&sys);
  snprintf(text, sizeof(text), "%02u:%02u:%02u",sys.wHour, sys.wMinute, sys.wSecond);
#endif
  widget_set_text_utf8(dis_date, text);

  return RET_REPEAT;
}

ret_t on_qq_open_window(void* ctx)
{
  log_debug("-----------------------------");
  window_open((const char *)ctx);
}

//uint8_t *label_dis_title = "<font color=&quota;blue&quota; size==&quota;18&quota;>中国移动</font>";
uint8_t *label_dis_title = "<font align=\"center\" color=\"white\" size=\"25\">中国移动</font>";

ret_t awtk_demo_index() 
{
  widget_t *system_bar = window_open("system_bar");
  widget_t *dis_title = widget_lookup(system_bar, "dis_title", TRUE);
  widget_set_text_utf8(dis_title, label_dis_title);

  widget_t *index = window_open("index");

  install_click_hander(index);
  
  // timer_add(awtk_get_currtime, system_bar, 0);
  return RET_OK;
}
