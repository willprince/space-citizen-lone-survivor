#include "animation.hpp"
#include "animator.hpp"
#include "model.hpp"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
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
  // tell stb_image.h to flip loaded texture's on the y-axis (before loading
  // model).
  stbi_set_flip_vertically_on_load(true);

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

  // load models
  // -----------
  Model ourModel("src/resources/objects/square/test1.fbx");
  // Animation danceAnimation("src/resources/objects/square/test1.fbx",
  // &ourModel); Animator animator(&danceAnimation);

  while (window.isOpen()) {

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    currentFrame = frameClock.getElapsedTime().asSeconds();
    deltaFrame = currentFrame - lastFrame;
    lastFrame = currentFrame;

    wasdEventHandler(camera, deltaFrame);
    sf::Vector2 mousePos = sf::Mouse::getPosition();

    // animator.UpdateAnimation(deltaFrame);

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

    float sessionTime = sessionClock.getElapsedTime().asSeconds();

    shader.use();

    glm::mat4 view;
    view = camera.GetViewMatrix();

    glm::mat4 projection = glm::mat4();
    projection = glm::perspective(glm::radians(fov),
                                  float(windowWidth) / float(windowHeight),
                                  nearPlane, farPlane);

    shader.setMat("projection", projection);
    shader.setMat("view", view);

    // auto transforms = animator.GetFinalBoneMatrices();
    // for (int i = 0; i < transforms.size(); ++i) {
    //   shader.setMat("finalBonesMatrices[" + std::to_string(i) + "]",
    //                 transforms[i]);
    // }
    // // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    // it's a bit too big for our scene, so scale it down
    model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
    shader.setMat("model", model);
    ourModel.Draw(shader);
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

  // Cleanup
  ImGui::SFML::Shutdown();
  return 0;
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
