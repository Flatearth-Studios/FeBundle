#include "FeBundle/Core/Memory/Memory.hpp"
#include <atomic>
#include <cstddef>

namespace febundle::memory {

static inline std::size_t roundUpPow(std::size_t a) noexcept {
  if (a < 2) {
    return 1;
  }

  if ((a & (a - 1)) == 0) {
    return a;
  }

  std::size_t p = 1;
  while (p < a) {
    p <<= 1;
  }

  return p;
}

struct BlockHeader {
  void *base;
  std::size_t size, align;
  Tag tag;
};

string tagToString(Tag tag) noexcept {
  switch (tag) {
  case Tag::Application:
    return "Application";
  default:
    return "Unknown";
  }
}

BlockHeader *headerFromUser(void *user) noexcept {
  return reinterpret_cast<BlockHeader *>(
      static_cast<char *>(user) -
      static_cast<std::ptrdiff_t>(sizeof(BlockHeader)));
}

string formatBytes(uint32 bytes) {
  const uint32 kib = 1024;
  const uint32 mib = kib * kib;
  const uint32 gib = mib * kib;

  if (bytes < kib) {
    return std::format("{} B", bytes);
  }

  string unit = "KiB";
  const float32 div = (float32)bytes / kib;
  if (div >= (float32)mib && div < (float32)gib) {
    unit = "MiB";
  } else if (div >= gib) {
    unit = "GiB";
  }

  return std::format("{:.2f} %s", div, unit);
}

MemoryManager::MemoryManager() {
  _stats.bytesInUsePerTag.emplace(Tag::Unknown, 0);
  _stats.bytesInUsePerTag.emplace(Tag::Application, 0);
}

MemoryManager &MemoryManager::Self() {
  static MemoryManager instance;
  return instance;
}

void *MemoryManager::Alloc(std::size_t n, Tag tag, std::size_t align) noexcept {
  if (align < alignof(std::max_align_t)) {
    align = roundUpPow(align);
  }

  const std::size_t total = n + sizeof(BlockHeader) + (align - 1);
  char *base = static_cast<char *>(::operator new(total, std::nothrow));
  if (!base) {
    return nullptr;
  }

  uintptr_t start = reinterpret_cast<uintptr_t>(base) + sizeof(BlockHeader);
  uintptr_t aligned = (start + (align - 1)) & ~(uintptr_t(align - 1));
  char *user = reinterpret_cast<char *>(aligned);

  BlockHeader *h = reinterpret_cast<BlockHeader *>(user - sizeof(BlockHeader));
  h->base = base;
  h->size = n;
  h->align = align;
  h->tag = tag;

  _stats.totalBytesInUse.fetch_add(n, std::memory_order_relaxed);
  _stats.bytesInUsePerTag[tag].fetch_add(n, std::memory_order_relaxed);

  return static_cast<void *>(user);
}

void MemoryManager::Free(void *p, std::size_t n, Tag tag) noexcept {
  if (!p) {
    LOG_WARN("attempt to free invalid memory address of nullptr");
    return;
  }

  BlockHeader *h = headerFromUser(p);
  _stats.totalBytesInUse.fetch_sub(h->size, std::memory_order_relaxed);
  _stats.bytesInUsePerTag[tag].fetch_sub(h->size, std::memory_order_relaxed);

  ::operator delete(h->base);
}

const MemStats &MemoryManager::Stats(bool verbose) const noexcept {
  if (!verbose) {
    return _stats;
  }

  const uint32 totalBytes = _stats.totalBytesInUse.load();
  const string formattedBytes = formatBytes(totalBytes);

  std::println("┌───────────────────────────────┐");
  std::println("│      Memory Usage Stats       │");
  std::println("└───────────────────────────────┘");

  std::print(" {:<15} {:>12}\n", "Total", formattedBytes);
  std::println("──────────────────────────────────");

  for (const auto &[tag, atomicBytes] : _stats.bytesInUsePerTag) {
    const string tagName = tagToString(tag);
    const string usedBytes = formatBytes(atomicBytes.load());
    std::print(" {:<15} {:>12}\n", tagName, usedBytes);
  }

  return _stats;
}

} // namespace febundle::memory
