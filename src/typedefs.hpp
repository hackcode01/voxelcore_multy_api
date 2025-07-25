#ifndef __TYPEDEFS_HPP__
#define __TYPEDEFS_HPP__

#include <memory>
#include <cstdint>

using script_env_t = std::shared_ptr<int_fast32_t>;

class ObserverHandler;

/// @brief dynamic integer type (64 bit signed integer)
using integer_t = int64_t;
/// @brief dynamic floating-point type (64 bit floating point)
using number_t = double;

using uint = unsigned int;
using u64id_t = uint64_t;

/// @brief use for bytes arrays
using ubyte = uint8_t;

// content indices
using itemid_t = uint32_t;
using blockid_t = uint16_t;

using entityid_t = uint64_t;
using itemcount_t = uint32_t;
using blockstate_t = uint16_t;
using light_t = uint16_t;

#endif
