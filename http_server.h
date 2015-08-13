#pragma once
#include "base_server.h"

// 注意 http 协议 不支持 pipe line 处理
// 一个连接上只有一个请求

// 一个 server
struct HttpRequst {
  // public
  std::string raw_header;
  std::string resopnse;
  // imples
  Link * raw_link;
  Buffer * in;
  Buffer * out;
};
typedef boost::function<void(void)> Done;
class Service {
 public:
  virtual void OnRequest(HttpRequst * req, Done done) = 0;
};

class HttpServer : public BaseServer {
 public:
  HttpServer(int port, Service * s):BaseServer(port),service(s) {}
  void OnDataRead(Link * link, Buffer * in, Buffer * out){
    // step 1. 解码 如果 header 没收全 return 
    HttpRequst * req = new HttpRequst();
    req->raw_link = link;
    req->in = in;
    req->out = out;
    req->in->GetDataAsString(&req->raw_header);
    req->in->TakeData(req->raw_header.size());
    // step 2. 
    service->OnRequest(req, boost::bind(&HttpServer::Done, this, req));
  }
  void Done(HttpRequst * req) {
    // package
    req->out->Append(req->resopnse);
    WakeUpForWriteBackInEventLoop(req->raw_link);
    delete req;
  }
 private:
  Service * service;
};
