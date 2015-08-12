#pragma once
#include "handler.h"
#include "eventloop.h"
#include "wrapper.h"
#include "acceptor.h"
#include "link.h"
// 基本的连接管理
// 
class BaseServer {
 public:
  BaseServer(int port):acceptor(port, &mloop) {}
  void Run() {
    acceptor.SetOnAcceptorCallBack(
        boost::bind(&BaseServer::OnNewConnetion, this, _1, _2, _3)
                                   );
    acceptor.Listen();
    mloop.Loop();
  }
  void OnDataRead(Buffer * in, Buffer * out) {
    std::string ss;
    in->GetDataAsString(&ss);
    in->TakeData(ss.size());
    std::cout << ss << std::endl;
    out->Append("Out : " + ss);
  }
  void OnNewConnetion(int fd, std::string ip, int port) {
    std::cout << "OnNewConnetion" << std::endl;
    Link * link = new Link(fd, ip, port, &mloop,
            boost::bind(&BaseServer::OnDataRead, this, _1, _2)
                           );
    links.push_back(link);
  }
 private:
  EventLoop mloop;
  Acceptor acceptor;
  // 先用这个管理 link ... 
  std::vector<Link *>  links;
};
