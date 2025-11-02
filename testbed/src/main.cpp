#define FE_DEBUG
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/UI/Button.hpp"
#include "imgui.h"
#include <FeBundle/Core/Application.hpp>
#include <FeBundle/Core/Commands/AudioCommands.hpp>
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

  scene::Entity camEntity = scene.Create();
  scene::Transform camTransform(0.0f, 0.0f);

  // Create the underlying camera object
  scene::Camera camComp;
  camComp.pCamera = memory::MakeShared<camera::Camera>(
                        memory::Tag::Scene,
                        camera::Camera::Default(outGame.windowSpecs.width,
                                                outGame.windowSpecs.height))
                        .value();
  camComp.pCamera->active = true;

  scene.AddComponent<scene::Transform>(camEntity, camTransform);
  scene.AddComponent<scene::Camera>(camEntity, camComp);

  scene::Entity e = scene.Create();  // player
  scene::Entity e2 = scene.Create(); // obstacle

  // --- Player setup ---
  scene::Transform transform(5.0f, 4.0f);
  scene::Sprite sprite(e, 64, 64);
  scene::Input input;
  scene::BoxCollider collider(64, 64);

  sprite.assetHandle = outGame.assetLoader.LoadFor(
      e, assets::AssetType::Texture, "assets/player.png");

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
  scene::Audio audio;
  audio.assetHandle = outGame.assetLoader.LoadFor(e2, assets::AssetType::Audio,
                                                  "assets/bgm.wav");
  scene.AddComponent<scene::Transform>(e2, t2);
  scene.AddComponent<scene::BoxCollider>(e2, collider);
  scene.AddComponent<scene::Audio>(e2, audio);

  return scene;
}

static scene::Scene makeUIScene(Game &outGame) {
  using namespace scene;
  Scene uiScene(SceneType::UI);

  // Create UI entity
  Entity uiEntity = uiScene.Create(entity::Tag::UI);

  // Create UI component
  UI uiComp;

  // Create Play button widget
  uiComp.pWidget =
      memory::MakeShared<ui::Button>(memory::Tag::Scene, "Play", [&]() {
        FLOG_INFO("Play button clicked!");
      }).value();

  if (auto *button = dynamic_cast<ui::Button *>(uiComp.pWidget.get())) {
    button->SetPosition(ImVec2(540.0f, 440.0f));
    button->SetSize(ImVec2(200.0f, 60.0f));
  }

  // Attach UI component to entity
  uiScene.AddComponent<UI>(uiEntity, uiComp);
  return uiScene;
}

std::expected<void, Error> febundle::CreateGame(Game &outGame) {
  outGame.windowSpecs.height = 960;
  outGame.windowSpecs.width = 1280;
  outGame.windowSpecs.title = "Test Game";

  outGame.Initialize = [](Game &g) -> bool {
    // --- Collision response example ---

    g.collisionSystem.OnCollisionEnter(
        [&](const febundle::systems::CollisionEvent &evt) {
          auto *scene = g.SceneReference("world");
          if (scene == nullptr) {
            FLOG_ERROR("no scene under the alias 'world' was found");
            return;
          }

          // Rollback only entities that are kinematic (e.g. player)
          for (auto e : {evt.e1, evt.e2}) {
            auto *transform = scene->GetComponent<scene::Transform>(e);
            auto *kin = scene->GetComponent<scene::Kinematic>(e);
            if (transform != nullptr && kin != nullptr) {
              transform->x = kin->lastSafePos.x;
              transform->y = kin->lastSafePos.y;
              FLOG_DEBUG("Entity {} rolled back due to collision", e);
            }
          }
        });

    return true;
  };

  outGame.Update = [](Game &g, float32 deltaTime) -> bool {
    auto *world = g.SceneReference("world");
    if (world == nullptr) {
      FLOG_ERROR("no scene under the alias 'world' was found");
      return false;
    }

    auto *ui = g.SceneReference("ui"); // our UI scene
    if (ui == nullptr) {
      FLOG_ERROR("no scene under the alias 'ui' was found");
      return false;
    }

    if (!g.isSuspended) {
      const auto entities = world->AccessAll();
      for (const auto &[e, comp] : entities) {
        auto *transform = world->GetComponent<scene::Transform>(e);
        auto *input = world->GetComponent<scene::Input>(e);
        auto *kin = world->GetComponent<scene::Kinematic>(e);
        if (!input || !kin) {
          continue;
        }


        const float32 velocity = 70.0f;
        kin->lastSafePos = {transform->x, transform->y};

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
      }


      g.collisionSystem.Update(*world);
    }

    g.pBridge->RenderScene(*world);
    g.pBridge->RenderScene(*ui);
    return true;
  };

  outGame.OnResize = [](Game &g, uint32 w, uint32 h) -> bool { return true; };

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

  auto lv1 = makeLevel1(gameInstance);
  auto uis = makeUIScene(gameInstance);

  gameInstance.LoadScene("world", std::move(lv1));
  gameInstance.LoadScene("ui", std::move(uis));

  // Get the active scene (level 1 at start)
  auto *scene = gameInstance.SceneReference("world");
  auto *uiScene = gameInstance.SceneReference("ui");

  if (scene == nullptr || uiScene == nullptr) {
    FLOG_ERROR("ui scene or world scene was not correctly set");
    return -3;
  }

  auto uiEntities = uiScene->FindByTag(scene::entity::Tag::UI);

  if (!uiEntities.empty()) {
    FLOG_DEBUG("we have UI");
    auto *ui = uiScene->GetComponent<scene::UI>(uiEntities[0]);
    if (auto *button = dynamic_cast<ui::Button *>(ui->pWidget.get())) {
      button->SetVisible(true);
    }
  }

  // Find an entity that has an Audio component
  for (auto &[entity, _] : scene->AccessAll()) {
    if (auto *audio = scene->GetComponent<scene::Audio>(entity)) {
      // Build the play command directly from the component
      febundle::commands::PlaySoundCommand cmd{
          .assetHandle = audio->assetHandle,
          .volume = 1.0f,
      };

      gameInstance.pBridge->PostCommand("PlaySound", &cmd);
      FLOG_INFO("Playing sound from entity {}", entity);
      break;
    }
  }

  // --- Run the game loop ---
  if (auto res = app.Run(); !res.has_value()) {
    LOG_ERROR("Application did not exit gracefully");
    return -1;
  }

  return 0;
}
