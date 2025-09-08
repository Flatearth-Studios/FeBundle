#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_

#include "FeBundle/Core/Defines.hpp"

namespace febundle::scene {

using TextureHandle = uintptr_t;

enum class Component {
  Input,
  Transform,
  Texture,
  Sprite,
};

struct IComponent {
  virtual ~IComponent() = default;
  virtual enum Component Component() const = 0;
};

struct Input : public IComponent {
  enum Component Component() const override {
    return type;
  }

  static constexpr enum Component type = Component::Input;
  bool left, right, up, down;
};

struct Texture : public IComponent {
  enum Component Component() const override {
    return type;
  }

  static constexpr enum Component type = Component::Texture;
  fs::path path;
  TextureHandle texHandle;

  Texture(TextureHandle tex, const string &path)
    : path(path), texHandle(tex) {}
};

struct Transform : public IComponent {
  enum Component Component() const override {
    return type;
  } 
  

  static constexpr enum Component type = Component::Transform;
  float32 x{0}, y{0}, rot{0}, sx{1}, sy{1};

  Transform() {}
  Transform(float32 x, float32 y) 
    : x(x), y(y) {}
};

struct Sprite : public IComponent {
  enum Component Component() const override {
    return type;
  }

  static constexpr enum Component type = Component::Sprite;
  uint64 id{0};
  float32 width{1}, height{1};
  uint8 r{255}, g{255}, b{255}, a{255};

  Sprite() {}
  Sprite(uint64 id, float32 width, float32 height)
    : id(id), width(width), height(height) {}
};

}

#endif // INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_
