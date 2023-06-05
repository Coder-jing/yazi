#include "AutoLock.h"

using namespace yazi::thread;

AutoLock::AutoLock(Mutex* mutex)    //自动加锁
{
    m_mutex = mutex;
    m_mutex->lock();
}

AutoLock::~AutoLock()       //解锁
{
    m_mutex->unlock();
}
