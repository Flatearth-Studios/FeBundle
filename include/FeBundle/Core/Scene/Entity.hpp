#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_ENTITY_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_ENTITY_HPP_

#include "FeBundle/Core/Defines.hpp"

namespace febundle::scene {

namespace entity {
enum class Tag {
  Character,
  Enemy,
  UI,
};
} // namespace entity

using Entity = uint32;

struct EntityMetadata {
  string name;
  entity::Tag tag;
};



} // namespace febundle::scene

#endif // INCLUDE_FEBUNDLE_CORE_SCENE_ENTITY_HPP_
