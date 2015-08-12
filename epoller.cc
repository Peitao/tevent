#include <iostream>
#include "epoller.h"
#include "handler.h"

EPoller::EPoller() {
  epollfd_ = ::epoll_create1(EPOLL_CLOEXEC);
  if (epollfd_ < 0) {
    std::cerr << "EPoller::epoll_create1 fail!" << std::endl;
  }
  events_.resize(10);
}

void EPoller::GetActiveHandler(std::vector<Handler*> *pHandlerVec, int timeout) {
  pHandlerVec->clear();
  int ret = ::epoll_wait(epollfd_, &(events_[0]), events_.size(), timeout);
  if (ret < 0) {
    std::cerr << "EPoller::epoll_wait fail!" << std::endl;
  } else if (ret > 0) {
    for (int i = 0; i < ret; i++) {
      Handler * handler = (Handler*)events_[i].data.ptr;
      if (events_[i].events & handler->watch_events_) {
        handler->active_events_ = events_[i].events;
        pHandlerVec->push_back(handler);
      }
    }
    if (ret == events_.size()) events_.resize(ret * 2);
  }
}

void EPoller::DoUpdateHandler(int op, Handler * handler) {
  struct epoll_event ev;
  ev.events = handler->watch_events_;
  ev.data.ptr = handler;
  int ret = ::epoll_ctl(epollfd_, op, handler->fd_, &ev);
  if (ret != 0) {
    std::cerr << "EPoller::epoll_ctl " << op << " fail!" << std::endl;
  }
}

void EPoller::AddHandler(Handler * handler) {
  DoUpdateHandler(EPOLL_CTL_ADD, handler);
}

void EPoller::UpdateHandler(Handler * handler) {
  DoUpdateHandler(EPOLL_CTL_MOD, handler);
}

void EPoller::RemoveHandler(Handler * handler) {
  DoUpdateHandler(EPOLL_CTL_DEL, handler);
}
