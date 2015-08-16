#pragma once
#include <signal.h>
#include "handler.h"
#include "eventloop.h"
#include "wrapper.h"
#include "acceptor.h"
#include "link.h"
#include "threads.h"
// 基本的连接管理
// 
class BaseServer {
 public:
  BaseServer(int port):acceptor(port, &mloop) {
    signal(SIGPIPE, SIG_IGN);
  }
  void Run() {
    acceptor.SetOnAcceptorCallBack(
        boost::bind(&BaseServer::OnNewConnetion, this, _1, _2, _3)
                                   );
    acceptor.Listen();
    wake_up_handler = new Handler(write_back_queue.fd(), &mloop);
    wake_up_handler->AddToEventLoop();
    wake_up_handler->watch_events_ = EPOLLIN;
    wake_up_handler->callback_ = NewMethodCallBack(&BaseServer::WriteBack, this);
    wake_up_handler->UpdateToEventLoop();

    mloop.Loop();
  }
  // 这里目前是同步往回写
  // 如果是 cpu 密集型 server 
  // 需要改成异步地回写 把 in 放到线程中计算
  // 通过 done 函数 把回调函数加到 event pool 中执行
  // 在 event pool 中开启写 并销毁 link
  virtual void OnDataRead(Link * link, Buffer * in, Buffer * out) = 0;
  void OnNewConnetion(int fd, std::string ip, int port) {
    std::cout << "OnNewConnetion" << std::endl;
    Link * link = new Link(fd, ip, port, &mloop,
            boost::bind(&BaseServer::OnDataRead, this, _1, _2, _3)
                           );
    links.push_back(link);
  }
  // 由于 Buffer 不是线程安全的
  // 所以需要在 io 线程写 buffer 
  struct WriteBackStruct {
    Link * link;
    Buffer * out;
    std::string data;
    bool close_when_write_done;
  };
  // 用回异步回写 link 的输出缓冲区
  // 这个函数是多线程安全的 用于 具体 server 
  // 的 done 函数调用
  void WakeUpForWriteBackInEventLoop(WriteBackStruct * write_back_data) {
    std::cout << "add wake up " << std::endl;
    write_back_queue.push(write_back_data);
  }
  void WriteBack(Handler * h, uint32 events) {
    std::cout << "WriteBack" << std::endl;
    if (events & EPOLLIN) {
      WriteBackStruct * wb_data;
      if (write_back_queue.pop(&wb_data) != 1) {
        std::cout << "SelectableQueue pop -1" << std::endl;
      }
      // 开启这个 link 的写事件
      std::cout << "do wake up " << std::endl;
      wb_data->out->Append(wb_data->data);
      wb_data->link->EnableWrite();
      wb_data->link->DecRequest();
      if (wb_data->close_when_write_done)
        wb_data->link->SetCloseWhenWriteDone();
      delete wb_data;
    }
  }
 private:
  EventLoop mloop;
  Acceptor acceptor;
  // 异步回写
  SelectableQueue<WriteBackStruct *> write_back_queue;
  Handler * wake_up_handler;
  // 先用这个管理 link ... 
  std::vector<Link *>  links;
};
