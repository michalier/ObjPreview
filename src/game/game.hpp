#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../imgui/imgui_filebrowser.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

#include "../camera/camera.hpp"

#include "../util/exception.hpp"
#include "../util/shader.hpp"

#include "../gameObject/gameObject.hpp"

using namespace glm;
using namespace std;

class Game
{
protected:
  GLFWwindow* window;

  GLuint VertexArrayID;

  bool isRunning;

public:
  Game() = delete;
  Game(int argc, char **argv, GLFWwindow* window);

  void run();
};