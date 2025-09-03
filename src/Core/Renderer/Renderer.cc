#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Systems/AssetManager.hpp"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_surface.h>
#define FE_DEBUG
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Renderer/Renderer.hpp"
#include "FeBundle/Core/Window/Window.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace febundle::renderer {

bool FeRenderer::_sInitialized = false;

FeRenderer::FeRenderer(window::Window &feWindow, systems::AssetManager &am)
    : _feWindow(feWindow), _assetMgr(am) {}

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

  _sInitialized = true;
  SDL_SetRenderDrawBlendMode(_pRenderer, SDL_BLENDMODE_BLEND);
  _assetMgr.unloadAll(_pRenderer);
  _texturesLoaded = _assetMgr.TextureQty();
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

  if (_texturesLoaded != _assetMgr.TextureQty()) {
    _assetMgr.unloadAll(_pRenderer);
    _texturesLoaded = _assetMgr.TextureQty();
  }

  const auto sprites = _cpScene->Sprites().Data();
  const auto transforms = _cpScene->Transforms().Data();

  if (sprites.size() != transforms.size()) {
    FLOG_ERROR("sprites and transforms do not share the same size "
              "sprites size: {}; transforms size: {}",
              sprites.size(), transforms.size());
    return std::unexpected{Error(ErrorName::RenderSprites)};
  }

  for (std::size_t i = 0; i < sprites.size(); i++) {
    if (!renderSprite(transforms[i], sprites[i])) {
      FLOG_WARN("renderer failed to render sprite with id {}", sprites[i].id);
    }
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

bool FeRenderer::renderSprite(const scene::Transform &t,
                              const scene::Sprite &s) {
  SDL_Texture *sdlTexture = resolveTexture(s.texture);
  if (sdlTexture == nullptr) {
    LOG_WARN("Sprite texture of entity id '{}' is nullptr", s.id);
    return false;
  }

  SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(sdlTexture, s.r, s.g, s.b);
  SDL_SetTextureAlphaMod(sdlTexture, s.a);

  const float32 width = s.width * t.sx;
  const float32 height = s.height * t.sy;
  const SDL_FPoint origin = {0.5f, 0.5f};

  SDL_FRect destination{
      .x = t.x - origin.x * width,
      .y = t.y - origin.y * height,
      .w = width,
      .h = height,
  };

  const float64 angleDegree = t.rot * 180.0 / FE_PI;
  SDL_FPoint center{
      .x = origin.x * width,
      .y = origin.y * height,
  };

  return SDL_RenderTextureRotated(_pRenderer, sdlTexture, nullptr, &destination,
                                  angleDegree, &center, SDL_FLIP_NONE);
}

SDL_Texture *FeRenderer::resolveTexture(scene::TextureHandle tex) {
  return _assetMgr.texture(tex);
}

void FeRenderer::cleanup() {
  if (_pRenderer == nullptr) {
    return;
  }

  SDL_DestroyRenderer(_pRenderer);
  _pRenderer = nullptr;
}

} // namespace febundle::renderer
