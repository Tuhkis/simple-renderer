#ifndef SIMPLE_RENDERER_H
#include "simple_renderer.h"
#endif /* SIMPLE_RENDERER_H */

#ifndef SR_NO_GLAD_IMPL
#include "./glad/src/glad.c"
#endif /* SR_NO_GLAD_IMPL */

#define SR_INVALID_TEXTURE (1248)

static const char* default_vertex_source =
#include "vertex.glsl"

static const char* default_fragment_source =
#include "fragment.glsl"

static unsigned int _sr_cached_white = 4096;

sr_Mat4 sr_mat4_ortho(float left, float right, float top, float bottom, float near, float far) {
  float width = right - left;
  float height = top - bottom;
  float depth = far - near;
  sr_Mat4 ret = {0};
  ret.a[0] = 2.f / width; ret.a[1] = 0.f; ret.a[2] = 0.f; ret.a[3] = 0.f;
  ret.a[4] = 0.f; ret.a[5] = 2.f / height; ret.a[6] = 0.f; ret.a[7] = 0.f;
  ret.a[8] = 0.f; ret.a[9] = 0; ret.a[10] = 2.f / depth; ret.a[11] = 0.f;
  ret.a[12] = -(right + left) / width;
  ret.a[13] = -(top + bottom) / height;
  ret.a[14] = -(far + near) / depth;
  ret.a[15] = 1.f;
  return ret;
}

const sr_ShaderProgram sr_shader_program_create(const char* vertex_source, const char* fragment_source) {
  GLuint vertex_shader;
  GLuint fragment_shader;
  sr_ShaderProgram program;

  program = glCreateProgram();
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(vertex_shader);
  {
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
      printf("Couldn't compile vertex shader. %s%s",info_log, "\n");
    }
  }
  glCompileShader(fragment_shader);
  {
    int success;
    char info_log[512];
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
      printf("Couldn't compile fragment shader. %s%s", info_log, "\n");
    }
  }
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  {
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
      glGetProgramInfoLog(program, 512, NULL, info_log);
      printf("Couldn't link shader program. %s%s", info_log, "\n");
    }
  }
  glDetachShader(program, vertex_shader);
  glDetachShader(program, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glUseProgram(program);
  return program;
}

void sr_init(sr_Renderer* render, unsigned short width, unsigned short height) {
  int tex_loc;
  int textures[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

  glGenVertexArrays(1, &render->vao);
  glBindVertexArray(render->vao);

  glGenBuffers(1, &render->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
  glBufferData(GL_ARRAY_BUFFER, SR_MAX_VERTICES * sizeof(sr_RenderVertex),
    NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(sr_RenderVertex), (void*)sr_offsetof(sr_RenderVertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(sr_RenderVertex), (void*)sr_offsetof(sr_RenderVertex, colour));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sr_RenderVertex), (void*)sr_offsetof(sr_RenderVertex, uv));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(sr_RenderVertex), (void*)sr_offsetof(sr_RenderVertex, tex_index));
  glEnableVertexAttribArray(3);

  render->shaders = sr_shader_program_create_default();

  sr_resize(render, width, height);

  tex_loc = glGetUniformLocation(render->shaders, "u_tex");
  glUniform1iv(tex_loc, 8, textures);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  render->moment = 0;
  render->previous_moment = -1;
  return;
}

void sr_free(sr_Renderer* render) {
  glDeleteBuffers(1, &render->vbo);
  glDeleteVertexArrays(1, &render->vao);
  glDeleteProgram(render->shaders);
  return;
}

const sr_ShaderProgram sr_shader_program_create_default(void) {
  return sr_shader_program_create(default_vertex_source, default_fragment_source);
}

void sr_resize(sr_Renderer* render, unsigned short width, unsigned short height) {
  GLuint proj_uniform;

  render->proj = sr_mat4_ortho(0, width, 0, height, -.01f, 1.f);
  proj_uniform = glGetUniformLocation(render->shaders, "u_proj");
  glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, render->proj.a);
  return;
}

void sr_render_begin(sr_Renderer* render) {
  render->triangle_count = 0;
  render->tex_count = 0;
  return;
}

