#include "glad/glad.h"
#include "processMonitor.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <ctime>

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

  // Initialize GLAD - MUST be done after context creation
  if (!gladLoadGL()) {
    return -1;
  }

  eng::ProcessMonitor processMonitor(FONT);

  // window.setFramerateLimit(120);
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
    window.draw(processMonitor);
    window.display();
  }
}
