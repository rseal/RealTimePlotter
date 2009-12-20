Introduction:
The real-time plotter is a lightweight fltk-based plotting tool that reads
binary files of floating point data of format x0,y0,x1,y1,... and plots these
points. Using shared memory and a locking mechanism (e.g. boost::file_lock),
the user can have separate program that "produces" data at a specific rate.
The rtPlotter can be programmed to refresh/reread the file and display the new
data at rapid rates without eating up too much cpu. Look in the examples
folder for a look at how this is accomplished. After building, take a look in
the bin/ folder to find the plotter and data generator binaries.

Dependencies:
1. fltk version 2
2. pthreads library
3. boost libraries
4. boost-build tools

Installation:
From the root directory run "bjam"

