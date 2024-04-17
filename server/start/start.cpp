#include "start.h"
#include "log.h"
//主要用于初始化各项模块
using namespace std;
using namespace kuril::utility;

void start()
{
    //初始化日志模块
    log::getInstance()->open("testLog.txt");
    log::getInstance()->level(log::DEBUG);

    start_epoll(6969);//启动epoll
}