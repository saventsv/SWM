#include "../include/client.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  Workspace ws;
  ws.clients = init_client_arr();

  Client c1, c2, c3, c4, c5;

  append_client(&ws, &c1);
  append_client(&ws, &c2);
  append_client(&ws, &c3);
  append_client(&ws, &c4);

  insert_client(&ws, &c5, 0);

  assert(ws.clients.data[0] == &c5);
  assert(ws.clients.data[1] == &c1);
  assert(ws.clients.data[2] == &c2);
  assert(ws.clients.data[3] == &c3);
  assert(ws.clients.data[4] == &c4);

  for(int i = 0; i < ws.clients.size; i++) {
    ws.clients.data[i] = NULL;
  }
  ws.clients.size = 0;

  append_client(&ws, &c1);
  append_client(&ws, &c2);
  append_client(&ws, &c3);
  append_client(&ws, &c4);

  insert_client(&ws, &c5, 4);
  assert(ws.clients.data[4] == &c5);

  for(int i = 0; i < ws.clients.size; i++) {
    ws.clients.data[i] = NULL;
  }
  ws.clients.size = 0;

  append_client(&ws, &c1);
  append_client(&ws, &c2);
  append_client(&ws, &c3);
  append_client(&ws, &c4);

  insert_client(&ws, &c5, 3);

  assert(ws.clients.data[0] == &c1);
  assert(ws.clients.data[1] == &c2);
  assert(ws.clients.data[2] == &c3);
  assert(ws.clients.data[3] == &c5);
  assert(ws.clients.data[4] == &c4);

  for(int i = 0; i < ws.clients.size; i++) {
    ws.clients.data[i] = NULL;
  }
  ws.clients.size = 0;

  append_client(&ws, &c1);
  append_client(&ws, &c2);
  append_client(&ws, &c3);
  append_client(&ws, &c4);

  insert_client(&ws, &c5, 10);

  assert(ws.clients.size == 4);

  insert_client(&ws, &c5, -1);

  assert(ws.clients.size == 4);

  return 0;
}


