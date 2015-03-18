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

####Deferred Lighting

Deferred Lighting is a technique used to calculate the light effect on each pixel after the scene render. The basic idea is to render the whole scene without any light calculation, which is called the geometry pass. Then, for each light you do the calculation for each pixel that was rendered, called light pass. 

The standard technique for rendering, where you render the whole scene doing the light calculations at the same time, is called Forward Rendering. It works perfectly, but the number of lights that you can have per scene is limited, because you need to pass them all together to the GPU. This will impact in your application performance. When you use the Forward Rendering the rendering complexity will depend on how many pixels will be rasterized and how many light you have. Because, you will do a loop through all the lights for each pixel rasterized, even if the pixel fails in the depth test.

```
Forward Rendering = O(pixels_rasterized*n_lights)
```

In the Deferred Lighting technique, you will render your geometry without any light calculations, which will speed up your rendering by not doing extra effort in pixels which will not appear in the screen. And after that you perform the calculation for each pixel. As a result, your complexity will depend on the size of your frame buffer and the number of lights.

```
Deferred Lighting = O(w*h + n_lights)
w = framebuffer width
h = framebuffer height
```

To have enough information to shade each pixel you need different types of data: normals, positions, and diffuse color; the information varies with the lighting model used. And it is possible to get the data in a single geometry pass, in order to do so, you have to attach more than one texture to your [Framebuffer Object]. In the next images you can see the output of my geometry pass, where I am outputting the position of each fragment, an alpha map, the normals, the diffuse color, the specular color and shininess coefficient, and the ambient and emission color.

![Diffuse and Alpha map](/screenshots/diffuse-and-alpha.jpg)

![Position and normals map](/screenshots/position-and-normals.jpg)

![Specular and ambient](/screenshots/specular-and-ambient.jpg)

The framebuffer that stores the result of you geometry pass is called g-buffer. As explained in [Framebuffer Objects], the size of your textures should be as smaller as you can, and you also you must use less textures as you can. Otherwise, you will be passing to much data between the CPU and the GPU. However, for this application, which performance and optimizations weren't the focus, I used a simple g-buffer configuration, as you can see in this image:

![G-Buffer](/screenshots/g-buffer.jpg)

Although, there is a better configuration. I'm using RGBA16 as my texture format, what is too much for textures that are going just to store colors, a better format would be RGBA8. But, for the position you need a better precision then RGBA8, that is why I choose RGBA16. However, there are techniques to recover your world space position, based on your image space position and depth. Which would eliminate the position texture to give place to a smaller texture that just stores the depth value. And we can also eliminate the alpha map buffer by putting the alpha value in the diffuse buffer fourth component, because we don't need a high precision for the alpha component since the value will 0 or 1. As a result, you can see a new configuration for the G-Buffer in the next image:

![G-Buffer Improved](/screenshots/better-g-buffer.jpg)

After collecting the geometry pass data, you can now perform the light pass. In this pass you need to send all those textures to the GPU and calculate the light with them. One performance problem appears when, for each light, you try to shade every pixel in your screen (width * height), which is not necessary. What you can do, for point lights and spot lights, is to calculate the light radius and render a cube scaled with the radius. This will limit the pixels that will be rasterized during your light pass, speeding up your application.

![Light Pass](/screenshots/light-pass.jpg)

Deferred Lighting has a couple of downsides. One of them is when you separate the data in the g-buffer textures, you lose the ability of using semi-transparency in your images, because it won't produce the same effect when you calculate the light. A semi-transparent surface should show the light reflection of the pixel that is behind of it blended with the transparency color. However, when you are in the light pass, you loose this notion of depth, and you can't get the actual position of the pixel that is behind, leading you to a wrong result.

Another downside is the handle the use of different materials, because to handle more materials properties, like the specular color, you need to add extra textures to your G-buffer, slowing down the communication between GPU and CPU.


**References:**
- [Coding labs - Deferred Rendering]
- [Tut+ Game Development - Forward Rendering vs Deferred Rendering]
- [OGLDev - Deferred Shading]
- [Neuro Production - Making a 3D Game with OpenGL: Deferred shading and stuff]
- [Fabio Policarpo and Francisco Fonseca - Deferred Shading Tutorial]
- [OpenGL - glTexImage2D Formats]


####Bloom Shader


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



[Framebuffer Objects]:#framebuffer-objects
[Framebuffer Object]:#framebuffer-objects
[CPE 471]:http://users.csc.calpoly.edu/~zwood/teaching/csc471/csc471.html
[Lighthouse3D - OpenGL Framebuffer Objects]:http://www.lighthouse3d.com/tutorials/opengl-short-tutorials/opengl_framebuffer_objects/
[OpenGl Wiki - Framebuffer Objects]:https://www.opengl.org/wiki/Framebuffer_Object
[Opengl Tutorial Org - Render to texture]:http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
[Make project Tutorial]:http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/
[Coding labs - Deferred Rendering]:http://www.codinglabs.net/tutorial_simple_def_rendering.aspx
[Tut+ Game Development - Forward Rendering vs Deferred Rendering]:http://gamedevelopment.tutsplus.com/articles/forward-rendering-vs-deferred-rendering--gamedev-12342
[OGLDev - Deferred Shading]:http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html
[Neuro Production - Making a 3D Game with OpenGL: Deferred shading and stuff]:http://www.neuroproductions.be/opengl/making-a-3d-game-with-opengl-deferred-shading-and-stuff/
[Fabio Policarpo and Francisco Fonseca - Deferred Shading Tutorial]:http://gamedevs.org/uploads/deferred-shading-tutorial.pdf
[OpenGL - glTexImage2D Formats]:https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
