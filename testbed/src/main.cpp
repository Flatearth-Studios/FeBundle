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
		return true;
	};

	outGame.Update = [](Game& g, float32 deltaTime) -> bool {
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