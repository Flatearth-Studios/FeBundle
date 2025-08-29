#ifndef INCLUDE_FEBUNDLE_CORE_ENTRYPOINT_HPP_
#define INCLUDE_FEBUNDLE_CORE_ENTRYPOINT_HPP_

#include "GameTypes.hpp"

namespace febundle {

std::expected<void, Error> CreateGame(Game &outGame);

}

#endif // INCLUDE_FEBUNDLE_CORE_ENTRYPOINT_HPP_
