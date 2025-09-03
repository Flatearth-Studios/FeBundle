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

#if defined(_MSVC_LANG)
#define STATIC_ASSERT static_assert
#elif defined(__cplusplus) && __cplusplus >= 202303L
#define STATIC_ASSERT static_assert
#else
#error "C++17 or newer needed to proceed"
#endif

#define FEXPORT
#ifdef FEXPORT
// Exports
#ifdef _MSC_VER
#define FEAPI __declspec(dllexport)
#else
#define FEAPI __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define FEAPI __declspec(dllimport)
#else
#define FEAPI
#endif
#endif

#ifndef _DEBUG
// Set to false if in release
#define _DEBUG true 
#endif

static constexpr float64 FE_PI = 3.14159265358979323846f;
static constexpr float64 FE_2_PI = 2 * FE_PI;
static constexpr float64 FE_HALF_PI = FE_PI / 2.0f;
static constexpr float64 FE_QUARTER_PI = FE_PI / 4.0f;
static constexpr float64 FE_1_OVER_PI = 1 / FE_PI;
static constexpr float64 FE_1_OVER_2_PI = 1 / FE_2_PI;
static constexpr float64 FE_SQRT_2 = 1.41421356237309504880f;
static constexpr float64 FE_SQRT_3 = 1.73205080756887729352f;
static constexpr float64 FE_1_OVER_SQRT_2 = 0.70710678118654752440f;
static constexpr float64 FE_1_OVER_SQRT_3 = 0.57735026918962576450f;
static constexpr float64 FE_DEG_TO_RAD_MUL = FE_PI / 180.0f;
static constexpr float64 FE_RAD_TO_DEG_MUL = 180.0f / FE_PI;

static constexpr float64 FE_MS_TO_SEC_MUL = 1 / 1000.0f;
static constexpr float64 FE_SEC_TO_MS_MUL = 1000.0f;

static constexpr float64 FE_F64MAX = 1e30f;
static constexpr float64 FE_F64EPS = 1.192092896e-7f;

#endif // INCLUDE_FEBUNDLE_CORE_DEFINES_HPP_
