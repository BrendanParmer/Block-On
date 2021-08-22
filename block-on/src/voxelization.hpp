/**
TODO
	-create custom vector class of uint16_t's for maybe half the memory? something to test out i suppose
	
	(separate header file so voxelization stays kinda general?)
	-implement voxel data structure to cubes function 
	-implement cross-section viewer
		-probably should optimize so we only draw the 2D component (4 vertices) instead of
		 the whole cube (8 vertices), since we only need to see the top anyways
			-might get tricky if we want a smooth transition from 2D to 3D
			-does this matter if we're ultimately making this a Blender add-on anyways?
				-let's be real this engine's pretty crappy lol
	-figure out way to visualize difference between -1, 0 (current), and +1 cross sections
		-probably involves using color and alpha channels

*/



#ifndef VOXELIZATION_H
#define VOXELIZATION_H

#include "../glm-master/glm/glm.hpp"
#include "../glm-master/glm/gtc/type_ptr.hpp"

#include <stdexcept>
#include <list>
#include <iostream>
#include <forward_list>

#include "octree.h"
#include "util.h"

typedef uint8_t axis; //x=0, y=1, z=2

void init(uint8_t setDepth);
void voxelizeTriangle(float x0, float y0, float z0,
					  float x1, float y1, float z1,
					  float x2, float y2, float z2);
//void voxelizeTriangle(glm::ivec3 p0, glm::ivec3 p1, glm::ivec3 p2);

glm::ivec3 voxelizePoint(glm::vec3 p);
glm::ivec3 voxelizePoint(float x, float y, float z);

bool pointEquals(glm::ivec3 P0, glm::ivec3 P1);

axis dominantAxis(glm::ivec3 P0, glm::ivec3 P1, glm::ivec3 P2);
void sortThreeIntPoints(glm::ivec3 P0, glm::ivec3 P1, glm::ivec3 P2, axis anAxis);

void ILV(glm::ivec3 P0, glm::ivec3 P1, std::forward_list<glm::ivec3> list);
void ILV(glm::ivec3 P0, glm::ivec3 P1, Octnode root);
int8_t sign(int num);
void ILV_2D(glm::ivec3 P0, glm::ivec3 P1, std::forward_list<glm::ivec3> list, axis w);
void ILV_2D(glm::ivec3 P0, glm::ivec3 P1, Octnode root, axis w);
void ILV_1D(glm::ivec3 P0, glm::ivec3 P1, std::forward_list<glm::ivec3> list, axis w);
void ILV_1D(glm::ivec3 P0, glm::ivec3 P1, Octnode root, axis w);

void fillInterior(std::forward_list<glm::ivec3> E1, 
				  std::forward_list<glm::ivec3> E2, 
				  glm::ivec3 P0, 
				  glm::ivec3 P2, 
				  axis domAxis);
std::forward_list<glm::ivec3> getSubSequence(std::forward_list<glm::ivec3>::iterator it, axis w, int compare);
bool lineCondition(glm::ivec3 point, axis w, int dU, int dV, int U, int V);

void addVoxelToOctree(glm::ivec3 P, uint8_t level, uint8_t depth, Octnode root);

int smallIntPow(int x, uint8_t p); //from util.h

std::forward_list<glm::ivec3> end();
std::string i3_to_string(glm::ivec3 P);






//variables
uint8_t depth = 6;
Octnode* root = new Octnode(glm::ivec3(0, 0, 0), NULL);
uint8_t MAX_DEPTH = 10;





void init(uint8_t setDepth)
{
	depth = setDepth;
	Octnode* root = new Octnode(glm::ivec3(0, 0, 0), NULL);
}

