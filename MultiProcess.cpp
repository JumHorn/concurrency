/**
 * @file MultiProcess.cpp
 * @author JumHorn (JumHorn@gamil.com)
 * @brief 测试多进程
 * @version 0.1
 * @date 2020-12-02
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <iostream>
#include <unistd.h>
using namespace std;

/**
 * @brief copy on write
 * @brief 两个进程不共享这个全局变量,相互之间不影响
 */
static int global = 0;

int main()
{
	/* Fork off the parent process */
	int pid = fork();
	if (pid < 0)
	{
		exit(EXIT_FAILURE);
	}
	/* this is parent */
	if (pid > 0)
	{
		global = pid;
		for (int i = 0; i < 10; ++i)
		{
			sleep(1);
			cout << global << endl;
		}
	}
	else //(pid == 0)this is child
	{
		for (int i = 0; i < 10; ++i)
		{
			sleep(1);
			cout << global << endl;
		}
	}
	return 0;
}