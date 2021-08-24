#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
/*
	bunch of utility math stuff
*/

int smallIntPow(int x, uint8_t p);



int smallIntPow(int x, uint8_t p)
{
	if (p == 0) return 1;
	if (p == 1) return x;

	int tmp = smallIntPow(x, p / 2);
	if (p % 2 == 0) return tmp * tmp;
	else return x * tmp * tmp;
}
unsigned int size(std::forward_list<glm::ivec3> list)
{
	auto it = list.begin();
	unsigned int sum = 0;
	while (it != list.end())
	{
		sum++;
		it++;
	}
	return sum;
}
#endif // !MATH_H
