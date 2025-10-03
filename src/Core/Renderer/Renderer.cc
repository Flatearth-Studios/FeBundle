#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Systems/AssetManager.hpp"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#define FE_DEBUG
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Renderer/Renderer.hpp"
#include "FeBundle/Core/Window/Window.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace febundle::renderer {

bool FeRenderer::_sInitialized = false;

FeRenderer::FeRenderer(window::Window &feWindow, systems::AssetManager &am)
    : _feWindow(feWindow), _assetManager(am) {}

FeRenderer::~FeRenderer() { cleanup(); }

std::expected<void, Error> FeRenderer::Init() {
  if (_sInitialized) {
    FLOG_WARN("attempt to initialize renderer twice");
    return {};
  }

  SDL_Window *window = _feWindow.Handle();
  _pRenderer = SDL_CreateRenderer(window, nullptr);
  if (_pRenderer == nullptr) {
    FLOG_ERROR("failed to create renderer: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::CreateRenderer)};
  }

  if (!SDL_SetRenderVSync(_pRenderer, 1)) {
    FLOG_ERROR("failed to eneble VSync for renderer: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::EnableVSync)};
  }

  SDL_SetRenderDrawBlendMode(_pRenderer, SDL_BLENDMODE_BLEND);
  _sInitialized = true;
  FLOG_INFO("renderer initialized successfully");
  return {};
}

std::expected<void, Error> FeRenderer::Render() {
  if (_pRenderer == nullptr) {
    FLOG_ERROR("attempt to render on nullptr");
    return std::unexpected{Error(ErrorName::RenderCall)};
  }

  if (_cpScene == nullptr) {
    FLOG_WARN("no scene to render");
    return {};
  }

  std::size_t seen = 0, drawn = 0;
  const auto entities = _cpScene->AccessAll();
  for (const auto &[e, components] : entities) {
    seen++;
    if (!components.contains(scene::Component::Sprite) ||
        !components.contains(scene::Component::Transform)) {
      FLOG_WARN("entity '{}' has sprite or transform component, but "
                "not both",
                e);
      continue;
    }

    const auto *sprite = _cpScene->GetComponent<scene::Sprite>(e);
    const auto *transform = _cpScene->GetComponent<scene::Transform>(e);
    if (!renderSprite(*transform, *sprite)) {
      FLOG_WARN("renderer failed to render sprite of component {}", e);
      continue;
    }

    drawn++;
  }
  return {};
}

SDL_Renderer *FeRenderer::Handle() { return _pRenderer; }

void FeRenderer::SetScene(const scene::Scene *scene) { _cpScene = scene; }

void FeRenderer::Resize(uint32 width, uint32 height) {
  if (_pRenderer == nullptr) {
    return;
  }
}

void FeRenderer::BeginFrame() { SDL_RenderClear(_pRenderer); }

void FeRenderer::EndFrame() { SDL_RenderPresent(_pRenderer); }

SDL_Texture *FeRenderer::loadTexture(assets::AssetHandle ah) {
  if (_mapOfpTextures.contains(ah)) {
    return _mapOfpTextures.at(ah);
  }

  assets::IAsset *asset = _assetManager.AssetOf(ah);
  if (asset == nullptr) {
    return nullptr;
  }

  if (asset->Type() != assets::AssetType::Texture) {
    return nullptr;
  }

  assets::Texture *texture = static_cast<assets::Texture *>(asset);

  auto *surf = texture->Surface();
  SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(_pRenderer, surf);

  if (sdlTexture != nullptr) {
    _mapOfpTextures.emplace(ah, sdlTexture);
  }

  return sdlTexture;
}


bool FeRenderer::renderSprite(const scene::Transform &transform,
                              const scene::Sprite &sprite) {
  SDL_Texture *texture = loadTexture(sprite.assetHandle);
  if (texture == nullptr) {
    return false; 
  }

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(texture, sprite.r, sprite.g, sprite.b);
  SDL_SetTextureAlphaMod(texture, sprite.a);

  const float32 width = sprite.width * transform.sx;
  const float32 height = sprite.height * transform.sy;
  const SDL_FPoint origin = {0.5f, 0.5f};

  SDL_FRect destination{
      .x = transform.x - origin.x * width,
      .y = transform.y - origin.y * height,
      .w = width,
      .h = height,
  };

  const float64 angleDegree = transform.rot * 180.0 / FE_PI;
  SDL_FPoint center{
      .x = origin.x * width,
      .y = origin.y * height,
  };

  return SDL_RenderTextureRotated(_pRenderer, texture, nullptr,
                                  &destination, angleDegree, &center,
                                  SDL_FLIP_NONE);
}

void FeRenderer::cleanup() {
  if (_pRenderer == nullptr) {
    return;
  }

  SDL_DestroyRenderer(_pRenderer);
  _pRenderer = nullptr;
}

} // namespace febundle::renderer
