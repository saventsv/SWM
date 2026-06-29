#include "../include/client.h"
#include <assert.h>

int main(void) {
  Workspace ws;
  ws.clients = init_client_arr();
  assert(ws.clients.capacity == 4);
  assert(ws.clients.size == 0);

  Client c1, c2, c3, c4, c5;

  append_client(&ws, &c1);
  assert(ws.clients.size == 1);
  assert(ws.clients.capacity == 4);

  append_client(&ws, &c2);
  assert(ws.clients.size == 2);

  append_client(&ws, &c3);
  assert(ws.clients.size == 3);

  append_client(&ws, &c4);
  assert(ws.clients.size == 4);
  assert(ws.clients.capacity == 4);

  append_client(&ws, &c5);
  assert(ws.clients.size == 5);
  assert(ws.clients.capacity == 8);

  return 0;
}


