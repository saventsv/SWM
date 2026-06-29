#include "../include/wm.h"
#include "../include/client.h"
#include "../include/layout.h"
#include <assert.h>

int main(void) {
  Workspace ws;
  Monitor monitor;
  ws.clients = init_client_arr();

  Client c1, c2, c3, c4, c5;

  append_client(&ws, &c1);
  append_client(&ws, &c2);
  append_client(&ws, &c3);
  append_client(&ws, &c4);
  append_client(&ws, &c5);

  monitor.usable_width = 1920;
  monitor.usable_height = 1080;

  calculate_geometry(&ws, &monitor);

  assert(c1.width == 960);
  assert(c1.height == 1080);
  assert(c2.width == 960);
  assert(c2.height == 270);
  assert(c2.width == c3.width);
  assert(c2.height == c3.height);
  assert(c3.width == c4.width);
  assert(c3.height == c4.height);
  assert(c3.width == c4.width);
  assert(c3.height == c4.height);
  assert(c4.width == c5.width);
  assert(c4.height == c5.height);

  return 0;
}


