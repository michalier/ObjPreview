#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform float scale;
layout(location = 2) uniform vec3 lPos;
layout(location = 3) uniform float nMulti;
layout(location = 4) uniform vec3 aColor;
layout(location = 5) uniform vec3 dColor;
layout(location = 6) uniform vec2 useFlags;
layout(location = 7) uniform vec3 nOffset;
layout(location = 8) uniform float nScale;
layout(location = 9) uniform float nLayers;

out vec3 normal;
out vec3 fragPos;
out vec3 lightPos;
out float noiseMulitplier;
out vec3 ambientColor;
out vec3 diffuseColor;
out float useTexture;
out float useNoise;
out vec3 noiseOffset;
out float noiseScale;
out float noiseLayers;

out vec2 texCoord;

void main()
{
  gl_Position = MVP * vec4(scale * pos, 1.0);

  normal          = norm;
  fragPos         = scale * pos;
  lightPos        = lPos;
  noiseMulitplier = nMulti;
  ambientColor    = aColor;
  diffuseColor    = dColor;
  texCoord        = uv;
  useTexture      = useFlags.x;
  useNoise        = useFlags.y;
  noiseOffset     = nOffset;
  noiseScale      = nScale;
  noiseLayers     = nLayers;
}
