#ifndef _LOCK_FREE_QUEUE_H_
#define _LOCK_FREE_QUEUE_H_

#include <memory>
#include <mutex>
#include <queue>
using std::queue;
using std::shared_ptr;
using std::unique_lock;

#include "SpinLock.h"

template <typename T>
class threadsafe_queue
{
public:
	threadsafe_queue()
	{
	}

	void push(T new_value)
	{
		unique_lock<SpinLock> lock(splock);
		data_queue.push(std::move(new_value));
	}

	void wait_and_pop(T &value)
	{
		unique_lock<SpinLock> lock(splock);
		value = std::move(data_queue.front());
		data_queue.pop();
	}

	shared_ptr<T> wait_and_pop()
	{
		unique_lock<SpinLock> lock(splock);
		shared_ptr<T> res(make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}

	bool try_pop(T &value)
	{
		unique_lock<SpinLock> lock(splock);
		if (data_queue.empty())
			return false;
		value = std::move(data_queue.front());
		data_queue.pop();
		return true;
	}

	shared_ptr<T> try_pop()
	{
		unique_lock<SpinLock> lock(splock);
		if (data_queue.empty())
			return shared_ptr<T>();
		shared_ptr<T> res(make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}

	bool empty() const
	{
		unique_lock<SpinLock> lock(splock);
		return data_queue.empty();
	}

private:
	mutable SpinLock splock;
	queue<T> data_queue;
};

#endif