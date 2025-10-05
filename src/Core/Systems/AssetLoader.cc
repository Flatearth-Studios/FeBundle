#define FE_DEBUG
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Logger.hpp"

namespace febundle::systems {

static fs::path norm(const fs::path &p) {
  try {
    auto abs = fs::absolute(p).lexically_normal();
    auto rel = fs::relative(abs, fs::current_path());
    return rel;
  } catch (...) {
    return p.lexically_normal();
  }
}


assets::AssetHandle AssetLoader::LoadFor(scene::Entity e,
                                         assets::AssetType assetType,
                                         const fs::path &rawPath) {
  const auto path = norm(rawPath);

  // Candidate handle at last allocated id
  assets::AssetHandle retHandle(_nextAssetId - 1, assetType, path);

  auto &assetHandles = _mapOfAssetHandles[e]; // auto-creates if missing

  // If identical handle exists → return it
  if (assetHandles.contains(retHandle)) {
    _nextAssetId++; // still advance counter
    return retHandle;
  }

  // Otherwise, assign a new id (monotonic)
  retHandle.id++;
  assetHandles.insert(retHandle);
  _pathToHandle.emplace(path, retHandle);
  _dirtyAssets.insert(retHandle);
  _nextAssetId++;
  _version.fetch_add(1, std::memory_order_relaxed);
  return retHandle;
}

uint64 AssetLoader::AssetCount() const { return _nextAssetId - 1; }

const umap<scene::Entity, uset<assets::AssetHandle, assets::AssetHandleHash>> &
AssetLoader::AllAssets() const {
  return _mapOfAssetHandles;
}

std::vector<assets::AssetHandle> AssetLoader::DirtyAssets() {
  std::scoped_lock lk(_mtx);
  std::vector<assets::AssetHandle> out;
  out.reserve(_dirtyAssets.size());
  for (auto &h : _dirtyAssets) {
    out.push_back(h);
  }

  _dirtyAssets.clear();
  return out;
}

uint64 AssetLoader::Version() const {
  return _version.load(std::memory_order_relaxed);
}

void AssetLoader::MarkAsDirty(const fs::path &rawPath) {
  const auto path = norm(rawPath);
  std::scoped_lock lock(_mtx);

  auto it = _pathToHandle.find(path);
  if (it == _pathToHandle.end()) {
    FLOG_WARN("cannot mark unknown asset as dirty at {}", path.string());
    return;
  }

  _dirtyAssets.insert(it->second);
  _version.fetch_add(1, std::memory_order_seq_cst); // stronger ordering
  FLOG_DEBUG("AssetLoader: marked {} as dirty (ver now {})", path.string(), _version.load());
}


} // namespace febundle::systems
