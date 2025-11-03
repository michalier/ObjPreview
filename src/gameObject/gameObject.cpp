#include "gameObject.hpp"

gameObject::gameObject(GLuint programID)
{
  this->programID = programID;

  positon = vec3(0, 0, 0);
  lightPos = vec3(0, 5, 0);
  scale = 1.0f;
}

gameObject::Ptr gameObject::create(GLuint programID)
{
  gameObject::Ptr w(new gameObject(programID));
  if (w == nullptr)
    throw exc("Could not create game object", __LINE__, __FILE__);

  return w;
}

gameObject::~gameObject()
{
  glDeleteBuffers(vertexBuffer.size(), &vertexBuffer[0]);
  glDeleteBuffers(normalBuffer.size(), &normalBuffer[0]);
  glDeleteBuffers(uvBuffer.size(), &uvBuffer[0]);
  glDeleteTextures(textures.size(), &textures[0]);
}

void gameObject::setPosition(vec3 position)
{
  this->positon = position;
}

vec3 gameObject::getPosition() const
{
  return this->positon;
}

void gameObject::move(vec3 offset)
{
  this->positon += offset;
}

void gameObject::draw(camera::Ptr cam) const
{
  glUseProgram(programID);
  
  for (unsigned i=0; i < vertexBuffer.size(); i++) {
    if (doDraw[i]) {
      glBindTexture(GL_TEXTURE_2D, textures[i]);

      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[i]);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
      
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, normalBuffer[i]);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[i]);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      glUniformMatrix4fv(0, 1, 0, &cam->getMVP()[0][0]);
      glUniform1fv(1, 1, &scale);
      glUniform3fv(2, 1, &lightPos[0]);
      glUniform1fv(3, 1, &noiseMultiplier[i]);
      glUniform3fv(4, 1, &ambientColor[i][0]);
      glUniform3fv(5, 1, &diffuseColor[i][0]);
      glUniform2fv(6, 1, &useFlags[i][0]);
      glUniform3fv(7, 1, &noiseOffset[i][0]);
      glUniform1fv(8, 1, &noiseScale[i]);
      glUniform1fv(9, 1, &noiseLayers[i]);

      glDrawArrays(GL_TRIANGLES, 0, vertexCount[i]);

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(2);
    }
    
  }
}