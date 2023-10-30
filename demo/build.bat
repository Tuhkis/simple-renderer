@echo off

call gcc -Ofast ..\glad\src\glad.c glfw-ez\glfw-ez.c demo.c -o demo.exe -lkernel32 -luser32 -lshell32 -lgdi32 -lAdvapi32 -lopengl32

