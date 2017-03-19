// vim: sw=4 ts=4 : //
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <climits>
#include <stack>
#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/program_options.hpp>
#include <boost/graph/simple_point.hpp>
#include <boost/graph/metric_tsp_approx.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>

/**
 * Function: christofides
 * Description: Implements the christofides algorithm for approximating TSP
 * Parameters: a vector of points that store x and y coordinates, 
 * an int for the number of 
 * vertices, and a pointer to store a new array with the tour stored in it.
 * Pre-conditions: vector stores multiple points with initialized values
 * Post-Conditions: Route for the salesman has been determined, along with
 * solution. The route is stored as an array in the pointer that was passed
 * as a parameter.
 */
template<typename PositionVector>
int christofides(PositionVector *position_vec, int numVertices, int *arr) {
	/*
	The following will find a minimum-spanning tree using prim's algorithm
	Note that boost has a function for this so feel free to replace
	so long as that functions output can be adapted to the rest of the
	program
	*/

	int parentVertex[numVertices];
	int vertexKey[numVertices];
	int inTree[numVertices];

	int minIndex;
	int currentMin;

	for (int i = 0; i < numVertices; i++) 
	{
		//vertex's key attribute set to MAX_INT
		vertexKey[i] = INT_MAX;
		//vertex has parent attribute set to NULL
		parentVertex[i] = 0;
	}
	
	vertexKey[0] = 0;
	parentVertex[0] = -1;
	int stopPoint = numVertices - 1;
	int distance;
	
	for (int i = 0; i < stopPoint; i++)
	{
		currentMin = INT_MAX;

		for (int u = 0; u < numVertices; u++) 
		{
			if ((vertexKey[u] < currentMin) && inTree[u] != true)
			{
				minIndex = u;
				currentMin = vertexKey[u];
			}
		}

		inTree[minIndex] = true;

		for (int j = 0; j < numVertices; j++)
		{
			//TODO: Implement dist function to find edge length

			distance = dist(position_vec[i], position_vec[j]);

			if (inTree[j] != true && distance < vertexKey[j])
			{
				parentVertex[j] = minIndex;
				vertexKey[j] = distance;
			}
		}
	}
	


	//Construct new adjacency list, but edges are
	//between a vertex and whichever is in its parent attribute
	//Next 28 lines adapted from pseudocode at 
	//https://github.com/beckysag/traveling-salesman/

	std::vector< std::vector<int> > mst(numVertices);
	int u;

	for (int v = 0; v < numVertices; v++)
	{
		u = parentVertex[v];
		if (u != 1)
		{
			mst[u].push_back(v);
			mst[v].push_back(u);
		}
	}

	//Next, we find a set of Odd vertices
	std::vector<int> odd;
	int degree;
	
	for (int i = 0; i < numVertices; i++)
	{
		degree = mst[i].size();
		
		if (degree % 2 != 0) 
		{
			odd.push_back(i);
		}
	}
	
	/*
	Next, find minimum weight matching on vertices from Odd, adding
	them to MST s you do so.
	*/
	int match;
	std::vector<int>::iterator start;
	std::vector<int>::iterator minDistance;
	std::vector<int>::iterator last;
	std::vector<int>::iterator current;

	//intialize match, a variable storing a vertex
	//while Odd isn't empty
	while (odd.size())
	{
		//remove vertex from odd, storing it in variable x
		start = odd.begin();
		distance = INT_MAX;
		last = odd.end();
		current = odd.begin() + 1;
		
		while (current != last)
		{
			int testDistance = dist(position_vec[*start], position_vec[*current]);
			if (testDistance < distance) 
			{
				distance = testDistance;
				match = *current;
				minDistance = current;
			}

			++current;
		}

		//create edge between x and match
		mst[match].push_back(*start);
		mst[*start].push_back(match);
		//remove match from Odd
		odd.erase(start);
		odd.erase(minDistance);
	}

	/**
	 * Next, make Euler tour. Create a stack for vertices and an int vector called tour
	 * Initialize a variable called current, set equal to start
	 * put current in int vector
	 * Next is a do-while loop that iterates while stack isn't empty or
	 * current's spot on the adjacency list isn't empty.
	 * In this loop check if current's adjacency list is empty. If not
	 * push current onto the stack and assign its first neighbor to
	 * be the new current. Then, remove the edge between the two.
	 * If current's adjacency list was empty, however, add current to
	 * the tour, assign the top vertex from stack to current and remove
	 * it from the stack. Adapted from http://www.graph-magics.com/articles/euler.php
	 */

	std::vector<int> euler;
	std::stack<int> eStack;

	int currentVertex = 0;
	int tempIndex;
	int size;

	do
	{
		if (mst[currentVertex].size() != 0)
		{
			tempIndex = currentVertex;
			eStack.push(tempIndex);
			currentVertex = mst[tempIndex].back(); //get first neighbor
			mst[tempIndex].pop_back();
			size = mst[currentVertex].size();

			//TODO:rewrite loop as while loop to avoid need for break statement
			for (int i = 0; i < size; i++)
			{
				if (mst[currentVertex][i] == tempIndex)
				{
					mst[currentVertex].erase(mst[currentVertex].begin() + i);
					break; 
				}
			}
		}
		else
		{
			euler.push_back(currentVertex);
			currentVertex = eStack.top();
			eStack.pop();
		}
	} 
	while ((mst[currentVertex].size() > 0) || (!eStack.empty()));

	euler.push_back(currentVertex);

	// remove repeated vertices
	bool inHam[numVertices];
	int cityDistance;
	int tourLength = 0;

	for (int i = 0; i < numVertices; i++) 
		inHam[i] = false;

	int begin = euler[0];
	inHam[begin] = true;
	std::vector<int>::iterator cityOne = euler.begin();
	std::vector<int>::iterator cityTwo = euler.begin() + 1;

	do {
		if (inHam[*cityTwo] == false) 
		{
			cityDistance = dist(position_vec[*cityOne], position_vec[*cityTwo]);
			tourLength = tourLength + cityDistance;
			inHam[*cityTwo] = true;
			cityOne = cityTwo;
			cityTwo = cityOne + 1;
		}
		else if (inHam[*cityTwo] == true) 
		{
			cityTwo = euler.erase(cityTwo);
		}
	} while (cityTwo != euler.end());

	tourLength += dist(position_vec[*cityOne], position_vec[*cityTwo]);

	//make arr into a new array of size num vertices, add elements of tour
	//in order
	int totalVertices = euler.size();
	arr = new int[totalVertices];

	//return sum of weights of edges between vertices as they appear in
	//tour
	return tourLength;
}

int main ()
{
	typedef std::vector<boost::simple_point<double> > PositionVec;

	// create a container for our points
	PositionVec position_vec;

	// create one point
	boost::simple_point<double> vertex;

	// Propose a case where the input is a potential tuple of strings.
	// This is consistent with reading input from a text file
	std::string x("10");
	std::string y("20");

	// set the euclidean coordinates of our vertex, or 'point' in 2D space
	vertex.x = boost::lexical_cast<double>(x);
	vertex.y = boost::lexical_cast<double>(y);

	// push this point to a vector (list) of position vertices (position_vec)
	position_vec.push_back(vertex);

	int *solution_set;

	//TODO: Fix the function paramater, or decide if an *arr is important
	//call the function with a reference to the vector of coordinates, tell
	//the function explicitly that we have only 1 element.
	auto tour_length = christofides(&position_vec, 1, solution_set);

	std::cout << tour_length << std::endl;

	return 0;
}