#define FE_DEBUG
#include "FeBundle/Core/Systems/FileWatcher.hpp"
#include "FeBundle/Core/Logger.hpp"
#include <filesystem>
#include <thread>

namespace febundle::systems {

FileWatcher::~FileWatcher() {
  _pAssetLoader = nullptr;
}

void FileWatcher::SetLoader(AssetLoader *pAl) { _pAssetLoader = pAl; }

void FileWatcher::Watch(const fs::path &rootDir) {
  if (_pAssetLoader == nullptr) {
    FLOG_ERROR("cannot watch if loader is not set");
    return;
  }

  if (_watching.exchange(true)) {
    FLOG_WARN("already watching!");
    return;
  }

  const fs::path absPath = fs::absolute(rootDir);
  _watchThread = std::thread([this, absPath]() { this->watchLoop(absPath); });
  FLOG_INFO("watching...");
}

void FileWatcher::Relax() {
  if (!_watching.exchange(false)) {
    return;
  }

  if (_watchThread.joinable()) {
    _watchThread.join();
    _watching.store(false);
  }
}

void FileWatcher::watchLoop(const fs::path &rootDir) {
  FLOG_INFO("FileWatcher thread started for {}", rootDir.string());
  while (_watching.load()) {
    if (_pAssetLoader == nullptr) {
      // Loader not yet attached, wait a bit and try again
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    walkDirectories(rootDir);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  FLOG_INFO("FileWatcher thread stopped");
}


std::vector<fs::path>
FileWatcher::updateFilesAndListDirectoriesAt(const fs::path &path) {
  std::vector<fs::path> directories;
  try {
    for (const fs::directory_entry &entry : fs::directory_iterator(path)) {
      if (entry.is_directory()) {
        directories.push_back(entry.path());
        continue;
      }

      const auto pathStr = entry.path().string();
      const auto lastWrite = entry.last_write_time();

      std::scoped_lock lock(_mtx);
      auto it = _fileTimestamps.find(pathStr);

      if (it == _fileTimestamps.end()) {
        _fileTimestamps[pathStr] = lastWrite;
        continue;
      }

      if (lastWrite > it->second) {
        it->second = lastWrite;
        FLOG_DEBUG("{} changed", pathStr);
        _pAssetLoader->MarkAsDirty(entry.path());
      }
    }
  } catch (const std::exception &e) {
    FLOG_ERROR("FileWatcher: error scanning {} -> {}", path.string(), e.what());
  }
  return directories;
}

void FileWatcher::walkDirectories(const std::vector<fs::path> &dirs) {
  if (dirs.empty()) {
    return;
  }

  for (const fs::path &path : dirs) {
    auto childDirs = updateFilesAndListDirectoriesAt(path);
    walkDirectories(childDirs);
  }
}

void FileWatcher::walkDirectories(const fs::path &path) {
  auto childDirs = updateFilesAndListDirectoriesAt(path);
  walkDirectories(childDirs);
}

void FileWatcher::markDirty(const fs::directory_entry &entry) {
  std::scoped_lock lock(_mtx);
  auto &timestamp = _fileTimestamps.at(entry.path().string());
  if (timestamp < entry.last_write_time()) {
    timestamp = entry.last_write_time();
    _pAssetLoader->MarkAsDirty(entry.path());
  }
}

} // namespace febundle::systems
