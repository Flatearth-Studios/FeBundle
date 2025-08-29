#ifndef INCLUDE_FEBUNDLE_CORE_DEFINES_HPP_
#define INCLUDE_FEBUNDLE_CORE_DEFINES_HPP_

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <expected>
#include <fstream>
#include "Errors.hpp"

namespace fs = std::filesystem;

using string = std::string;
using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;

using float64 = double;
using float32 = float;

using int64 = int64_t;
using int32 = int32_t;
using int16 = int16_t;
using int8 = int8_t;

template <typename ...Args>
using fstring = std::format_string<Args...>;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename ...Args>
using umap = std::unordered_map<Args...>;

template <typename ...Args>
using uset = std::unordered_set<Args...>;

template <typename ...Args>
using omap = std::map<Args...>;

template <typename ...Args>
using oset = std::set<Args...>;

template <typename T>
using Result = std::expected<T, febundle::Error>;

using atomic_bool = std::atomic<bool>;

#define NODISCARD [[nodiscard]]

#endif // INCLUDE_FEBUNDLE_CORE_DEFINES_HPP_
