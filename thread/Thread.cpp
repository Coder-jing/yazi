#include "Thread.h"
#include "AutoLock.h"
using namespace yazi::thread;

Thread::Thread() : m_tid(0), m_task(NULL)
{
}

Thread::~Thread()
{
}

void Thread::start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    /*pthread_attr_setscope 函数设置线程的作用域（scope），PTHREAD_SCOPE_SYSTEM 表示线程可以与系统中的其他线程进行竞争。作用域设置为系统范围允许线程在系统中的任何处理器上执行。*/
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    /*pthread_attr_setdetachstate 函数设置线程的分离状态（detach state），PTHREAD_CREATE_DETACHED 表示线程将以分离状态创建。分离状态的线程在其执行结束后会自动释放资源，不需要显式调用 pthread_join 函数来等待线程结束。*/
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);    
    /*pthread_create 函数创建一个新线程，并传入线程属性（attr）、线程函数指针（thread_func）和传递给线程函数的参数（(void *)this）。m_tid 是一个 pthread_t 类型的变量，用于存储新线程的标识符。*/
    pthread_create(&m_tid, &attr, thread_func, (void *)this);       
    pthread_attr_destroy(&attr);    //pthread_attr_destroy 函数销毁线程属性对象，释放相关资源。
}

void Thread::stop()
{
    pthread_exit(PTHREAD_CANCELED);     //导致当前线程立即退出，并且返回值被设置为 PTHREAD_CANCELED。
}

void* Thread::thread_func(void* ptr)
{
    Thread* thread = (Thread *)ptr;
    thread->run();
    return ptr;
}

void Thread::set_task(Task* task)
{
    m_mutex.lock();
    m_task = task;
    m_cond.signal();
    m_mutex.unlock();
}

Task* Thread::get_task()
{
    AutoLock lock(&m_mutex);
    return m_task;
}
