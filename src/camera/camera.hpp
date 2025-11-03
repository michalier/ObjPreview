#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <memory>

#include "../util/exception.hpp"

using namespace std;
using namespace glm;

class camera
{
public:
  using Ptr = std::shared_ptr<camera>;

protected:  
  vec3 offset;
  vec2 angles;
  float vow;
  float speed;
  float mouseSpeed;
  float mouseKeySpeed;
  float flip;

  double prevMouseX, prevMouseY;

  mat4 mvp;

  camera(vec3 position, bool perspective);

public:
  float zoom;
  
  camera() = delete;
  static camera::Ptr create(vec3 position, bool perspective=true);

  void update(GLFWwindow* window, float delta, bool downL, bool downR);

  mat4& getMVP();
  vec3& getOffset();
};