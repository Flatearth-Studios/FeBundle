#include "FeBundle/Core/Systems/CollisionSystem.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include <cmath>

namespace febundle::systems {

CollisionSystem::CollisionSystem(float32 cellSize) : _cellSize(cellSize) {}

void CollisionSystem::Update(scene::Scene &scene,
                             events::EventQueue<CollisionEvent> &queue) {
  _grid.clear();

  const auto entityMap = scene.AccessAll();
  for (auto &[e, comp] : entityMap) {
    auto *transform = scene.GetComponent<scene::Transform>(e);
    auto *collider = scene.GetComponent<scene::Collider>(e);
    if (transform == nullptr || collider == nullptr) {
      continue;
    }

    auto [cx, cy] = worldToCell(transform->x, transform->y);
    _grid[hashCell(cx, cy)].entities.push_back(e);
  }

  for (auto &[hash, cell] : _grid) {
    for (size_t i = 0; i < cell.entities.size(); i++) {
      for (size_t j = i + 1; j < cell.entities.size(); j++) {
        auto e1 = cell.entities[i];
        auto e2 = cell.entities[j];
        auto *t1 = scene.GetComponent<scene::Transform>(e1);
        auto *t2 = scene.GetComponent<scene::Transform>(e2);
        auto *c1 = scene.GetComponent<scene::Collider>(e1);
        auto *c2 = scene.GetComponent<scene::Collider>(e2);
        if (t1 == nullptr || t2 == nullptr || c1 == nullptr || c2 == nullptr) {
          continue;
        }

        if (checkCollision(*t1, *c1, *t2, *c2)) {
          queue.Push(CollisionEvent{
              .e1 = e1,
              .e2 = e2,
          });
        }
      }
    }
  }
}

bool CollisionSystem::checkCollision(const scene::Transform &transform1,
                                     const scene::Collider &collider1,
                                     const scene::Transform &transform2,
                                     const scene::Collider &collider2) {
  using scene::ShapeType;

  ShapeType st1 = collider1.Shape();
  ShapeType st2 = collider2.Shape();

  if (st1 == ShapeType::Box && st2 == ShapeType::Box) {
    auto &b1 = static_cast<const scene::BoxCollider &>(collider1);
    auto &b2 = static_cast<const scene::BoxCollider &>(collider2);

    return (transform1.x < transform2.x + b2.width &&
            transform1.x + b1.width > transform2.x &&
            transform1.y < transform2.y + b2.height &&
            transform1.y + b1.height > transform2.y);
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

} // namespace febundle::systems
