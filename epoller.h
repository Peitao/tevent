#ifndef EPOLLER_YTKWJIDV

#define EPOLLER_YTKWJIDV

#include <sys/epoll.h>
#include <vector>
// 主要功能
// 1. 返回当前活动的事件列表
// 2. 注册关注的事件
class Handler;
class EPoller {
 public:
  EPoller();
  void GetActiveHandler(std::vector<Handler*> *pHandlerVec, int timeout);
  void UpdateHandler(Handler * handler);
  void AddHandler(Handler * handler);
  void RemoveHandler(Handler * handler);
 private:
  void DoUpdateHandler(int op, Handler * handler);
  int epollfd_;
  std::vector<struct epoll_event> events_;
};

#endif /* end of include guard: EPOLLER_YTKWJIDV */
