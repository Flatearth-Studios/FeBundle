#ifndef INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_
#define INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Logger.hpp"

#include <functional>

namespace febundle::core::events {

template <typename Event>
using EventCallback = std::function<std::expected<void, Error>(const Event &e)>;

template <typename Event> struct EventSubscription {
  string subscriber;
  EventCallback<Event> callback;
};

template <typename Event> class EventQueue {
public:
  inline void DispatchAll() {
    for (const auto &event : _events) {
      for (auto &subscription : _subscriptions) {
        auto res = subscription.callback(event);
        if (!res.has_value()) {
          FLOG_ERROR("failed to call callback for event subscriber: {}",
                     subscription.subscriber);
        }
      }
    }

    _events.clear();
  }

  inline void Push(const Event &evt) { _events.push_back(evt); }

  inline void Subscribe(const EventSubscription<Event> &sub) {
    _subscriptions.push_back(sub);
  }

  inline void Unsubscribe(const string &subscriber) {
    std::erase_if(_subscriptions, [&](const EventSubscription<Event> &sub) {
      return subscriber == sub.subscriber;
    });
  }

  inline const std::vector<Event> &Events() const noexcept { return _events; }

  inline void Clear() noexcept { _events.clear(); }

private:
  std::vector<Event> _events;
  std::vector<EventSubscription<Event>> _subscriptions;
};

} // namespace febundle::core::events

#endif // INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_