void sr_render_end(sr_Renderer* render) {
  unsigned int i;
  int moment;

  {
    for (i = 0; i < render->tex_count; ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, render->tex[i]);
    }
  }

  render->moment = 0;
  for (i = 0; i < render->triangle_count * 3; ++i) {
    moment = ((render->triangle_data[i].pos.y - render->triangle_data[i].pos.x) * 32014);
    render->moment = (moment ^ render->moment ^ (int)(render->triangle_data[i].tex_index * 772213) ^ ((int)(render->triangle_data[i].uv.x - render->triangle_data[i].uv.y) * 22891)) * 321;
  }

  if (render->moment != render->previous_moment) {
    glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, render->triangle_count * 3 * sizeof(sr_RenderVertex),
      render->triangle_data);
  }
  glUseProgram(render->shaders);
  glBindVertexArray(render->vao);
  glDrawArrays(GL_TRIANGLES, 0, render->triangle_count * 3);
  render->previous_moment = render->moment;
  return;
}

void sr_render_push_triangle(sr_Renderer* render, sr_Vec2 a, sr_Vec2 b, sr_Vec2 c, sr_Vec4 a_c, sr_Vec4 b_c, sr_Vec4 c_c, sr_Vec2 a_uv, sr_Vec2 b_uv, sr_Vec2 c_uv, unsigned int tex) {
  unsigned int tex_index = SR_INVALID_TEXTURE;
  {
    unsigned int i;
    for (i = 0; i < render->tex_count; ++i)
      if (render->tex[i] == tex) {
        tex_index = i;
        break;
      }
  }
  if (tex_index == SR_INVALID_TEXTURE && render->tex_count < 8) {
    render->tex[render->tex_count] = tex;
    tex_index = render->tex_count;
    ++render->tex_count;
  }

  /* Flush renderer if need be. */
  if (render->triangle_count == SR_MAX_TRIANGLES || tex_index == SR_INVALID_TEXTURE) {
    sr_render_end(render);
    sr_render_begin(render);
  }

  render->triangle_data[render->triangle_count * 3].pos = a;
  render->triangle_data[render->triangle_count * 3].colour = a_c;
  render->triangle_data[render->triangle_count * 3].uv = a_uv;
  render->triangle_data[render->triangle_count * 3].tex_index = (float)(tex_index);

  render->triangle_data[render->triangle_count * 3 + 1].pos = b;
  render->triangle_data[render->triangle_count * 3 + 1].colour = b_c;
  render->triangle_data[render->triangle_count * 3 + 1].uv = b_uv;
  render->triangle_data[render->triangle_count * 3 + 1].tex_index = (float)(tex_index);

  render->triangle_data[render->triangle_count * 3 + 2].pos = c;
  render->triangle_data[render->triangle_count * 3 + 2].colour = c_c;
  render->triangle_data[render->triangle_count * 3 + 2].uv = c_uv;
  render->triangle_data[render->triangle_count * 3 + 2].tex_index = (float)(tex_index);

  ++render->triangle_count;
  return;
}

void sr_render_push_quad(sr_Renderer* render, sr_Vec2 pos, sr_Vec2 size, sr_Vec4 colour, unsigned int texture) {
  sr_render_push_triangle(render, pos, sr_vec2(pos.x + size.x, pos.y), sr_vec2(pos.x, pos.y + size.y),
    colour, colour, colour, sr_vec2(0, 0), sr_vec2(1, 0), sr_vec2(0, 1), texture);

  sr_render_push_triangle(render, sr_vec2(pos.x + size.x, pos.y + size.y), sr_vec2(pos.x + size.x, pos.y), sr_vec2(pos.x, pos.y + size.y),
    colour, colour, colour, sr_vec2(1, 1), sr_vec2(1, 0), sr_vec2(0, 1), texture);

  return;
}

sr_Vec2 sr_vec2(float x, float y) {
  sr_Vec2 ret;
  ret.x = x;
  ret.y = y;
  return ret;
}

sr_Vec4 sr_vec4(float x, float y, float z, float w) {
  sr_Vec4 ret;
  ret.x = x;
  ret.y = y;
  ret.z = z;
  ret.w = w;
  return ret;
}

unsigned int sr_get_white_texture(void) {
  /* Cache the value, so that the texture isn't created more than once. */
	if (_sr_cached_white == 4096) {
		unsigned int tex;
		unsigned char image[3] = {255, 255, 255};
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_sr_cached_white = tex;
	}
	return _sr_cached_white;
}

