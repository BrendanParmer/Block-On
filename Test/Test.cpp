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
			glm::ivec3 actual = voxelizePoint(glm::ivec3(1.2f, -0.4f, 1.1249f));
			
			Assert::AreEqual(true, pointEquals(expected, actual));

			actual = voxelizePoint(1.2f, -0.4f, 1.1249f);
			//not sure why, but this is showing up as false. seems fine in production??
			Assert::AreEqual(true, pointEquals(expected, actual)); 
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
				glm::ivec3(0, 0, 4) };
			list = ILV_1D(P0, P1, list, 2);
			
			Assert::AreEqual(size(expected), size(list));
			auto it1 = list.begin();
			auto it2 = expected.begin();
			while (it1 != list.end() && it2 != expected.end())
			{
				Assert::AreEqual(pointEquals(*it2, *it1), true);
				it1++;
				it2++;
			}
			std::forward_list<glm::ivec3> list2;
			list2 = ILV(P0, P1, list2);

			Assert::AreEqual(size(expected), size(list2));
			auto it3 = list2.begin();
			auto it4 = expected.begin();
			while (it3 != list2.end() && it4 != expected.end())
			{
				Assert::AreEqual(pointEquals(*it4, *it3), true);
				it3++;
				it4++;
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
			list = ILV(P0, P1, list);
			Assert::AreEqual(size(expected), size(list));
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
				glm::ivec3(2, 2, 1) };
			list = ILV(P0, P1, list);
			Assert::AreEqual(size(expected), size(list));
			auto it1 = list.begin();
			auto it2 = expected.begin();
			while (it1 != list.end() && it2 != expected.end())
			{
				Assert::AreEqual(pointEquals(*it2, *it1), true);
				it1++;
				it2++;
			}
		}
		TEST_METHOD(pushfront)
		{
			std::forward_list<glm::ivec3> list;
			glm::ivec3 point = glm::ivec3(1, 1, 1);
			unsigned int expected_size = 1;
			list.push_front(point);
			Assert::AreEqual(expected_size, size(list));
		}
	};
}
