#define FE_DEBUG
#include "FeBundle/Core/Systems/RenderSystem.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Events/RenderEvents.hpp"
#include "FeBundle/Core/Events/SceneLoadedEvent.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"

namespace febundle::systems {

RenderSystem::RenderSystem(core::events::EventBus &evtBus, UIManager &uiManager)
    : _eventBus(evtBus), _uiManager(uiManager) {
  core::events::EventSubscription<core::events::SceneLoadedEvent> subscription{
      .subscriber = "RenderSystem",
      .callback = [&](const core::events::SceneLoadedEvent &evt)
          -> std::expected<void, Error> {
        if (evt.pScene == nullptr) {
          FLOG_ERROR("attempt to load nullptr scene");
          return std::unexpected{Error(ErrorName::LoadSceneEvent)};
        }

        switch (evt.pScene->Type()) {
        case scene::SceneType::UI:
          registerUIElements(*evt.pScene);
          break; 
        case scene::SceneType::World:
          break;
        }

        return {};
      },
  };

  if (auto res = _eventBus.Subscribe(subscription); !res.has_value()) {
    FLOG_WARN("failed to subscribe to SceneLoadedEvents");
  }
}

void RenderSystem::RenderScene(const scene::Scene &scene) {
  switch (scene.Type()) {
  case scene::SceneType::World: {
    renderWorld(scene);
    break;
  }
  case scene::SceneType::UI: {
    renderUI(scene);
    break;
  }
  }
}

void RenderSystem::registerUIElements(const scene::Scene &scene) {
  using namespace scene;
  _uiManager.Clear();
  for (auto &[entity, components] : scene.AccessAll()) {
    if (!components.contains(scene::Component::UI)) {
      continue;
    }

    auto *ui = scene.GetComponent<scene::UI>(entity);
    _uiManager.Register(scene.SceneId(), ui->pWidget);
  }
}

void RenderSystem::renderWorld(const scene::Scene &scene) {
  using namespace scene;
  for (auto &[entity, comps] : scene.AccessAll()) {
    if (!comps.contains(scene::Component::Sprite) ||
        !comps.contains(scene::Component::Transform))
      continue;

    const auto *sprite = scene.GetComponent<scene::Sprite>(entity);
    const auto *transform = scene.GetComponent<scene::Transform>(entity);

    core::events::WorldRenderEvent evt(*sprite, *transform);

    if (auto res = _eventBus.Push(evt); !res.has_value()) {
      FLOG_ERROR("failed to register WorldRenderEvent");
    }
  }
}

void RenderSystem::renderUI(const scene::Scene &scene) {
  const auto *pWidgets = _uiManager.WidgetsOf(scene.SceneId());
  if (pWidgets == nullptr) {
    return;
  }

  for (auto pWidget : *pWidgets) {
    core::events::UIRenderEvent evt([pWidget]() {
      if (pWidget == nullptr) {
        FLOG_WARN("cannot draw nullptr widget!");
        return;
      }
      pWidget->Draw();
    });

    if (auto res = _eventBus.Push(evt); !res.has_value()) {
      FLOG_ERROR("failed to register UIRenderEvent");
    }
  }
}

} // namespace febundle::systems
