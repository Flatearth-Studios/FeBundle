#include "EventQueueTests.hpp"
#include "../Expect.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"

namespace febundle::tests {

using febundle::core::events::EventQueue;

// --- Tests ---

bool TestEventQueue_PushAndRead() {
  EventQueue<int> q;
  q.Push(42);
  q.Push(13);

  const auto &events = q.Events();
  ASSERT_EQ_INT(2, events.size());
  ASSERT_EQ_INT(42, events[0]);
  ASSERT_EQ_INT(13, events[1]);
  return true;
}

bool TestEventQueue_Clear() {
  EventQueue<int> q;
  q.Push(7);
  q.Push(8);
  q.Clear();

  const auto &events = q.Events();
  ASSERT_EQ_INT(0, events.size());
  return true;
}

bool TestEventQueue_OrderPreserved() {
  EventQueue<std::string> q;
  q.Push("first");
  q.Push("second");
  q.Push("third");

  const auto &events = q.Events();
  ASSERT_EQ_INT(3, events.size());
  if (events[0] != "first" || events[1] != "second" || events[2] != "third") {
    LOG_ERROR("--> Expected FIFO order but got different. File: {}:{}",
              __FILE__, __LINE__);
    return false;
  }
  return true;
}

// --- Registration ---

void EventQueueRegisterTests(TestManager &tm) {
  std::vector<TestEntry> tests = {
      {TestEventQueue_PushAndRead, "EventQueue pushes and reads correctly"},
      {TestEventQueue_Clear, "EventQueue clear empties all events"},
      {TestEventQueue_OrderPreserved, "EventQueue preserves FIFO order"},
  };

  auto regFunc = [tests]() -> TestMetadata {
    return TestMetadata{
        .name = "EventQueue",
        .callbacks = tests,
    };
  };

  tm.RegisterTests(regFunc);
}

} // namespace febundle::tests
