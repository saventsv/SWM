#include "../include/window_manager.h"
#include "../include/wm.h"
#include "../include/client.h"
#include <stdlib.h>

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

  wm.n_monitors = n_monitors;

  wm.monitors = malloc(n_monitors * sizeof(Monitor));

  if(wm.monitors == NULL) abort();

  for(int i = 0; i < n_monitors; i++) {
    Monitor monitor;
    monitor.monitor_x = monitors[i].x;
    monitor.monitor_y = monitors[i].y;
    monitor.monitor_width = monitors[i].width;
    monitor.monitor_height = monitors[i].height;
    // Temporarilly add these until I get status bar detection
    monitor.usable_width = monitors[i].width;
    monitor.usable_height = monitors[i].height;

    wm.monitors[i] = monitor;
  }

  return wm;
}

