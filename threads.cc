#include <iostream>
#include <stdlib.h>
#include "threads.h"

void* ThreadPool::_work_run(void * f) {
  ThreadPool * pool = (ThreadPool *)f;
  Task t;
  while (true) {
    if ( pool->tasks.pop(&t) == 1 ) {
        t();
        //std::cout << "thread = " << pthread_self() << std::endl;
    } else {
      std::cerr << "pop -1" << std::endl;
    }
  }
  std::cerr << ".......child die" << std::endl;
  exit(0);
  return NULL;
}
