#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_EVENT_BUS_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_EVENT_BUS_HPP_

#include "FeBundle/Core/Errors.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include <expected>
#include <typeindex>

namespace febundle::core::events {

// subscribers and emitters
// emitters emit eventqueus and subscribers consume them
class EventBus {
public:
  template <typename Event> std::expected<void, Error> Dispatch() {
    auto res = getQueue<Event>();
    if (!res.has_value()) {
      return std::unexpected{res.error()};
    }

    EventQueue<Event> *queue = res.value();
    queue->DispatchAll();
    return {};
  }

  template <typename Event>
  std::expected<void, Error> Subscribe(EventSubscription<Event> sub) {
    auto res = getQueue<Event>();
    if (!res.has_value()) {
      return std::unexpected{res.error()};
    }

    EventQueue<Event> *queue = res.value();
    queue->Subscribe(sub);
    return {};
  }

  template <typename Event>
  std::expected<void, Error> Unsubscribe(const string &subsystem) {
    auto res = getQueue<Event>();
    if (!res.has_value()) {
      return std::unexpected{res.error()};
    }

    EventQueue<Event> *queue = res.value();
    queue->Unsubscribe(subsystem);
    return {};
  }

  template <typename Event> std::expected<void, Error> Push(const Event &evt) {
    auto res = getQueue<Event>();
    if (!res) {
      return std::unexpected{res.error()};
    }

    EventQueue<Event> *queue = res.value();
    queue->Push(evt);
    return {};
  }

  template <typename Event> std::expected<EventQueue<Event> *, Error> Queue() {
    return getQueue<Event>();
  }

  template <typename Event>
  std::expected<const EventQueue<Event> *, Error> Queue() const {
    return getQueue<Event>();
  }

private:
  struct IQueueWrapper {
    virtual ~IQueueWrapper() = default;
  };

  template <typename Event> struct QueueWrapper : IQueueWrapper {
    EventQueue<Event> queue;
  };

  template <typename Event>
  std::expected<EventQueue<Event> *, Error> getQueue() const {
    const auto id = std::type_index(typeid(Event));
    if (_queues.contains(id)) {
      auto *base = _queues.at(id).get();
      auto *wrap = static_cast<QueueWrapper<Event> *>(base);
      return &wrap->queue;
    }

    // Allocate new queue wrapper
    auto res = memory::MakeUniquePoly<IQueueWrapper, QueueWrapper<Event>>(
        memory::Tag::EventBus);
    if (!res) {
      return std::unexpected{Error(ErrorName::AllocationException)};
    }

    auto wrapper = std::move(res.value());
    auto *wrap = static_cast<QueueWrapper<Event> *>(wrapper.get());
    auto *ptr = &wrap->queue;

    _queues.emplace(id, std::move(wrapper));
    return ptr;
  }

private:
  mutable umap<
      std::type_index,
      std::unique_ptr<IQueueWrapper, memory::PolyDeleter<IQueueWrapper>>>
      _queues;
};

} // namespace febundle::core::events

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_EVENT_BUS_HPP_
