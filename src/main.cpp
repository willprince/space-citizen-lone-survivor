#include "camera.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "processMonitor.hpp"
#include "shader_s.hpp"
#include "stb_image/stb_image.h"
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <minwindef.h>

using namespace std;

void wasdEventHandler(Camera &camera, float frameDelta);

int main() {
  const sf::Font FONT = sf::Font("src/fonts/3270NerdFontMono-Regular.ttf");
  const uint16_t windowWidth = 1920u;
  const uint16_t windowHeight = 1080u;
  auto window = sf::RenderWindow(sf::VideoMode({windowWidth, windowHeight}),
                                 "IsoMetric engine");
  window.setMouseCursorVisible(false);
  window.setVerticalSyncEnabled(false);

  if (!window.setActive(true)) {
    return -1;
  }

  if (!gladLoadGL()) {
    return -1;
  }

  bool loadDefaultFont = true;
  auto gui = ImGui::SFML::Init(window, loadDefaultFont);

  eng::ProcessMonitor processMonitor(FONT);
  Shader shader = Shader("src/shaders/shader.vs", "src/shaders/shader.fs");

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  unsigned int indices[] = {
      0, 1, 3, // R Triangle
      1, 2, 3  // L Triangle
  };

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // vertex att
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // text att
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Loading container texture 1
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // Configuration Textute
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("src/textures/container.jpg", &width, &height, &nrChannels, 0);
  if (data) {

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    cout << "Failed to load container texture" << endl;
  }
  stbi_image_free(data);

  // Loading container texture 2
  unsigned int texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  // Configuration Textute
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);
  data = stbi_load("src/textures/awesomeface.png", &width, &height, &nrChannels,
                   0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    cout << "Failed to load container texture" << endl;
  }
  stbi_image_free(data);

  float smileyTransparence = 0;
  shader.use();
  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);
  shader.setFloat("texture2Transparance", smileyTransparence);

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  sf::Clock sessionClock;
  sf::Clock guiClock;
  sf::Clock frameClock;

  // Frustum
  float fov = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 100.0f;
  // Camera
  Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
  float lastX = windowWidth / 2.0f;
  float lastY = windowHeight / 2.0f;
  bool firstMouse = true;

  float currentFrame = 0.0f;
  float deltaFrame = 0.0f;
  float lastFrame = 0.0f;

  while (window.isOpen()) {

    currentFrame = frameClock.getElapsedTime().asSeconds();
    deltaFrame = currentFrame - lastFrame;
    lastFrame = currentFrame;

    wasdEventHandler(camera, deltaFrame);
    sf::Vector2 mousePos = sf::Mouse::getPosition();

    float offsetX = mousePos.x - lastX;
    float offsetY = lastY - mousePos.y;
    lastX = mousePos.x;
    lastY = mousePos.y;
    camera.ProcessMouseMovement(offsetX, offsetY);

    while (const std::optional event = window.pollEvent()) {
      ImGui::SFML::ProcessEvent(window, event.value());
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto *resized = event->getIf<sf::Event::Resized>()) {
        glViewport(0, 0, resized->size.x, resized->size.y);
      } else if (const auto *keyPressed =
                     event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
          window.close();
        else {
        }
      }
    }

    window.clear();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glEnable(GL_DEPTH_TEST);

    float sessionTime = sessionClock.getElapsedTime().asSeconds();

    glm::mat4 view;
    view = camera.GetViewMatrix();

    glm::mat4 projection = glm::mat4();
    projection = glm::perspective(glm::radians(fov),
                                  float(windowWidth) / float(windowHeight),
                                  nearPlane, farPlane);
    shader.setMat("view", view);
    shader.setMat("projection", projection);

    // Draw cubePositions
    glBindVertexArray(VAO);
    for (int i = 0; i < 10; i++) {

      glm::mat4 model = glm::mat4(1.0f);

      model = glm::translate(model, cubePositions[i]);

      shader.setMat("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    //  We have to unbind to avoid sfml trowing errors due to compatibility
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Unbind texture so other draw are not affected like processMonitorFonts
    // for exemple
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);

    ImGui::SFML::Update(window, guiClock.restart());
    // ImGui::ShowDemoWindow();
    ImGui::Begin("ENGINE CONTROLS");
    ImGui::SliderFloat("FOV", &fov, 0.0f, 100.0f);
    ImGui::SliderFloat("Near plane", &nearPlane, 0.0f, 100.0f);
    ImGui::SliderFloat("Far plane", &farPlane, 0.0f, 100.0f);
    ImGui::End();

    ImGui::SFML::Render(window);
    window.draw(processMonitor);
    window.display();
    processMonitor.update();
  }
}

void wasdEventHandler(Camera &camera, float frameDelta) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
    camera.ProcessKeyboard(Camera_Movement::FORWARD, frameDelta);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
    camera.ProcessKeyboard(Camera_Movement::BACKWARD, frameDelta);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
    camera.ProcessKeyboard(Camera_Movement::LEFT, frameDelta);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
    camera.ProcessKeyboard(Camera_Movement::RIGHT, frameDelta);
  }
}
