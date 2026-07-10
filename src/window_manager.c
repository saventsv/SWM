#include "../include/window_manager.h"
#include "../include/wm.h"
#include "../include/client.h"
#include <stdlib.h>
#include <stdio.h>

MonitorArray init_monitor_arr() {
  MonitorArray monitor_arr;

  monitor_arr.capacity = 0;
  monitor_arr.size = 0;
  monitor_arr.data = malloc(monitor_arr.capacity * sizeof(Monitor*));

  if(monitor_arr.data == NULL) abort();

  return monitor_arr;
}

void append_monitor(WindowManager *wm, Monitor *monitor) {
  if(wm->monitors.size == wm->monitors.capacity) {
    wm->monitors.data = realloc(wm->monitors.data, wm->monitors.capacity * 2 * sizeof(Client*));

    if(wm->monitors.data == NULL) abort();

    wm->monitors.capacity*=2;
  } 
  wm->monitors.data[wm->monitors.size] = monitor;
  wm->monitors.size++;
}

void remove_monitor(WindowManager *wm) {

  Monitor *monitor = wm->monitors.data[wm->monitors.size];

  free(monitor);

  wm->monitors.data[wm->monitors.size] = NULL;

  wm->monitors.size--;

  return;
}



WindowManager init_wm() {
  WindowManager wm;

  wm.current_workspace = 0;
  wm.running = 1;

  if(!(wm.dpy = XOpenDisplay(0x0))) abort();

  for(int i = 0; i < MAX_WORKSPACES; i++) {
    Workspace workspace;
    workspace.focused_idx = -1;
    workspace.clients = init_client_arr();
    workspace.id = i;

    wm.workspaces[i] = workspace;
  }

  int n_monitors;

  XRRMonitorInfo *monitors = XRRGetMonitors(
      wm.dpy, 
      DefaultRootWindow(wm.dpy), 
      True,
      &n_monitors);


  wm.monitors = init_monitor_arr();

  for(int i = 0; i < n_monitors; i++) {

    Monitor monitor;
    monitor.monitor_x = monitors[i].x;
    monitor.monitor_y = monitors[i].y;
    monitor.monitor_width = monitors[i].width;
    monitor.monitor_height = monitors[i].height;
    // Temporarilly add these until I get status bar detection
    monitor.usable_width = monitors[i].width;
    monitor.usable_height = monitors[i].height;

    append_monitor(&wm, &monitor);
    wm.monitors.data[i] = &monitor;
  }

  return wm;
}

