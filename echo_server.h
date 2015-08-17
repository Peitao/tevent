#pragma once
#include "base_server.h"

// 一个 server
// 典型的单线程服务 like redis

class EchoServer : public BaseServer {
 public:
  EchoServer(int port):BaseServer(port) {}
  void OnDataRead(Link * link, Buffer * in, Buffer * out){
    std::string ss;
    in->GetDataAsString(&ss);
    in->TakeData(ss.size());
    std::cout << ss << std::endl;
    out->Append("Out : " + ss);
  }
 private:
};
