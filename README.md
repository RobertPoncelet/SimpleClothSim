**Cloth Simulation**
================
A simple CPU-based cloth simulation I created using NGL as part of university coursework. Please note that this requires NGL (available [here](https://github.com/NCCA/NGL)) to be installed in your home directory on Linux, and "$HOME/NGL/lib" to be added to your LD_LIBRARY_PATH environment variable if running the program outside of Qt Creator.

The following is copied from my coursework submission:

----------

Summary
-------

This program aims to give a visually plausible simulation of a hanging sheet of cloth using Verlet integration and provide a UI to the user for experimenting with different settings and variables of the simulation. The main window of the program is divided into a viewport where the simulation is displayed and a UI column where the simulation variables and settings can be changed.

![Preview](preview.png)

----------

Instructions
------------

Dragging with the left mouse button on the viewport will orbit the camera around the origin. Dragging with the right will translate the camera, and the middle mouse button will move the collision sphere around. The texture containing the world-space vertex normals is displayed in the bottom-left corner. Red represents X values, green represents Y and blue Z.
On the right are the UI options: 

-	**Width** and **Height** control the dimensions of the cloth sheet.
-	**Width Resolution** and **Height Resolution** control how many particles are used along those respective axes.
-	**Spring Constant** controls how stiff the springs are.
-	**Damping Constant** controls how quickly the springs will stop oscillating.
-	**Gravity** controls the strength of the gravity acting on the particles.
-	**Simulation Speed** is a multiplier for how fast the simulation runs.
-	**Apply Sphere Collision** sets whether the cloth will collide with the yellow sphere.
-	**Apply Self Collision** sets whether the cloth will collide with itself.
-	**Apply Wind** sets whether a turbulent wind-like force is applied to the cloth sheet.
-	**Paused** sets whether the simulation is in suspended animation.
-	Each of the **Anchored Corners** check-boxes sets whether the respective corner of the sheet is "anchored" i.e. the cloth will hang from that point.
-	The **Reset Cloth** button will set the cloth back to its initial position using the current options. This is useful if the cloth "explodes" due to the variable values crossing a certain threshold and exponentially increasing the energy in the system.

----------

Method
------

The simulation uses the mass-spring system, a commmon technique used for cloth. In this system the cloth is represented by a grid of particles (masses) connected by springs which obey Hooke's law. 
In my implementation, at first I represented these components with a Spring and Particle class with public attributes, but later replaced these with struct equivalents so their attributes could be accessed with a memory address and an offset. The particles would obey Newton's second law of motion, and since each particle would be connected to multiple springs as well as being affected by gravity, the forces acting on it would need to be summed up first before their next position was calculated. This was achieved fairly easily with two loops: once iterating through the springs to add the forces they exert on the particles to the particles' "pending force" attributes, and once through the particles themselves to add forces such as gravity and air resistance before updating their positions.

Another feature was Verlet integration; rather than simple Euler integration which stores position and velocity values, Verlet uses a position and a previous position. As well as still being fairly fast, the velocity interpreted from these values never becomes "out of sync" with the position, resulting in much better energy conservation in physics calculations. 
