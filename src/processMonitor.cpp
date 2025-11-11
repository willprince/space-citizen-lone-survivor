#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include "processMonitor.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <psapi.h>
#include <windows.h>
namespace eng {

ProcessMonitor::ProcessMonitor(const sf::Font &font)
    : m_font(font), m_sessionClock(), m_fpsClock(), m_fps(0),
      m_sessionElapsedTimeSecond(0), m_sessionTimeText(m_font, "", 12),
      m_fpsText(m_font, "", 12), m_memoryText(m_font, "", 12) {
  m_fpsText.setPosition({12, 12});
  m_fpsText.setFillColor(sf::Color::White);

  m_sessionTimeText.setPosition({100, 12});
  m_sessionTimeText.setFillColor(sf::Color::White);

  m_memoryText.setPosition({300, 12});
  m_memoryText.setFillColor(sf::Color::White);
}

void ProcessMonitor::update() {
  this->updateSessionElapsedTime();
  this->updateAvgFps();
  this->updateMemoryUsage();

  m_sessionTimeText.setString("sessionTime: " +
                              std::to_string(m_sessionElapsedTimeSecond));
  m_fpsText.setString("fps: " + std::to_string(m_fpsAvg));
  m_memoryText.setString("memory: " + std::to_string(m_privateMemoryUsage));
};

void ProcessMonitor::updateSessionElapsedTime() {
  m_sessionElapsedTimeSecond = m_sessionClock.getElapsedTime().asSeconds();
}

void ProcessMonitor::updateAvgFps() {
  m_fps = 1 / m_fpsClock.getElapsedTime().asSeconds();
  m_fpsTotal += m_fps;
  if (m_fpsCount < m_fpsArrSize) {
    m_queue.push_back(m_fps);
    m_fpsCount += 1;
  } else {
    int oldestFps = m_queue.front();
    m_queue.pop_front();
    m_fpsTotal -= oldestFps;
    m_queue.push_back(m_fps);
  }
  m_fpsAvg = m_fpsTotal / m_fpsCount;
  m_fpsClock.restart();
}

void ProcessMonitor::updateMemoryUsage() {
  //  Private bytes refer to the amount of memory allocated to a process that
  //  cannot be shared with other processes, while the working set is the total
  //  amount of physical memory (RAM) a process is actively using, including
  //  both private and shared memory
  //
  //  You should look at private memory if you are trying to identify a memory
  //  leak, and at the working set if you are trying to understand a process's
  //  impact on physical RAM or if the system is low on memory
  if (GetProcessMemoryInfo(m_hProcess, (PROCESS_MEMORY_COUNTERS *)&m_pmc,
                           sizeof(m_pmc))) {
    m_workinkMemoryUsage =
        static_cast<double>(m_pmc.WorkingSetSize) / (1024 * 1024);
    m_privateMemoryUsage =
        static_cast<double>(m_pmc.PrivateUsage) / (1024 * 1024);
  } else {
    std::cerr << "Error getting process memory info." << std::endl;
  }
}

} // namespace eng
