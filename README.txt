Program 3
CPE 471 - Introduction to Computer Graphics
Author: Matheus de Sousa Faria

ABOUT
=====
In this program I implemented an easter scene, where there is a bunny on
the center and easter eggs on the sides. I made each object to rotate
seperataly to watch the effect of light.

My normal calculation is done in the mesh.cpp file, the function is
calculateNormals in the line 42.

The material that I added is chocolate. And that is the why the main theme
is easter.

If you want to see the light object, just press I until it gots into the 
screen. I modeled it like a small sphere, which emisses white light.

CONTROLS
========

A, D : Rotates the objects around Y axis
W, S : Rotates the objects around X axis
Q, E : Moves the light on the X axis
I, K : Moves the light on the Z axis
N : Shows the normal coloring
Z : Switches between Gouraud and Phong, respectively
X : Switches the material (Plastic, Clay, Gold, Chocolate)
G : Switches the central object (bunny, cube, sphere)

LIBRARIES
=========

- GLFW
- GLM
- GLAD

COMPILATION
===========
I used the structure of the Lab 6. So, you do need the local 
GLM and glad to compile my code, just like in the Lab 6. As far as I 
concern, there is no need to change any file nor hearder.
Altough, I'm hadin just the .cpp, .h and .glsl files. So, in order to 
compile you will need to paste my files inside the Lab 6 structure, this
means, source files to the source folder and non-source files to the 
root project folder. By the way, I'm sending my sources.txt if you will 
compile with the CMake.
