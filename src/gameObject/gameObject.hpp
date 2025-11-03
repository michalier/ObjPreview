#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "../camera/camera.hpp"
#include "../util/exception.hpp"
#include "../util/stb_image.hpp"

using namespace glm;
using namespace std;

class Material {
public:
  vec3 ambient;
  vec3 diffuse;
  float shininess;

  string texture;

  Material();
};

class gameObject
{
public:
  using Ptr = shared_ptr<gameObject>;

protected:
  GLuint programID;
  vector<GLuint> vertexBuffer;
  vector<GLuint> normalBuffer;
  vector<GLuint> uvBuffer;
  vector<GLuint> textures;

  unordered_map<string, GLuint> texture_dict;

  vec3 positon;

  gameObject(GLuint programID);

public:
  vector<GLsizei> vertexCount;
  vector<string> names;
  vector<bool> doDraw;
  vector<vec2> useFlags;
  vector<vec3> noiseOffset;
  vector<float> noiseScale;
  vector<float> noiseLayers;
  vector<float> noiseMultiplier;
  vector<vec3> ambientColor;
  vector<vec3> diffuseColor;

  gameObject() = delete;
  static gameObject::Ptr create(GLuint programID);
  ~gameObject();

  vec3 lightPos;
  float scale;

  void setPosition(vec3 position);
  vec3 getPosition() const;

  void move(vec3 offset);

  void loadFromOBJ(string path);
  unordered_map<string, Material> loadMTL(string path);
  void loadTexture(string path, unsigned idx);

  virtual void draw(camera::Ptr cam) const;
};