#ifndef __DELEGATES_HPP__
#define __DELEGATES_HPP__

#include <glm/glm.hpp>

#include <cstdint>
#include <functional>
#include <string>

using runnable_t = std::function<void()>;

template <class T>
using supplier_t = std::function<T()>;

template <class T>
using consumer_t = std::function<void(T)>;

using KeyCallback_t = std::function<bool()>;

/* data sources */
using wstring_supplier_t = std::function<std::wstring()>;
using double_supplier_t = std::function<double()>;
using bool_supplier_t = std::function<bool()>;
using vec2_supplier_t = std::function<glm::vec2()>;
using key_handler_t = std::function<bool(int_fast32_t)>;

using string_consumer_t = std::function<void(const std::string&)>;
using wstring_consumer_t = std::function<void(const std::wstring&)>;
using double_consumer_t = std::function<void(double)>;
using bool_consumer_t = std::function<void(bool)>;
using int_array_consumer_t = std::function<void(const int_fast32_t[], size_t)>;
using wstring_checker_t = std::function<bool(const std::wstring&)>;

#endif
