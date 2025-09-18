#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_COMPONENTS_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Input/Inputs.hpp"
#include "FeBundle/Core/Math/Math.hpp"

namespace febundle::scene {

using TextureHandle = uintptr_t;

enum class Component {
  Input,
  Collider,
  BoxCollider,
  CircleCollider,
  Transform,
  Texture,
  Sprite,
};

enum class ShapeType {
  Null,
  Box,
  Circle,
};

struct IComponent {
  virtual ~IComponent() = default;
  virtual enum Component Component() const = 0;
};

struct Collider : public IComponent {
  enum Component Component() const override {
    return Component::Collider; 
  }

  virtual float32 Width() const {
    return 0.0f;
  };
  virtual float32 Height() const {
    return 0.0f;
  }
  virtual ShapeType Shape() const {
    return ShapeType::Null; 
  }
};

struct BoxCollider : public Collider {
  enum Component Component() const override {
    return type;
  }

  float32 Width() const override {
    return width;
  }

  float32 Height() const override {
    return height;
  }

  ShapeType Shape() const override {
    return ShapeType::Box;
  }

  BoxCollider() : width(0), height(0) {}
  BoxCollider(float32 width, float32 height)
    : width(width), height(height) {}
  
  static constexpr enum Component type = Component::BoxCollider; 
  float32 width, height;
};

struct CircleCollider : public Collider {
  enum Component Component() const override {
    return type;
  }

  float32 Width() const override {
    return radius * 2.0f;
  }

  float32 Height() const override {
    return radius * 2.0f;
  }

  ShapeType Shape() const override {
    return ShapeType::Circle;
  }

  CircleCollider() : radius(0) {}
  CircleCollider(float32 radius) : radius(radius) {}

  static constexpr enum Component type = Component::CircleCollider;
  float32 radius;
};

struct Input : public IComponent {
  enum Component Component() const override {
    return type;
  }

  static constexpr enum Component type = Component::Input;
  umap<core::input::Key, bool> keyMap;
};

struct Kinematic : public IComponent {
  enum Component Component() const override {
    return type;
  }
  
  static constexpr enum Component type = Component::Input;
  math::Vec2 lastSafePos{};
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
