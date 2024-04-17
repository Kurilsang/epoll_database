#include "epoll.h"
#include "log.h"
using namespace kuril::utility;

int start_epoll(int port)
{
    //socket创建
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == server)
    {
        perror("server socket error");
        return -1;
    }

    //绑定地址簇
    struct sockaddr_in ser_addr;//server_addr
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(port);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);//获取本地所有 与 addr_inet("0.0.0.0")等同
    int ret = bind(server,(sockaddr*)&ser_addr, sizeof(ser_addr));
    if (-1 == ret)
    {
        perror("bind error");
        fatal("bind失败");
        return -1;
    }
    //设置端口复用
    int opt = 1;
    setsockopt(server, SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    //listen
    ret = listen(server,128);
    if (-1 == ret)
    {
        perror("listen error");
        fatal("listen失败");
        return -1;
    }
    //epoll模型
    int epfd = epoll_create(128);
    if (-1 == epfd)
    {
        perror("epoll create error");
        fatal("epoll模型创建失败");
        return -1;
    }
    //添加检测节点
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server;
    ret = epoll_ctl(epfd,EPOLL_CTL_ADD,server,&ev);
    if (-1 == ret)
    {
        perror("epoll_ctl error");
        fatal("epoll模型添加事件失败");
        return -1;
    }
    //事件集及其个数
    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(struct epoll_event);
    //用于获取客户端ip和端口
    struct sockaddr_in client_addr;
    memset(&client_addr,0,sizeof(sockaddr_in));
    client_addr.sin_family = AF_INET;
    int len_c = sizeof(sockaddr_in);//转化为左值，后面转换类型能用
    debug("epoll模型建立完成,开始进入循环工作...");
    //accept
    while(1)
    {
        //每轮开始先检测
        int num = epoll_wait(epfd,evs,size,-1);
        for (int i = 0; i < num; ++i)
        {
            int curfd = evs[i].data.fd;
            if (server == curfd)//若为监听描述符
            {
                int acfd = accept(server,(sockaddr*)&client_addr,(socklen_t*)&len_c);
                //接收到了就将其加入epoll
                ev.events = EPOLLIN;
                ev.data.fd = acfd;
                ret = epoll_ctl(epfd,EPOLL_CTL_ADD, acfd, &ev);
                if (-1 == ret)
                {
                    perror("accept epoll_ctl error");
                    return -1;
                }
            }
            else//处理业务
            {
                char buf[1024];
                memset(&buf, 0 , sizeof(buf));
                int len = recv(curfd, buf, sizeof(buf), 0);
                if (0 == len)//断开
                {
                    cout << "客户端断开连接" << endl;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,curfd,NULL);
                    close(curfd);
                }
                else if (0 < len)//正常工作
                {
                    cout<<"客户端：" << buf << endl;
                    send(curfd, "1", sizeof("1"), 0);
                }
                else
                {
                    perror("recv error");
                    return -1;
                }
            }
        }
    }
    return 0;
}