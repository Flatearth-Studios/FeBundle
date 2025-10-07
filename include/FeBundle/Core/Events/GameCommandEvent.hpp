#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_GAME_COMMAND_EVENT_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_GAME_COMMAND_EVENT_HPP_

#include "FeBundle/Core/Defines.hpp"

namespace febundle::core::events {

enum class GameCommandKind {
  Audio, 
};

struct GameCommandEvent {
  string name;
  GameCommandKind commandKind;
  void *payload;
};


}

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_GAME_COMMAND_EVENT_HPP_
