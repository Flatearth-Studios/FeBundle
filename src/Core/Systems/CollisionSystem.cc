#define FE_DEBUG
#include "FeBundle/Core/Systems/CollisionSystem.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include <cmath>

namespace febundle::systems {

CollisionSystem::CollisionSystem(float32 cellSize, int64 maxCallbacksAllowed)
    : _cellSize(cellSize), _cMaxCallbacks(maxCallbacksAllowed),
      _enterCallbackCount(0), _stayCallbackCount(0), _exitCallbackCount(0) {
  _enterCallbacks.reserve(maxCallbacksAllowed);
  _stayCallbacks.reserve(maxCallbacksAllowed);
  _exitCallbacks.reserve(maxCallbacksAllowed);
}

void CollisionSystem::Update(scene::Scene &scene) {
  _grid.clear();
  _eventQ.Clear();

  const auto entityMap = scene.AccessAll();
  for (auto &[e, comp] : entityMap) {
    auto *transform = scene.GetComponent<scene::Transform>(e);
    auto *box = scene.GetComponent<scene::BoxCollider>(e);
    if (transform == nullptr || box == nullptr) {
      FLOG_TRACE("No box collider or transform for entity {}", e);
      continue;
    }

    // compute min/max corners
    float32 minX = transform->x;
    float32 minY = transform->y;
    float32 maxX = transform->x + box->width;
    float32 maxY = transform->y + box->height;

    // convert to grid cells
    auto [minCellX, minCellY] = worldToCell(minX, minY);
    auto [maxCellX, maxCellY] = worldToCell(maxX, maxY);

    // insert into every covered cell
    for (int cx = minCellX; cx <= maxCellX; ++cx) {
      for (int cy = minCellY; cy <= maxCellY; ++cy) {
        _grid[hashCell(cx, cy)].entities.push_back(e);
      }
    }
  }

  for (auto &[hash, cell] : _grid) {
    for (size_t i = 0; i < cell.entities.size(); i++) {
      for (size_t j = i + 1; j < cell.entities.size(); j++) {
        auto e1 = cell.entities[i];
        auto e2 = cell.entities[j];
        auto *t1 = scene.GetComponent<scene::Transform>(e1);
        auto *t2 = scene.GetComponent<scene::Transform>(e2);
        auto *c1 = scene.GetComponent<scene::BoxCollider>(e1);
        auto *c2 = scene.GetComponent<scene::BoxCollider>(e2);
        if (t1 == nullptr || t2 == nullptr || c1 == nullptr || c2 == nullptr) {
          continue;
        }

        if (checkCollision(*t1, *c1, *t2, *c2)) {
          FLOG_DEBUG("Collision event being created");
          _eventQ.Push(CollisionEvent{
              .e1 = e1,
              .e2 = e2,
          });
        }
      }
    }
  }

  dispatchEvents();
}

void CollisionSystem::OnCollision(
    std::function<void(const CollisionEvent &)> callback,
    CollisionPhase phase) {
  switch (phase) {
  case CollisionPhase::Enter: {
    OnCollisionEnter(callback);
    break;
  }
  case CollisionPhase::Exit: {
    OnCollisionExit(callback);
    break;
  }
  case CollisionPhase::Stay: {
    OnCollisionStay(callback);
    break;
  }
  }
}

void CollisionSystem::OnCollisionEnter(
    std::function<void(const CollisionEvent &)> callback) {
  if (_enterCallbackCount >= _cMaxCallbacks) {
    FLOG_WARN("attempt to allocate more callbacks than what is supported. Give "
              "the collision system more capacity if needed!");
    FLOG_ERROR("failed to register collision event callback");
    return;
  }
  _enterCallbacks.push_back(std::move(callback));
  _enterCallbackCount = _enterCallbacks.size();
}

void CollisionSystem::OnCollisionStay(
    std::function<void(const CollisionEvent &)> callback) {
  if (_stayCallbackCount >= _cMaxCallbacks) {
    FLOG_WARN("attempt to allocate more callbacks than what is supported. Give "
              "the collision system more capacity if needed!");

    FLOG_ERROR("failed to register collision event callback");
    return;
  }
  _stayCallbacks.push_back(std::move(callback));
  _stayCallbackCount = _stayCallbacks.size();
}

void CollisionSystem::OnCollisionExit(
    std::function<void(const CollisionEvent &)> callback) {
  if (_exitCallbackCount >= _cMaxCallbacks) {
    FLOG_WARN("attempt to allocate more callbacks than what is supported. Give "
              "the collision system more capacity if needed!");
    FLOG_ERROR("failed to register collision event callback");
    return;
  }
  _exitCallbacks.push_back(std::move(callback));
  _exitCallbackCount = _exitCallbacks.size();
}

bool CollisionSystem::checkCollision(const scene::Transform &transform1,
                                     const scene::Collider &collider1,
                                     const scene::Transform &transform2,
                                     const scene::Collider &collider2) {
  using scene::ShapeType;

  FLOG_TRACE("checkCollision called");

  ShapeType st1 = collider1.Shape();
  ShapeType st2 = collider2.Shape();

  if (st1 == ShapeType::Box && st2 == ShapeType::Box) {
    auto &b1 = static_cast<const scene::BoxCollider &>(collider1);
    auto &b2 = static_cast<const scene::BoxCollider &>(collider2);

    float l1 = transform1.x - b1.width / 2.0f;
    float r1 = transform1.x + b1.width / 2.0f;
    float t1 = transform1.y - b1.height / 2.0f;
    float btm1 = transform1.y + b1.height / 2.0f;

    float l2 = transform2.x - b2.width / 2.0f;
    float r2 = transform2.x + b2.width / 2.0f;
    float t2 = transform2.y - b2.height / 2.0f;
    float btm2 = transform2.y + b2.height / 2.0f;

    return (l1 < r2 && r1 > l2 && t1 < btm2 && btm1 > t2);
  }

  if (st1 == ShapeType::Circle && st2 == ShapeType::Circle) {
    auto &ca = static_cast<const scene::CircleCollider &>(collider1);
    auto &cb = static_cast<const scene::CircleCollider &>(collider2);

    float32 dx = transform1.x - transform2.x;
    float32 dy = transform1.y - transform2.y;
    float32 dist = dx * dx + dy * dy;
    float32 rSum = ca.radius + cb.radius;
    return dist < rSum * rSum;
  }

  const scene::CircleCollider *circ;
  const scene::BoxCollider *box;
  const scene::Transform *tc;
  const scene::Transform *tb;

  if (st1 == ShapeType::Circle && st2 == ShapeType::Box) {
    circ = &static_cast<const scene::CircleCollider &>(collider1);
    tc = &transform1;
    box = &static_cast<const scene::BoxCollider &>(collider2);
    tb = &transform2;
  } else if (st1 == ShapeType::Box && st2 == ShapeType::Circle) {
    circ = &static_cast<const scene::CircleCollider &>(collider2);
    tc = &transform2;
    box = &static_cast<const scene::BoxCollider &>(collider1);
    tb = &transform1;
  } else {
    return false; // unknown shape combo
  }

  float32 cx = tc->x;
  float32 cy = tc->y;

  float32 closestX = std::clamp(cx, tb->x, tb->x + box->width);
  float32 closestY = std::clamp(cy, tb->y, tb->y + box->height);

  float32 dx = cx - closestX;
  float32 dy = cy - closestY;

  return (dx * dx + dy * dy) <= (circ->radius * circ->radius);
}

int64 CollisionSystem::hashCell(int32 cx, int32 cy) const {
  return (static_cast<int64>(cx) << 32) ^ static_cast<int64>(cy);
}

std::pair<int32, int32> CollisionSystem::worldToCell(float32 x,
                                                     float32 y) const {
  return {static_cast<int32>(std::floor(x / _cellSize)),
          static_cast<int32>(std::floor(y / _cellSize))};
}

void CollisionSystem::dispatchEvents() {
  for (auto &evt : _eventQ.Events()) {
    switch (evt.phase) {
    case CollisionPhase::Enter: {
      for (auto &callback : _enterCallbacks) {
        callback(evt);
      }
      break;
    }
    case CollisionPhase::Stay: {
      for (auto &callback : _stayCallbacks) {
        callback(evt);
      }
      break;
    }
    case CollisionPhase::Exit: {
      for (auto &callback : _exitCallbacks) {
        callback(evt);
      }
      break;
    }
    }
  }
}

} // namespace febundle::systems
