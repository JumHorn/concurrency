//C++11
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

//for ThreadSafeQueue
#include "ThreadSafeQueue.h"
//for LockFreeQueue
#include "LockFreeQueue.h"
using namespace std;

/*
效率测试 一写一读两个线程
结果说明
*/

//读写总次数
#define ACCUMULATE 10000

template <typename T>
void producer(T &queue)
{
	for (int i = 0; i < ACCUMULATE; ++i)
		queue.push(i);
}

template <typename T>
void consumer(T &queue)
{
	for (int i = 0; i < ACCUMULATE; ++i)
	{
		int value;
		queue.wait_and_pop(value);
		assert(i == value);
	}
}

int main()
{
	lockfree_queue<int> lfqueue;
	threadsafe_queue<int> thsqueue;

	//for threadsafe queue
	auto start = chrono::steady_clock::now();
	thread threadsafe_consumer(consumer<threadsafe_queue<int>>, std::ref(thsqueue));
	thread threadsafe_producer(producer<threadsafe_queue<int>>, std::ref(thsqueue));
	threadsafe_consumer.join();
	threadsafe_producer.join();
	auto end = chrono::steady_clock::now();
	cout << "threadsafe queue time consuming: "
		 << chrono::duration_cast<chrono::microseconds>(end - start).count()
		 << "ms" << endl;

	//for lockfree queue
	start = chrono::steady_clock::now();
	thread lockfree_consumer(consumer<lockfree_queue<int>>, std::ref(lfqueue));
	thread lockfree_producer(producer<lockfree_queue<int>>, std::ref(lfqueue));
	lockfree_consumer.join();
	lockfree_producer.join();
	end = chrono::steady_clock::now();
	cout << "lockfree queue time consuming: "
		 << chrono::duration_cast<chrono::microseconds>(end - start).count()
		 << "ms" << endl;
	return 0;
}