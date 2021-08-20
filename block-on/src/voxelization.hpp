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

#include "octree.h"
#include "math.h"

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

void ILV(glm::ivec3 P0, glm::ivec3 P1, std::list<glm::ivec3> list);
void ILV(glm::ivec3 P0, glm::ivec3 P1, Octnode root);

void fillInterior(std::list<glm::ivec3> E1, 
				  std::list<glm::ivec3> E2, 
				  glm::ivec3 P0, 
				  glm::ivec3 P2, 
				  axis domAxis);
std::list<glm::ivec3> getSubSequence(std::list<glm::ivec3>::iterator it, axis w, int compare);
bool lineCondition(glm::ivec3 point, axis w, int dU, int dV, int U, int V);

void addVoxelToOctree(glm::ivec3 P, uint8_t level, uint8_t depth, Octnode root);

int smallIntPow(int x, uint8_t p); //from math.h

std::list<glm::ivec3> end();
std::string i3_to_string(glm::ivec3 P);

//variables
uint8_t depth = 6;
Octnode* root = new Octnode(glm::ivec3(0, 0, 0), NULL);
uint8_t MAX_DEPTH = 10;

void init(uint8_t setDepth = 6)
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
	std::cout << "New vertices\n" << "P0: " << i3_to_string(P0)
		<< "\nP1: " << i3_to_string(P1)
		<< "\nP2: " << i3_to_string(P2) << "\n";
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
		std::cout << "Determined dominant axis\n";
		sortThreeIntPoints(P0, P1, P2, domAxis);
		std::cout << "Points are sorted\n\n";

		//establishes edge voxels in a linked list
		std::list<glm::ivec3> E0;
		std::list<glm::ivec3> E1;
		std::list<glm::ivec3> E2;

		ILV(P0, P1, E0);
		ILV(P0, P2, E1);
		ILV(P1, P2, E2);
		std::cout << "Edge voxels are calculated\n";

		for (const glm::ivec3& x : E0) //these aren't running
		{
			std::cout << "Adding point " << i3_to_string(x) << " to octree\n";
			addVoxelToOctree(x, 0, depth, *root);
		}
		std::cout << "Edge 0 voxels are added to octree\n";
		for (const glm::ivec3& x : E1)
		{
			std::cout << "Adding point " << i3_to_string(x) << " to octree\n";
			addVoxelToOctree(x, 0, depth, *root);
		}
		std::cout << "Edge 1 voxels are added to octree\n";
		//creates hybrid edge
		for (const glm::ivec3& x : E2)
		{
			std::cout << "Adding point " << i3_to_string(x) << " to octree\n";
			addVoxelToOctree(x, 0, depth, *root);
			E0.push_back(x);
		}
		std::cout << "Edge 2 voxels are added to octree\n\n";
		fillInterior(E0, E1, P0, P2, domAxis);
		std::cout << "Interior filled\n";
		std::cout << "Finished\n";
	}
}

/**
* Overloaded function that allows for glm::vec3 inputs
*/
/*void voxelizeTriangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
	voxelizeTriangle(p0.x, p0.y, p0.z,
					 p1.x, p1.y, p1.z,
					 p2.x, p2.y, p2.z);
}*/

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

	int x = E0.y * E1.z - E0.z * E1.y;
	int max = x;
	int y = E0.z * E1.x - E0.x * E1.z;
	if (max < y)
		max = y;
	int z = E0.x * E1.y - E0.y * E1.x;

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
		throw std::invalid_argument("Dominant Axis must be 0, 1, or 2");
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
void ILV(glm::ivec3 P0, glm::ivec3 P1, std::list<glm::ivec3> list)
{
	glm::ivec3 dP = glm::ivec3(P1.x - P0.x,
							   P1.y - P0.y,
							   P1.z - P0.z);
	glm::ivec3 M = glm::ivec3(abs(dP.y * dP.z),
							  abs(dP.x * dP.z),
							  abs(dP.x * dP.y));
	glm::ivec3 L = M;
	glm::ivec3 currentP = P0;
	while (currentP.x != P1.x && currentP.y != P1.y && currentP.z != P1.z) //probably should do this better
	{
		//find axis with minimum distance to next voxel region face
		unsigned int min = 0;
		for (int i = 1; i < 3; i++) //worth it to even have a for loop?
		{
			if (L[i] > L[min])
				min = i;
		}
		currentP[min] += dP[min];
		L -= glm::ivec3(L[min], L[min], L[min]);
		L[min] = 2 * M[min];
		list.push_back(currentP);
	}
}

