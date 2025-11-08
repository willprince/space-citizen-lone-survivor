#include "glad/glad.h"
#include "processMonitor.hpp"
#include "shader_s.hpp"
#include "stb_image/stb_image.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <ctime>
#include <iostream>
using namespace std;

int main() {
  const sf::Font FONT = sf::Font("src/fonts/3270NerdFontMono-Regular.ttf");
  const uint16_t windowWidth = 1920u;
  const uint16_t windowHeight = 1080u;
  auto window = sf::RenderWindow(sf::VideoMode({windowWidth, windowHeight}),
                                 "CMake SFML Project");
  window.setVerticalSyncEnabled(false);
  if (!window.setActive(true)) {
    return -1;
  }

  if (!gladLoadGL()) {
    return -1;
  }

  eng::ProcessMonitor processMonitor(FONT);
  Shader shader = Shader("src/shaders/shader.vs", "src/shaders/shader.fs");
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = {
      // positions         // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
  };
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // uncomment this call to draw in wireframe polygons.
  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  while (window.isOpen()) {

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto *resized = event->getIf<sf::Event::Resized>()) {
        glViewport(0, 0, resized->size.x, resized->size.y);
      }
    }
    processMonitor.update();
    window.clear();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    shader.setFloat("xOffset", 0.5f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(0);

    //  We have to unbind to avoid sfml trowing errors due to compatibility
    glBindVertexArray(0);
    glUseProgram(0);

    window.draw(processMonitor);
    window.display();
  }
}
