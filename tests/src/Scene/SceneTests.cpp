#include "SceneTests.hpp"
#include "../Expect.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Input/Inputs.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"

namespace febundle::tests {

using febundle::scene::Scene;
using febundle::scene::Entity;
using febundle::scene::Transform;
using febundle::scene::Sprite;
using febundle::scene::Input;
using febundle::scene::Component;
using febundle::systems::InputEvent;
using febundle::core::input::Key;
using febundle::core::input::KeyState;

// --- Tests ---

bool TestScene_CreateEntity() {
  Scene scene;
  Entity e1 = scene.Create();
  Entity e2 = scene.Create();
  ASSERT_EQ_INT(0, e1);
  ASSERT_EQ_INT(1, e2);
  return true;
}

bool TestScene_AddAndGetComponent() {
  Scene scene;
  Entity e = scene.Create();

  Transform t;
  t.x = 10;
  t.y = 20;
  scene.AddComponent<Transform>(e, t);

  ASSERT_EQ_INT(true, scene.HasComponent<Transform>(e));
  Transform* tPtr = scene.GetComponent<Transform>(e);
  ASSERT_EQ_INT(10, tPtr->x);
  ASSERT_EQ_INT(20, tPtr->y);
  return true;
}

bool TestScene_RemoveComponent() {
  Scene scene;
  Entity e = scene.Create();

  Sprite s;
  s.id = 123;
  scene.AddComponent<Sprite>(e, s);
  ASSERT_EQ_INT(true, scene.HasComponent<Sprite>(e));

  scene.RemoveComponent<Sprite>(e);
  ASSERT_EQ_INT(false, scene.HasComponent<Sprite>(e));
  return true;
}

bool TestScene_DestroyEntityRemovesComponents() {
  Scene scene;
  Entity e = scene.Create();

  scene.AddComponent<Transform>(e, Transform{});
  scene.AddComponent<Sprite>(e, Sprite{});
  scene.AddComponent<Input>(e, Input{});

  scene.Destroy(e);

  ASSERT_EQ_INT(false, scene.HasComponent<Transform>(e));
  ASSERT_EQ_INT(false, scene.HasComponent<Sprite>(e));
  ASSERT_EQ_INT(false, scene.HasComponent<Input>(e));
  return true;
}

bool TestScene_AccessAllIntegrity() {
  Scene scene;
  Entity e1 = scene.Create();
  Entity e2 = scene.Create();

  scene.AddComponent<Transform>(e1, Transform{});
  scene.AddComponent<Sprite>(e2, Sprite{});

  const auto& all = scene.AccessAll();
  ASSERT_EQ_INT(2, all.size());
  ASSERT_EQ_INT(true, all.at(e1).contains(Component::Transform));
  ASSERT_EQ_INT(true, all.at(e2).contains(Component::Sprite));
  return true;
}

bool TestScene_ProcessInputEvent() {
  Scene scene;
  Entity e = scene.Create();

  Input input;
  input.keyMap[Key::A] = false;
  scene.AddComponent<Input>(e, input);

  InputEvent evt{.key = Key::A, .keyState = KeyState::Pressed};
  scene.ProcessInputEvent(&evt);

  Input* iPtr = scene.GetComponent<Input>(e);
  ASSERT_EQ_INT(true, iPtr->keyMap[Key::A]);
  return true;
}

// --- Registration ---

void SceneRegisterTests(TestManager &tm) {
  std::vector<TestEntry> tests = {
      {TestScene_CreateEntity, "Scene creates entities with incrementing IDs"},
      {TestScene_AddAndGetComponent, "Scene adds and retrieves components"},
      {TestScene_RemoveComponent, "Scene removes components"},
      {TestScene_DestroyEntityRemovesComponents, "Scene destroys entity and removes components"},
      {TestScene_AccessAllIntegrity, "Scene AccessAll returns consistent entity-component map"},
      {TestScene_ProcessInputEvent, "Scene processes input events and updates Input component"},
  };

  auto regFunc = [tests]() -> TestMetadata {
    return TestMetadata{
        .name = "Scene",
        .callbacks = tests,
    };
  };

  tm.RegisterTests(regFunc);
}

} // namespace febundle::tests
