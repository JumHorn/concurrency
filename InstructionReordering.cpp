#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

/*
cpu将不相关的指令重排，只保证单线程的最终一致性
多线程编程会出现难以发现的错误
*/

int x = 0, y = 0;
int a = 0, b = 0;

void worker1()
{
	this_thread::sleep_for(0us);
	a = 1;
	x = b;
}

void worker2()
{
	b = 1;
	y = a;
}

int main()
{
	for (int i = 0; true; ++i)
	{
		x = y = a = b = 0;
		thread t1(worker1), t2(worker2);
		t1.join();
		t2.join();
		cout << "turn: " << i << " x:" << x << " y:" << y << endl;
		assert(x != 0 || y != 0);
	}
	return 0;
}