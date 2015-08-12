#ifndef HANDLER_R7DKUGRU

#define HANDLER_R7DKUGRU
#include <functional>
#include "buffer.h"
#include "types.h"
#include "eventloop.h"
// 语义：
// 如果 fd_ 有我要的事件 watch_events_ 就调用我的 callback_
// typedef std::function<void f(Handler *, uint32)> CallBack;
// 我擦 开发机 g++不支持 bind 
// 仿照 google callback 
// 主要是不想让 大家继承自 Handler - -~
// 生命周期由 T 释放

class CallBack {
 public:
  virtual void operator()(Handler * handler, uint32 events) = 0;
 private:
};
template <class T>
class MethodCallBack : public CallBack {
 public:
  typedef void (T::*Method)(Handler * handler, uint32 events);
  MethodCallBack(T * instance, Method m){
    instance_ = instance;
    method_ = m;
  }
  void operator()(Handler * handler, uint32 events) {
    (instance_->*method_)(handler, events);
  }
private:
  Method method_;
  T * instance_;
};
template <typename T>
CallBack * NewMethodCallBack(void (T::*m)(Handler * handler, uint32 events), T * t) {
  return new MethodCallBack<T>(t, m);
}

class Handler {
 public:
  Handler(int fd, EventLoop * loop):active_events_(0), watch_events_(0), fd_(fd), callback_(NULL),loop_(loop) {}
  void UpdateToEventLoop() {
    loop_->UpdateHandler(this);
  }
  void AddToEventLoop() {
    loop_->AddHandler(this);
  }
  void RemoveFromEventLoop() {
    loop_->RemoveHandler(this);
  }

  // 正在发生的事件
  uint32 active_events_;
  // 关注的事件
  uint32 watch_events_;
  // 文件描述符
  int fd_;
  // 回调函数
  CallBack * callback_;
  // 所属的 loop
  EventLoop * loop_;
  // 输入输出缓冲区
  Buffer in_buf_;
  Buffer out_buf_;
};
#endif /* end of include guard: HANDLER_R7DKUGRU */
