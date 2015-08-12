#ifndef EVENTLOOP_7NKLYNSP

#define EVENTLOOP_7NKLYNSP
#include <vector>
#include "epoller.h"
class Handler;
class EventLoop {
 public:
  EventLoop():stopping_(false){}
  void Loop();
  void UpdateHandler(Handler * handler);
  void AddHandler(Handler * handler);
  void RemoveHandler(Handler * handler);
 private:
  EPoller epoller_;
  bool stopping_;
  std::vector<Handler *> actives_;
};

#endif /* end of include guard: EVENTLOOP_7NKLYNSP */
