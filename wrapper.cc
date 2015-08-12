#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include "wrapper.h"

void err_sys(const char * str) {
  std::cerr << "errno = " << errno << " " << ::strerror(errno) << std::endl;
  std::cerr << "err_sys... \"" << str << "\" exiting..." << std::endl;
  exit(1);
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int n;
again:
    if ( (n = accept(fd, sa, salenptr) ) < 0) {
      if (errno == EPROTO || errno == ECONNABORTED)
              goto again;
      else
          err_sys("accept error");
    }
    return n;
}
void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (bind(fd, sa, salen) < 0)
      err_sys("bind error");
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (connect(fd, sa, salen) < 0)
      err_sys("connect error");
}

void Listen(int fd, int backlog) {
    if (listen(fd, backlog) < 0)
      err_sys("listen error");
}
void MakeSocketListen(int sock, int port) {
  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);
  Bind(sock, (sockaddr*)&servaddr, sizeof(servaddr));
  Listen(sock, 1024);
}
void SetNoBlock(int fd) {
  int val = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, val | O_NONBLOCK);
}
void SetNoDelay(int fd) {
  int opt = 1;
  ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
}

