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
