/*
this Semaphore class was implemented based on condition variable and mutex
*/
#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <condition_variable>
#include <mutex>
using namespace std;

class Semaphore
{
public:
	Semaphore(int permits = 1) : resource(permits) {}
	~Semaphore() {}

	void wait(int permits = 1)
	{
		unique_lock<mutex> lock(m);
		while (permits > resource)
		{
			cv.wait(lock);
		}
		resource -= permits;
		cv.notify_one();
	}

	void post(int permits = 1)
	{
		unique_lock<mutex> lock(m);
		resource += permits;
		cv.notify_one();
	}

private:
	int resource;
	condition_variable cv;
	mutex m;
};

#endif