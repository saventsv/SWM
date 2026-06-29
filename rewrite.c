#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>
#include <stdlib.h>


#define MAX_WORKSPACES 9

typedef struct {
  Window window;
  int x, y, width, height;
} Client;


typedef struct {
  Client **data;
  size_t capacity;
  size_t size;
} ClientArray;

typedef struct {
  ClientArray clients;
  int focused_idx;
  int n_clients;
  int id;
} Workspace;

typedef struct Monitor{
  int monitor_width, monitor_height;
  int monitor_x, monitor_y;
  // eventually used for handling reserving space for status bars
  int usable_width, usable_height;
  int usable_x, usable_y;
  Workspace *ws;
  // eventual multi-monitor support
  // struct Monitor *next
} Monitor;

typedef struct {
  Workspace workspaces[MAX_WORKSPACES];
  int current_workspace;
  int running;
  Monitor *monitors;
  int n_monitors;
  Display *dpy;
} WindowManager;


void handle_maprequest(XEvent event);
void handle_configurerequest(XEvent event);
void handle_destorynotify(XEvent event);
void tile(Display *dpy, Workspace *ws, Monitor *monitor);
void calculate_geometry(Workspace *ws, Monitor *monitor);
WindowManager init_wm();
Client *get_client_win(Workspace *ws, Window win);
void swap_clients(Workspace *ws, int idx1, int idx2);
void append_client(Workspace *ws, Client *client);
void insert_client(Workspace *ws, Client *client, int idx);
ClientArray init_client_arr();


WindowManager init_wm() {
  WindowManager wm;

  wm.current_workspace = 0;
  wm.running = 1;

  if(!(wm.dpy = XOpenDisplay(0x0))) abort();

  for(int i = 0; i < MAX_WORKSPACES; i++) {
    Workspace workspace;
    workspace.focused_idx = -1;
    workspace.clients = init_client_arr();
    workspace.n_clients = 0;
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

    wm.monitors[i] = monitor;
  }

  return wm;
}

ClientArray init_client_arr() {
  ClientArray client_arr;
  client_arr.capacity = 4;
  client_arr.size = 0;

  client_arr.data = malloc(client_arr.capacity * sizeof(Client));

  if(client_arr.data == NULL) abort();

  return client_arr;
}



int main(void) {

  WindowManager WM = init_wm();

  XEvent event;




  XSelectInput(WM.dpy, 
      DefaultRootWindow(WM.dpy), 
      SubstructureNotifyMask | SubstructureRedirectMask
  );

  while(WM.running)  {
    XFlush(WM.dpy);
    XNextEvent(WM.dpy, &event);

    switch(event.type) {
      case MapRequest:
        {
          handle_maprequest(event);
        }

      case ConfigureRequest:
        {
          handle_configurerequest(event);
        }
      case DestroyNotify:
        {
          handle_destorynotify(event);
        }
      // case UnmapNotify:
      //   {
      //
      //   }
      // case FocusIn:
      //   {
      //
      //   }
      // case FocusOut:
      //   {
      //
      //   }
      // case ClientMessage:
      //   {
      //
      //   }
    }
  }


  return 0;
}
