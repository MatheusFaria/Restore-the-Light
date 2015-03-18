Restore the Light
=================
![Restore the Light Main Image](/screenshots/main.jpg)

Restore the light is a game where the hero is a ball of light. And his world is completely taken by the darkness, which is you main enemy. In this first stage, the darkness servants captured your friends, and you, as a hero, has to save them all to win and restore the world's light.

This is my final project for the introduction to computer graphics course at Cal Poly ([CPE 471]). In this project I learned and implemented several computer graphics concepts: Blinn-Phong Illumination Model, Hierarchical Modelling, Texture Mapping, Framebuffer Objects, Deferred Lighting, and Bloom Shading. To practice those concepts, I created this simple 3D application.

Implemented Concepts
--------------------

In this section I will talk about the concept and techniques that weren't learned in class and I implemented.

#### Framebuffer Objects
Framebuffer Objects are objects that let you draw you geometry in a alternative buffer besides the screen buffer. With them you can render your scene in a different buffer, which will not appear in the screen, and get the rendered result as a texture. Which is a very useful feature, because with it you can post process the texture returned, and use it as you would do with a common texture. 

The post process stage is very important in 3D applications. In this stage you can apply different algorithms to a texture to get different effects. For example, you can blur the whole screen, create the bloom effect, even create a glass effect to your object. Besides post processing, framebuffer objects can be used for different things, like deferred lighting or multiple pass rendering, which I will explain in later sections.

To render in a framebuffer, you need to attach some texture to it, which will be the same textures that you will receive the rendered output. And one of the things that you must beware when you are creating those textures, is their format. The format of a texture, also the framebuffer width and height, determine what is the texture size in bytes. And it is this size that will be passed from the CPU to the GPU when you are using the framebuffer, this will impact in the application performance. However, the format will also determine the precision of the data stored in the texture, if you want a better precision you will have to increase your texture size.

In my application I used framebuffers to generate a bloom effect on my scene, and to perform several light passes. On those framebuffers I used the size GL_RGBA16F, because I needed a better precision to register the position of each fragment during my geometry pass. But if you will just render color a smaller size is better.

**References:**
- [Lighthouse3D - OpenGL Framebuffer Objects]
- [OpenGl Wiki - Framebuffer Objects]
- [Opengl Tutorial Org - Render to texture]


Libraries
---------
- GLFW
- GLM
- GLEW
 
Platforms
----------

- Windows
- Linux
- Mac

How to Install
--------------
This project uses the CMake to generate the development environment. In order to get the project running your machine you need to install the CMake and download the project.

After that, you need to create a build folder inside the project. Then you can build the project with the CMake

```
#Linux commands
$ cd restore-the-light
$ mkdir build
$ cd build
$ cmake ..
$ make all
$ ../restore-the-light/restore-the-light
```

On Windows, you need to follow the same steps, but the final final compilation you need to perform with Visual Studio. After compiling the project an executable will appear in the restore-the-light folder.

For more information about the steps you can follow [this tutorial](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/), which is from where I got the base of my CMake and project structure.

Credits and Contacts
-------------------
- Matheus de Sousa Faria (matheus.sousa.faria@gmail.com): Main and Only Coder
- Arianna Cabebe: Darkness Creature 3D Designer


[CPE 471]:http://users.csc.calpoly.edu/~zwood/teaching/csc471/csc471.html
[Lighthouse3D - OpenGL Framebuffer Objects]:http://www.lighthouse3d.com/tutorials/opengl-short-tutorials/opengl_framebuffer_objects/
[OpenGl Wiki - Framebuffer Objects]:https://www.opengl.org/wiki/Framebuffer_Object
[Opengl Tutorial Org - Render to texture]:http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
[Make project Tutorial]:http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/
