#include "camera.hpp"

camera::camera(vec3 position, bool perspective)
{
  this->offset = position;
  this->angles = vec2(0.0f, 0.0f);
  this->vow = 60.0f;
  this->speed = 0.15f;
  this->mouseSpeed = 0.02f;
  this->mouseKeySpeed = 0.75f;
  this->flip = 1.0f;
  this->zoom = 10.0f;
}

camera::Ptr camera::create(vec3 position, bool perspective)
{
  camera::Ptr p(new camera(position, perspective));
  if (p == nullptr)
    throw exc("Could not create camera", __LINE__, __FILE__);

  return p;
}

void camera::update(GLFWwindow* window, float delta, bool downL, bool downR)
{
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  {
    angles.x -= delta * mouseKeySpeed * flip;
  }

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  {
    angles.x += delta * mouseKeySpeed * flip;
  }

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    angles.y -= delta * mouseKeySpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    angles.y += delta * mouseKeySpeed;
  }

  double mousePosX, mousePosY;
  glfwGetCursorPos(window, &mousePosX, &mousePosY);

  float mDeltaX = prevMouseX - mousePosX;
  float mDeltaY = prevMouseY - mousePosY;

  prevMouseX = mousePosX;
  prevMouseY = mousePosY;

  if (downL) {
    angles.x += mouseSpeed * delta * mDeltaX * flip;
    angles.y += mouseSpeed * delta * mDeltaY;
  }

  if (angles.x < 0)
    angles.x += 2 * M_PI;

  if (angles.x >= 2 * M_PI)
    angles.x -= 2 * M_PI;

  if (angles.y < 0)
    angles.y += 2 * M_PI;

  if (angles.y >= 2 * M_PI)
    angles.y -= 2 * M_PI;

  if (angles.y >= M_PI / 2.0f && angles.y <= M_PI * 3.0f / 2.0f)
    flip = -1.0f;
  else
    flip = 1.0f;

  vec3 diff(0);
  vec3 direction(
    cosf(angles.y) * sinf(angles.x),
    sinf(angles.y),
    cosf(angles.y) * cosf(angles.x)
  );

  vec3 right(
      sinf(angles.x - 3.14f / 2.0f),
      0,
      cosf(angles.x - 3.14f / 2.0f));

  vec3 up = cross(right, direction);

  if (downR) {
    diff += right * delta * mDeltaX * speed * zoom;
    diff -= up * delta * mDeltaY * speed * zoom;
  }

  /*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    diff += direction * delta * speed;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    diff -= direction * delta * speed;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    diff -= right * delta * speed;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    diff += right * delta * speed;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    diff += up * delta * speed;
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    diff -= up * delta * speed;
  }*/

  this->offset += diff;

  GLint data[4] = { -1 };
  glGetIntegeri_v(GL_VIEWPORT, 0, data);

  mat4 projection = perspective(
    radians(vow), 
    (float)data[2] / (float)data[3], 
    0.1f, 
    100.0f
  );
  mat4 view = lookAt(offset - zoom * direction, offset, up);
  mat4 model = mat4(1.0f);

  this->mvp = projection * view * model;
}

mat4 &camera::getMVP()
{
  return this->mvp;
}

vec3 &camera::getOffset()
{
  return this->offset;
}