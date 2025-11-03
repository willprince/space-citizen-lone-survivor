#include "util.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <ctime>
#include <string>
int main() {
  const uint16_t windowWidth = 1920u;
  const uint16_t windowHeight = 1080u;
  auto window = sf::RenderWindow(sf::VideoMode({windowWidth, windowHeight}),
                                 "CMake SFML Project");
  sf::Clock clock;
  sf::Time sessionElapsedTimeTime = clock.getElapsedTime();
  sf::Font clockFont = sf::Font("src/fonts/3270NerdFontMono-Regular.ttf");
  sf::Text sessionElapsedTimeClockText =
      sf::Text(clockFont, std::to_string(sessionElapsedTimeTime.asSeconds()));
  sessionElapsedTimeClockText.setCharacterSize(20);
  sessionElapsedTimeClockText.setPosition({200, 200});
  window.setFramerateLimit(144);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::KeyReleased>()) {
        window.close();
      }
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    window.clear();
    sf::Time sessionElapsedTimeTime = clock.getElapsedTime();
    sessionElapsedTimeClockText.setString(
        std::to_string(sessionElapsedTimeTime.asSeconds()));
    window.draw(sessionElapsedTimeClockText);
    window.display();
  }
}
