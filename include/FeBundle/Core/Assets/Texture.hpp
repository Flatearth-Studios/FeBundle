#ifndef INCLUDE_FEBUNDLE_CORE_ASSETS_TEXTURE_HPP_
#define INCLUDE_FEBUNDLE_CORE_ASSETS_TEXTURE_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Defines.hpp"
#include <SDL3/SDL_surface.h>

namespace febundle::assets {

class Texture : public IAsset {
public:
  explicit Texture(SDL_Surface *surf) : _surface(surf) {}
  ~Texture() {
    if (_surface) {
      SDL_DestroySurface(_surface);
      _surface = nullptr;
    }
  }

  const assets::AssetType &Type() const override { return _handle.type; }

  uint64 Id() const override { return _handle.id; }

  SDL_Surface *Surface() { return _surface; }

private:
  SDL_Surface *_surface;
};

}

#endif // INCLUDE_FEBUNDLE_CORE_ASSETS_TEXTURE_HPP_