/*
	version of ILV function that adds points directly to an octree rather than a list

	glm::ivec3 P0, P1 - voxelized endpoints of the line
	Octnode root - root of the main octree
*/
void ILV(glm::ivec3 P0, glm::ivec3 P1, Octnode root)
{
	glm::ivec3 dP = glm::ivec3(P1.x - P0.x,
		P1.y - P0.y,
		P1.z - P0.z);
	glm::ivec3 M = glm::ivec3(abs(dP.y * dP.z),
		abs(dP.x * dP.z),
		abs(dP.x * dP.y));
	glm::ivec3 L = M;
	glm::ivec3 currentP = P0;
	while (currentP.x != P1.x && currentP.y != P1.y && currentP.z != P1.z)
	{
		//find axis with minimum distance to next voxel region face
		unsigned int min = 0;
		for (int i = 1; i < 3; i++) //worth it to even have a for loop?
		{
			if (L[i] > L[min])
				min = i;
		}
		currentP[min] += dP[min];
		L -= glm::ivec3(L[min], L[min], L[min]);
		L[min] = 2 * M[min];
		std::cout << "Adding point " << i3_to_string(currentP) << " to octree\n";
		addVoxelToOctree(currentP, 0, depth, root);
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
void fillInterior(std::list<glm::ivec3> E0, 
				 std::list<glm::ivec3> E1, 
				 glm::ivec3 P0, 
				 glm::ivec3 P2, 
				 axis domAxis)
{
	if (domAxis < 0 || domAxis > 2)
		throw std::invalid_argument("Dominant axis must be 0, 1, or 2");

	
	std::list<glm::ivec3>::iterator itE0 = E0.begin();
	std::list<glm::ivec3>::iterator itE1 = E1.begin();

	std::cout << "Starting to slice...\n";
	//splices triangle into 2D splices based on the dominant axis
	for (uint16_t i = 0; i < P2[domAxis] - P0[domAxis]; i++)
	{
		int slice = P0[domAxis] + i;
		std::cout << "Calculating at axis " << std::to_string(domAxis) << std::to_string(slice) << ".\n";
		std::cout << "Getting first subsequence...\n";
		std::list<glm::ivec3> sliceE0 = getSubSequence(itE0, domAxis, slice);
		std::cout << "Getting second subsequence...\n";
		std::list<glm::ivec3> sliceE1 = getSubSequence(itE1, domAxis, slice);

		std::list<glm::ivec3>::iterator itSliceE0 = sliceE0.begin();
		std::list<glm::ivec3>::iterator itSliceE1 = sliceE1.begin();
		
		std::cout << "Calculating line from " << i3_to_string(*itSliceE0) 
			<< " to " << i3_to_string(*itSliceE1) << ".\n";
		ILV(*itSliceE0, *itSliceE1, *root);
		
		glm::ivec3 last0 = *itSliceE0;
		glm::ivec3 last1 = *itSliceE1;

		int U0, V0, U1, V1;
		if (domAxis == 0) //x
		{
			U0 = last0.y;
			V0 = last0.z;
			U1 = last1.y;
			V1 = last1.z;
		}
		else if (domAxis == 1) //y
		{
			U0 = last0.z;
			V0 = last0.x;
			U1 = last1.z;
			V1 = last1.x;
		}
		else if (domAxis == 2) //z
		{
			U0 = last0.x;
			V0 = last0.y;
			U1 = last1.x;
			V1 = last1.y;
		}
		else
			throw std::invalid_argument("Dominant Axis must be 0, 1, or 2");

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

				//probably should condense into a function
				if (domAxis == 0) //x
				{
					U0 = last0.y;
					V0 = last0.z;
					U1 = last1.y;
					V1 = last1.z;
				}
				else if (domAxis == 1) //y
				{
					U0 = last0.z;
					V0 = last0.x;
					U1 = last1.z;
					V1 = last1.x;
				}
				else if (domAxis == 2) //z
				{
					U0 = last0.x;
					V0 = last0.y;
					U1 = last1.x;
					V1 = last1.y;
				}
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
std::list<glm::ivec3> getSubSequence(std::list<glm::ivec3>::iterator it, axis w, int compare)
{
	std::list<glm::ivec3> subsequence;
	std::cout << "Dominant axis: " << std::to_string(w) << '\n';
	int i = 0; // strange bug where some triangles (randomly) appear to infinitely
			   // add voxels to a list, so this is just preventing that while
			   // I need to fix it
			   
	//not happy with this, it's not very clean
	if (w == 0) //dom axis = x
	{
		while (it->x == compare && i < 4000)
		{
			subsequence.push_back(*it);
			std::advance(it, 1);
			i++;
		}
	}
	else if (w == 1) //dom axis = y
	{
		while (it->y == compare && i < 4000)
		{
			subsequence.push_back(*it);
			std::advance(it, 1);
			i++;
		}
	}
	else if (w == 2) //dom axis = z
	{
		while (it->z == compare && i < 4000)
		{
			subsequence.push_back(*it);
			std::advance(it, 1);
			i++;
		}
	}
	else
		throw std::invalid_argument("Axis w must be 0, 1, or 2");

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

/*
	adds a voxel to an octree

	glm::ivec3 P: coordinates of the voxel
	uint8_t depth: user-defined depth of the octree
	Octnode root: the root of our octree, used for easy level of detail calculations
*/
void addVoxelToOctree(glm::ivec3 P, uint8_t level, uint8_t depth, Octnode main)
{
	//bitwise little thing to keep track of which octant our point is in 
	//relative to the main node's coordinate
	
	// x = 4 (100), y = 2 (010), and z = 1 (001)
	//if the point's x-coordinate is greater than the main node's, then we put a 1 in the 4 spot
	//this helps us avoid messy, long chains of if statements
	//std::cout << "Setting up initial variable a...\n";
	uint8_t a = 0;

	//std::cout << "Level is: " << level << " out of " << depth << "\n";
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

		//assign child
		//std::cout << "Assigning child...\n";
		if (main.children[a] == NULL)
		{
			//std::cout << "Creating new Octnode for child...\n";
			main.children[a] = new Octnode(newPoint, &main);
			//std::cout << "Creation successful\n";
		}
		//std::cout << "Onto the next layer in the tree\n";
		addVoxelToOctree(P, level + 1, depth, *main.children[a]);
	}
}

std::list<glm::ivec3> end()
{
	std::list<glm::ivec3> list;
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