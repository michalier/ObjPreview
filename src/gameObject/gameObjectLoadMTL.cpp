#include "gameObject.hpp"

unordered_map<string, Material> gameObject::loadMTL(string path) 
{
  ifstream file(path);
  if (!file.is_open())
    throw exc(("File " + path + " does not exist").c_str(), __LINE__, __FILE__);

  string part;
  string current;

  unordered_map<string, Material> library;

  while (!file.eof())
  {
    file >> part;
    if (part == "newmtl") {
      file >> current;
      library.insert(pair(current, Material()));
    } else if (part == "Ka") {
      float a, b, c;
      file >> a >> b >> c;
      library.at(current).ambient = vec3(a, b, c);
    } else if (part == "Kd") {
      float a, b, c;
      file >> a >> b >> c;
      library.at(current).diffuse = vec3(a, b, c);
    } else if (part == "map_Kd") {
      file >> part;
      library.at(current).texture = part; 
    } else {
      getline(file, part);
    }    
  }

  return library;
}

Material::Material()
{
  this->ambient = vec3(1.0f);
  this->diffuse = vec3(1.0f);
  this->shininess = 1.0f;
  this->texture = "";
}