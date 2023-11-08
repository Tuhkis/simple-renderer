/* This is essentially just a scuffed version of this: https://youtu.be/NPnQF4yABwg?si=5zMVWmcGZWTtigde */

#ifndef SIMPLE_RENDERER_H
#define SIMPLE_RENDERER_H

#include "stddef.h"
#include "stdio.h"

#ifndef sr_offsetof
#define sr_offsetof(type, member) offsetof(type, member)
#endif /* sr_offsetof */

#include "./glad/include/glad/glad.h"

#define SR_MAX_TRIANGLES (2048)
#define SR_MAX_VERTICES (SR_MAX_TRIANGLES * 3)

#define sr_load_loader(getProc) gladLoadGLES2Loader((GLADloadproc)(getProc));

typedef struct sr_Vec2 { float x, y; } sr_Vec2;
typedef struct sr_Vec4 { float x, y, z, w; } sr_Vec4;

typedef struct sr_Mat4 { float a[4 * 4]; } sr_Mat4;

typedef struct sr_RenderVertex {
  sr_Vec2 pos;
  sr_Vec4 colour;
  sr_Vec2 uv;
  float tex_index;
} sr_RenderVertex;

typedef struct sr_Renderer {
  /* OpenGL object */
  unsigned int shaders;
  unsigned int vao;
  unsigned int vbo;
  
  sr_Mat4 proj;

  /* The triangle data required for the rendering. */
  sr_RenderVertex triangle_data[SR_MAX_VERTICES];
  unsigned int triangle_count;

  /* Texture data */
  unsigned int tex[8];
  unsigned int tex_count;
} sr_Renderer;

/**
 * Returns an orthographic projection matrix.
 */
sr_Mat4 sr_mat4_ortho(float left, float right, float top, float bottom, float near, float far);
/**
 * Creates a sr_Vec2 struct.
 */
sr_Vec2 sr_vec2(float x, float y);
/**
 * Creates a sr_Vec4 struct.
 */
sr_Vec4 sr_vec4(float x, float y, float z, float w);
/**
 * Returns the ID of a white texture.
 */
unsigned int sr_get_white_texture(void);
/**
 * Uninitialises the renderer. Call at the end of your program.
 */
void sr_free(sr_Renderer* render);
/**
 * Initialises the renderer and opengl. Primes everything for rendering.
 */
void sr_init(sr_Renderer* render, unsigned short width, unsigned short height);
/**
 * Begins a frame. After this you can push triangles to the renderer.
 */
void sr_render_begin(sr_Renderer* render);
/**
 * Ends a frame and renders everything.
 */
void sr_render_end(sr_Renderer* render);
/**
 * Pushes a triangle to the renderer.
 * If and when triangle cap is exceeded,
 * will automatically flush itself and start rendering again.
 */
void sr_render_push_triangle(sr_Renderer* renderer, sr_Vec2 a, sr_Vec2 b, sr_Vec2 c, sr_Vec4 a_c, sr_Vec4 b_c, sr_Vec4 c_c, sr_Vec2 a_uv, sr_Vec2 b_uv, sr_Vec2 c_uv, unsigned int texture);
/**
 * Pushes two triangles to the renderer, to make a rectangle.
 */
void sr_render_push_quad(sr_Renderer* render, sr_Vec2 pos, sr_Vec2 size, sr_Vec4 colour, unsigned int texture);
/**
 * Resizes the renderer.
 */
void sr_resize(sr_Renderer* render, unsigned short width, unsigned short height);

#endif /* SIMPLE_RENDERER_H */

