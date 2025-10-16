#ifndef INCLUDE_FEBUNDLE_CORE_UI_WIDGETS_HPP_
#define INCLUDE_FEBUNDLE_CORE_UI_WIDGETS_HPP_

#include "imgui.h"
namespace febundle::ui {

class Widget {
public:
  virtual ~Widget() = default;
  virtual void Draw() = 0;

  inline virtual void SetPosition(const ImVec2 &position) {
    _position = position;
  }

  inline virtual void SetSize(const ImVec2 &size) {
    _size = size;
  }

  inline virtual void SetVisible(bool visibility) {
    _isVisible = visibility;
  }

protected:
  bool _isVisible{true};
  ImVec2 _position, _size;
};

}

#endif // INCLUDE_FEBUNDLE_CORE_UI_WIDGETS_HPP_
