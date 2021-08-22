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
			
			Assert::AreEqual(true, pointEquals(expected, actual));

			Assert::AreEqual(true, pointEquals(expected, voxelizePoint(1.2, -0.4, 1.1249)));
		}
		TEST_METHOD(point_equals)
		{
			bool test = true;
			glm::ivec3 P0 = glm::ivec3(1, 0, 2);
			glm::ivec3 P1 = glm::ivec3(1, 0, 2);
			Assert::AreEqual(true, pointEquals(P0, P1));
			P1.x -= 1214;
			Assert::AreEqual(false, pointEquals(P0, P1));
		}
		TEST_METHOD(dom_axis)
		{
			glm::ivec3 P0 = glm::ivec3(0, 0, 0);
			glm::ivec3 P1 = glm::ivec3(0, 0, 1);
			glm::ivec3 P2 = glm::ivec3(0, 1, 0);
			axis expected = 0;
			Assert::AreEqual(expected, dominantAxis(P0, P1, P2));
		}
		TEST_METHOD(ilv_1d_list)
		{
			//1D
			glm::ivec3 P0 = glm::ivec3(0, 0, 0);
			glm::ivec3 P1 = glm::ivec3(0, 0, 5);
			std::forward_list<glm::ivec3> list;
			std::forward_list<glm::ivec3> expected = { 
				P0, 
				glm::ivec3(0, 0, 1), 
				glm::ivec3(0, 0, 2), 
				glm::ivec3(0, 0, 3), 
				glm::ivec3(0, 0, 4), 
				P1 };
			ILV(P0, P1, list);
			
			//Assert::AreEqual(expected.size(), list.size());
			auto it1 = list.begin();
			auto it2 = expected.begin();
			while (it1 != list.end() && it2 != expected.end())
			{
				Assert::AreEqual(pointEquals(*it2, *it1), true);
				it1++;
				it2++;
			}
		}
		TEST_METHOD(ilv_2d_list)
		{
			glm::ivec3 P0 = glm::ivec3(0, 0, 0);
			glm::ivec3 P1 = glm::ivec3(0, 2, 2);
			std::forward_list<glm::ivec3> list;
			std::forward_list<glm::ivec3> expected = {
				P0,
				glm::ivec3(0, 1, 0),
				glm::ivec3(0, 1, 1),
				glm::ivec3(0, 2, 1),
				P1 };
			ILV(P0, P1, list);
			//Assert::AreEqual(expected.size(), list.size());
			auto it1 = list.begin();
			auto it2 = expected.begin();
			while (it1 != list.end() && it2 != expected.end())
			{
				Assert::AreEqual(pointEquals(*it2, *it1), true);
				it1++;
				it2++;
			}
		}
		TEST_METHOD(ilv_3d_list)
		{
			glm::ivec3 P0 = glm::ivec3(0, 0, 0);
			glm::ivec3 P1 = glm::ivec3(2, 2, 2);
			std::forward_list<glm::ivec3> list;
			std::forward_list<glm::ivec3> expected = {
				P0,
				glm::ivec3(1, 0, 0),
				glm::ivec3(1, 1, 0),
				glm::ivec3(1, 1, 1),
				glm::ivec3(2, 1, 1),
				glm::ivec3(2, 2, 1),
				P1 };
			ILV(P0, P1, list);
			//Assert::AreEqual(expected.size(), list.size());
			auto it1 = list.begin();
			auto it2 = expected.begin();
			while (it1 != list.end() && it2 != expected.end())
			{
				Assert::AreEqual(pointEquals(*it2, *it1), true);
				it1++;
				it2++;
			}
		}
	};
}
