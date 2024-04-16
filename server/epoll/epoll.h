#pragma once
//使用的有关网络的库
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>//close
#include <iostream>//perror
#include <string.h>//memset

using namespace std;

int start_epoll();
