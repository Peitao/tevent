#pragma once
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
// 由于需要使用 bind 和 function
#include "boost/bind.hpp"
#include "boost/function.hpp"

void Listen(int fd, int backlog);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void err_sys(const char * str);
void SetNoBlock(int fd);
void SetNoDelay(int fd);
void MakeSocketListen(int sock, int port);

