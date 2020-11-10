#include <mutex>
using namespace std;

/*
线程安全单例
double check(双重加锁)
单例创建成功后，第一个null判断可以提高效率，后面的线程不会在有锁
多个线程同时进入第一个null判断后，需要加锁值允许一个线程创建单例
*/

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
