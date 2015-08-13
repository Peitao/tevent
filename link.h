#pragma once

// 一个 link 表示一个 client 连接
// 这个
class Link {
 public:
  typedef boost::function<void(Link *, Buffer *,Buffer *)> OnReadCallBack;
  Link(int fd, std::string ip, int port, EventLoop * loop, OnReadCallBack rcb) {
    fd_ = fd;
    ip_ = ip;
    port_ = port;
    loop_ = loop;
    rcb_ = rcb;
    SetNoBlock(fd_);
    SetNoDelay(fd_);
    handler_ = new Handler(fd, loop);
    handler_->AddToEventLoop();
    handler_->callback_ = NewMethodCallBack(&Link::CallBack, this);
    handler_->watch_events_ = EPOLLIN|EPOLLPRI|EPOLLERR;
    handler_->UpdateToEventLoop();
  }
  ~Link() {
    if (handler_) {
      handler_->RemoveFromEventLoop();
      delete handler_;
    }
    // 每个 link 和 fd 绑定 生命周期一样
    ::close(fd_);
  }
  void EnableWrite() {
    handler_->watch_events_ |= EPOLLOUT;
    handler_->UpdateToEventLoop();
  }
  void CallBack(Handler * h, uint32 events) {
    std::cout << "LinkCallBack" << std::endl;
    if (events & EPOLLIN) {
      std::cout << "\tEPOLLIN" << std::endl;
      Buffer * in = &handler_->in_buf_;
      Buffer * out = &handler_->out_buf_;
      in->MakeBufferSize(1024);
      int size = ::read(handler_->fd_, in->GetBuffer(), in->GetBufferSize());
      if (size == 0) {
        // ...
      } else if (size > 0) {
        in->SetBufferUsed(size);
      }
      rcb_(this, in, out);
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
      }
    }
    if (events & EPOLLRDHUP) std::cout << "\tEPOLLRDHUP" << std::endl;
    if (events & EPOLLPRI) std::cout << "\tEPOLLPRI" << std::endl;
    if (events & EPOLLERR) std::cout << "\tEPOLLERR" << std::endl;
    if (events & EPOLLHUP) std::cout << "\tEPOLLHUP" << std::endl;
    if (events & EPOLLET) std::cout << "\tEPOLLET" << std::endl;
    if (events & EPOLLONESHOT) std::cout << "\tEPOLLONESHOT" << std::endl;
  }
 private:
  Handler * handler_;
  EventLoop * loop_;
  std::string ip_;
  OnReadCallBack rcb_;
  int port_;
  int fd_;
};
