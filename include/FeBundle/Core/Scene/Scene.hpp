#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_SCENE_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_SCENE_HPP_

#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Store.hpp"
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"
#include <typeindex>

namespace febundle::scene {

class Scene {
public:
  Scene();
  ~Scene() = default;

  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  Scene(Scene&&) noexcept = default;
  Scene& operator=(Scene&&) noexcept = default;

  Entity Create();
  void Destroy(Entity e);

  const umap<Entity, uset<Component>> &AccessAll() const;
  void ProcessInputEvent(const systems::InputEvent *ie);

  template <typename C> bool HasComponent(Entity e) const {
    return getStore<C>().Has(e);
  }

  template <typename C> C *GetComponent(Entity e) {
    return getStore<C>().Get(e);
  }

  template <typename C> const C *GetComponent(Entity e) const {
    return getStore<C>().Get(e);
  }

  template <typename C> void AddComponent(Entity e, C component) {
    static_assert(std::is_base_of_v<IComponent, C>,
                  "typename C must derive from IComponent");
    const Component cid =
        static_cast<const IComponent &>(component).Component();
    getStore<C>().Emplace(e, std::move(component));
    _entityComponents[e].insert(cid);
  }

  template <typename C> void RemoveComponent(Entity e) {
    static_assert(std::is_base_of_v<IComponent, C>,
                  "typename C must derive from IComponent");
    C *component = getStore<C>().Get(e);
    if (component) {
      _entityComponents[e].erase(
          static_cast<const IComponent &>(*component).Component());
      getStore<C>().Remove(e);
    }
  }

private:
  template <typename C> void registerStore() {
    const auto id = std::type_index(typeid(C));
    if (_stores.contains(id)) {
      return;
    }
    _stores[id] = memory::MakeUniquePoly<IStoreWrapper, StoreWrapper<C>>(
                      memory::Tag::Scene)
                      .value();
  }

  template <typename C> Store<C> &getStore() {
    const auto id = std::type_index(typeid(C));
    if (!_stores.contains(id)) {
      registerStore<C>();
    }

    auto *base = _stores.at(id).get();
    auto *wrap = static_cast<StoreWrapper<C> *>(base);
    return wrap->store;
  }

  template <typename C> const Store<C> &getStore() const {
    const auto id = std::type_index(typeid(C));
    auto *base = _stores.at(id).get();
    auto *wrap = static_cast<const StoreWrapper<C> *>(base); 
    return wrap->store;                                      
  }

  struct IStoreWrapper {
    virtual ~IStoreWrapper() = default;
  };
  template <typename C> struct StoreWrapper : IStoreWrapper {
    Store<C> store;
  };

private:
  umap<std::type_index,
       std::unique_ptr<IStoreWrapper, memory::PolyDeleter<IStoreWrapper>>>
      _stores;
  umap<Entity, uset<Component>> _entityComponents;
  Entity _next{0};
};

} // namespace febundle::scene

#endif // INCLUDE_FEBUNDLE_CORE_SCENE_SCENE_HPP_
