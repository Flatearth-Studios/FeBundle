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
  outGame.windowSpecs.width  = 1280;
  outGame.windowSpecs.title  = "Test Game";

  outGame.Initialize = [](Game &g) -> bool {
    scene::Entity e  = g.scene.Create(); // player
    scene::Entity e2 = g.scene.Create(); // obstacle

    // --- Player setup ---
    scene::Transform transform(5.0f, 4.0f);
    scene::Sprite sprite(e, 64, 64);
    scene::Texture texture(1, "assets/player.png");
    scene::Input input;
    scene::BoxCollider collider(64, 64);

    input.keyMap[core::input::Key::W] = false;
    input.keyMap[core::input::Key::A] = false;
    input.keyMap[core::input::Key::S] = false;
    input.keyMap[core::input::Key::D] = false;

    scene::Kinematic kin;
    kin.lastSafePos = {transform.x, transform.y};

    g.scene.AddComponent<scene::Transform>(e, transform);
    g.scene.AddComponent<scene::Sprite>(e, sprite);
    g.scene.AddComponent<scene::Texture>(e, texture);
    g.scene.AddComponent<scene::Input>(e, input);
    g.scene.AddComponent<scene::BoxCollider>(e, collider);
    g.scene.AddComponent<scene::Kinematic>(e, kin);

    // --- Obstacle setup ---
    scene::Transform t2(120.f, 120.f);
    g.scene.AddComponent<scene::Transform>(e2, t2);
    g.scene.AddComponent<scene::BoxCollider>(e2, collider);

    // --- Collision response ---
    g.collisionSystem.OnCollisionEnter(
        [&](const febundle::systems::CollisionEvent &evt) {
          if (evt.e1 == e || evt.e2 == e) {
            auto *transform = g.scene.GetComponent<scene::Transform>(e);
            auto *kin       = g.scene.GetComponent<scene::Kinematic>(e);
            if (transform && kin) {
              transform->x = kin->lastSafePos.x;
              transform->y = kin->lastSafePos.y;
              FLOG_DEBUG("Entity {} rolled back due to collision", e);
            }
          }
        });

    return true;
  };

  outGame.Update = [](Game &g, float32 deltaTime) -> bool {
    const auto entities = g.scene.AccessAll();

    for (const auto &[e, comp] : entities) {
      auto *transform = g.scene.GetComponent<scene::Transform>(e);
      auto *input     = g.scene.GetComponent<scene::Input>(e);
      auto *kin       = g.scene.GetComponent<scene::Kinematic>(e);
      if (!input || !kin) continue;

      const float32 velocity = 70.0f;

      // Save last safe position before moving
      kin->lastSafePos = {transform->x, transform->y};

      if (input->keyMap[core::input::Key::W]) transform->y -= velocity * deltaTime;
      if (input->keyMap[core::input::Key::A]) transform->x -= velocity * deltaTime;
      if (input->keyMap[core::input::Key::S]) transform->y += velocity * deltaTime;
      if (input->keyMap[core::input::Key::D]) transform->x += velocity * deltaTime;

      FLOG_TRACE("entity {} pos=({}, {}) rot={}", e, transform->x, transform->y, transform->rot);
    }

    // Collision system runs once per frame
    g.collisionSystem.Update(g.scene);

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
