#define FE_DEBUG
#include "FeBundle/Core/Events/RenderEvents.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Events/AssetLoadEvent.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Renderer/Renderer.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Window/Window.hpp"
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

namespace febundle::renderer {

bool FeRenderer::_sInitialized = false;

FeRenderer::FeRenderer(window::Window &feWindow, core::events::EventBus &evtBus)
    : _feWindow(feWindow), _eventBus(evtBus) {
  LOG_INFO("Renderer bus: {}", fmt::ptr(&_eventBus));
}

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

  subscribeToEvents();

  _sInitialized = true;
  FLOG_INFO("renderer initialized successfully");
  return {};
}

std::expected<void, Error> FeRenderer::Render() {
  if (_pRenderer == nullptr) {
    FLOG_ERROR("attempt to render on nullptr");
    return std::unexpected{Error(ErrorName::RenderCall)};
  }

  auto res = _eventBus.Dispatch<core::events::WorldRenderEvent>();
  if (!res.has_value()) {
    FLOG_ERROR("failed to dispatch render calls");
    return std::unexpected{res.error()};
  }
  
  return {};
}

SDL_Renderer *FeRenderer::Handle() { return _pRenderer; }

void FeRenderer::Resize(uint32 width, uint32 height) {
  if (_pRenderer == nullptr) {
    return;
  }
}

void FeRenderer::BeginFrame() { SDL_RenderClear(_pRenderer); }

void FeRenderer::EndFrame() { SDL_RenderPresent(_pRenderer); }

void FeRenderer::SetViewProjection(const core::math::Mat3 &view,
                                   const core::math::Mat3 &projection) {
  _view = view;
  _projection = projection;
}

SDL_Texture *FeRenderer::loadTexture(assets::AssetHandle ah) {
  if (_mapOfpTextures.contains(ah)) {
    return _mapOfpTextures.at(ah);
  }

  return nullptr;
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

  core::math::Vec2 worldPos{transform.x, transform.y};
  core::math::Vec2 cameraSpace = _view * worldPos;
  core::math::Vec2 projected = _projection * cameraSpace;

  const SDL_FPoint origin = {0.5f, 0.5f};
  SDL_FRect destination{
      .x = projected.x - origin.x * width,
      .y = projected.y - origin.y * height,
      .w = width,
      .h = height,
  };


  FLOG_DEBUG("world {},{} view[2]={},{} projection={},{}",
             transform.x, transform.y,
             _view[2][0], _view[2][1],
             _projection[0][0], _projection[1][1]);

  const float64 angleDegree = transform.rot * 180.0 / FE_PI;
  SDL_FPoint center{origin.x * width, origin.y * height};

  return SDL_RenderTextureRotated(_pRenderer, texture, nullptr, &destination,
                                  angleDegree, &center, SDL_FLIP_NONE);
}

void FeRenderer::subscribeToEvents() {
  // AssetLoadEvent Subscription
  core::events::EventSubscription<core::events::AssetLoadEvent> subscription{
      .subscriber = "FeRenderer",
      .callback = [&](const core::events::AssetLoadEvent &evt)
          -> std::expected<void, Error> {
        auto *asset = evt.asset;
        const auto type = evt.assetType;
        if (type != assets::AssetType::Texture) {
          return {};
        }

        auto *texture = static_cast<assets::Texture *>(asset);
        auto *surf = texture->Surface();

        switch (evt.eventKind) {
        case core::events::AssetEventKind::Load: {
          SDL_Texture *sdlTexture =
              SDL_CreateTextureFromSurface(_pRenderer, surf);
          if (!sdlTexture) {
            FLOG_ERROR("failed to create texture on load: {}", SDL_GetError());
            return std::unexpected{Error(ErrorName::CreateRenderer)};
          }

          _mapOfpTextures.emplace(evt.assetHandle, sdlTexture);
          FLOG_DEBUG("loaded new texture {}", evt.assetHandle.id);
          break;
        }
        case core::events::AssetEventKind::Reload: {
          auto it = _mapOfpTextures.find(evt.assetHandle);
          if (it == _mapOfpTextures.end()) {
            FLOG_WARN("reload event for unknown texture, creating fresh");
            SDL_Texture *sdlTexture =
                SDL_CreateTextureFromSurface(_pRenderer, surf);
            if (!sdlTexture) {
              FLOG_ERROR("failed to create texture on reload: {}",
                         SDL_GetError());
              return std::unexpected{Error(ErrorName::CreateRenderer)};
            }
            _mapOfpTextures.emplace(evt.assetHandle, sdlTexture);
            break;
          }

          // Destroy old GPU texture first to avoid leaks
          if (it->second != nullptr) {
            SDL_DestroyTexture(it->second);
          }

          SDL_Texture *newTex = SDL_CreateTextureFromSurface(_pRenderer, surf);
          if (!newTex) {
            FLOG_ERROR("failed to recreate texture on reload: {}",
                       SDL_GetError());
            return std::unexpected{Error(ErrorName::CreateRenderer)};
          }

          it->second = newTex;
          FLOG_INFO("reloaded texture {}", evt.assetHandle.id);
          break;
        }
        default:
          break;
        }

        return {};
      },
  };

  auto res = _eventBus.Subscribe(subscription);
  if (!res.has_value()) {
    FLOG_ERROR("failed to subscribe to AssetLoadEvent");
  } else {
    FLOG_TRACE("FeRenderer subscribed to AssetLoadEvent");
  }

  core::events::EventSubscription<core::events::WorldRenderEvent>
      renderEvtSubscription{
          .subscriber = "FeRenderer",
          .callback = [&](const core::events::WorldRenderEvent &evt)
              -> std::expected<void, Error> {
            renderSprite(evt.transform, evt.sprite);
            return {};
          },
      };

  auto res2 = _eventBus.Subscribe(renderEvtSubscription);
  if (!res.has_value()) {
    FLOG_ERROR("failed to subscribe to WorldRenderEvent");
  } else {
    FLOG_TRACE("FeRenderer subscribed to WorldRenderEvent");
  }
}

void FeRenderer::cleanup() {
  if (_pRenderer == nullptr) {
    return;
  }

  SDL_DestroyRenderer(_pRenderer);
  _pRenderer = nullptr;
}

} // namespace febundle::renderer
