#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_FILE_WATCHER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_FILE_WATCHER_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include <mutex>
#include <thread>

namespace febundle::systems {

enum class DirectoryType {
  File,
  Directory,
};

struct DirectoryEntry {
  DirectoryType type;
  fs::path path;
};

class FileWatcher {
public:
  ~FileWatcher();
  void SetLoader(AssetLoader *pAl);

  void Watch(const fs::path &rootDir);
  void Relax();

private:
  void watchLoop(const fs::path &rootDir);
  void checkAndMarkDirty(const fs::path &path,
                         const fs::file_time_type &lastWrite);

  std::vector<fs::path> updateFilesAndListDirectoriesAt(const fs::path &path);
  void walkDirectories(const std::vector<fs::path> &dirs);
  void walkDirectories(const fs::path &path);

  void markDirty(const fs::directory_entry &entry);

private:
  std::thread _watchThread;
  std::mutex _mtx;
  AssetLoader *_pAssetLoader;
  atomic_bool _watching{false};
  std::vector<DirectoryEntry> _dirEntries;
  umap<string, fs::file_time_type> _fileTimestamps;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_FILE_WATCHER_HPP_