/*
* function that voxelizes a triangle given three floating point vertices, p0, p1, and p2
* the nine arguments correspond with the 9 floats needed to define the triangle
*/
void voxelizeTriangle(float x0, float y0, float z0,
					  float x1, float y1, float z1,
					  float x2, float y2, float z2)
{

	glm::ivec3 P0 = voxelizePoint(x0, y0, z0);
	glm::ivec3 P1 = voxelizePoint(x1, y1, z1);
	glm::ivec3 P2 = voxelizePoint(x2, y2, z2);
	
	std::cout << "Vertices voxelized\n";
	std::cout << "New vertices:\n" << "P0: " << i3_to_string(P0)
		<< "    P1: " << i3_to_string(P1)
		<< "    P2: " << i3_to_string(P2) << "\n";
	//check if any of our voxels are the same to save computation time
	if (pointEquals(P0, P1))
	{
		if (pointEquals(P0, P2))
		{
			std::cout << "All vertices are the same.\n";
			std::cout << "Adding point " << i3_to_string(P0) << " to octree\n";
			addVoxelToOctree(P0, 0, depth, *root);
		}
		else
		{
			std::cout << "Some points are equal\n";
			std::cout << "Creating line from " << i3_to_string(P0) << 
				" to " << i3_to_string(P2) << " ...\n";
			ILV(P0, P2, *root);
		}
		std::cout << "Finished\n";
	}
	else if (pointEquals(P0, P2) || pointEquals(P1, P2))
	{
		std::cout << "Some points are equal\n";
		std::cout << "Creating line from " << i3_to_string(P0) <<
			" to " << i3_to_string(P1) << " ...\n";
		ILV(P0, P1, *root);
		std::cout << "Finished\n";
	}
	else //our voxels are all distinct
	{
		std::cout << "Vertices are distinct\n";
		axis domAxis = dominantAxis(P0, P1, P2);
		std::cout << "Dominant axis: " << std::to_string(domAxis) << "\n";
		sortThreeIntPoints(P0, P1, P2, domAxis);
		std::cout << "Points are sorted\n\n";

		//establishes edge voxels in a linked list
		std::forward_list<glm::ivec3> E0;
		std::forward_list<glm::ivec3> E1;
		std::forward_list<glm::ivec3> E2;

		//Calculate edge voxels
		std::cout << "\nILV P0: " << i3_to_string(P0) << " and P1: " << i3_to_string(P1) << "\n";
		ILV(P0, P1, E0);
		std::cout << "\nILV P0: " << i3_to_string(P0) << " and P2: " << i3_to_string(P2) << "\n";
		ILV(P0, P2, E1);
		std::cout << "\nILV P1: " << i3_to_string(P1) << " and P2: " << i3_to_string(P2) <<"\n";
		ILV(P1, P2, E2);
		std::cout << "Edge voxels are calculated\n\n";
		
		//Add edge voxels to octree
		//was printing size but we don't need a doubly linked list
		/*std::cout << "E0.size() = " << std::to_string(E0.size()) << "\n";
		std::cout << "E1.size() = " << std::to_string(E1.size()) << "\n";
		std::cout << "E2.size() = " << std::to_string(E2.size()) << "\n";*/

		for (auto E0it = E0.begin(); E0it != E0.end(); ++E0it)
		{
			std::cout << "Adding point " << i3_to_string(*E0it) << " to octree\n";
			addVoxelToOctree(*E0it, 0, depth, *root);
		}
		std::cout << "Edge 0 voxels are added to octree\n";
		for (auto E1it = E1.begin(); E1it != E1.end(); ++E1it)
		{
			std::cout << "Adding point " << i3_to_string(*E1it) << " to octree\n";
			addVoxelToOctree(*E1it, 0, depth, *root);
		}
		std::cout << "Edge 1 voxels are added to octree\n";
		//creates hybrid edge of E0 and E2
		for (auto E2it = E2.begin(); E2it != E2.end(); ++E2it)
		{
			std::cout << "Adding point " << i3_to_string(*E2it) << " to octree\n";
			addVoxelToOctree(*E2it, 0, depth, *root);
			E0.push_front(*E2it);
		}
		std::cout << "Edge 2 voxels are added to octree\n\n";
		fillInterior(E0, E1, P0, P2, domAxis);
		std::cout << "Interior filled\n";
		std::cout << "Finished\n";
	}
}

