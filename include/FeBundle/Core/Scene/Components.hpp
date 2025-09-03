#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_

#include "FeBundle/Core/Defines.hpp"

namespace febundle::scene {

using TextureHandle = uintptr_t;

struct Transform {
  float32 x{0}, y{0}, rot{0}, sx{1}, sy{1};

  Transform() {}
  Transform(float32 x, float32 y) 
    : x(x), y(y) {}
};

struct Sprite {
  uint64 id{0};
  TextureHandle texture{0};
  float32 width{1}, height{1};
  uint8 r{255}, g{255}, b{255}, a{255};

  Sprite() {}
  Sprite(uint64 id, TextureHandle tex, float32 width, float32 height)
    : id(id), texture(tex), width(width), height(height) {}
};

}

#endif // INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_
