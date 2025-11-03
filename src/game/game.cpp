#include "game.hpp"


Game::Game(int argc, char **argv, GLFWwindow* window)
{
  this->window = window;
  this->isRunning = true;

  if (!glfwInit())
  {
    throw exc("Failed to initialize GLFW", __LINE__, __FILE__);
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  this->window = glfwCreateWindow(1280, 720, "Rocinante", NULL, NULL);
  
  if (this->window == NULL)
    throw exc("Failed to open GLFW window", __LINE__, __FILE__);
  
  glfwMakeContextCurrent(this->window);

  glewExperimental = true;

  if (glewInit() != GLEW_OK)
    throw exc("Failed to initialize GLEW", __LINE__, __FILE__);

  glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GL_TRUE);
  //glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
}