#include "../include/window_manager.h"
#include "../include/events.h"
#include "../include/wm.h"

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
          handle_maprequest(&WM, event);
          break;
        }

      case ConfigureRequest:
        {
          handle_configurerequest(&WM, event);
          break;
        }
      case DestroyNotify:
        {
          handle_destorynotify(&WM, event);
          break;
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
      // case KeyPress:
      //   {
      //
      //   }
    }
  }


  return 0;
}
