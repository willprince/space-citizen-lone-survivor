#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

using namespace std;

class Background {
public:
  enum Background_Style { SPACE_1, SPACE_2 };
  enum Background_Style style = Background_Style::SPACE_1;
  void draw() {
    float vertices[] = {
        -1, 1,  0, // top left
        -1, -1, 0, // bottom left
        1,  -1, 0, // bottom right
        -1, 1,  0, // top left
        1,  1,  0, // top right
        1,  -1, 0, // bottom right
    };
    int VBO, VBA, EBO;
  }
};
#endif
