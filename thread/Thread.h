#pragma once

#include <pthread.h>
#include "Mutex.h"
#include "Condition.h"
#include "Task.h"

namespace yazi {
namespace thread {

class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual void run() = 0;

    void start();
    void stop();

    void set_task(Task* task);
    Task* get_task();

protected:
    static void* thread_func(void* ptr);

protected:  //派生类可以访问成员变量
    pthread_t           m_tid;      //线程ID
    Task*               m_task;     //线程任务
    Mutex               m_mutex;    //Mutex变量
    Condition           m_cond;     //Cond变量
};

}}
