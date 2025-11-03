#version 330 core

uniform sampler2D ourTexture;

in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;
in float noiseMulitplier;
in vec3 ambientColor;
in vec3 diffuseColor;
in float useTexture;
in float useNoise;
in vec3 noiseOffset;
in float noiseScale;
in float noiseLayers;

in vec2 texCoord;

out vec4 color;

const int PERMUTATION[512] = int[](
  151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
  140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
  247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
   57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
   74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
   60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
   65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
  200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
   52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
  207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
  119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
  129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
  218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
   81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
  184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
  222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180,
  151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
  140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
  247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
   57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
   74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
   60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
   65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
  200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
   52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
  207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
  119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
  129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
  218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
   81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
  184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
  222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
);

float fade(float t) {
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float lerp(float a, float b, float t) {
  return a + t * (b - a);
}

float grad(int hash, vec3 p) {
  int h = hash & 15;
  float u = h < 8 ? p.x : p.y;
  float v = h < 4 ? p.y : (h == 12 || h == 14 ? p.x : p.z);
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float perlinNoise(vec3 p) {
  vec3 ip = floor(p);
  vec3 fp = p - ip;

  vec3 f = vec3(fade(fp.x), fade(fp.y), fade(fp.z));

  int i = int(ip.x) & 255;
  int j = int(ip.y) & 255;
  int k = int(ip.z) & 255;

  int a = PERMUTATION[i] + j;
  int aa = PERMUTATION[a] + k;
  int ab = PERMUTATION[a + 1] + k;
  int b = PERMUTATION[i + 1] + j;
  int ba = PERMUTATION[b] + k;
  int bb = PERMUTATION[b + 1] + k;

  float x1 = lerp(grad(PERMUTATION[aa], fp - vec3(0.0, 0.0, 0.0)), grad(PERMUTATION[ba], fp - vec3(1.0, 0.0, 0.0)), f.x);
  float x2 = lerp(grad(PERMUTATION[ab], fp - vec3(0.0, 1.0, 0.0)), grad(PERMUTATION[bb], fp - vec3(1.0, 1.0, 0.0)), f.x);
  float y1 = lerp(x1, x2, f.y);

  float x3 = lerp(grad(PERMUTATION[aa + 1], fp - vec3(0.0, 0.0, 1.0)), grad(PERMUTATION[ba + 1], fp - vec3(1.0, 0.0, 1.0)), f.x);
  float x4 = lerp(grad(PERMUTATION[ab + 1], fp - vec3(0.0, 1.0, 1.0)), grad(PERMUTATION[bb + 1], fp - vec3(1.0, 1.0, 1.0)), f.x);
  float y2 = lerp(x3, x4, f.y);

  return lerp(y1, y2, f.z);
}

void main()
{
  vec3 ambient = 0.2 * ambientColor;

  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * diffuseColor;

  vec4 c = vec4(ambient + diffuse, 1.0);
  if (useTexture > 0.5) {
    c = c * texture(ourTexture, texCoord);
  }

  float noise = 0.0;
  float sum = 0.0;
  float str = noiseScale;
  float n = noiseLayers;

  if (useNoise > 0.5) {
    while (n > 0) {
      noise += ((1.0 + perlinNoise((noiseOffset + fragPos) / str)) / 2.0) * str;
      sum += str;
      str *= noiseMulitplier;
      n -= 1.0;
    }
    noise = noise / sum;

    c = c * vec4(vec3(noise), 1);
  }

  color = c;
}