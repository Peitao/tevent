#pragma once
#include <iostream>
#include "wrapper.h"
#include "buffer.h"
#include "handler.h"
// 一个 link 表示一个 client 连接
// 这个
class Link {
 public:
  typedef boost::function<void(Link *, Buffer *,Buffer *)> OnReadCallBack;
  Link(int fd, std::string ip, int port, EventLoop * loop, OnReadCallBack rcb) {
    std::cout << "New a Link. All link = " << ++debug_link_count << std::endl;
    fd_ = fd;
    ip_ = ip;
    port_ = port;
    loop_ = loop;
    rcb_ = rcb;
    fd_read_closed_ = 0;
    request_numbers_ = 0;
    SetNoBlock(fd_);
    SetNoDelay(fd_);
    handler_ = new Handler(fd, loop);
    handler_->AddToEventLoop();
    handler_->callback_ = NewMethodCallBack(&Link::CallBack, this);
    handler_->watch_events_ = EPOLLIN|EPOLLPRI|EPOLLERR|EPOLLHUP;
    handler_->UpdateToEventLoop();
  }
  ~Link() {
    std::cout << "Del a Link. All link = " << --debug_link_count << std::endl;
    if (handler_) {
      handler_->RemoveFromEventLoop();
      delete handler_;
    }
    // 每个 link 和 fd 绑定 生命周期一样
    ::close(fd_);
  }
  static int debug_link_count;
  void EnableWrite() {
    handler_->watch_events_ |= EPOLLOUT;
    handler_->UpdateToEventLoop();
  }
  void CallBack(Handler * h, uint32 events) {
    std::cout << "LinkCallBack" << std::endl;
    if (events & (EPOLLIN) ) {
      std::cout << "\tEPOLLIN" << std::endl;
      Buffer * in = &handler_->in_buf_;
      Buffer * out = &handler_->out_buf_;
      in->MakeBufferSize(1024);
      int size = ::read(handler_->fd_, in->GetBuffer(), in->GetBufferSize());
      if (size == 0) {
        if (request_numbers_ == 0 && (!(handler_->watch_events_ & EPOLLOUT)) ) {
          delete this;
          return;
        }
        fd_read_closed_ = 1;
        handler_->watch_events_ &= ~EPOLLIN;
        handler_->UpdateToEventLoop();
      } else if (size > 0) {
        in->SetBufferUsed(size);
        rcb_(this, in, out);
      }
      // 如果 out 不是空 开启写!
      if (out->GetDataSize() && (!(handler_->watch_events_& EPOLLOUT))) {
        handler_->watch_events_ |= EPOLLOUT;
        handler_->UpdateToEventLoop();
      }
    }
    if (events & EPOLLOUT) {
      std::cout << "\tEPOLLOUT" << std::endl;
      Buffer * out = &handler_->out_buf_;
      if (out->GetDataSize()) {
        int ret = ::write(handler_->fd_, out->GetData(), out->GetDataSize());
        // todo 检查返回值
        if (ret == -1) {
        } else {
          out->TakeData(ret);
        }
      }
      if (!out->GetDataSize()) {
        // 关掉读事件 防止 loop
        handler_->watch_events_ &= ~EPOLLOUT;
        handler_->UpdateToEventLoop();
        // 数据写完了 看看是否需要关闭
        if (fd_read_closed_ && request_numbers_ == 0) {
          delete this;
          return;
        }
      }
    }
    if (events & EPOLLRDHUP) std::cout << "\tEPOLLRDHUP" << std::endl;
    if (events & EPOLLPRI) std::cout << "\tEPOLLPRI" << std::endl;
    if (events & (EPOLLERR | EPOLLHUP)) {
      std::cout << "\tEPOLLERR" << std::endl;
      if (request_numbers_ == 0) {
        delete this;
        return;
      }
    }
    if (events & EPOLLHUP) std::cout << "\tEPOLLHUP" << std::endl;
    if (events & EPOLLET) std::cout << "\tEPOLLET" << std::endl;
    if (events & EPOLLONESHOT) std::cout << "\tEPOLLONESHOT" << std::endl;
  }
  void SetCloseWhenWriteDone() {
    fd_read_closed_ = 1;
  }
  void IncRequest() {
    request_numbers_++;
    std::cout << "inc link = " << this << " req number = " << request_numbers_ << std::endl;
  }
  int GetRequest() {
    return request_numbers_;
  }
  void DecRequest() {
    request_numbers_--;
    std::cout << "dec link = " << this << " req number = " << request_numbers_ << std::endl;
  }
 private:
  Handler * handler_;
  EventLoop * loop_;
  std::string ip_;
  OnReadCallBack rcb_;
  // 两个变量用于记录是否是已经读到 read 0 了
  // number 用于记录有多少请求在处理
  int fd_read_closed_;
  int request_numbers_;
  int port_;
  int fd_;
};
