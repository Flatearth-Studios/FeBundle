#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_STORE_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_STORE_HPP_

#include "Entity.hpp"

namespace febundle::scene {

static constexpr uint32 DEFAULT_CAPACITY = 10000;

template <typename T> class Store {
public:
  Store(uint32 capacity = DEFAULT_CAPACITY) {
    _data.reserve(capacity);
    _owners.reserve(capacity);
  }

  bool Has(Entity e) const { return _indices.contains(e); }

  T *Get(Entity e) {
    auto it = _indices.find(e);
    return it == _indices.end() ? nullptr : &_data[it->second];
  }

  const T *Get(Entity e) const {
    auto it = _indices.find(e);
    return it == _indices.end() ? nullptr : &_data[it->second];
  }

  void Emplace(Entity e, T c) {
    _indices[e] = _data.size();
    _owners.push_back(e);
    _data.push_back(std::move(c));
  }

  void Remove(Entity e) {
    auto it = _indices.find(e);
    if (it == _indices.end()) {
      return;
    }

    size_t i = it->second;
    size_t last = _data.size() - 1;

    if (i != last) {
      _data[i] = std::move(_data[last]);
      _owners[i] = _owners[last];
      _indices[_owners[i]] = i;
    }

    _data.pop_back();
    _owners.pop_back();
    _indices.erase(it);
  }

  const std::vector<T> &Data() const { return _data; }
  const std::vector<T> &Owners() const { return _owners; }

private:
  std::vector<T> _data;
  std::vector<Entity> _owners;
  umap<Entity, std::size_t> _indices;
};

} // namespace febundle::scene

#endif // INCLUDE_FEBUNDLE_CORE_SCENE_STORE_HPP_
