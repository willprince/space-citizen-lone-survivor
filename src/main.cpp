#include "processMonitor.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
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

  eng::ProcessMonitor processMonitor(FONT);

  // window.setFramerateLimit(120);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    processMonitor.update();
    window.clear();
    window.draw(processMonitor);
    window.display();
  }
}
