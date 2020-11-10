#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
using namespace std;

/*
spin lock already defined in posix

int pthread_spin_destroy(pthread_spinlock_t *);
int pthread_spin_init(pthread_spinlock_t *, int pshared);
int pthread_spin_lock(pthread_spinlock_t *);
int pthread_spin_trylock(pthread_spinlock_t *);
int pthread_spin_unlock(pthread_spinlock_t *);
*/

/*
user space spin lock
using atomic_flag
*/

class SpinLock
{
public:
	void lock()
	{
		while (sync.test_and_set(memory_order_acquire))
			;
	}

	void unlock()
	{
		sync.clear(memory_order_release);
	}

private:
	atomic_flag sync;
};

#endif