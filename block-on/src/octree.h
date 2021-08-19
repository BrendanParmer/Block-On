#ifndef OCTREE_H
#define OCTREE_H

#include "../glm-master/glm/glm.hpp"
#include "../glm-master/glm/gtc/type_ptr.hpp"

class Octnode
{
public:
	glm::ivec3 coordinate;
	Octnode* parent;
	//probably better way of doing this
	Octnode* node0; // (-x, -y, -z)
	Octnode* node1; // (-x, -y, +z)
	Octnode* node2; // (-x, +y, -z)
	Octnode* node3; // (-x, +y, +z)
	Octnode* node4; // (+x, -y, -z)
	Octnode* node5; // (+x, -y, +z)
	Octnode* node6; // (+x, +y, -z)
	Octnode* node7; // (+x, +y, +z)

	Octnode* children[8] = {
		node0,
		node1,
		node2,
		node3,
		node4,
		node5,
		node6,
		node7
	};

	Octnode(glm::ivec3 setCoordinate, Octnode* setParent)
	{
		coordinate = setCoordinate;
		parent = setParent;

		node0 = nullptr;
		node1 = nullptr;
		node2 = nullptr;
		node3 = nullptr;
		node4 = nullptr;
		node5 = nullptr;
		node6 = nullptr;
		node7 = nullptr;
	}

	std::list<glm::ivec3> allPointsAtDepth(uint8_t depth)
	{
	}
};

#endif