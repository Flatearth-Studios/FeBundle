#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#define FE_DEBUG
#include <FeBundle/Core/GameTypes.hpp>
#include <FeBundle/Core/Logger.hpp>
#include <FeBundle/Core/Application.hpp>
#include <FeBundle/Core/Entrypoint.hpp>

using febundle::Game;
using febundle::Error;

std::expected<void, Error> febundle::CreateGame(Game& outGame) {
	outGame.windowSpecs.height = 960;
	outGame.windowSpecs.width = 1280;
	outGame.windowSpecs.title = "Test Game";

	outGame.Initialize = [](Game& g) -> bool {
    scene::Entity e = g.scene.Create();
    scene::Transform transform = scene::Transform(5.0f, 4.0f);
    scene::Sprite sprite = scene::Sprite(e, 64, 64);
    scene::Texture texture = scene::Texture(1, "assets/player.png");
    g.scene.AddComponent<scene::Transform>(e, transform);
    g.scene.AddComponent<scene::Sprite>(e, sprite);
    g.scene.AddComponent<scene::Texture>(e, texture);
		return true;
	};

	outGame.Update = [](Game& g, float32 deltaTime) -> bool {
    const auto entities = g.scene.AccessAll();

    for (const auto &[e, comp] : entities) {
      auto *transform = g.scene.GetComponent<scene::Transform>(e);
      transform->x += 2 * deltaTime;
      transform->rot += 0.1 * deltaTime;
      FLOG_TRACE("entity {} pos=({}, {}) rot={}", e, transform->x, transform->y, transform->rot);
    }
		return true;
	};

	outGame.OnResize = [](Game& g, uint32 width, uint32 height) -> bool {
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
