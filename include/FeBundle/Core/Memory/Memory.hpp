#ifndef INCLUDE_FEBUNDLE_CORE_MEMORY_MEMORY_HPP_
#define INCLUDE_FEBUNDLE_CORE_MEMORY_MEMORY_HPP_

#include "../Defines.hpp"
#include "../Logger.hpp"
#include <atomic>
#include <cstddef>

namespace febundle::memory {

enum class Tag {
  Unknown,
  Application,
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

template <typename T> struct Deleter {
  Tag tag{Tag::Unknown};
  void operator()(T *ptr) const noexcept {
    if (!ptr)
      return;
    ptr->~T();
    MemoryManager::Self().Free(static_cast<void *>(ptr), sizeof(T), tag);
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

} // namespace febundle::memory

#endif // INCLUDE_FEBUNDLE_CORE_MEMORY_MEMORY_HPP_
