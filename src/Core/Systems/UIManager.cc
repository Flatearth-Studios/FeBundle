#define FE_DEBUG
#include "FeBundle/Core/Systems/UIManager.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Scene/Components.hpp"

namespace febundle::systems {

UIManager::UIManager(core::events::EventBus &evtBus) : _eventBus(evtBus) {}

void UIManager::Register(const string &sceneId, scene::WidgetPtr widget) {
  auto it = _widgets.find(sceneId);
  if (it != _widgets.end()) {
    auto &widgets = it->second;
    widgets.push_back(widget);
    return;
  }

  _widgets.emplace(sceneId, std::vector<scene::WidgetPtr>{widget});
}

const std::vector<scene::WidgetPtr> *
UIManager::WidgetsOf(const string &sceneId) const {
  auto it = _widgets.find(sceneId);
  if (it == _widgets.end()) {
    FLOG_ERROR("attempt to get widgets of unknown scene with id {}", sceneId);
    return nullptr;
  }

  return &_widgets.at(sceneId);
}

void UIManager::Clear() { _widgets.clear(); }

} // namespace febundle::systems
