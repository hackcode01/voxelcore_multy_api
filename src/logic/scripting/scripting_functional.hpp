#pragma once

#include <glm/glm.hpp>
#include <string>

#include "data/dv_fwd.hpp"
#include "delegates.hpp"
#include "typedefs.hpp"

namespace scripting {
    using common_func = std::function<dv::value(const std::vector<dv::value>&)>;
    using value_to_string_func = std::function<std::string(const dv::value&)>;

    runnable_t create_runnable(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    key_handler_t create_key_handler(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    string_consumer_t create_string_consumer(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    wstring_consumer_t create_wstring_consumer(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    wstring_supplier_t create_wstring_supplier(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    wstring_checker_t create_wstring_validator(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    bool_consumer_t create_bool_consumer(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    bool_supplier_t create_bool_supplier(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    double_consumer_t create_number_consumer(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    double_supplier_t create_number_supplier(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    int_array_consumer_t create_int_array_consumer(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    vec2_supplier_t create_vec2_supplier(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );

    value_to_string_func create_tostring(
        const script_env_t& env,
        const std::string& src,
        const std::string& file = "[string]"
    );
}
