#pragma once
#include <functional>
#include "handler.h"
#include "wrapper.h"

// 主要实现接收连接
class EventLoop;
class Acceptor {
 public:
  typedef boost::function<void(int, std::string, int)> OnAcceptorCallBack;
  Acceptor(int port, EventLoop * loop) {
    port_ = port;
    loop_ = loop;
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    handler_ = new Handler(fd_, loop_);
  }
  void Listen() {
    SetNoBlock(fd_);
    MakeSocketListen(fd_, port_);
    handler_->AddToEventLoop();
    handler_->callback_ = NewMethodCallBack(&Acceptor::AcceptCallBack, this);
    handler_->watch_events_ = EPOLLIN|EPOLLOUT|EPOLLRDHUP|EPOLLPRI|EPOLLERR;
    handler_->UpdateToEventLoop();
  }
  ~Acceptor() {
    if (handler_) {
      handler_->RemoveFromEventLoop();
      delete handler_;
    }
    ::close(fd_);
  }
  void AcceptCallBack(Handler * h, uint32 events) {
    std::cout << "AcceptCallBack" << std::endl;
    if (events & EPOLLIN) {
      std::cout << "\tEPOLLIN" << std::endl;
      std::cout << "\tAccepting ...." << std::endl;
      struct sockaddr client_addr;
      socklen_t  len = sizeof(client_addr);
      int client_fd = Accept(handler_->fd_, &client_addr, &len);
      sockaddr_in* pSin = (sockaddr_in*)&client_addr;
      std::string ip = inet_ntoa(pSin->sin_addr);
      cb_(client_fd, ip, pSin->sin_port);
    }
    if (events & EPOLLOUT) std::cout << "\tEPOLLOUT" << std::endl;
    if (events & EPOLLRDHUP) std::cout << "\tEPOLLRDHUP" << std::endl;
    if (events & EPOLLPRI) std::cout << "\tEPOLLPRI" << std::endl;
    if (events & EPOLLERR) std::cout << "\tEPOLLERR" << std::endl;
    if (events & EPOLLHUP) std::cout << "\tEPOLLHUP" << std::endl;
    if (events & EPOLLET) std::cout << "\tEPOLLET" << std::endl;
    if (events & EPOLLONESHOT) std::cout << "\tEPOLLONESHOT" << std::endl;
  }
  inline void SetOnAcceptorCallBack(OnAcceptorCallBack cb) {
    cb_ = cb;
  }
 private:
  Handler * handler_;
  EventLoop * loop_;
  OnAcceptorCallBack cb_;
  int port_;
  int fd_;
};
