#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow *window;
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "src/util/stb_image.hpp"

#include "src/game/game.hpp"
#include "src/util/exception.hpp"

using namespace std;

int main(int argc, char** argv) 
{
  try 
  {
    Game game(argc, argv, window);
    game.run();
  } 
  catch(exc& e)
  {
    cerr << e.what() << endl;
  }
  catch(string& e)
  {
    cout << e << endl;
  }
  catch(exception& e)
  {
    cout << e.what() << endl;
  }
  catch(...)  
  {
    cerr << "caught something" << endl;
  }

  return 0;
}