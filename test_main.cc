#include "base_server.h"


int main(int argc, const char *argv[]) {
  BaseServer server(9870);
  server.Run();
  return 0;
}

