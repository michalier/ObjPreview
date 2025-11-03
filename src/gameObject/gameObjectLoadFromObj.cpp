#include "gameObject.hpp"

void gameObject::loadFromOBJ(string path)
{
  glDeleteBuffers(vertexBuffer.size(), &vertexBuffer[0]);
  glDeleteBuffers(normalBuffer.size(), &normalBuffer[0]);
  glDeleteBuffers(uvBuffer.size(), &uvBuffer[0]);
  glDeleteTextures(textures.size(), &textures[0]);

  vertexBuffer.clear();
  normalBuffer.clear();
  uvBuffer.clear();
  textures.clear();
  
  doDraw.clear();
  names.clear();
  vertexCount.clear();
  useFlags.clear();
  noiseOffset.clear();
  noiseScale.clear();
  noiseLayers.clear();
  noiseMultiplier.clear();

  ifstream file(path);
  
  if (!file.is_open())
    throw exc(("File " + path + " does not exist").c_str(), __LINE__, __FILE__);

  vector<vec3> verts;
  vector<vec3> normals;
  vector<vec2> uvs;

  vector<GLfloat> finalVerts;
  vector<GLfloat> finalNormals;
  vector<GLfloat> finalUVs;

  unsigned offsetVert = 1;
  unsigned offsetNorm = 1;
  unsigned offsetUV = 1;

  string part;
  file >> part;

  int i = -1;
  int in_part = -1;

  unordered_map<string, Material> library;

  float max_size = 1.0;

  do
  {
    if (part[0] == '#')
    {
      getline(file, part);
      continue;
    }
    if (part == "mtllib") {
      string mat_path;
      file >> mat_path;

      mat_path = path.substr(0, path.find_last_of('/')) + "/" + mat_path;

      library = this->loadMTL(mat_path);
    } else if (part == "o") {
      in_part = -1;
      getline(file, part);
      names.push_back(part);
      vertexCount.push_back(0);
      doDraw.push_back(true);
      textures.push_back(0);

      useFlags.push_back(vec2(0.0));
      noiseOffset.push_back(vec3(0.0));
      noiseScale.push_back(1.0);
      noiseLayers.push_back(1.0);
      noiseMultiplier.push_back(0.5);

      ambientColor.push_back(vec3(1.0));
      diffuseColor.push_back(vec3(1.0));

      vertexBuffer.push_back(0);
      normalBuffer.push_back(0);
      uvBuffer.push_back(0);
      
      if (i >= 0) {
        glGenBuffers(1, &vertexBuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, finalVerts.size() * sizeof(GLfloat), &finalVerts[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &normalBuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, finalNormals.size() * sizeof(GLfloat), &finalNormals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &uvBuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, finalUVs.size() * sizeof(GLfloat), &finalUVs[0], GL_STATIC_DRAW);
      
        offsetVert += verts.size();
        offsetNorm += normals.size();
        offsetUV += uvs.size();
      }
      i++;

      verts.clear();
      normals.clear();
      uvs.clear();

      finalVerts.clear();
      finalNormals.clear();
      finalUVs.clear();
    } else if(part == "v") {
      float x, y, z;
      file >> x >> y >> z;

      if (x > max_size)
        max_size = x;
      if (y > max_size)
        max_size = y;
      if (z > max_size)
        max_size = z;

      verts.push_back(vec3(x, y, z));
    } else if (part == "vn") {
      float x, y, z;
      file >> x >> y >> z;
      normals.push_back(vec3(x, y, z));
    } else if (part == "vt") {
      float u, v;
      file >> u >> v;
      uvs.push_back(vec2(u, v));
    } else if (part == "usemtl") {
      string name;
      file >> name;

      if (library.find(name) == library.end()) 
        throw exc("No material " + name + " loaded", __LINE__, __FILE__);
      
      if (library.at(name).texture.length() > 0) {
        useFlags[i].x = 1.0;
        this->loadTexture("textures/" + library.at(name).texture, i);
      }
      
      this->ambientColor[i] = library.at(name).ambient;
      this->diffuseColor[i] = library.at(name).diffuse;

      if (in_part > -1) {
        names.push_back(names[i]);
        vertexCount.push_back(0);
        doDraw.push_back(true);
        textures.push_back(0);

        useFlags.push_back(vec2(0.0));
        noiseOffset.push_back(vec3(0.0));
        noiseScale.push_back(1.0);
        noiseLayers.push_back(1.0);
        noiseMultiplier.push_back(0.5);

        ambientColor.push_back(vec3(1.0));
        diffuseColor.push_back(vec3(1.0));

        vertexBuffer.push_back(0);
        normalBuffer.push_back(0);
        uvBuffer.push_back(0);
        
        if (i >= 0) {
          glGenBuffers(1, &vertexBuffer[i]);
          glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]);
          glBufferData(GL_ARRAY_BUFFER, finalVerts.size() * sizeof(GLfloat), &finalVerts[0], GL_STATIC_DRAW);

          glGenBuffers(1, &normalBuffer[i]);
          glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
          glBufferData(GL_ARRAY_BUFFER, finalNormals.size() * sizeof(GLfloat), &finalNormals[0], GL_STATIC_DRAW);

          glGenBuffers(1, &uvBuffer[i]);
          glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[i]);
          glBufferData(GL_ARRAY_BUFFER, finalUVs.size() * sizeof(GLfloat), &finalUVs[0], GL_STATIC_DRAW);
        }
        i++;
        finalVerts.clear();
        finalNormals.clear();
        finalUVs.clear();
      }
      in_part++;
    } else if (part == "f") {
      string a;

      unsigned vertI[3] = { 0 };
      unsigned normI[3] = { 0 };
      unsigned uvI[3] = { 0 };

      for (unsigned n=0; n < 3; n++)
      {
        file >> a;

        size_t pos = a.find_first_of('/');

        unsigned fi = atoi(a.substr(0, pos).c_str()) - offsetVert;

        vertI[n] = fi;
        vec3 vert = verts[fi];

        finalVerts.push_back(vert.x);
        finalVerts.push_back(vert.y);
        finalVerts.push_back(vert.z);

        a = a.substr(pos+1);
        pos = a.find_first_of('/');

        unsigned ui = atoi(a.substr(0, pos).c_str()) - offsetUV;

        uvI[n] = ui;
        vec2 uv = uvs[ui];

        finalUVs.push_back(uv.x);
        finalUVs.push_back(uv.y);
        
        a = a.substr(pos+1);

        unsigned ni = atoi(a.c_str()) - offsetNorm;

        normI[n] = ni;
        vec3 norm = normals[ni];

        finalNormals.push_back(norm.x);
        finalNormals.push_back(norm.y);
        finalNormals.push_back(norm.z);

        vertexCount[i] += 3;
      }

      file >> a;
      if (!file.eof() && a != "f" && a != "usemtl" && a != "vt" && a != "vn" && a != "v" && a != "s" && a != "l" && a != "o")
      {
        finalVerts.push_back(verts[vertI[2]].x);
        finalVerts.push_back(verts[vertI[2]].y);
        finalVerts.push_back(verts[vertI[2]].z);
        
        finalNormals.push_back(normals[normI[2]].x);
        finalNormals.push_back(normals[normI[2]].y);
        finalNormals.push_back(normals[normI[2]].z);

        finalUVs.push_back(uvs[uvI[2]].x);
        finalUVs.push_back(uvs[uvI[2]].y);

        size_t pos = a.find_first_of('/');

        vec3 vert = verts[atoi(a.substr(0, pos).c_str()) - offsetVert];

        finalVerts.push_back(vert.x);
        finalVerts.push_back(vert.y);
        finalVerts.push_back(vert.z);

        a = a.substr(pos+1);
        pos = a.find_first_of('/');

        vec2 uv = uvs[atoi(a.substr(0, pos).c_str()) - offsetUV];

        finalUVs.push_back(uv.x);
        finalUVs.push_back(uv.y);
        
        a = a.substr(pos+1);

        vec3 norm = normals[atoi(a.c_str()) - offsetNorm];

        finalNormals.push_back(norm.x);
        finalNormals.push_back(norm.y);
        finalNormals.push_back(norm.z);

        finalVerts.push_back(verts[vertI[0]].x);
        finalVerts.push_back(verts[vertI[0]].y);
        finalVerts.push_back(verts[vertI[0]].z);
        
        finalNormals.push_back(normals[normI[0]].x);
        finalNormals.push_back(normals[normI[0]].y);
        finalNormals.push_back(normals[normI[0]].z);

        finalUVs.push_back(uvs[uvI[0]].x);
        finalUVs.push_back(uvs[uvI[0]].y);

        vertexCount[i] += 3;
      } else {
        part = a;
        continue;
      }
    } else {
      getline(file, part);
    }
    file >> part;
  } while (!file.eof());

  glGenBuffers(1, &vertexBuffer[i]);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]);
  glBufferData(GL_ARRAY_BUFFER, finalVerts.size() * sizeof(GLfloat), &finalVerts[0], GL_STATIC_DRAW);   
  
  glGenBuffers(1, &normalBuffer[i]);
  glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
  glBufferData(GL_ARRAY_BUFFER, finalNormals.size() * sizeof(GLfloat), &finalNormals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uvBuffer[i]);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[i]);
  glBufferData(GL_ARRAY_BUFFER, finalUVs.size() * sizeof(GLfloat), &finalUVs[0], GL_STATIC_DRAW);

  scale = 1.0f / max_size;
}

void gameObject::loadTexture(string path, unsigned idx)
{
  if (this->texture_dict.find(path) == texture_dict.end())
  {
    glGenTextures(1, &textures[idx]);
    glBindTexture(GL_TEXTURE_2D, textures[idx]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
      std::cout << "Failed to load texture |" << path << "|" << std::endl;
    }
    stbi_image_free(data);

    texture_dict.insert(pair(path, textures[idx]));
  }
  else 
  {
    textures[idx] = texture_dict.at(path);
  }
}