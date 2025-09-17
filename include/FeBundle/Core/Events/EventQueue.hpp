#ifndef INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_
#define INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_

#include "FeBundle/Core/Defines.hpp"

namespace febundle::events {

template <typename Event>
class EventQueue {
public:
  void Push(const Event& evt) {
    _events.push_back(evt);
  }
  
  const std::vector<Event> &Events() const {
    return _events;
  }

  void Clear() {
    _events.clear();
  }


private:
  std::vector<Event> _events;
};

}

#endif // INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_