/*
*	Function to assign a 3D vector composed of floats to a voxel
* 
*	glm::vec3 p - the point to be voxelized
*/
glm::ivec3 voxelizePoint(glm::vec3 p)
{
	return glm::ivec3(round(p.x),
					  round(p.y),
					  round(p.z));
}

/**
*	Function that voxelizes 3D point based by floats
*	float x,y,z - the coordinates
*/
glm::ivec3 voxelizePoint(float x, float y, float z)
{
	return glm::ivec3(round(x), round(y), round(z));
}

/**
* Returns true if the two voxels are the same
* glm::ivec3 P0, P1 - the two voxels being compared
*/
bool pointEquals(glm::ivec3 P0, glm::ivec3 P1)
{
	return P0.x == P1.x && P0.y == P1.y && P0.z == P1.z;
}

/*
*	approximates the dominant axis by taking the cross product of the 
*	integer approximations of the edge vectors, determined by voxelized
*	vertex vectors
* 
*	glm::ivec3 P0, P1, P2 - the voxelized triangle vertices
*/
axis dominantAxis(glm::ivec3 P0, glm::ivec3 P1, glm::ivec3 P2)
{
	glm::ivec3 E0 = P0 - P1;
	glm::ivec3 E1 = P0 - P2;

	//pseudo cross-product stuff
	int x = abs(E0.y * E1.z - E0.z * E1.y);
	int max = x;
	int y = abs(E0.z * E1.x - E0.x * E1.z);
	if (max < y)
		max = y;
	int z = abs(E0.x * E1.y - E0.y * E1.x);
	if (max < z)
		max = z;

	if (max == x) {
		return 0;
	}
	if (max == y)
	{
		return 1;
	}
	if (max == z)
	{
		return 2;
	}
	else
	{
		throw std::logic_error("Dominant Axis must be 0, 1, or 2");
		return 0;
	}
}

/*
	helper function that reassigns ordering based on a given axis
	
	glm::ivec3 P0, P1, P2 - the voxelized triangle vertices
	axis domAxis - the dominant axis of the triangle
*/
void sortThreeIntPoints(glm::ivec3 P0, glm::ivec3 P1, glm::ivec3 P2, axis domAxis)
{

	if (P0[domAxis] > P1[domAxis])
		std::swap(P0[domAxis], P1[domAxis]);
	if (P0[domAxis] > P2[domAxis])
		std::swap(P0[domAxis], P2[domAxis]);
	if (P1[domAxis] > P2[domAxis])
		std::swap(P1[domAxis], P2[domAxis]);
}

