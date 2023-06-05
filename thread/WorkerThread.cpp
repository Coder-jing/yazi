#include "WorkerThread.h"

#include "Logger.h"
#include "Singleton.h"
using namespace yazi::utility;

#include "Task.h"
#include "ThreadPool.h"
using namespace yazi::thread;

WorkerThread::WorkerThread() : Thread()
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::cleanup(void* ptr)
{
    info("worker thread cleanup handler: %x", ptr);
}

void WorkerThread::run()
{   
    /*使用了 sigfillset 函数来填充信号集 mask，将其中的所有信号标志设置为1。如果 sigfillset 函数返回值不等于0，则表示填充信号集失败，可能是由于无法获取系统支持的所有信号集。*/
    sigset_t mask;
    if (0 != sigfillset(&mask))
    {
        error("worker thread sigfillset faile!");
    }
    /*
        在这段代码中，通过调用 pthread_sigmask(SIG_SETMASK, &mask, NULL) 来设置线程的信号屏蔽字为 mask 所代表的信号集。
        第一个参数 SIG_SETMASK 表示将信号屏蔽字设置为 mask，覆盖之前的屏蔽字。
        第二个参数 &mask 是指向要设置的信号集的指针。
        第三个参数为 NULL，表示不需要保存之前的信号屏蔽字。
        如果 pthread_sigmask 函数返回值不等于0，则输出错误信息表示设置信号屏蔽字失败。
        通过设置线程的信号屏蔽字，可以控制线程对信号的响应方式。屏蔽某些信号可以防止其中断线程的执行或触发信号处理函数。这样可以确保线程在特定操作期间不受特定信号的干扰。
    */
    if (0 != pthread_sigmask(SIG_SETMASK, &mask, NULL))
    {
        error("worker thread pthread_sigmask failed");
    }
    /*
        pthread_cleanup_push 是一个宏，用于将清理处理函数注册到线程的清理处理函数栈中。
        pthread_cleanup_push(cleanup, this) 将一个清理处理函数 cleanup 和参数 this 注册到当前线程的清理处理函数栈中。当线程退出时（无论是正常退出还是被取消），注册的清理处理函数将按照后进先出的顺序执行。
        清理处理函数的原型通常为 void cleanup(void *arg)，其中 arg 是传递给清理处理函数的参数。this 被传递给清理处理函数作为参数。
        通过使用 pthread_cleanup_push 和相应的 pthread_cleanup_pop（通常与 pthread_cleanup_push 成对使用），可以确保在线程退出时执行清理操作，无论是通过 pthread_exit、return 还是被取消。
        pthread_cleanup_push 和 pthread_cleanup_pop 必须成对使用，并且通常放置在代码块的开始和结束位置，以确保在退出代码块时执行清理处理函数。
    */
    pthread_cleanup_push(cleanup, this);

    while (true)
    {
        // start wait for task
        m_mutex.lock();
        while (m_task == NULL)
            m_cond.wait(&m_mutex);  //等待条件变量 m_cond 的信号。
        m_mutex.unlock();
        // end wait for task

        int rc = 0;
        int old_state = 0;
        rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);    //将当前线程的取消状态设置为禁止取消，并将之前的状态保存在 old_state 变量中。

        m_task->run();  //WorkTask->Run()
        m_task->destroy();  //WorkTask->destroy ()
        m_task = NULL;

        Singleton<ThreadPool>::instance()->move_to_idle_list(this); 

        rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);
        pthread_testcancel(); // cancel-point
    }
    pthread_cleanup_pop(1);
}
