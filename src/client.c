#include "../include/client.h"
#include <stdlib.h>

ClientArray init_client_arr() {
  ClientArray client_arr;
  client_arr.capacity = 4;
  client_arr.size = 0;

  client_arr.data = malloc(client_arr.capacity * sizeof(Client*));

  if(client_arr.data == NULL) abort();

  return client_arr;
}


