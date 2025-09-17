#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_INPUT_MANAGER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_INPUT_MANAGER_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Input/Inputs.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <queue>

namespace febundle::systems {

struct InputEvent {
  core::input::Key key;
  core::input::KeyState keyState;
  uint64 timestamp;
};

class InputManager {
public:
  InputManager();
  void Update();
  InputEvent *ProcessEvent(const SDL_Event &event);
  bool IsKeyPressed(core::input::Key key) const;
  bool WasKeyReleased(core::input::Key key) const;

private:
  core::input::Key toFeKey(SDL_Keycode key);

private:
  umap<core::input::Key, InputEvent> _keyEvents;
  std::queue<InputEvent> _eventQ;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_INPUT_MANAGER_HPP_
