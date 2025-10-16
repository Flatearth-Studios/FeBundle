#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_UI_MANAGER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_UI_MANAGER_HPP_

#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include <stduuid.h>

namespace febundle::systems {

class UIManager {
public:
  explicit UIManager(core::events::EventBus &evtBus);

  void Register(const string &sceneId, scene::WidgetPtr widget);

  const std::vector<scene::WidgetPtr> *WidgetsOf(const string &sceneId) const;
  void Clear();

private:
  core::events::EventBus &_eventBus;
  umap<string, std::vector<scene::WidgetPtr>> _widgets;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_UI_MANAGER_HPP_
