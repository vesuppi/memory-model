#include <iostream>
#include <atomic>
#include <thread>
#include <cassert>
using namespace std;

int x, y, a, b;

int test1() {
	x = y = 0;
	a = b = 0;
	__sync_synchronize();
	assert(x == 0 && y == 0 && a == 0 && b == 0);

	auto proc1 = [&]() {
		x = 1;
		asm volatile("" ::: "memory");
		//__sync_synchronize();
		if (y == 0) {      
			a = 1;
		}
	};

	auto proc2 = [&]() {
		y = 1;
		asm volatile("" ::: "memory");
		//__sync_synchronize();
		if (x == 0) {
			b = 1;
		}
	};

	thread t1(proc1);
	thread t2(proc2);

	t1.join();
	t2.join();

	__sync_synchronize();
	if (a == 1 && b == 1) {
		printf("got x == 0 && y == 0\n");
		return 1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	for (int i = 0; i < 100000000; i++) {
		if (test1() == 1) {
			printf("i: %d\n", i);
		}
	}

	return 1;
}
