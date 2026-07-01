#include "../include/events.h"
#include "../include/client.h"
#include "../include/layout.h"
#include "../include/wm.h"
#include <X11/Xlib.h>

void handle_maprequest(WindowManager *wm, XEvent event) {

  Workspace *ws = &wm->workspaces[wm->current_workspace];

  Client *client = init_client(event.xmaprequest.window);

  XSelectInput(wm->dpy, client->window, EnterWindowMask);
  insert_client(ws, client, ws->focused_idx + 1);

  // TODO change to focused monitor
  tile(wm->dpy, ws, wm->monitors);


  XMapWindow(wm->dpy, client->window);
}

void handle_destorynotify(WindowManager *wm, XEvent event) {
  Workspace *ws = &wm->workspaces[wm->current_workspace];

  int idx = get_client(ws, event.xdestroywindow.window);

  // if(ws->focused_idx == idx) ws->focused_idx--;

  remove_client(ws, idx);
}

void handle_configurerequest(WindowManager *wm, XEvent event) {
  XConfigureRequestEvent *configure_event = &event.xconfigurerequest;
  XWindowChanges wc;


  Workspace *ws = &wm->workspaces[wm->current_workspace];

  int idx = get_client(ws, configure_event->window);

  if(idx != -1) {
    wc.x = configure_event->x;
    wc.y = configure_event->y;
    wc.width = configure_event->width;
    wc.height = configure_event->height;
    wc.sibling = configure_event->above;
    wc.border_width = configure_event->border_width;
    wc.stack_mode = configure_event->detail;

    XConfigureWindow(
        wm->dpy, 
        configure_event->window, 
        configure_event->value_mask, 
        &wc
        );
  } 

  // TODO change to focused monitor
  tile(wm->dpy, ws, wm->monitors);
}