/*
* function that voxelizes a 3D line with integer endpoints
* not perfectly accurate, but more efficient than a floating point implementation
* 
* glm::ivec3 P0, P1, P2 - the voxelized triangle vertices
* std::list<glm::ivec3> list - the list that new voxels will be added to
*/
void ILV(glm::ivec3 P0, glm::ivec3 P1, std::forward_list<glm::ivec3> list)
{
	//difference between voxels
	glm::ivec3 dP = glm::ivec3(P1.x - P0.x,
		P1.y - P0.y,
		P1.z - P0.z);

	bool isLower = false;
	//check to see if this is lower than three dimensions
	for (int i = 0; i < 3; i++)
	{
		if (dP[i] == 0)
		{
			std::cout << "Dimension " << std::to_string(i) << " is the same\n";
			ILV_2D(P0, P1, list, i);
			isLower = true;
			break;
		}
	}

	if (!isLower)
	{
		//which direction to move currentP based on the axis
		glm::ivec3 sign_dP = glm::ivec3(sign(dP.x), sign(dP.y), sign(dP.z));

		unsigned int orig_distance = abs(dP.x) + abs(dP.y) + abs(dP.z); //testing junk

		//sorta a "slope" factor, see Zhang, et.al (2017) for more details
		glm::ivec3 M = glm::ivec3(abs(dP.y * dP.z),
			abs(dP.x * dP.z),
			abs(dP.x * dP.y));
		//distance to the next facet, scaled up for integer-only arithmetic
		//ex: T.x = the distance to the next yz-facet
		glm::ivec3 T = M;
		glm::ivec3 currentP = P0;
		unsigned int count = 0;
		while (!pointEquals(currentP, P1) && count < 100)
		{
			unsigned int distance = abs(P1.x - currentP.x) + abs(P1.y - currentP.y) + abs(P1.z - currentP.z);
			if (distance > orig_distance)
			{
				std::cout << "Error: Expect distance to decrease, but\n"
					<< "Original distance: " << std::to_string(orig_distance)
					<< "\nNew distance: " << std::to_string(distance) << "\n";
				std::cout << "P0: " << i3_to_string(P0)
					<< "\nP1: " << i3_to_string(P1)
					<< "\nCurrentP: " << i3_to_string(currentP) << "\n";
				std::logic_error("I don't know man");
			}
			//find axis with minimum distance to next voxel region face
			uint8_t min = 0;
			for (uint8_t i = 1; i < 3; i++)
			{
				if (T[i] < T[min])
					min = i;
			}
			std::cout << "Min axis is " << std::to_string(min) << " with T[min] = " << std::to_string(T[min]) << "\n";
			currentP[min] += sign_dP[min];
			//Reset T for next iteration
			T -= glm::ivec3(T[min], T[min], T[min]);
			T[min] = 2 * M[min];
			std::cout << "Adding " << i3_to_string(currentP) << " to list\n";
			list.push_front(currentP);
			count++;
		}
	}
}

/*
	version of ILV function that adds points directly to an octree rather than a list

	glm::ivec3 P0, P1 - voxelized endpoints of the line
	Octnode root - root of the main octree
*/
void ILV(glm::ivec3 P0, glm::ivec3 P1, Octnode root)
{
	//difference between voxels
	glm::ivec3 dP = glm::ivec3(P1.x - P0.x,
							   P1.y - P0.y,
							   P1.z - P0.z);

	//check to see if this is lower than three dimensions
	for (int i = 0; i < 3; i++)
	{
		if (dP[i] == 0)
		{
			ILV_2D(P0, P1, root, i);
			break;
		}
	}

	//which direction to move currentP based on the axis
	glm::ivec3 sign_dP = glm::ivec3(sign(dP.x), sign(dP.y), sign(dP.z)); 

	unsigned int orig_distance = abs(dP.x) + abs(dP.y) + abs(dP.z); //testing junk

	//sorta a "slope" factor, see Zhang, et.al (2017) for more details
	glm::ivec3 M = glm::ivec3(abs(dP.y * dP.z),
		abs(dP.x * dP.z),
		abs(dP.x * dP.y));
	//distance to the next facet, scaled up for integer-only arithmetic
	//ex: T.x = the distance to the next yz-facet
	glm::ivec3 T = M;
	glm::ivec3 currentP = P0;
	unsigned int count = 0;
	while (!pointEquals(currentP, P1) && count < 100)
	{
		unsigned int distance = abs(P1.x - currentP.x) + abs(P1.y - currentP.y) + abs(P1.z - currentP.z);
		if (distance > orig_distance)
		{
			std::cout << "Error: Expect distance to decrease, but\n"
				<< "Original distance: " << std::to_string(orig_distance)
				<< "\nNew distance: " << std::to_string(distance) << "\n";
			std::cout << "P0: " << i3_to_string(P0)
				<< "\nP1: " << i3_to_string(P1)
				<< "\nCurrentP: " << i3_to_string(currentP) << "\n";
			std::logic_error("Your logic's bad man");
		}
		//find axis with minimum distance to next voxel region face
		uint8_t min = 0;
		for (uint8_t i = 1; i < 3; i++)
		{
			if (T[i] < T[min])
				min = i;
		}
		std::cout << "Min axis is " << std::to_string(min) << " with T[min] = " << std::to_string(T[min]) << "\n";
		currentP[min] += sign_dP[min];
		//Reset T for next iteration
		T -= glm::ivec3(T[min], T[min], T[min]);
		T[min] = 2 * M[min];
		std::cout << "Adding " << i3_to_string(currentP) << " to octree\n";
		addVoxelToOctree(currentP, 0, depth, root);
		count++;
	}
}

