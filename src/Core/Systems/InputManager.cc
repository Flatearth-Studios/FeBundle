#define FE_DEBUG
#include "FeBundle/Core/Systems/InputManager.hpp"
#include "FeBundle/Core/Input/Inputs.hpp"
#include <SDL3/SDL_keycode.h>

namespace febundle::systems {

InputManager::InputManager() {
  // First key is null, so we start at 1
  for (int i = 1; i < static_cast<int>(core::input::Key::KeyCount); i++) {
    auto input = static_cast<core::input::Key>(i);
    if (input == core::input::Key::KeyCount) {
      // Dont register last key
      break;
    }

    _keyEvents.emplace(input, InputEvent{
                                  .key = input,
                                  .keyState = core::input::KeyState::Idle,
                              });
  }
}

void InputManager::Update() {
  using core::input::KeyState;

  for (auto &[key, evt] : _keyEvents) {
    switch (evt.keyState) {
    case KeyState::Pressed:
      evt.keyState = KeyState::Held;
      break;
    case KeyState::Released:
      evt.keyState = KeyState::Idle;
      break;
    default:
      break;
    }
  }

}

InputEvent *InputManager::ProcessEvent(const SDL_Event &event) {
  using core::input::KeyState;

  switch (event.type) {
  case SDL_EVENT_KEY_DOWN: {
    auto key = toFeKey(event.key.key);
    if (key == core::input::Key::Null) {
      break;
    }

    auto &inputEvent = _keyEvents[key];
    if (event.key.repeat == 0) {
      // New press
      inputEvent.keyState = KeyState::Pressed;
    } else {
      inputEvent.keyState = KeyState::Held;
    }
    return &inputEvent;
  }

  case SDL_EVENT_KEY_UP: {
    auto key = toFeKey(event.key.key);
    if (key == core::input::Key::Null) {
      break;
    }

    auto &inputEvent = _keyEvents[key];
    inputEvent.keyState = KeyState::Released;
    return &inputEvent;
  }

  default:
    break;
  }

  return nullptr;
}

bool InputManager::IsKeyPressed(core::input::Key key) const {
  const auto it = _keyEvents.find(key);
  bool isPressed = false;
  if (it == _keyEvents.end()) {
    return isPressed;
  }

  switch (it->second.keyState) {
  case core::input::KeyState::Held:
  case core::input::KeyState::Pressed:
    isPressed = true;
    break;
  default:
    break;
  }

  return isPressed;
}

bool InputManager::WasKeyReleased(core::input::Key key) const {
  const auto it = _keyEvents.find(key);
  if (it == _keyEvents.end()) {
    return false;
  }

  if (it->second.keyState == core::input::KeyState::Released) {
    return true;
  }

  return false;
}

core::input::Key InputManager::toFeKey(SDL_Keycode key) {
  using core::input::Key;

  switch (key) {
  case SDLK_Q:
    return Key::Q;
  case SDLK_W:
    return Key::W;
  case SDLK_E:
    return Key::E;
  case SDLK_R:
    return Key::R;
  case SDLK_T:
    return Key::T;
  case SDLK_Y:
    return Key::Y;
  case SDLK_U:
    return Key::U;
  case SDLK_I:
    return Key::I;
  case SDLK_O:
    return Key::O;
  case SDLK_P:
    return Key::P;

  case SDLK_A:
    return Key::A;
  case SDLK_S:
    return Key::S;
  case SDLK_D:
    return Key::D;
  case SDLK_F:
    return Key::F;
  case SDLK_G:
    return Key::G;
  case SDLK_H:
    return Key::H;
  case SDLK_J:
    return Key::J;
  case SDLK_K:
    return Key::K;
  case SDLK_L:
    return Key::L;

  case SDLK_Z:
    return Key::Z;
  case SDLK_X:
    return Key::X;
  case SDLK_C:
    return Key::C;
  case SDLK_V:
    return Key::V;
  case SDLK_B:
    return Key::B;
  case SDLK_N:
    return Key::N;
  case SDLK_M:
    return Key::M;

  case SDLK_ESCAPE:
    return Key::Esc;
  case SDLK_1:
    return Key::Num1;
  case SDLK_2:
    return Key::Num2;
  case SDLK_3:
    return Key::Num3;
  case SDLK_4:
    return Key::Num4;
  case SDLK_5:
    return Key::Num5;
  case SDLK_6:
    return Key::Num6;
  case SDLK_7:
    return Key::Num7;
  case SDLK_8:
    return Key::Num8;
  case SDLK_9:
    return Key::Num9;
  case SDLK_0:
    return Key::Num0;

  case SDLK_MINUS:
    return Key::Dash;
  case SDLK_EQUALS:
    return Key::EqualSign;
  case SDLK_BACKSPACE:
    return Key::Backspace;
  case SDLK_TAB:
    return Key::Tab;
  case SDLK_LEFTBRACKET:
    return Key::LBracket;
  case SDLK_RIGHTBRACKET:
    return Key::RBracket;
  case SDLK_BACKSLASH:
    return Key::Backslash;
  case SDLK_CAPSLOCK:
    return Key::Capslock;
  case SDLK_SEMICOLON:
    return Key::Semicolon;
  case SDLK_APOSTROPHE:
    return Key::SingleQuote;
  case SDLK_RETURN:
    return Key::Enter;

  case SDLK_LSHIFT:
    return Key::LShift;
  case SDLK_COMMA:
    return Key::Comma;
  case SDLK_PERIOD:
    return Key::Dot;
  case SDLK_RSHIFT:
    return Key::RShift;
  case SDLK_LCTRL:
    return Key::LCtrl;
  case SDLK_LGUI:
    return Key::WinKey; // Windows / Command key
  case SDLK_LALT:
    return Key::LAlt;
  case SDLK_SPACE:
    return Key::Spacebar;
  case SDLK_RALT:
    return Key::RAlt;
  case SDLK_RCTRL:
    return Key::RCtrl;

  case SDLK_F1:
    return Key::F1;
  case SDLK_F2:
    return Key::F2;
  case SDLK_F3:
    return Key::F3;
  case SDLK_F4:
    return Key::F4;
  case SDLK_F5:
    return Key::F5;
  case SDLK_F6:
    return Key::F6;
  case SDLK_F7:
    return Key::F7;
  case SDLK_F8:
    return Key::F8;
  case SDLK_F9:
    return Key::F9;
  case SDLK_F10:
    return Key::F10;
  case SDLK_F11:
    return Key::F11;
  case SDLK_F12:
    return Key::F12;
  
  case SDLK_UP:
    return Key::Up;
  case SDLK_DOWN:
    return Key::Down;
  case SDLK_LEFT:
    return Key::Left;
  case SDLK_RIGHT:
    return Key::Right;

  default:
    return Key::Null;
  }
}

} // namespace febundle::systems
