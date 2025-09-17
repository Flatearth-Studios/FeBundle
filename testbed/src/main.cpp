#include "FeBundle/Core/Input/Inputs.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#define FE_DEBUG
#include <FeBundle/Core/Application.hpp>
#include <FeBundle/Core/Entrypoint.hpp>
#include <FeBundle/Core/GameTypes.hpp>
#include <FeBundle/Core/Logger.hpp>

using febundle::Error;
using febundle::Game;

std::expected<void, Error> febundle::CreateGame(Game &outGame) {
  outGame.windowSpecs.height = 960;
  outGame.windowSpecs.width = 1280;
  outGame.windowSpecs.title = "Test Game";

  outGame.Initialize = [](Game &g) -> bool {
    scene::Entity e = g.scene.Create();
    scene::Entity e2 = g.scene.Create();
    scene::Transform transform = scene::Transform(5.0f, 4.0f);
    scene::Transform t2 = scene::Transform(120.f, 120.f);
    scene::Sprite sprite = scene::Sprite(e, 64, 64);
    scene::Texture texture = scene::Texture(1, "assets/player.png");
    scene::Input input = scene::Input();
    scene::BoxCollider collider = scene::BoxCollider(64, 64);
    input.keyMap[core::input::Key::W] = false;
    input.keyMap[core::input::Key::A] = false;
    input.keyMap[core::input::Key::S] = false;
    input.keyMap[core::input::Key::D] = false;
    g.scene.AddComponent<scene::Transform>(e, transform);
    g.scene.AddComponent<scene::Sprite>(e, sprite);
    g.scene.AddComponent<scene::Texture>(e, texture);
    g.scene.AddComponent<scene::Input>(e, input);
    g.scene.AddComponent<scene::Collider>(e, collider);
    g.scene.AddComponent<scene::Collider>(e2, collider);
    g.scene.AddComponent<scene::Transform>(e2, t2);
    return true;
  };

  outGame.Update = [](Game &g, float32 deltaTime) -> bool {
    const auto entities = g.scene.AccessAll();

    for (const auto &[e, comp] : entities) {
      auto *transform = g.scene.GetComponent<scene::Transform>(e);
      auto *input = g.scene.GetComponent<scene::Input>(e);
      if (input == nullptr) {
        continue;
      }

      float32 velocity = 70.0f;
      auto collisions = g.collisionEventQ.Events();
      for (auto colEvt : collisions) {
        if (colEvt.e1 != colEvt.e2) {
          FLOG_INFO("colliding");
          velocity = 0.0f;
        }
      }
      g.collisionEventQ.Clear();

      if (input->keyMap[core::input::Key::W]) {
        transform->y -= velocity * deltaTime; 
      }
      if (input->keyMap[core::input::Key::A]) {
        transform->x -= velocity * deltaTime;
      }

      if (input->keyMap[core::input::Key::S]) {
        transform->y += velocity * deltaTime;
      }
      if (input->keyMap[core::input::Key::D]) {
        transform->x += velocity * deltaTime;
      }
      FLOG_TRACE("entity {} pos=({}, {}) rot={}", e, transform->x, transform->y,
                 transform->rot);
    }
    return true;
  };

  outGame.OnResize = [](Game &g, uint32 width, uint32 height) -> bool {
    LOG_INFO("Window resizing to {}x{}", width, height);
    return true;
  };

  return {};
}

int main() {
  Game gameInstance;
  auto res = febundle::CreateGame(gameInstance);

  febundle::App app(&gameInstance);
  if (auto res = app.Init(); !res.has_value()) {
    LOG_ERROR("Application failed to initialize");
    return -2;
  }

  if (auto res = app.Run(); !res.has_value()) {
    LOG_ERROR("Application did not exit gracefully");
    return -1;
  }

  return 0;
}
