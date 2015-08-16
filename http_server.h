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
    link->IncRequest();
    // step 2. 
    service->OnRequest(req, boost::bind(&HttpServer::Done, this, req));
  }
  void Done(HttpRequst * req) {
    // package
    BaseServer::WriteBackStruct * wb_data = new BaseServer::WriteBackStruct();
    wb_data->link = req->raw_link;
    wb_data->out = req->out;
    wb_data->close_when_write_done = false;
    // test
    if (req->resopnse.find("notalive") != std::string::npos) {
      wb_data->close_when_write_done = true;
    }
    wb_data->data.swap(req->resopnse);
    WakeUpForWriteBackInEventLoop(wb_data);
    delete req;
  }
 private:
  Service * service;
};