/**
* Returns the sign function of an integer
*/
int8_t sign(int num)
{
	if (num > 0)
		return 1;
	else if (num < 0)
		return -1;
	else
		return 0;
}

/**
* 2D case of ILV algorithm
* glm::ivec3 P0, P1 - the endpoints of the line
* std::list<glm::ivec3> list - the list we are temporarily storing the voxels in
* axis w - the axis where P0 and P1 are aligned
*/
void ILV_2D(glm::ivec3 P0, glm::ivec3 P1, std::forward_list<glm::ivec3> list, axis w)
{
	//the other two axes
	axis u = (w + 1) % 3;
	axis v = (w - 1) % 3;

	//check if line will be one-dimensional
	if (P0[u] == P1[u])
	{
		std::cout << "Line is one-dimensional\n";
		ILV_1D(P0, P1, list, v);
	}
	else if (P0[v] == P1[v])
	{
		std::cout << "Line is one-dimensional\n";
		ILV_1D(P0, P1, list, u);
	}
	else
	{
		glm::ivec2 dP = glm::ivec2(P1[u] - P0[u], P1[v] - P0[v]);
		glm::ivec2 sign_dP = glm::ivec2(sign(dP[0]), sign(dP[1]));

		glm::ivec2 M = glm::ivec2(abs(dP[1]), abs(dP[0]));
		glm::ivec2 T = M;
		glm::ivec2 currentP = glm::ivec2(P0[u], P0[v]);
		unsigned int count = 0; //debugging stuff
		while (currentP[u] != P1[u] && currentP[v] != P1[v] && count < 100)
		{
			uint8_t min;
			if (T[v] > T[u])
				min = v;
			else
				min = u;
			currentP[min] += sign_dP[min];
			T -= glm::ivec2(T[min], T[min]);
			T[min] = 2 * M[min];
			
			glm::ivec3 newPoint;
			if (w == 0) //x
				newPoint = glm::ivec3(P0.x, currentP[u], currentP[v]);
			else if (w == 1) //y
				newPoint = glm::ivec3(currentP[v], P0.y, currentP[u]);
			else if (w == 2) //z
				newPoint = glm::ivec3(currentP[u], currentP[v], P0.z);
			else
				throw std::logic_error("Ope your axis was out of bounds");
			std::cout << "Adding " << i3_to_string(newPoint) << " to list\n";
			list.push_front(newPoint);
			count++;
		}
	}
}

