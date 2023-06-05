#include "Mutex.h"

using namespace yazi::thread;

Mutex::Mutex()
{
    //初始化mutex属性
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);     //设置互斥锁属性的类型, PTHREAD_MUTEX_ERRORCHECK 是一个常量，表示将互斥锁属性设置为错误检查类型。
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);    //删除锁
}

int Mutex::lock()
{
    return pthread_mutex_lock(&m_mutex);    //上锁
}

int Mutex::try_lock()
{
    return pthread_mutex_trylock(&m_mutex);     //尝试获取互斥锁
}

int Mutex::unlock()
{
    return pthread_mutex_unlock(&m_mutex);      //解锁
}


