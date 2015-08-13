#pragma once
#include <pthread.h>
#include <stdlib.h>
#include <queue>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <vector>
#include "wrapper.h"
// copy from ssdb
template <class T>
class SelectableQueue {
 private:
  int fds[2];
  pthread_mutex_t mutex;
  std::queue<T> items;
 public:
  SelectableQueue();
  ~SelectableQueue();
  int fd() {
    return fds[0];
  }
  // multi writer
  int push(const T item);
  // single reader
  int pop(T *data);
};

template <class T>
SelectableQueue<T>::SelectableQueue() {
  if(pipe(fds) == -1){
    exit(0);
  }
  pthread_mutex_init(&mutex, NULL);
}

template <class T>
SelectableQueue<T>::~SelectableQueue(){
  pthread_mutex_destroy(&mutex);
  close(fds[0]);
  close(fds[1]);
}

template <class T>
int SelectableQueue<T>::push(const T item) {
  if(pthread_mutex_lock(&mutex) != 0) {
    return -1;
  }
  {
    items.push(item);
  }
  if(::write(fds[1], "1", 1) == -1){
    exit(0);
  }
  pthread_mutex_unlock(&mutex);
  return 1;
}

template <class T>
int SelectableQueue<T>::pop(T *data){
  int n, ret = 1;
  char buf[1];
  while(1){
    n = ::read(fds[0], buf, 1);
    if (n < 0) {
      if (errno == EINTR){
        continue;
      } else {
         return -1;
      }
    } else if (n == 0){
      ret = -1;
    } else {
      if (pthread_mutex_lock(&mutex) != 0){
        return -1;
      }
      {
        if(items.empty()) {
          fprintf(stderr, "%s %d error!\n", __FILE__, __LINE__);
          pthread_mutex_unlock(&mutex);
          return -1;
        }
        *data = items.front();
        items.pop();
      }
      pthread_mutex_unlock(&mutex);
    }
    break;
  }
  return ret;
}

template <class T>
class Queue{
  private:
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    std::queue<T> items;
  public:
    Queue();
    ~Queue();
    int push(const T item);
    int pop(T *data);
    int size();
};

template <class T>
Queue<T>::Queue(){
  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&mutex, NULL);
}
template <class T>
Queue<T>::~Queue(){
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);
}
template <class T>
int Queue<T>::push(const T item){
  if(pthread_mutex_lock(&mutex) != 0){
    return -1;
  }
  {
    items.push(item);
  }
  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&cond);
  return 1;
}

template <class T>
int Queue<T>::pop(T *data){
  if(pthread_mutex_lock(&mutex) != 0){
    return -1;
  }
  {
    while(items.empty()){
        if(pthread_cond_wait(&cond, &mutex) != 0)  return -1;
    }
    *data = items.front();
    items.pop();
  }
  if(pthread_mutex_unlock(&mutex) != 0) return -1;
  return 1;
}
template <class T>
int Queue<T>::size(){
  int ret = -1;
  if(pthread_mutex_lock(&mutex) != 0){
    return -1;
  }
  ret = items.size();
  pthread_mutex_unlock(&mutex);
  return ret;
}
// 线程池
class ThreadPool {
 public:
  typedef boost::function<void(void)> Task;
  int AddTask(Task t){
    if ( tasks.push(t) != 1 )
      std::cerr << "add err" << std::endl;
  }
  ThreadPool(int num):_num(num) {
    Start();
  }
 private:
  void Start(){
    int err;
    pthread_t tid;
    for (int i = 0; i < _num; i++) {
      err = pthread_create(&tid, NULL, &ThreadPool::_work_run, (void*)this);
      if (err != 0) std::cout << "err ,can't create thread" << std::endl;
      else tids.push_back(tid);
    }
  }
  static void* _work_run(void * );
  Queue<Task>  tasks;
  std::vector<pthread_t> tids;
  int _num;
};

