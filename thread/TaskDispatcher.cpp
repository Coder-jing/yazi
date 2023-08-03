#include "TaskDispatcher.h"
#include "Logger.h"
#include "Singleton.h"

using namespace yazi::utility;
using namespace yazi::thread;

TaskDispatcher::TaskDispatcher()
{
}

TaskDispatcher::~TaskDispatcher()
{
}

void TaskDispatcher::init(int threads)
{
    Singleton<ThreadPool>::instance()->create(threads);     //创建线程池
    start();        
}

void TaskDispatcher::assign(Task* task)
{
    debug("task dispatcher assign task");
    m_mutex.lock();
    m_tasks.push_back(task);
    m_mutex.unlock();
    m_cond.signal();
}

void TaskDispatcher::handle(Task* task)
{
    debug("task dispatcher handle task");
    ThreadPool * threadpool = Singleton<ThreadPool>::instance();
    if (threadpool->get_idle_thread_numbers() > 0)
    {
        threadpool->assign(task);
    }
    else
    {
        m_mutex.lock();
        m_tasks.push_front(task);
        m_mutex.unlock();
        debug("all threads are busy!");
    }
}

void TaskDispatcher::run()
{
    sigset_t mask;  //声明一个变量 mask，它是一个信号集，用于在后续操作中设置线程的信号屏蔽字
    if (0 != sigfillset(&mask))     //sigfillset 函数用于将信号集 mask 设置为包含所有可能的信号。如果设置失败，会输出错误信息并返回。
    {
        error("thread manager sigfillset failed!");
        return;
    }
    if (0 != pthread_sigmask(SIG_SETMASK, &mask, NULL))     //pthread_sigmask 函数用于设置线程的信号屏蔽字，以屏蔽掉所有信号。这样做可以确保线程在执行任务时不会被信号中断。如果设置失败，会输出错误信息并返回。
    {
        error("thread manager pthread_sigmask failed!");
        return;
    }
    while (true)
    {
        //debug("task list: %d", m_actions.size());
        m_mutex.lock();
        while (m_tasks.empty()) 
            m_cond.wait(&m_mutex);
        Task* task = m_tasks.front();
        m_tasks.pop_front();
        m_mutex.unlock();
        handle(task);
    }
}
