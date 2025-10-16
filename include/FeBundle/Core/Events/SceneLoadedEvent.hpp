#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_SCENE_LOADED_EVENT_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_SCENE_LOADED_EVENT_HPP_

#include "FeBundle/Core/Scene/Scene.hpp"
namespace febundle::core::events {

struct SceneLoadedEvent {
  const scene::Scene *cpScene;
};

}

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_SCENE_LOADED_EVENT_HPP_
