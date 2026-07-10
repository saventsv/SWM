#pragma once
#include "wm.h"

WindowManager init_wm();
MonitorArray init_monitor_arr();
void append_monitor(WindowManager *wm, Monitor *monitor);
void remove_monitor(WindowManager *wm);
Monitor *init_monitor();

