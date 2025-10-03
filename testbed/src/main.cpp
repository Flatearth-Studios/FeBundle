#define FE_DEBUG
#include <FeBundle/Core/Application.hpp>
#include <FeBundle/Core/Entrypoint.hpp>
#include <FeBundle/Core/GameTypes.hpp>
#include <FeBundle/Core/Input/Inputs.hpp>
#include <FeBundle/Core/Logger.hpp>
#include <FeBundle/Core/Scene/Components.hpp>
#include <FeBundle/Core/Scene/Entity.hpp>
#include <FeBundle/Core/Scene/Scene.hpp>

using febundle::Error;
using febundle::Game;
using namespace febundle;

static scene::Scene makeLevel1(Game &outGame) {
  scene::Scene scene;
  scene::Entity e = scene.Create();  // player
  scene::Entity e2 = scene.Create(); // obstacle

  // --- Player setup ---
  scene::Transform transform(5.0f, 4.0f);
  scene::Sprite sprite(e, 64, 64);
  scene::Input input;
  scene::BoxCollider collider(64, 64);

  sprite.assetHandle = outGame.assetLoader.LoadFor(e, "assets/player.png",
                                                   assets::AssetType::Texture);

  input.keyMap[core::input::Key::W] = false;
  input.keyMap[core::input::Key::A] = false;
  input.keyMap[core::input::Key::S] = false;
  input.keyMap[core::input::Key::D] = false;

  scene::Kinematic kin;
  kin.lastSafePos = {transform.x, transform.y};

  scene.AddComponent<scene::Transform>(e, transform);
  scene.AddComponent<scene::Sprite>(e, sprite);
  scene.AddComponent<scene::Input>(e, input);
  scene.AddComponent<scene::BoxCollider>(e, collider);
  scene.AddComponent<scene::Kinematic>(e, kin);

  // --- Obstacle setup ---
  scene::Transform t2(120.f, 120.f);
  scene.AddComponent<scene::Transform>(e2, t2);
  scene.AddComponent<scene::BoxCollider>(e2, collider);

  return scene;
}

static scene::Scene makeLevel2(Game &outGame) {
  scene::Scene scene;
  scene::Entity e = scene.Create(); // player

  scene::Transform transform(200.f, 200.f);
  scene::Sprite sprite(e, 64, 64);
  scene::Input input;
  scene::BoxCollider collider(64, 64);

  sprite.assetHandle = outGame.assetLoader.LoadFor(e, "assets/player.png",
                                                   assets::AssetType::Texture);

  input.keyMap[core::input::Key::W] = false;
  input.keyMap[core::input::Key::A] = false;
  input.keyMap[core::input::Key::S] = false;
  input.keyMap[core::input::Key::D] = false;

  scene::Kinematic kin;
  kin.lastSafePos = {transform.x, transform.y};

  scene.AddComponent<scene::Transform>(e, transform);
  scene.AddComponent<scene::Sprite>(e, sprite);
  scene.AddComponent<scene::Input>(e, input);
  scene.AddComponent<scene::BoxCollider>(e, collider);
  scene.AddComponent<scene::Kinematic>(e, kin);

  return scene;
}

std::expected<void, Error> febundle::CreateGame(Game &outGame) {
  outGame.windowSpecs.height = 960;
  outGame.windowSpecs.width = 1280;
  outGame.windowSpecs.title = "Test Game";

  // push both levels into game
  outGame.scenes.push_back(makeLevel1(outGame));
  outGame.scenes.push_back(makeLevel2(outGame));
  outGame.activeSceneIndex = 0; // start on level1

  outGame.Initialize = [](Game &g) -> bool {
    // --- Collision response example ---

    g.collisionSystem.OnCollisionEnter(
        [&](const febundle::systems::CollisionEvent &evt) {
          auto &scene = g.scenes[g.activeSceneIndex];

          // Rollback only entities that are kinematic (e.g. player)
          for (auto e : {evt.e1, evt.e2}) {
            auto *transform = scene.GetComponent<scene::Transform>(e);
            auto *kin = scene.GetComponent<scene::Kinematic>(e);
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
    auto &scene = g.scenes[g.activeSceneIndex];
    const auto entities = scene.AccessAll();

    for (const auto &[e, comp] : entities) {
      auto *transform = scene.GetComponent<scene::Transform>(e);
      auto *input = scene.GetComponent<scene::Input>(e);
      auto *kin = scene.GetComponent<scene::Kinematic>(e);
      if (!input || !kin)
        continue;

      const float32 velocity = 70.0f;
      kin->lastSafePos = {transform->x, transform->y};

      if (input->keyMap[core::input::Key::W])
        transform->y -= velocity * deltaTime;
      if (input->keyMap[core::input::Key::A])
        transform->x -= velocity * deltaTime;
      if (input->keyMap[core::input::Key::S])
        transform->y += velocity * deltaTime;
      if (input->keyMap[core::input::Key::D])
        transform->x += velocity * deltaTime;
    }

    g.collisionSystem.Update(scene);

    // Example: swap level when player reaches x > 300
    auto *player =
        scene.GetComponent<scene::Transform>(0); // assume entity 0 = player
    if (player && player->x > 300 && g.activeSceneIndex == 0) {
      g.activeSceneIndex = 1;
      FLOG_INFO("Switched to level 2!");
    }

    return true;
  };

  return {};
}

int main() {
  Game gameInstance;
  auto res = febundle::CreateGame(gameInstance);

  febundle::App app(&gameInstance, true, false);
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
