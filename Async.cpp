#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <vector>

template <typename RandomIt>
int parallel_sum(RandomIt beg, RandomIt end)
{
	auto len = end - beg;
	if (len < 1000)
		return std::accumulate(beg, end, 0);

	RandomIt mid = beg + len / 2;
	// runs a function asynchronously
	// (potentially in a new thread)
	// and returns a std::future that will hold the result
	auto handle = std::async(std::launch::async,
							 parallel_sum<RandomIt>, mid, end);
	int sum = parallel_sum(beg, mid);
	return sum + handle.get();
}

int main()
{
	std::vector<int> v(10000, 1);
	std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';
}