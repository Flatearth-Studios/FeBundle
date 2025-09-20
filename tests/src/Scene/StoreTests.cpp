#include "StoreTests.hpp"


#include "../Expect.hpp"
#include "FeBundle/Core/Scene/Store.hpp"

namespace febundle::tests {

using febundle::scene::Store;
using febundle::scene::Entity;

// --- Dummy Component for Testing ---
struct TestComponent {
  int value;
};

// --- Tests ---

bool TestStore_EmplaceAndGet() {
  Store<TestComponent> store;

  Entity e1 = 1;
  store.Emplace(e1, TestComponent{42});

  ASSERT_EQ_INT(true, store.Has(e1));
  auto *c = store.Get(e1);
  ASSERT_EQ_INT(42, c->value);
  return true;
}

bool TestStore_Remove() {
  Store<TestComponent> store;

  Entity e1 = 1;
  Entity e2 = 2;
  store.Emplace(e1, TestComponent{10});
  store.Emplace(e2, TestComponent{20});

  store.Remove(e1);

  ASSERT_EQ_INT(false, store.Has(e1));
  ASSERT_EQ_INT(true, store.Has(e2));

  auto *c2 = store.Get(e2);
  ASSERT_EQ_INT(20, c2->value);
  return true;
}

bool TestStore_CompactionAfterRemove() {
  Store<TestComponent> store;

  Entity e1 = 1;
  Entity e2 = 2;
  Entity e3 = 3;

  store.Emplace(e1, TestComponent{10});
  store.Emplace(e2, TestComponent{20});
  store.Emplace(e3, TestComponent{30});

  // Remove middle entity
  store.Remove(e2);

  ASSERT_EQ_INT(false, store.Has(e2));
  ASSERT_EQ_INT(true, store.Has(e1));
  ASSERT_EQ_INT(true, store.Has(e3));

  // Data and Owners should stay consistent
  ASSERT_EQ_INT(store.Data().size(), store.Owners().size());
  ASSERT_EQ_INT(store.Data().size(), 2);
  return true;
}

bool TestStore_GetInvalidEntity() {
  Store<TestComponent> store;

  Entity e1 = 1;
  store.Emplace(e1, TestComponent{99});

  auto *c = store.Get(999); // invalid entity
  if (c != nullptr) {
    LOG_ERROR("--> Expected nullptr for invalid entity, got non-null");
    return false;
  }
  return true;
}

bool TestStore_RemoveInvalidEntity() {
  Store<TestComponent> store;

  Entity e1 = 1;
  store.Emplace(e1, TestComponent{55});

  // Removing non-existent entity should not crash
  store.Remove(999);

  ASSERT_EQ_INT(true, store.Has(e1));
  return true;
}

// --- Registration ---

void StoreRegisterTests(TestManager &tm) {
  std::vector<TestEntry> tests = {
      {TestStore_EmplaceAndGet, "Store emplaces and retrieves components"},
      {TestStore_Remove, "Store removes components correctly"},
      {TestStore_CompactionAfterRemove, "Store compacts after removal"},
      {TestStore_GetInvalidEntity, "Store returns nullptr for invalid entity"},
      {TestStore_RemoveInvalidEntity, "Store ignores removal of invalid entity"},
  };

  auto regFunc = [tests]() -> TestMetadata {
    return TestMetadata{
        .name = "SceneStore",
        .callbacks = tests,
    };
  };

  tm.RegisterTests(regFunc);
}

}
