*This project has been created as part of the 42 curriculum by vloureir, zali.*

# miniRT

## Description
MiniRT is a introduction to 3D graphical projects from 42 School, it incentivise you to learn about vectors and their math, to recreate a 3D environment. 

### Goal
The goal of your program is to generate images using the Raytracing protocol. Those computer-generated images will each represent a scene, as seen from a specific angle and position, defined by simple geometric objects, and each with its own lighting system.

### Overview
In the mandatory part of the subject, you need to:
- Render spheres, cylinders and planes.
- Have a camera that can be places anywhere in the scene.
- Accept one light source and have one ambient light on the scene.

---

## Instructions
To use the program you need to use minilibx, which is a library made by 42 School which abstracts some things from the X11 system, to make it easier for students to get started into graphical programs. There are different versions of minilibx, but the one we are using here works on Ubuntu, can't gurantee it works on every Linux distributions.

### Compilation
To compile, just to do the root of the repository and use `make mlx` (to download the minilibx from the 42 repository) and use `make` after that, to compile all the necessary files.

### Usage Example
To use the program, start it with `./miniRT` and you need to pass a scene for it to render, it must be in the `.rt` format.
In the scene, you need to have something like the example below:
```
A 0.2         230,170,100
C 1,-1,2       0,0,-1     70
L     10.0,-4.0,3.0    0.9    10,0,255


sp -2,-1,-5      1.0       255,100,100

sp     0.7,0.9,-2.5     0.2       255,0,0

pl 0,0.2,-0.5       0,1,0         0,0,255

cy 0.4,0.0,-0.5 0.4082,0.8165,0.4082 0.1   0.1   0,255,255
```
---

## Resources

### Documentation & References
[In depth explanations of a lot of the themes that are common in Raytracers](https://www.scratchapixel.com/index.html)

[Helpful guide on a faster implementation of a Raytracer](https://raytracing.github.io/books/RayTracingInOneWeekend.html#positionablecamera)

[Great series on a Raytracing implementation](https://www.youtube.com/watch?v=gfW1Fhd9u9Q&list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl)

[Great series about how to handle vector math](https://www.youtube.com/watch?v=VofMMbD2QtQ&list=PL3WoIG-PLjSv9vFx2dg0BqzDZH_6qzF8-)

### AI Usage
AI was used to have a clearer understading of the vector formulas.
It was also used for debugging purposes

---
