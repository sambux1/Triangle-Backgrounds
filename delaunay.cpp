// Delaunay Triangulation class file

// include class header file
#include "delaunay.h"

#include <iostream>
#include <vector>
#include <set>

using namespace std;


// constructor
// takes list of points to triangulate as input
Delaunay::Delaunay(const vector<point> &points_input) {
    points = points_input;
    
    width = 1920;
    height = 1080;
}

/*
 * static math helper functions
*/

// compute the cross product of 2D vectors (points)
double Delaunay::cross_product(point a, point b) {
    return (a.x * b.y) - (b.x * a.y);
}

// get the squared distance between two points, used for finding the radius
// squared because it avoids the need for a square root and still allows comparisons
double Delaunay::square_distance(point a, point b) {
    double delta_x = b.x - a.x;
    double delta_y = b.y - a.y;

    return (delta_x * delta_x) + (delta_y * delta_y);
}

// get the midpoint of a line segment of 2 points
point Delaunay::get_midpoint(point a, point b) {
    point ret;

    ret.x = (a.x + b.x) / 2;
    ret.y = (a.y + b.y) / 2;

    return ret;
}

// get the slope vector of a line segment of 2 points
point Delaunay::get_slope(point a, point b) {
    point ret;

    ret.x = b.x - a.x;
    ret.y = b.y - a.y;

    return ret;
}

// get the vector of the slope perpendicular to the line segment slope
point Delaunay::get_perpendicular_slope(point slope) {
    point ret;

    ret.x = slope.y;
    ret.y = -1 * slope.x;

    return ret;
}

// get the intersection point between 2 pairs of points and slope vectors
point Delaunay::get_intersection(point p1, point p2, point slope1, point slope2) {
    /*
     * math found here:
     * https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
     *
     * two parameterized lines starting at p and q with slopes r and s have the following equations
     * 	p + tr
     * 	q + us
     *
     * the intersection can be found by calculating the value of t when the two lines intersect
     * 	p + tr = q + us
     * 	(p + tr) x s = (q + us) x s
     * 	(p x s) + (tr x s) = (q x s) + 0
     *	t (r x s) = (q x s) - (p x s)
     *	t = ((q - p) x s) / (r x s)
     *
     *	t = ((p2 - p1) x slope2) / (slope1 x slope2)
     *	
    */
    
    point p2_minus_p1 = {(p2.x - p1.x), (p2.y - p1.y)};
    
    double t = cross_product(p2_minus_p1, slope2) / cross_product(slope1, slope2);
    
    point intersection = {(p1.x + (t * slope1.x)), (p1.y + (t * slope1.y))};

    return intersection;
}

// check if a point is in the circumcircle of a triangle
bool Delaunay::is_in_circumcircle(point p, triangle t) {
    
    // get the midpoints of 2 edges of the triangle
    point mid1 = get_midpoint(t.a, t.b);
    point mid2 = get_midpoint(t.b, t.c);

    // get the perpendicular slopes of the 2 edges of the triangle
    point slope1 = get_perpendicular_slope(get_slope(t.a, t.b));
    point slope2 = get_perpendicular_slope(get_slope(t.b, t.c));

    // get the intersection point of the two perpendicular bisectors
    point circumcenter = get_intersection(mid1, mid2, slope1, slope2);

    double radius = square_distance(circumcenter, t.a);

    double distance = square_distance(circumcenter, p);

    if (distance < radius)
	    return true;
    else
	    return false;
}

// check if 2 points are the same
bool Delaunay::are_points_equal(point a, point b) {
    return (a.x == b.x) && (a.y == b.y);
}

// check if a point is one of the vertices of a triangle
bool Delaunay::is_triangle_vertex(point p, triangle t) {
    return are_points_equal(p, t.a) ||
	       are_points_equal(p, t.b) ||
	       are_points_equal(p, t.c);
}

// check if 2 triangles are the same
bool Delaunay::are_triangles_equal(triangle t1, triangle t2) {
    return is_triangle_vertex(t1.a, t2) &&
	       is_triangle_vertex(t1.b, t2) &&
	       is_triangle_vertex(t1.c, t2);
}

// generates the super triangle and adds it to the list of triangles
void Delaunay::create_super_triangle() {
    // in units of the full screen size, the super triangle points are
    // (-1, -1), (-1, 5), (5, -1)

    point p1 {(double) -1 * width, (double) -1 * height};
    point p2 {(double) -1 * width, (double)  5 * height};
    point p3 {(double)  5 * width, (double) -1 * height};
    
    super_triangle = {p1, p2, p3};
    
    triangles.push_back(super_triangle);
}

// removes all connections to the super triangle after triangulation is complete
// not necessary and not used
// removing the super triangle leaves some gaps as not all pixels are guaranteed to be in a triangle without it
void Delaunay::remove_super_triangle() {
    // loop through all triangles and check if they contain one of the original super triangle points
    vector<triangle>::iterator itr;
    for (itr = triangles.begin(); itr != triangles.end(); itr++) {
        if (is_triangle_vertex(super_triangle.a, *itr) ||
            is_triangle_vertex(super_triangle.b, *itr) ||
            is_triangle_vertex(super_triangle.c, *itr))   {
        
            // decrement the iterator and remove the next element
            itr--;
            triangles.erase(itr + 1);
        }
    }
}

// generates the triangulation
// the bulk of the algorithm, calls subprocesses
vector<triangle> Delaunay::triangulate() {

    // the first step is to generate the surrounding triangle
    create_super_triangle();

    // add one point at a time to the triangulation
    for (point p : points) {
	
        // keep a list of all bad triangles (those that are no longer valid with the new point)
        vector<triangle> bad_triangles;

        // iterate over all triangles and check if the point is in the circumcircle
        for (triangle t : triangles) {
            // if the point is in the circumcircle, add the triangle to the list of bad triangles
            if (is_in_circumcircle(p, t)) {
                bad_triangles.push_back(t);
            }
        }

        // create a set of all edges in the bad triangles
        vector<vector<point>> polygon_edges;

        // iterate over all the bad triangles to find the polygon hole
        for (triangle t : bad_triangles) {
            // create a list with the triangle's edges to allow them to be iterated over
            vector<vector<point>> edges = {{t.a, t.b}, {t.b, t.c}, {t.c, t.a}};
            
            // iterate over each edge in the triangle and check if it is in any other bad triangles
            for (vector<point> edge : edges) {
            
                // keep track of how many bad triangles contain the edge
                // the current triangle should, so the count should be 1 if no other bad triangles do
                int count = 0;

                // iterate over the list of bad triangles and check if they contain the edge
                for (triangle t2 : bad_triangles) {
                    // check if both point of the edge are in the triangle, increment count if they are
                    if (is_triangle_vertex(edge[0], t2) && is_triangle_vertex(edge[1], t2)) {
                        count++;
                    }
                }

                // add the edge to the polygon hole if no other bad triangles contain it
                if (count <= 1) {
                    polygon_edges.push_back(edge);
                }

            }

        }

        // remove all bad triangles from the list of triangles
        for (triangle t : bad_triangles) {
            // search the list of bad_triangles until t is found
            vector<triangle>::iterator itr;
            for (itr = triangles.begin(); itr != triangles.end(); itr++) {
                if (are_triangles_equal(t, *itr)) {
                    triangles.erase(itr);
                    break;
                }
            }
        }

        // add new triangles between the point and each edge of the polygon hole
        for (vector<point> edge : polygon_edges) {
            triangle t = {p, edge[0], edge[1]};
            triangles.push_back(t);
        }

    }
    
    return triangles;
}

