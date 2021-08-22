#include "pch.h"
#include "CppUnitTest.h"
#include "../block-on/src/voxelization.hpp"
#include "../cpp/include/extern/glm-master/glm/glm.hpp"
#include "../cpp/include/extern/glm-master/glm/gtc/type_ptr.hpp"
#include "../block-on/src/octree.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(voxelizeTriangle)
	{
	public:
		TEST_METHOD(voxelize_point)
		{
			glm::ivec3 expected = glm::ivec3(1, 0, 2);
			glm::ivec3 actual = voxelizePoint(glm::ivec3(1.2, -0.4, 1.1249));
			assert(pointEquals(expected, actual));

			assert(pointEquals(expected, voxelizePoint(1.2, -0.4, 1.1249)));
		}
		TEST_METHOD(point_equals)
		{
			glm::ivec3 P0 = glm::ivec3(1, 0, 2);
			glm::ivec3 P1 = glm::ivec3(1, 0, 2);
			assert(pointEquals(P0, P1));
			P1.x -= 1214;
			assert(!pointEquals(P0, P1));
		}
		TEST_METHOD(dom_axis)
		{
			glm::ivec3 P0 = glm::ivec3(0, 0, 0);
			glm::ivec3 P1 = glm::ivec3(0, 0, 1);
			glm::ivec3 P2 = glm::ivec3(0, 1, 0);
			axis expected = 0;
			assert(dominantAxis(P0, P1, P2) == 0);
		}
		TEST_METHOD(ilv)
		{
			glm::ivec3 P0 = glm::ivec3(0, 0, 0);
			glm::ivec3 P1 = glm::ivec3(0, 0, 5);
			std::list<glm::ivec3> list;
			std::list<glm::ivec3> expected = { P0, glm::ivec3(0, 0, 1), glm::ivec3(0,0,2), glm::ivec3(0,0,3), glm::ivec3(0,0,4), P1 };
			ILV(P0, P1, list);
			
		}
	};
}
