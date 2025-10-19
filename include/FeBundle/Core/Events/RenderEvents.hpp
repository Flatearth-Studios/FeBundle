#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_RENDER_EVENTS_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_RENDER_EVENTS_HPP_

#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Math/Math.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include <functional>

namespace febundle::core::events {

enum class RenderLayer {
  World,
  UI,
};

struct RenderEvent {
  RenderLayer layer{RenderLayer::World};
  virtual ~RenderEvent() = default;
};

struct WorldRenderEvent : RenderEvent {
    scene::Sprite sprite{};
    scene::Transform transform{};
    WorldRenderEvent(const scene::Sprite &s, 
                     const scene::Transform &t) {
        sprite = s;
        transform = t;
        layer = RenderLayer::World;
    }
};

struct UIRenderEvent : RenderEvent {
  std::function<void()> drawFn;

  UIRenderEvent() : drawFn(nullptr) {}
  UIRenderEvent(std::function<void()> fn) : drawFn(fn) {}
};

}

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_RENDER_EVENTS_HPP_
