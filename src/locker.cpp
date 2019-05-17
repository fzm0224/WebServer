#include "locker.h"

/* 封装信号量的类 */
sem::sem()
{   
	if( sem_init( &m_sem, 0, 0 ) != 0 ) 
	{   
		throw std::exception();
	}   
}

sem::~sem()
{
	sem_destroy( &m_sem );
}

/* 等待信号量 */
bool sem::wait()
{
	return sem_wait( &m_sem ) == 0;
}

/* 增加信号量 */
bool sem::post()
{
	return sem_post( &m_sem ) == 0;
}



/* 封装互斥锁的类 */
locker::locker()
{   
	if( pthread_mutex_init( &m_mutex, NULL ) != 0 ) 
	{   
		throw std::exception();
	}   
}

locker::~locker()
{
	pthread_mutex_destroy( &m_mutex );
}

/* 获取互斥锁 */
bool locker::lock()
{
	return pthread_mutex_lock( &m_mutex ) == 0;
}

/* 释放互斥锁 */
bool locker::unlock()
{
	return pthread_mutex_unlock( &m_mutex ) == 0;
}



/* 封装条件变量的类 */
cond::cond()
{
	if( pthread_mutex_init( &m_mutex, NULL ) != 0 )
	{
		throw std::exception();
	}
	if ( pthread_cond_init( &m_cond, NULL ) != 0 )
	{
		pthread_mutex_destroy( &m_mutex );
		throw std::exception();
	}
}

cond::~cond()
{
	pthread_mutex_destroy( &m_mutex );
	pthread_cond_destroy( &m_cond );
}

/* 等待条件变量 */
bool cond::wait()
{
	int ret = 0;
	pthread_mutex_lock( &m_mutex );
	ret = pthread_cond_wait( &m_cond, &m_mutex );
	pthread_mutex_unlock( &m_mutex );
	return ret == 0;
}

/* 唤醒等待条件变量的线程 */
bool cond::signal()
{
	return pthread_cond_signal( &m_cond ) == 0;
}
