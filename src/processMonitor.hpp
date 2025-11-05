#pragma once
#ifndef PROCESS_MONITOR_HPP
#define PROCESS_MONITOR_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <deque>
#include <processthreadsapi.h>
#include <psapi.h>
#include <winnt.h>
using namespace std;
namespace eng {
// TODO
// PROCESS MONITORING
// [x] Display time elapsed
// [x] Display avg fps
// [x] Display memory usage
// [] Display cpu usage
// [] Display disk usage
class ProcessMonitor : public sf::Drawable {

public:
  ProcessMonitor(const sf::Font &font);
  void update();

private:
  sf::Font m_font;

  sf::Clock m_sessionClock;
  float m_sessionElapsedTimeSecond;

  sf::Clock m_fpsClock;
  int m_fps;
  const static int m_fpsArrSize = 1000;
  std::deque<int> m_queue;
  int m_fpsCount = 0;
  int m_fpsTotal = 0;
  int m_fpsAvg;

  PROCESS_MEMORY_COUNTERS_EX m_pmc;
  HANDLE m_hProcess = GetCurrentProcess();
  double m_workinkMemoryUsage = 0;
  double m_privateMemoryUsage = 0;

  mutable sf::Text m_sessionTimeText;
  mutable sf::Text m_fpsText;
  mutable sf::Text m_memoryText;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
    target.draw(m_fpsText);
    target.draw(m_sessionTimeText);
    target.draw(m_memoryText);
  }
  void updateSessionElapsedTime();
  void updateAvgFps();
  void updateMemoryUsage();
};
} // namespace eng

#endif // PROCESS_MONITOR_HPP
