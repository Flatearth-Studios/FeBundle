#include "FeBundle/Core/Scene/Components.hpp"
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

FeRenderer::FeRenderer(window::Window &feWindow)
    : _feWindow(feWindow) {}

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
        !components.contains(scene::Component::Transform) ||
        !components.contains(scene::Component::Texture)) {
      FLOG_WARN("entity '{}' has sprite, texture, or transform component, but "
                "not the trio", e);
      continue;
    }

    const auto *sprite = _cpScene->GetComponent<scene::Sprite>(e);
    const auto *transform = _cpScene->GetComponent<scene::Transform>(e);
    const auto *texture = _cpScene->GetComponent<scene::Texture>(e);
    if (!renderSprite(*transform, *sprite, *texture)) {
      FLOG_WARN("renderer failed to render sprite of component {}", e);
      continue;
    }

    drawn++;
  }
  return {};
}

SDL_Renderer *FeRenderer::Handle() { return _pRenderer; }

void FeRenderer::SetScene(const scene::Scene *scene) { 
  _cpScene = scene; 
  _texturesLoaded = loadTextures();
}

void FeRenderer::Resize(uint32 width, uint32 height) {
  if (_pRenderer == nullptr) {
    return;
  }
}

void FeRenderer::BeginFrame() { SDL_RenderClear(_pRenderer); }

void FeRenderer::EndFrame() { SDL_RenderPresent(_pRenderer); }

bool FeRenderer::renderSprite(const scene::Transform &transform,
                              const scene::Sprite &sprite,
                              const scene::Texture &texture) {
  SDL_Texture *sdlTexture = loadTexture(texture);
  if (sdlTexture == nullptr) {
    LOG_WARN(
        "Sprite texture of entity id '{}' could not be resolved nor loaded",
        sprite.id);
    return false;
  }

  SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(sdlTexture, sprite.r, sprite.g, sprite.b);
  SDL_SetTextureAlphaMod(sdlTexture, sprite.a);

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

  return SDL_RenderTextureRotated(_pRenderer, sdlTexture, nullptr, &destination,
                                  angleDegree, &center, SDL_FLIP_NONE);
}

SDL_Texture *FeRenderer::loadTexture(const scene::Texture &texture) {
  if (_cpScene == nullptr) {
    FLOG_WARN("cannot load texture if no scene is set");
    return nullptr;
  }

  auto it = _mapOfpTextures.find(texture.texHandle);
  if (it != _mapOfpTextures.end()) {
    return it->second;
  }

  SDL_Texture *sdlTexture = IMG_LoadTexture(_pRenderer, texture.path.string().c_str());
  if (sdlTexture == nullptr) {
    FLOG_ERROR("failed to load texture {}: {}", texture.path.string(),
               SDL_GetError());
    return nullptr;
  }

  _mapOfpTextures[texture.texHandle] = sdlTexture;
  _texturesLoaded = _mapOfpTextures.size();
  return sdlTexture;
}

std::size_t FeRenderer::loadTextures() {
  if (_cpScene == nullptr) {
    FLOG_WARN("cannot load textures if no scene is set");
    return 0;
  }

  if (!_mapOfpTextures.empty()) {
    _mapOfpTextures.clear();
  }  

  for (const auto &[e, components] : _cpScene->AccessAll()) {
    if (!components.contains(scene::Component::Texture)) {
      continue;
    }

    const auto *texture = _cpScene->GetComponent<scene::Texture>(e);
    const auto texIt = _mapOfpTextures.find(texture->texHandle);
    if (texIt != _mapOfpTextures.end()) {
      // already loaded
      continue;
    }

    SDL_Texture *sdlTexture =
        IMG_LoadTexture(_pRenderer, texture->path.string().c_str());
    if (!sdlTexture) {
      FLOG_ERROR("failed to load texture {}: {}", texture->path.string(),
                 SDL_GetError());
      continue;
    }

    _mapOfpTextures[texture->texHandle] = sdlTexture;
  }

  return _mapOfpTextures.size();
}

void FeRenderer::cleanup() {
  if (_pRenderer == nullptr) {
    return;
  }

  SDL_DestroyRenderer(_pRenderer);
  _pRenderer = nullptr;
}

} // namespace febundle::renderer
