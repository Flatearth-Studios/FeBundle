#ifndef INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_
#define INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_

#include <vector>

namespace febundle::core::events {

template <typename Event>
class EventQueue {
public:
  void Push(const Event& evt) {
    _events.push_back(evt);
  }
  
  inline const std::vector<Event> &Events() const noexcept {
    return _events;
  }

  inline void Clear() noexcept {
    _events.clear();
  }


private:
  std::vector<Event> _events;
};

}

#endif // INCLUDE_CORE_EVENTS_EVENT_QUEUE_HPP_
