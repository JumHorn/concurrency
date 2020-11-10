//C++11
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

//for asm atomic
#include "Atomic.h"
//for user space spin lock
#include "SpinLock.h"
using namespace std;

/*
效率测试 累加sum
结果说明
加锁执行任务时间短,spinlock效率高
加锁执行任务时间长,mutex效率高
*/
#define THREAD_NUM 100
SpinLock sp;
mutex m;
static int sum = 0;

void increase()
{
	++sum;
	cout << "thread id:" << this_thread::get_id()
		 << " sum: " << sum << endl;
	// this_thread::sleep_for(100ms);
}

void increase_using_mutex()
{
	unique_lock<mutex> lock(m);
	increase();
}

void increase_using_spinlock()
{
	lock_guard<SpinLock> lock(sp);
	increase();
}

int main()
{
	auto start = chrono::steady_clock::now();
	vector<thread> thread_using_mutex;
	for (int i = 0; i < THREAD_NUM; ++i)
		thread_using_mutex.push_back(thread(increase_using_mutex));
	for (auto &t : thread_using_mutex)
		t.join();
	auto end = chrono::steady_clock::now();
	cout << "using mutex time consuming: "
		 << chrono::duration_cast<chrono::microseconds>(end - start).count()
		 << "ms sum:" << sum << endl;

	sum = 0;
	start = chrono::steady_clock::now();
	vector<thread> thread_using_spinlock;
	for (int i = 0; i < THREAD_NUM; ++i)
		thread_using_spinlock.push_back(move(thread(increase_using_spinlock)));
	for (auto &t : thread_using_spinlock)
		t.join();
	end = chrono::steady_clock::now();
	cout << "using spinlock time consuming: "
		 << chrono::duration_cast<chrono::microseconds>(end - start).count()
		 << "ms sum:" << sum << endl;
	return 0;
}