#include "game.hpp"

void Game::run()
{
  GLuint programID = LoadShaders(
    "shaders/basicVertShader.glsl",
    "shaders/basicFragShader.glsl"
  );

  gameObject::Ptr go = gameObject::create(programID);
  go->loadFromOBJ("./objects/icosphere.obj");

  static camera::Ptr cam = camera::create(vec3(0, 0, 0));

  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
  });
  
  glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
    if (cam->zoom - (0.5f * yOffset) > 0)
      cam->zoom += -0.5f * yOffset;
  });

  ImGuiIO& io = ImGui::GetIO(); (void) io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  
  ImGui::StyleColorsDark();

  ImGui::FileBrowser fileDialog;
  fileDialog.SetTitle("Select file");

  const char* glsl_version = "#version 150";

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  double prevTime;
  double currentTime = glfwGetTime();
  bool downL = false;
  bool downR = false;

  bool doDraw[50];
  bool useTexture[50];
  bool useNoise[50];
  for (unsigned i=0; i<50; i++) {
    doDraw[i]     = true;
    useTexture[i] = true;
    useNoise[i]   = false;
  }

  int textureIdx = -1;

  while (this->isRunning)
  {
    glfwPollEvents();
    
    prevTime = currentTime;
    currentTime = glfwGetTime();

    float delta = currentTime - prevTime;

    if (!io.WantCaptureMouse)
    {
      downL = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
      downR = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

      if (downL || downR) 
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    cam->update(this->window, delta, downL, downR);

    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
    {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow();
    
    ImGui::Begin("World options", NULL);

    ImGui::SeparatorText("Light settings");

    ImGui::SliderFloat("Light X", &go->lightPos.x, -10.0f, 10.0f);
    ImGui::SliderFloat("Light Y", &go->lightPos.y, -10.0f, 10.0f);
    ImGui::SliderFloat("Light Z", &go->lightPos.z, -10.0f, 10.0f);

    ImGui::SeparatorText("Object settings");

    if (ImGui::Button("Open")) {
      fileDialog.SetTypeFilters({".obj"});
      fileDialog.Open();
    }

    //ImGui::ColorEdit3("Object color", (float*)&go->objectColor[0]);

    ImGui::SliderFloat("Scale", &go->scale, 0.001f, 2.0f);

    ImGui::SeparatorText("Sub object options");

    for (unsigned i=0; i < go->vertexCount.size(); i++)
    {
      ImGui::PushID(i);
      char name[100] = { 0 }; 
      sprintf(name, "%s %d", go->names[i].c_str(), go->vertexCount[i] / 9);

      if (ImGui::CollapsingHeader(name)) {
        
        ImGui::Checkbox("Draw", &doDraw[i]);

        ImGui::Checkbox("Use texture", &useTexture[i]);
        if (ImGui::CollapsingHeader("Texture options", &useTexture[i])) {
          if (ImGui::Button("Load texture")) {
            fileDialog.SetTypeFilters({".png"});
            fileDialog.Open();
            textureIdx = i;
          }
        }   

        ImGui::Checkbox("Use noise", &useNoise[i]);
        if (ImGui::CollapsingHeader("Noise options", &useNoise[i])) {
          static float a[3] = { 0.0, 0.0, 0.0 };
          ImGui::SliderFloat3("Noise offset", a, -10.0, 10.0);
          go->noiseOffset[i] = vec3(a[0], a[1], a[2]);

          static float s = 1.0;
          ImGui::SliderFloat("Noise scale", &s, 0.001, 10.0);
          go->noiseScale[i] = s;

          static float sc = 0.5;
          ImGui::SliderFloat("Noise multiplier", &sc, 0.001, 2.0);
          go->noiseMultiplier[i] = sc;

          static int l = 1;
          ImGui::SliderInt("Noise layers", &l, 1, 10);
          go->noiseLayers[i] = (float)l;
        }

        go->useFlags[i].x = (useTexture[i] ? 1.0 : 0.0);
        go->useFlags[i].y = (useNoise[i]   ? 1.0 : 0.0);
        go->doDraw[i] = doDraw[i];
      }
      ImGui::PopID();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    fileDialog.Display();

    if (fileDialog.HasSelected()) {
      if (textureIdx == -1) {
        go->loadFromOBJ(fileDialog.GetSelected().string());  
      } else {
        go->loadTexture(fileDialog.GetSelected().string(), textureIdx);
        textureIdx = -1;
      }
      
      fileDialog.ClearSelected();
    }

    ImGui::Render();

    /*for (unsigned i=0; i < go->doDraw.size() && i < 50; i++) {
      go->doDraw[i] = doDraw[i];
      go->useFlags[i].x = (useTexture[i] ? 1.0 : 0.0);
      go->useFlags[i].y = (useNoise[i] ? 1.0 : 0.0);
    }*/

    glClearColor(130.0 / 255.0, 130.0 / 255.0, 130.0 / 255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    go->draw(cam);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->window);

    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwWindowShouldClose(this->window) != 0)
    {
      this->isRunning = false;
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glDeleteVertexArrays(1, &VertexArrayID);
  glfwDestroyWindow(window);
  glfwTerminate();
}