#ifndef INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_
#define INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_

#include "FeBundle/Core/Window/Window.hpp"
#include <functional>

namespace febundle {

struct Game {
  window::WindowSpecs windowSpecs;

  std::function<bool(struct Game &)> Initialize;
  std::function<bool(struct Game &, float32)> Update;
  std::function<bool(struct Game &, float32)> Render;
  std::function<bool(struct Game &, uint32 width, uint32 height)> OnResize;

  Game()
      : Initialize(nullptr), Update(nullptr), Render(nullptr),
        OnResize(nullptr) {}
};

} // namespace febundle

#endif // INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_
