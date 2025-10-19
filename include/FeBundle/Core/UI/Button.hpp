#ifndef INCLUDE_FEBUNDLE_CORE_UI_BUTTON_HPP_
#define INCLUDE_FEBUNDLE_CORE_UI_BUTTON_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/UI/Widget.hpp"
#include "imgui.h"
#include <functional>

namespace febundle::ui {

class Button : public Widget {
public:
  Button(const string &text, std::function<void()> onClick,
         ImVec2 position = ImVec2(0, 0), ImVec2 size = ImVec2(0, 0))
      : _text(text), _onClick(std::move(onClick)) {
    this->SetPosition(position);
    this->SetSize(size);
  }

  void Draw() override {
    if (!_isVisible) {
      return;
    }

    // Move ImGui cursor before drawing
    ImGui::SetCursorPos(_position);
    // Draw the button (use size only if explicitly set)
    bool clicked = (_size.x > 0 && _size.y > 0)
                       ? ImGui::Button(_text.c_str(), _size)
                       : ImGui::Button(_text.c_str());

    if (clicked && _onClick) {
      _onClick();
    }
  }

private:
  string _text;
  std::function<void()> _onClick;
};

} // namespace febundle::ui

#endif // INCLUDE_FEBUNDLE_CORE_UI_BUTTON_HPP_
