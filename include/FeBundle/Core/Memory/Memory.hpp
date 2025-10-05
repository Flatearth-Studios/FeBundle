#ifndef INCLUDE_FEBUNDLE_CORE_MEMORY_MEMORY_HPP_
#define INCLUDE_FEBUNDLE_CORE_MEMORY_MEMORY_HPP_

#include "../Defines.hpp"
#include "../Logger.hpp"
#include <atomic>
#include <cstddef>
#include <new>

namespace febundle::memory {

enum class Tag {
  Unknown,
  Application,
  Scene,
  Renderer,
  AssetManager,
  EventBus,
};

struct MemStats {
  umap<Tag, std::atomic<uint64>> bytesInUsePerTag{};
  std::atomic<uint64> totalBytesInUse{0};
};

class MemoryManager {
public:
  static MemoryManager &Self();
  void *Alloc(std::size_t n, Tag tag,
              std::size_t align = alignof(std::max_align_t)) noexcept;
  void Free(void *p, std::size_t n, Tag = Tag::Unknown) noexcept;
  const MemStats &Stats(bool verbose = false) const noexcept;

private:
  MemoryManager();
  MemStats _stats{};
};

template <typename Base> struct PolyDeleter {
  Tag tag{Tag::Unknown};
  std::size_t size{0};               // sizeof(Derived)
  void (*destroy)(Base *) = nullptr; // calls Derived::~Derived

  void operator()(Base *p) const noexcept {
    if (!p)
      return;
    destroy(p); // run the right destructor
    MemoryManager::Self().Free(static_cast<void *>(p), size, tag);
  }
};

template <typename T> struct Deleter {
  Tag tag{Tag::Unknown};
  void operator()(T *ptr) const noexcept {
    if (!ptr)
      return;
    ptr->~T();
    MemoryManager::Self().Free(static_cast<void *>(ptr), sizeof(T), tag);
  }
};

template <typename Base, typename Derived, typename... Args>
  requires std::derived_from<Derived, Base> &&
           std::is_constructible_v<Derived, Args...>
NODISCARD std::expected<std::unique_ptr<Base, PolyDeleter<Base>>, Error>
MakeUniquePoly(Tag tag, Args &&...args) {
  if (tag == Tag::Unknown) {
    LOG_WARN("Allocating memory with tag 'Unknown'");
  }

  void *raw =
      MemoryManager::Self().Alloc(sizeof(Derived), tag, alignof(Derived));
  if (!raw)
    return std::unexpected{Error(ErrorName::BadAllocation)};

  try {
    auto *obj = new (raw) Derived(std::forward<Args>(args)...);
    PolyDeleter<Base> del{
        .tag = tag, .size = sizeof(Derived), .destroy = [](Base *p) {
          static_cast<Derived *>(p)->~Derived();
        }};
    return std::unique_ptr<Base, PolyDeleter<Base>>(static_cast<Base *>(obj),
                                                    del);
  } catch (...) {
    MemoryManager::Self().Free(raw, sizeof(Derived), tag);
    return std::unexpected{Error(ErrorName::AllocationException)};
  }
}

template <typename T> struct TaggedAllocator {
  using value_type = T;

  Tag tag{Tag::Unknown};

  template <typename U>
  TaggedAllocator(const TaggedAllocator<U> &other) noexcept : tag(other.tag) {}
  TaggedAllocator() = default;
  explicit TaggedAllocator(Tag tag) : tag(tag) {};

  T *allocate(std::size_t n) {
    void *p = MemoryManager::Self().Alloc(n * sizeof(T), tag, alignof(T));
    if (!p) {
      throw std::bad_alloc();
    }
    return static_cast<T *>(p);
  }

  void deallocate(T *p, std::size_t n) noexcept {
    MemoryManager::Self().Free(static_cast<void *>(p), n * sizeof(T), tag);
  }

  template <typename U>
  bool operator==(const TaggedAllocator<U> &rhs) const noexcept {
    return tag == rhs.tag;
  }

  template <typename U>
  bool operator!=(const TaggedAllocator<U> &rhs) const noexcept {
    return tag != rhs.tag;
  }
};

template <typename T, typename... Args>
  requires(std::is_constructible_v<T, Args...>)
NODISCARD std::expected<std::unique_ptr<T, Deleter<T>>, Error>
MakeUnique(Tag tag, Args &&...args) {
  if (tag == Tag::Unknown) {
    LOG_WARN("Allocating memory with tag 'Unknown'");
  }
  void *raw = MemoryManager::Self().Alloc(sizeof(T), tag, alignof(T));
  if (!raw)
    return std::unexpected{Error(ErrorName::BadAllocation)};
  try {
    T *obj = new (raw) T(std::forward<Args>(args)...);
    return std::unique_ptr<T, Deleter<T>>(obj, Deleter<T>{tag});
  } catch (...) {
    MemoryManager::Self().Free(raw, sizeof(T), tag);
    return std::unexpected{Error(ErrorName::AllocationException)};
  }
}

template <typename T, typename... Args>
  requires(std::is_constructible_v<T, Args...>)
NODISCARD std::expected<std::unique_ptr<T, Deleter<T>>, Error>
UniquePtr(Tag tag, Args &&...args) {
  return MakeUnique<T>(tag, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
  requires(std::is_constructible_v<T, Args...>)
NODISCARD std::expected<std::shared_ptr<T>, Error> MakeShared(Tag tag,
                                                              Args &&...args) {
  if (tag == Tag::Unknown) {
    LOG_WARN("Allocating memory with tag 'Unknown'");
  }
  try {
    auto sp = std::allocate_shared<T>(TaggedAllocator<T>{tag},
                                      std::forward<Args>(args)...);
    return sp;
  } catch (const std::bad_alloc &) {
    return std::unexpected{Error(ErrorName::BadAllocation)};
  } catch (...) {
    return std::unexpected{Error(ErrorName::AllocationException)};
  }
}

template <typename T, typename... Args>
  requires(std::is_constructible_v<T, Args...>)
NODISCARD std::expected<std::shared_ptr<T>, Error> SharedPtr(Tag tag,
                                                             Args &&...args) {
  return MakeShared(tag, std::forward<Args>(args)...);
}

template <typename T>
std::weak_ptr<T> MakeWeak(const std::shared_ptr<T> &sp) noexcept {
  return std::weak_ptr<T>(sp);
}

} // namespace febundle::memory

#endif // INCLUDE_FEBUNDLE_CORE_MEMORY_MEMORY_HPP_
