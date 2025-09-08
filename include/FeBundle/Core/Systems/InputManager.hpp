#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_INPUT_MANAGER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_INPUT_MANAGER_HPP_

#include "FeBundle/Core/Defines.hpp"
#include <SDL3/SDL_events.h>
#include <queue>

namespace febundle::systems {

enum class Key;

enum class KeyState;

struct InputEvent {
  Key key;
  KeyState keyState;
  uint64 timestamp;
};

class InputManager {
public:
  void ProcessEvents(const SDL_Event &event);
  bool IsKeyPressed(Key key) const;
  bool WasKeyReleased(Key key) const;

private:
  umap<Key, InputEvent> _keyEvents;
  std::queue<InputEvent> _eventQ;
};

enum class KeyState {
  Idle,
  Pressed,
  Released,
  Held,
};

enum class Key {
  Null,
  Q,
  W,
  E,
  R,
  T,
  Y,
  U,
  I,
  O,
  P,
  A,
  S,
  D,
  F,
  G,
  H,
  J,
  K,
  L,
  Z,
  X,
  C,
  V,
  B,
  N,
  M,
  Esc,
  Num1,
  Num2,
  Num3,
  Num4,
  Num5,
  Num6,
  Num7,
  Num8,
  Num9,
  Num0,
  Dash,
  EqualSign,
  Backspace,
  Tab,
  LBracket,
  RBracket,
  Backslash,
  Capslock,
  Semicolon,
  SingleQuote,
  Enter,
  LShift,
  Comma,
  Dot,
  RShift,
  LCtrl,
  WinKey,
  LAlt,
  Spacebar,
  RAlt,
  RCtrl,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12
};

}

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_INPUT_MANAGER_HPP_
