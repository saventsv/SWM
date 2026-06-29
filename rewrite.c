#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MAX_WORKSPACES 9

typedef struct Client {
  Window window;
  int x, y, width, height;
  struct Client *next_client;
} Client;

typedef struct {
  Client *master_client;
  int focused_idx;
  int n_clients;
  int id;
} Workspace;

typedef struct {
  Workspace workspaces[MAX_WORKSPACES];
  int current_workspace;
  int running;
  Display *dpy;
} WindowManager;

typedef struct Monitor{
  int monitor_width, monitor_height;
  int usable_width, usable_height;
  Workspace *ws;
  // eventual multi-monitor support
  // struct Monitor *next
} Monitor;

void handle_maprequest(XEvent event);
void handle_configurerequest(XEvent event);
void handle_destorynotify(XEvent event);
void tile(Display *dpy, Workspace *ws);
void calculate_geometry(Workspace *ws);
WindowManager init_wm();
int get_client_idx(Workspace *ws, Client *client);
Client *get_client(Workspace *ws, int idx);
Client *get_client_win(Workspace *ws, Window win);
void swap_clients(Workspace *ws, int idx1, int idx2);
void append_client(Workspace *ws, Client *client);
void insert_client(Workspace *ws, Client *client, int idx);


WindowManager init_wm() {
  WindowManager wm;

  wm.current_workspace = 0;
  wm.running = 1;

  for(int i = 0; i < MAX_WORKSPACES; i++) {
    Workspace workspace;
    workspace.focused_idx = -1;
    workspace.master_client = NULL;
    workspace.n_clients = 0;
    workspace.id = i;

    wm.workspaces[i] = workspace;
  }

  return wm;
}



int main(void) {

  WindowManager WM = init_wm();

  XEvent event;

  if(!(WM.dpy = XOpenDisplay(0x0))) return 1;



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
