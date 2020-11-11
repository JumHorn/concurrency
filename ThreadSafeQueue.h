#ifndef _THREAD_SAFE_QUEUE_H_
#define _THREAD_SAFE_QUEUE_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
using std::condition_variable;
using std::mutex;
using std::queue;
using std::shared_ptr;
using std::unique_lock;

template <typename T>
class threadsafe_queue
{
public:
	threadsafe_queue()
	{
	}

	void push(T new_value)
	{
		unique_lock<mutex> lock(mut);
		data_queue.push(std::move(new_value));
		data_cond.notify_one();
	}

	void wait_and_pop(T &value)
	{
		unique_lock<mutex> lock(mut);
		data_cond.wait(lock, [this] { return !data_queue.empty(); });
		value = std::move(data_queue.front());
		data_queue.pop();
	}

	shared_ptr<T> wait_and_pop()
	{
		unique_lock<mutex> lock(mut);
		data_cond.wait(lock, [this] { return !data_queue.empty(); });
		shared_ptr<T> res(make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}

	bool try_pop(T &value)
	{
		unique_lock<mutex> lock(mut);
		if (data_queue.empty())
			return false;
		value = std::move(data_queue.front());
		data_queue.pop();
		return true;
	}

	shared_ptr<T> try_pop()
	{
		unique_lock<mutex> lock(mut);
		if (data_queue.empty())
			return shared_ptr<T>();
		shared_ptr<T> res(make_shared<T>(std::move(data_queue.front())));
		data_queue.pop();
		return res;
	}

	bool empty() const
	{
		unique_lock<mutex> lock(mut);
		return data_queue.empty();
	}

private:
	mutable mutex mut;
	queue<T> data_queue;
	condition_variable data_cond;
};

#endif