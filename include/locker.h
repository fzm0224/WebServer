//线程同步机制包装类
#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

/* 封装信号量的类 */
class sem
{
public:
    sem();
    ~sem();
	/* 等待信号量 */
    bool wait();
	/* 增加信号量 */
    bool post();

private:
    sem_t m_sem;
};

/* 封装互斥锁的类 */
class locker
{
public:
    locker();
    ~locker();
	/* 获取互斥锁 */
    bool lock();
	/* 释放互斥锁 */
    bool unlock();

private:
    pthread_mutex_t m_mutex;
};

/* 封装条件变量的类 */
class cond
{
public:
    cond();
    ~cond();
	/* 等待条件变量 */
    bool wait();
	/* 唤醒等待条件变量的线程 */
    bool signal();

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif
