#include "../simple_renderer.c"

#include "stdio.h"

#include "./glfw-ez/glfw/include/GLFW/glfw3.h"

int main(int argc, char** argv) {
  GLFWwindow* win;
  sr_Renderer renderer;

  sr_Vec2 a = sr_vec2(0, 0);
  sr_Vec2 b = sr_vec2(0, 64);
  sr_Vec2 c = sr_vec2(64, 64);
  sr_Vec2 d = sr_vec2(64, 0);

  sr_Vec2 a1 = sr_vec2(64, 0);
  sr_Vec2 b1 = sr_vec2(64, 64);
  sr_Vec2 c1 = sr_vec2(128, 64);
  sr_Vec2 d1 = sr_vec2(128, 0);

  sr_Vec2 uv_0 = sr_vec2(0, 0);
  sr_Vec2 uv_1 = sr_vec2(1, 0);
  sr_Vec2 uv_2 = sr_vec2(1, 1);
  
  sr_Vec4 red = sr_vec4(1, 0, 0, 1);
  sr_Vec4 green = sr_vec4(0, 1, 0, 1);

  unsigned int white_tex;
  
  glfwInit();
  glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
  win = glfwCreateWindow(1024, 600, "Demo", NULL, NULL);
  glfwMakeContextCurrent(win);
	glfwSwapInterval(0);
	sr_load_loader(glfwGetProcAddress);

	sr_init(&renderer, 1024, 600);

	white_tex = sr_get_white_texture();

	while (!glfwWindowShouldClose(win)) {
    glClearColor(.5f, .5f, .5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    sr_render_begin(&renderer);
      sr_render_push_triangle(&renderer, a, b, c, red, red, red, uv_0, uv_1, uv_2, white_tex);
      sr_render_push_triangle(&renderer, a, d, c, red, red, red, uv_0, uv_1, uv_2, white_tex);

      sr_render_push_triangle(&renderer, a1, b1, c1, green, green, green, uv_0, uv_1, uv_2, white_tex);
      sr_render_push_triangle(&renderer, a1, d1, c1, green, green, green, uv_0, uv_1, uv_2, white_tex);
    sr_render_end(&renderer);

    glfwSwapBuffers(win);
    glfwPollEvents();
	}

  sr_free(&renderer);
  glfwTerminate();
  return 0;
}

