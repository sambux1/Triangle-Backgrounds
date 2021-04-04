// generate colors
// check every triangle's square area

#include "delaunay.h"

#include <Magick++.h>

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace Magick;

// global screen dimension
const int WIDTH = 1920;
const int HEIGHT = 1080;

// get the minimum of 3 floating point values
double min_3_doubles(double a, double b, double c) {
    if ((a <= b) && (a <= c))
	    return a;
    else if ((b <= a) && (b <= c))
	    return b;
    else
	    return c;
}

// get the maximum of 3 floating point values
double max_3_doubles(double a, double b, double c) {
    if ((a >= b) && (a >= c))
	    return a;
    else if ((b >= a) && (b >= c))
	    return b;
    else
	    return c;
}

// calculate the area of a triangle
double triangle_area(triangle t) {
    return abs(0.5 * (t.a.x * (t.b.y - t.c.y) + t.b.x * (t.c.y - t.a.y) + t.c.x * (t.a.y - t.b.y) ) );
}

// check if a point is inside a triangle
bool point_in_triangle(point p, triangle t) {
    // calculate the area of the triangle and the areas of each subtriangle formed with p
    float total_area = triangle_area(t);
    
    float area1 = triangle_area({p, t.a, t.b});
    float area2 = triangle_area({p, t.b, t.c});
    float area3 = triangle_area({p, t.a, t.c});
    
    return (total_area == (area1 + area2 + area3));
}

// generate the list of points for the triangulation
vector<point> generate_points_list(int width, int height, int num_points) {

    vector<point> ret;
    
    // add the 4 corners
    ret.push_back({0, 0});
    ret.push_back({0, (double) height - 1});
    ret.push_back({(double) width - 1, 0});
    ret.push_back({(double) width - 1, (double) height - 1});

    // randomly generate points in the middle of the image
    srand(time(NULL));
    
    // generate points until there are num_points points
    while (ret.size() < num_points) {
        double x, y;

        // define the probability (percent) of a boundary case
        int prob_boundary = 8;

        // determine if there will be a boundary case
        int edge_x = rand() % 100;
        int edge_y = rand() % 100;

        if (edge_x < prob_boundary)
            x = 0;
        else if (edge_x < prob_boundary * 2)
            x = width - 1;
        else
            x = rand() % (width - 2) + 1;

        if (edge_y < prob_boundary)
            y = 0;
        else if (edge_y < prob_boundary * 2)
            y = height - 1;
        else
            y = rand() % (height - 2) + 1;
        
        bool unique = true;
        // check if the point already exists in the set
        vector<point>::iterator itr;
        for (itr = ret.begin(); itr != ret.end(); itr++) {
            if ((x == itr->x) && (y == itr->y)) {
                unique = false;
                break;
            }
        }

        if (unique)
            ret.push_back({x, y});
    }

    return ret;
}

// assign a color to all pixels within a triangle
void assign_color_to_triangle(unsigned char pixels[HEIGHT][WIDTH][3], triangle t, unsigned char color[3]) {
    
    // get the bounding rectangle
    double min_x = min_3_doubles(t.a.x, t.b.x, t.c.x);
    double max_x = max_3_doubles(t.a.x, t.b.x, t.c.x);
    
    double min_y = min_3_doubles(t.a.y, t.b.y, t.c.y);
    double max_y = max_3_doubles(t.a.y, t.b.y, t.c.y);

    // make sure to only check points within the image (for triangles including the super triangle)
    if (min_x < 0)
	    min_x = 0;
    if (max_x > WIDTH - 1)
	    max_x = (int) WIDTH - 1;
    
    if (min_y < 0)
	    min_y = 0;
    if (max_y > HEIGHT - 1)
	    max_y = (int) HEIGHT - 1;

    // iterate over every point in the triangle's bounding box
    for (int i = (int) min_x; i <= max_x; i++) {
        for (int j = (int) min_y; j <= max_y; j++) {
            point p = {(double) i, (double) j};
            
            if (point_in_triangle(p, t)) {
                pixels[j][i][0] = color[0];
                pixels[j][i][1] = color[1];
                pixels[j][i][2] = color[2];
            }
        }
    }

}

// iterate through the triangles and assign colors to the appropriate pixels
void generate_colors(unsigned char pixels[HEIGHT][WIDTH][3], const vector<triangle> &triangles) {

    // set up the random number generator
    srand(time(NULL));

    // how much each triangle should be able to deviate from the base RGB values
    // should be random for each image between 10 and 50
    int variation = rand() % 40 + 10;

    
    // generate the base color
    int base_r = rand() % (256 - variation * 2) + variation;
    int base_g = rand() % (256 - variation * 2) + variation;
    int base_b = rand() % (256 - variation * 2) + variation;
    
    // iterate over the list of triangles and generate a color variation for each
    for (triangle t : triangles) {
	    int r = base_r + (rand() % (variation * 2) - variation);
	    int g = base_g + (rand() % (variation * 2) - variation);
	    int b = base_b + (rand() % (variation * 2) - variation);
	
	    unsigned char color[3] = {(unsigned char) r, (unsigned char) g, (unsigned char) b};

	    assign_color_to_triangle(pixels, t, color);
    }
}

// generates an image from start to finish
void generate(int num_points) {

    // generate the points to triangulate
    vector<point> points = generate_points_list(WIDTH, HEIGHT, num_points);
    
    // generate the triangulation
    Delaunay delaunay(points);
    vector<triangle> triangles = delaunay.triangulate();
    
    // create the pixel array
    unsigned char pixels[HEIGHT][WIDTH][3];

    // fill the image with colors
    generate_colors(pixels, triangles);
    
    // export the image
    Image image;
    image.read(WIDTH, HEIGHT, "RGB", CharPixel, pixels);

    image.write("out.png");
}


int main() {
    generate(100);
}
