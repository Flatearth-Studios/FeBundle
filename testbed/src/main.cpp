#define FE_DEBUG
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
    scene::Scene uiScene(scene::SceneType::UI);
    scene::Entity uiEntity = uiScene.Create();
    scene::UI uiComp;

    uiComp.drawFn = [&]() {
        // ESC toggles pause
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            outGame.isSuspended = !outGame.isSuspended;

        if (!outGame.isSuspended)
            return; // Don’t draw pause menu if running

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImVec2 center = vp->GetCenter();

        ImGui::SetNextWindowBgAlpha(0.45f);
        ImGui::SetNextWindowPos(ImVec2(center.x - 150, center.y - 100),
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, 200));

        ImGui::Begin("Pause Menu", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoSavedSettings);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.95f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.35f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4(0.30f, 0.36f, 0.56f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4(0.45f, 0.50f, 0.70f, 1.0f));

        ImGui::SetWindowFontScale(1.4f);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("PAUSED").x) * 0.5f);
        ImGui::Text("PAUSED");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Resume Game", ImVec2(250, 40))) {
            outGame.isSuspended = false;
            FLOG_INFO("Game resumed");
        }

        if (ImGui::Button("Restart Level", ImVec2(250, 40))) {
            FLOG_INFO("Level restart requested");
            // future: trigger scene reload event here
        }

        if (ImGui::Button("Exit Game", ImVec2(250, 40))) {
            outGame.isRunning = false;
            FLOG_INFO("Game exiting");
        }

        ImGui::PopStyleColor(4);
        ImGui::End();
    };

    uiScene.AddComponent<scene::UI>(uiEntity, uiComp);
    return uiScene;
}

std::expected<void, Error> febundle::CreateGame(Game &outGame) {
  outGame.windowSpecs.height = 960;
  outGame.windowSpecs.width = 1280;
  outGame.windowSpecs.title = "Test Game";

  // push both levels into game
  outGame.scenes.push_back(makeLevel1(outGame));
  outGame.scenes.push_back(makeUIScene(outGame));

  outGame.Initialize = [](Game &g) -> bool {
    // --- Collision response example ---

    g.collisionSystem.OnCollisionEnter(
        [&](const febundle::systems::CollisionEvent &evt) {
          auto &scene = g.scenes[0];

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
    auto &world = g.scenes[0];
    auto &ui = g.scenes[1]; // our UI scene

    if (!g.isSuspended) {
      const auto entities = world.AccessAll();
      for (const auto &[e, comp] : entities) {
        auto *transform = world.GetComponent<scene::Transform>(e);
        auto *input = world.GetComponent<scene::Input>(e);
        auto *kin = world.GetComponent<scene::Kinematic>(e);
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
      g.collisionSystem.Update(world);
    }

    g.collisionSystem.Update(world);
    g.pBridge->RenderScene(world);
    g.pBridge->RenderScene(ui);
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

  // Get the active scene (level 1 at start)
  auto &scene = gameInstance.scenes[0];

  // Find an entity that has an Audio component
  for (auto &[entity, _] : scene.AccessAll()) {
    if (auto *audio = scene.GetComponent<scene::Audio>(entity)) {
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
