#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_SCENE_LOADED_EVENT_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_SCENE_LOADED_EVENT_HPP_

#include "FeBundle/Core/Scene/Scene.hpp"
#include <optional>
namespace febundle::core::events {

struct SceneLoadedEvent {
  scene::Scene *pScene;
  std::optional<float32> oViewportWidth, oViewportHeight;
};

}

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_SCENE_LOADED_EVENT_HPP_
