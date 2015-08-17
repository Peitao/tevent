
#include "echo_server.h"

int main(int argc, const char *argv[])
{
  EchoServer server(9877);
  server.Run();
  return 0;
}
