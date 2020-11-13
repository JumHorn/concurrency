# 并发编程 Concurrency Programming

## 锁
1. 互斥锁(mutex)
2. 共享锁(shared_mutex)

	一写多读场景下使用shared_mutex

2. 条件锁(condition_variable)

```C++
//release lock and blocked then unblocked and reaquire lock
cv.wait(lk, [] { return i == 1; });
//blocked waiting for notify
```

3. 自旋锁(spinlock)

在C++中用atomic_flag实现
```C++
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
```

4. 无锁(lock-free)

	无锁队列

## 不同场景下锁的效率

## 原子操作
1. X86架构汇编

```C++
static int lxx_atomic_add(int *ptr, int increment)
{
	int old_value = *ptr;
	__asm__ volatile("lock; xadd %0, %1 \n\t"
					 : "=r"(old_value), "=m"(*ptr)
					 : "0"(increment), "m"(*ptr)
					 : "cc", "memory");
	return *ptr;
}
```


## 案例1
1. 指令重排 (Instruction Reordering)
2. 内存屏障（Memory Barrier)
3. kfifo

## 案例2
1. 单例双重加锁

```C++
class Singleton
{
public:
	Singleton *getInstance()
	{
		if (instance == nullptr)
		{
			//use unique_lock and scope_lock instead of lock_guard
			unique_lock<mutex> lock(m_mutex);
			if (instance == nullptr)
				instance = new Singleton();
		}
		return instance;
	}

private:
	Singleton() {}
	~Singleton() {}

	static Singleton *instance;
	static mutex m_mutex;
};

Singleton *Singleton::instance = nullptr;
```

## C++线程妙用
1. C++ std::async