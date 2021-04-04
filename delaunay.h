// Delaunay Triangulation header file

#ifndef DELAUNAY_H
#define DELAUNAY_H

#include <vector>

using namespace std;

/*
 * necessary structs for storing data
*/

// create a data structure for a 2D point
// can also be used as a 2D vector
typedef struct point {

    double x;
    double y;

} point;

// create a data structure for an edge between 2 points
typedef struct edge {

    point a;
    point b;

} edge;

// create a data structure for a triangle
typedef struct triangle {
    
    point a;
    point b;
    point c;
    
} triangle;


/*
 * class header
*/

class Delaunay {
    
    public:
		// constructor
		// using pass by constant reference since input list is large
		Delaunay(const vector<point> &points_input);
		
		// generates the triangulation
		// primary function of the algorithm
		vector<triangle> triangulate();
		

		private:
		// screen dimensions
		int width;
		int height;
		
		// the lists of points and triangles in the mesh
		vector<point> points;
		vector<triangle> triangles;

		/*
		* static math helper functions
		*/
		
		// compute the cross product of 2D vectors (points)
		static double cross_product(point a, point b);

		// get the squared distance between two points, used for finding the radius
		// squared because it avoids the need for a square root and still allows comparisons
		static double square_distance(point a, point b);

		// get the midpoint of a line segment of 2 points
		static point get_midpoint(point a, point b);
		
		// get the slope vector of a line segment of 2 points
		static point get_slope(point a, point b);

		// get the vector of the slope perpendicular to the line segment slope
		static point get_perpendicular_slope(point slope);
		
		// get the intersection point between 2 pairs of points and slope vectors
		static point get_intersection(point p1, point p2, point slope1, point slope2);

		// check if a point is in the circumcircle of a triangle
		static bool is_in_circumcircle(point p, triangle t);

		// check if 2 points are the same
		static bool are_points_equal(point a, point b);

		// check if a point is one of the vertices of a triangle
		static bool is_triangle_vertex(point p, triangle t);
		
		// check if 2 triangles are the same
		static bool are_triangles_equal(triangle t1, triangle t2);

		// keep track of the original super triangle
		triangle super_triangle;
		// generates the super triangle and adds it to the list of triangles
		void create_super_triangle();
		// removes all connections to the super triangle after triangulation is complete
		void remove_super_triangle();
};

#endif