/**
* 2D case of ILV algorithm
* glm::ivec3 P0, P1 - the endpoints of the line
* Octnode root - the root of the octree we are storing the voxels in
* axis w - the axis where P0 and P1 are aligned
*/
void ILV_2D(glm::ivec3 P0, glm::ivec3 P1, Octnode root, axis w)
{
	//the other two axes
	axis u = (w + 1) % 3;
	axis v = (w - 1) % 3;

	//check if line will be one-dimensional
	if (P0[u] == P1[u])
	{
		std::cout << "Line is one-dimensional\n";
		ILV_1D(P0, P1, root, v);
	}
	else if (P0[v] == P1[v])
	{
		std::cout << "Line is one-dimensional\n";
		ILV_1D(P0, P1, root, u);
	}
	else
	{
		glm::ivec2 dP = glm::ivec2(P1[u] - P0[u], P1[v] - P0[v]);
		glm::ivec2 sign_dP = glm::ivec2(sign(dP[0]), sign(dP[1]));

		glm::ivec2 M = glm::ivec2(abs(dP[1]), abs(dP[0]));
		glm::ivec2 T = M;
		glm::ivec2 currentP = glm::ivec2(P0[u], P0[v]);
		unsigned int count = 0;
		while (currentP[u] != P1[u] && currentP[v] != P1[v] && count < 100)
		{
			uint8_t min;
			if (T[v] > T[u])
				min = v;
			else
				min = u;
			currentP[min] += sign_dP[min];
			T -= glm::ivec2(T[min], T[min]);
			T[min] = 2 * M[min];

			glm::ivec3 newPoint;
			if (w == 0) //x
				newPoint = glm::ivec3(P0.x, currentP[u], currentP[v]);
			else if (w == 1) //y
				newPoint = glm::ivec3(currentP[v], P0.y, currentP[u]);
			else if (w == 2) //z
				newPoint = glm::ivec3(currentP[u], currentP[v], P0.z);
			std::cout << "Adding " << i3_to_string(newPoint) << " to octree\n";
			addVoxelToOctree(newPoint, 0, depth, root);
			count++;
		}
	}
}

/**
* 1D case of ILV algorithm
* glm::ivec3 P0, P1 - the endpoints of the line
* std::list<glm::ivec3> list - the list we are temporarily storing the voxels in
* axis w - the axis where P0 and P1 aren't aligned
*/
void ILV_1D(glm::ivec3 P0, glm::ivec3 P1, std::forward_list<glm::ivec3> list, axis w)
{
	if (!pointEquals(P0, P1)) //if the endpoints are the same, then it's already in the octree
	{
		std::cout << "Points are distinct\n";
		glm::ivec3 currentP = P0;
		while (currentP[w] != P1[w])
		{
			std::cout << "currentP is " << i3_to_string(currentP) << "\n";
			currentP[w] += sign(P1[w] - P0[w]);
			std::cout << "Adding " << i3_to_string(currentP) << " to list\n";
			list.push_front(currentP);
		}
	}
	else
		std::cout << "Points " << i3_to_string(P0) << " and " << i3_to_string(P1) << " are the same\n";
}

/**
* 1D case of ILV algorithm
* glm::ivec3 P0, P1 - the endpoints of the line
* Octnode root - the root of the octree we're storing our new voxels in
* axis w - the axis where P0 and P1 aren't aligned
*/
void ILV_1D(glm::ivec3 P0, glm::ivec3 P1, Octnode root, axis w)
{
	if (!pointEquals(P0, P1)) //if the endpoints are the same, then it's already in the octree
	{
		glm::ivec3 currentP = P0;
		while (currentP[w] != P1[w])
		{
			std::cout << "currentP is " << i3_to_string(currentP) << "\n";
			currentP[w] += sign(P1[w] - P0[w]);
			std::cout << "Adding " << i3_to_string(currentP) << " to octree\n";
			addVoxelToOctree(currentP, 0, depth, root);
		}
	}
}

