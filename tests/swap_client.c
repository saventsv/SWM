#include "../include/client.h"
#include <assert.h>

int main(void) {
  Workspace ws;
  ws.clients = init_client_arr();
  Client c1, c2, c3, c4, c5;

  append_client(&ws, &c1);
  append_client(&ws, &c2);
  append_client(&ws, &c3);
  append_client(&ws, &c4);
  append_client(&ws, &c5);

  assert(ws.clients.data[0] == &c1);
  assert(ws.clients.data[4] == &c5);

  swap_clients(&ws, 0, 4);
  
  assert(ws.clients.data[0] == &c5);
  assert(ws.clients.data[4] == &c1);

  swap_clients(&ws, 0, 4);

  assert(ws.clients.data[0] == &c1);
  assert(ws.clients.data[4] == &c5);

  swap_clients(&ws, -1, 14);

  assert(ws.clients.data[0] == &c1);
  assert(ws.clients.data[4] == &c5);

  assert(ws.clients.data[1] == &c2);
  assert(ws.clients.data[3] == &c4);

  swap_clients(&ws, 1, 3);

  assert(ws.clients.data[1] == &c4);
  assert(ws.clients.data[3] == &c2);

  return 0;
}


