/*
lock用于锁定前端串行总线FSB
*/

static int lxx_atomic_add(int *ptr, int increment)
{
	int old_value = *ptr;
	__asm__ volatile("lock; xadd %0, %1 \n\t"
					 : "=r"(old_value), "=m"(*ptr)
					 : "0"(increment), "m"(*ptr)
					 : "cc", "memory");
	return *ptr;
}