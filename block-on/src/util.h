#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
/*
	bunch of utility math stuff
*/

int smallIntPow(int x, uint8_t p);
unsigned int size(std::forward_list<glm::ivec3> list);
std::string i3_to_string(glm::ivec3 P);
bool pointEquals(glm::ivec3 P0, glm::ivec3 P1);

/**
* Raise an integer to a small power,
* also no standard library function
* O(logn)
*/
int smallIntPow(int x, uint8_t p)
{
	if (p == 0) 
		return 1;
	if (p == 1) 
		return x;

	int tmp = smallIntPow(x, p / 2);
	if (p % 2 == 0) 
		return tmp * tmp;
	else 
		return x * tmp * tmp;
}

/**
* Returns the size of a forward list
* No function in standard library cuz I guess they weren't happy with an O(n) algorithm
* and didn't feel like storing it?
* 
* Only use for debugging cause this will be fairly slow
*/
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

/**
* Converts a P to a string for debuggin
*/
std::string i3_to_string(glm::ivec3 P)
{
	return "(" + std::to_string(P.x) + ", " +
		std::to_string(P.y) + ", " +
		std::to_string(P.z) + ")";
}

/**
* Returns true if the two voxels are the same
* glm::ivec3 P0, P1 - the two voxels being compared
*/
bool pointEquals(glm::ivec3 P0, glm::ivec3 P1)
{
	return P0.x == P1.x && P0.y == P1.y && P0.z == P1.z;
}
#endif // !UTIL_H
