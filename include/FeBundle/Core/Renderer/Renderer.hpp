#ifndef INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
#define INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_

#include "../Window/Window.hpp"
#include <SDL3/SDL_render.h>

namespace febundle::renderer {

class IRenderer {
public:
  virtual ~IRenderer() = default;
  virtual std::expected<void, Error> Init() = 0;
  virtual std::expected<void, Error> Render() = 0;
  virtual void Resize(uint32 width, uint32 height) = 0;
  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;
};

class FeRenderer final : public IRenderer {
public:
  FeRenderer();
  explicit FeRenderer(const window::WindowSpecs &specs);
  ~FeRenderer();
  std::expected<void, Error> Init() override;
  std::expected<void, Error> Render() override;
  void Resize(uint32 width, uint32 height) override;
  void BeginFrame() override;
  void EndFrame() override;

private:
  void cleanup();

private:
  SDL_Renderer *_pRenderer;
  window::Window _feWindow;
};

} // namespace febundle::renderer

#endif // INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
