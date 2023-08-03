#include "EventPoller.h"

using namespace yazi::socket;

EventPoller::EventPoller(bool et) : m_epfd(0), m_max_connections(0), m_events(NULL), m_et(et)
{
}

EventPoller::~EventPoller()
{
    if (m_epfd > 0)
    {
        close(m_epfd);
        m_epfd = 0;
    }

    if (m_events != NULL)
    {
        delete[] m_events;
        m_events = NULL;
    }
}

void EventPoller::ctrl(int fd, void * ptr, __uint32_t events, int op)
{
    //声明一个 epoll_event 结构体 ev，用于设置需要注册的事件。
    struct epoll_event ev;
    //将传入的 ptr 赋值给 ev.data.ptr，将辅助的数据关联到事件。
    ev.data.ptr = ptr;
    //如果启用了边缘触发（ET）模式（m_et 为 true），则进入条件判断。
    if (m_et)
    {
        ev.events = events | EPOLLET;   //将传入的事件 events 与 EPOLLET（边缘触发模式标志）进行按位或操作，将事件触发模式设置为边缘触发模式。
    }
    else
    {
        ev.events = events;
    }
    //使用 epoll_ctl 函数对 epoll 实例进行控制操作，m_epfd 为 epoll 文件描述符，op 为控制操作，fd 为需要操作的句柄，&ev 为要注册的事件结构体。
    epoll_ctl(m_epfd, op, fd, &ev);
}

/*创建一个 epoll 实例并初始化相关资源*/
void EventPoller::create(int max_connections)
{
    m_max_connections = max_connections;
    //调用 epoll_create 函数创建一个 epoll 实例，传入参数 max_connections + 1 表示需要支持的最大连接数加 1。将返回的 epoll 文件描述符赋值给成员变量 m_epfd。
    m_epfd = epoll_create(max_connections + 1);
    if (m_epfd < 0)
    {
        return;
    }
    if (m_events != NULL)
    {
        delete[] m_events;
        m_events = NULL;
    }
    //使用 new 运算符分配一个大小为 max_connections + 1 的 epoll_event 数组，将指针赋值给成员变量 m_events，用于存储 epoll 事件。
    m_events = new epoll_event[max_connections + 1];
}

void EventPoller::add(int fd, void * ptr, __uint32_t events)
{
    ctrl(fd, ptr, events, EPOLL_CTL_ADD);
}

void EventPoller::mod(int fd, void * ptr, __uint32_t events)
{
    ctrl(fd, ptr, events, EPOLL_CTL_MOD);
}

void EventPoller::del(int fd, void * ptr, __uint32_t events)
{
    ctrl(fd, ptr, events, EPOLL_CTL_DEL);
}

int EventPoller::wait(int millsecond)
{
    return epoll_wait(m_epfd, m_events, m_max_connections + 1, millsecond);
}