/*
	function to fill the interior of the triangle

	it splices the triangle by the dominant axis, creating a bunch of 2D polygons
	then, it determines the best endpoints on opposite sides of the triangle
	so we don't have to call ILV() more times than we need to

	std::list<glm::ivec3> E0, E1 - the edges of the original triangle
		-note: E0 is a hybrid of the original E0 and E2
	glm::ivec3 P0, P2 - the two points connected directly by E1
	axis domAxis - the dominant axis of the triangle
*/
void fillInterior(std::forward_list<glm::ivec3> E0, 
				 std::forward_list<glm::ivec3> E1, 
				 glm::ivec3 P0, 
				 glm::ivec3 P2, 
				 axis domAxis)
{
	if (domAxis < 0 || domAxis > 2)
		throw std::invalid_argument("Dominant axis must be 0, 1, or 2");

	
	auto itE0 = E0.begin();
	auto itE1 = E1.begin();

	std::cout << "Starting to slice...\n";
	//splices triangle into 2D splices based on the dominant axis
	for (uint16_t i = 0; i < P2[domAxis] - P0[domAxis]; i++)
	{
		int slice = P0[domAxis] + i;
		std::cout << "Calculating at axis " << std::to_string(domAxis) << 
			": " << std::to_string(slice) << ".\n";
		std::cout << "Getting first subsequence...\n";
		std::forward_list<glm::ivec3> sliceE0 = getSubSequence(itE0, domAxis, slice);
		std::cout << "Getting second subsequence...\n";
		std::forward_list<glm::ivec3> sliceE1 = getSubSequence(itE1, domAxis, slice);

		auto itSliceE0 = sliceE0.begin();
		auto itSliceE1 = sliceE1.begin();
		
		std::cout << "Calculating line from " << i3_to_string(*itSliceE0) 
			<< " to " << i3_to_string(*itSliceE1) << ".\n";
		ILV(*itSliceE0, *itSliceE1, *root);
		
		glm::ivec3 last0 = *itSliceE0;
		glm::ivec3 last1 = *itSliceE1;

		axis u = (domAxis + 1) % 3;
		axis v = (domAxis - 1) % 3;
		int U0 = last0[u];
		int U1 = last1[u];
		int V0 = last0[v];
		int V1 = last1[v];

		while (itSliceE0 != sliceE0.end() && itSliceE1 != sliceE1.end())
		{
			if (lineCondition(*std::next(itSliceE0, 1), domAxis, U1 - U0, V1 - V0, U0, V0)) 
				std::advance(itSliceE0, 1);
			else if (lineCondition(*std::next(itSliceE1, 1), domAxis, U1 - U0, V1 - V0, U0, V0))
				std::advance(itSliceE1, 1);
			else
			{
				std::cout << "Calculating line from " << i3_to_string(*itSliceE0) 
					<< " to " << i3_to_string(*itSliceE1) << ".\n";
				ILV(*itSliceE0, *itSliceE1, *root);
				last0 = *itSliceE0;
				last1 = *itSliceE1;

				int U0 = last0[u];
				int U1 = last1[u];
				int V0 = last0[v];
				int V1 = last1[v];
			}

		}

		std::cout << "Calculating line from " << i3_to_string(*sliceE0.end()) 
			<< " to " << i3_to_string(*sliceE1.end()) << ".\n";
		ILV(*sliceE0.end(), *sliceE1.end(), *root);
	}
}

/*
	helper function to fillInterior() that gives subsequence of edge by the dominant axis

	std::list<glm::ivec3>::iterator it - an iterator over one of the triangle edges
	axis w - the dominant axis
	int compare - what w-level we are comparing it to
*/
std::forward_list<glm::ivec3> getSubSequence(std::forward_list<glm::ivec3>::iterator it, axis w, int compare)
{
	std::forward_list<glm::ivec3> subsequence;
	int i = 0; // strange bug where some triangles (randomly) appear to infinitely
			   // add voxels to a list, so this is just preventing that while
			   // I need to fix it
	std::cout << "Entering while loop\n";
	if (w == 0)
	{
		while (it->x == compare && i < 1000)
		{
			std::cout << "Adding " << i3_to_string(*it) << " to subsequence\n";
			subsequence.push_front(*it);
			it++;
			i++;
		}
	}
	else if (w == 1)
	{
		while (it->y == compare && i < 1000)
		{
			std::cout << "Adding " << i3_to_string(*it) << " to subsequence\n";
			subsequence.push_front(*it);
			it++;
			i++;
		}
	}
	else if (w == 2)
	{
		while (it->z == compare && i < 1000)
		{
			std::cout << "Adding " << i3_to_string(*it) << " to subsequence\n";
			subsequence.push_front(*it);
			it++;
			i++;
		}
	}
	return subsequence;
}

