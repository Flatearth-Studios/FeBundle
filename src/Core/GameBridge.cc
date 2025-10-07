#define FE_DEBUG
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Events/GameCommandEvent.hpp"
#include "FeBundle/Core/GameBridge.hpp"
#include "FeBundle/Core/Logger.hpp"

namespace febundle {

GameBridgeImpl::GameBridgeImpl(core::events::EventBus &evtBus)
    : _eventBus(evtBus) {}

void GameBridgeImpl::PostCommand(const string &name, void *payload) {
  core::events::GameCommandEvent evt{
      .name = name,
      .payload = payload,
  };

  auto res = _eventBus.Push<core::events::GameCommandEvent>(evt);
  if (!res.has_value()) {
    FLOG_WARN("failed to push game command '{}'", name);
  }
}

} // namespace febundle
