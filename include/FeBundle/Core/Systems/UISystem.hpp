#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_UI_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_UI_SYSTEM_HPP_

#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Math/Math.hpp"

namespace febundle::systems {

class UISystem {
public:
  explicit UISystem(core::events::EventBus &evtBus);

  void AddElement(const std::shared_ptr<assets::Texture> &texture,
                  core::math::Vec2 position, core::math::Vec2 size);

  void Update(float32 deltaTime);

private:
  struct guiElement {
    std::shared_ptr<assets::Texture> texture;
    core::math::Vec2 position, size;
  };

private:
  std::vector<guiElement> _elements;
  core::events::EventBus &_eventBus;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_UI_SYSTEM_HPP_
