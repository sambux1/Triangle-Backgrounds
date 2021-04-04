# Triangle-Backgrounds

A program to generate an image of a mesh of triangles. I currently have this script running in the background every 15 seconds on my computer to generate a new background.

### The Algorithm

I am generating a random set of points and using [Delaunay triangulation](https://en.wikipedia.org/wiki/Delaunay_triangulation) to generate the mesh. I am using an unoptimized version of Delaunay triangulation which is *O(n<sup>2</sup>)*, while an efficient implementation would be *O(n\*log(n))*. This is not an issue, however, as for the purposes of an intermittent background generator, the program is fast enough that it is not an issue.

### Screenshots

![image](./saved/blue/3.png =500x)
![image](./saved/purple/4.png =500x)
![image](./saved/green/2.png =500x)
![image](./saved/orange/5.png =500x)
![image](./saved/pastel/2.png =500x)
![image](./saved/pastel/4.png =500x)
![image](./saved/rainbow/2.png =500x)