/*
	helper function to fillInterior() that determines which endpoints to draw lines at
	if the condition is true, then we can try the next point in the list

	glm::ivec3 point - the potential endpoint
	axis w - the dominant axis
	int dU, dV - the respective distances along the non-dominant axis of the last drawn line
	int U, V - the non-dominant axis coordinates of the last point on the edge we are testing on
*/
bool lineCondition(glm::ivec3 point, axis w, int dU, int dV, int U, int V)
{
	int m = dV * U - dU * V;
	int n = abs(dU) + abs(dV);
	
	int u, v;
	
	if (w == 0)
	{
		u = point.y;
		v = point.z;
	}
	else if (w == 1)
	{
		u = point.z;
		v = point.x;
	}
	else if (w == 2)
	{
		u = point.x;
		v = point.y;
	}
	else
		throw std::invalid_argument("Axis w must be 0, 1, or 2");

	int compare = dV * u - dU * v;
	return (m - n <= compare && compare <= m + n);
}

//probably should put in octree class
/*
	adds a voxel to an octree

	glm::ivec3 P: coordinates of the voxel
	uint8_t depth: user-defined depth of the octree
	Octnode root: the root of our octree, used for easy level of detail calculations
*/
void addVoxelToOctree(glm::ivec3 P, uint8_t level, uint8_t depth, Octnode main)
{
	/**
	bitwise little thing to keep track of which octant our point is in
	relative to the main node's coordinate
	
	 x = 4 (100), y = 2 (010), and z = 1 (001)
	if the point's x-coordinate is greater than the main node's, then we put a 1 in the 4 spot
	this helps us avoid messy, long chains of if statements
	*/
	//std::cout << "Setting up initial variable a...\n";
	uint8_t a = 0;

	//std::cout << "Level is: " << std::to_string(level) << " out of " << std::to_string(depth) << "\n";
	if (level < depth)
	{
		//determines relative octant and sets a new point
		//std::cout << "Determining half...\n";
		int half = smallIntPow(2, depth - 1 - level);
		//std::cout << "Setting up newPoint...\n";
		glm::ivec3 newPoint = main.coordinate;

		//std::cout << "Determining x-axis placement...\n";
		//x
		if (P.x > main.coordinate.x)
		{
			a |= 4;
			newPoint.x += half;
		}
		else
			newPoint.x -= half;

		//std::cout << "Determining y-axis placement...\n";
		//y
		if (P.y > main.coordinate.y)
		{
			a |= 2;
			newPoint.y += half;
		}
		else
			newPoint.y -= half;
		
		//std::cout << "Determining z-axis placement...\n";
		//z
		if (P.z > main.coordinate.z)\
		{
			a |= 1;
			newPoint.z += half;
		}
		else
			newPoint.z -= half;

		//std::cout << std::to_string(a) << '\n';
		//std::cout << i3_to_string(main.coordinate) << '\n';
		//assign child
		//std::cout << "Assigning child...\n";
		if (main.children[a] == nullptr)
		{
			//std::cout << "Creating new Octnode for child...\n";
			main.children[a] = new Octnode(newPoint, &main);
			//std::cout << i3_to_string(main.children[a]->coordinate) << "\n";
			//std::cout << "Creation successful\n";
		}
		//std::cout << "Onto the next layer in the tree\n";
		addVoxelToOctree(P, level + 1, depth, *main.children[a]);
	}
}

std::forward_list<glm::ivec3> end()
{
	std::forward_list<glm::ivec3> list;
	(*root).allPointsAtDepth(list, depth);
	return list;
}

std::string i3_to_string(glm::ivec3 P)
{
	return "(" + std::to_string(P.x) + ", " + 
				 std::to_string(P.y) + ", " + 
				 std::to_string(P.z) + ")";
}
#endif