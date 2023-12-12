#define SR_NO_GLAD_IMPL
#define SR_IMPL
#include "../simple_renderer.h"

#include "math.h"
#include "stdio.h"
#include "stdlib.h"

#include "./glfw-ez/glfw/include/GLFW/glfw3.h"

int main(int argc, char** argv) {
  GLFWwindow* win;
  sr_Renderer renderer;

  unsigned int white_tex;
  double xpos, ypos;

  sr_Vec4 red = sr_vec4(1, 0, 0, 1);
  sr_Vec2 t_pos1 = sr_vec2(33, 33);
  float time = 0;
  
  glfwInit();
  glfwWindowHint(GLFW_SAMPLES, 4);
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
    time += .001f;
    t_pos1.x = sin(time * .5f) * 200.f + 300;
    t_pos1.y = cos(time * .25f) * 150.f + 250;
    
    glClearColor(.5f, .5f, .5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    sr_render_begin(&renderer);
      { unsigned short y, x;
      for (y = 0; y < 14; ++y)
        for (x = 0; x < 24; ++x)
          sr_render_push_quad(&renderer, sr_vec2(x * 44, y * 44), sr_vec2(32, 32), sr_vec4(0, 1, .5, 1), white_tex); }

      sr_render_push_quad(&renderer, sr_vec2(xpos, ypos), sr_vec2(64, 64), sr_vec4(1, 0, .5, 1), white_tex);
      sr_render_push_quad(&renderer, sr_vec2(xpos + 64, ypos + 64), sr_vec2(64, 64), sr_vec4(0, 1, 0, 1), white_tex);
      sr_render_push_quad(&renderer, sr_vec2(xpos + 32, ypos + 32), sr_vec2(64, 64), sr_vec4(0, 0, 1, .4), white_tex);
      sr_render_push_triangle(&renderer, t_pos1, sr_vec2(xpos, ypos), sr_vec2(55, 200), red, red, red, sr_vec2(0, 0), sr_vec2(1, 1), sr_vec2(1, 0), white_tex);
    sr_render_end(&renderer);

    glfwSwapBuffers(win);
    glfwPollEvents();
    glfwGetCursorPos(win, &xpos, &ypos);
	}

  sr_free(&renderer);
  glfwTerminate();
  return 0;
}

