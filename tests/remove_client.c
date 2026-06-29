#include "../include/client.h"
#include <assert.h>
#include <stdlib.h>

int main(void) {
  Workspace ws;
  ws.clients = init_client_arr();


  Client *c1 = malloc(sizeof(Client *));
  Client *c2 = malloc(sizeof(Client *));
  Client *c3 = malloc(sizeof(Client *));
  Client *c4 = malloc(sizeof(Client *));
  Client *c5 = malloc(sizeof(Client *));

  append_client(&ws, c1);
  append_client(&ws, c2);
  append_client(&ws, c3);
  append_client(&ws, c4);
  append_client(&ws, c5);

  remove_client(&ws, 0);

  assert(ws.clients.data[0] == c2);
  assert(ws.clients.data[1] == c3);
  assert(ws.clients.data[2] == c4);
  assert(ws.clients.data[3] == c5);
  assert(ws.clients.size == 4);

  for(int i = 0; i < ws.clients.capacity; i++) {
    ws.clients.data[i] = NULL;
  }
  ws.clients.size = 0;


  append_client(&ws, c1);
  append_client(&ws, c2);
  append_client(&ws, c3);
  append_client(&ws, c4);
  append_client(&ws, c5);

  remove_client(&ws, 4);

  assert(ws.clients.data[0] == c1);
  assert(ws.clients.data[1] == c2);
  assert(ws.clients.data[2] == c3);
  assert(ws.clients.data[3] == c4);
  // assert(ws.clients.data[4] == NULL);

  for(int i = 0; i < ws.clients.capacity; i++) {
    ws.clients.data[i] = NULL;
  }
  ws.clients.size = 0;

  append_client(&ws, c1);
  append_client(&ws, c2);
  append_client(&ws, c3);
  append_client(&ws, c4);
  append_client(&ws, c5);

  remove_client(&ws, 3);

  assert(ws.clients.data[0] == c1);
  assert(ws.clients.data[1] == c2);
  assert(ws.clients.data[2] == c3);
  assert(ws.clients.data[3] == c5);
  assert(ws.clients.data[4] == NULL);

  for(int i = 0; i < ws.clients.capacity; i++) {
    ws.clients.data[i] = NULL;
  }
  ws.clients.size = 0;

  append_client(&ws, c1);
  append_client(&ws, c2);
  append_client(&ws, c3);
  append_client(&ws, c4);
  append_client(&ws, c5);

  remove_client(&ws, 10);

  assert(ws.clients.data[0] == c1);
  assert(ws.clients.data[1] == c2);
  assert(ws.clients.data[2] == c3);
  assert(ws.clients.data[3] == c4);
  assert(ws.clients.data[4] == c5);

  remove_client(&ws, -1);

  assert(ws.clients.data[0] == c1);
  assert(ws.clients.data[1] == c2);
  assert(ws.clients.data[2] == c3);
  assert(ws.clients.data[3] == c4);
  assert(ws.clients.data[4] == c5);

  return 0;
}


