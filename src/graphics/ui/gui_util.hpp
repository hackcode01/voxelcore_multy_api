#pragma once

#include <memory>
#include <string>

#include "delegates.hpp"
#include "typedefs.hpp"

class Engine;

namespace gui {
    class GUI;
    class UINode;
}

namespace guiutil {
    /// @brief Create element from XML
    /// @param source XML
    std::shared_ptr<gui::UINode> create(
        gui::GUI& gui, const std::string& source, script_env_t env = 0
    );

    void alert(
        Engine& engine,
        const std::wstring& text,
        const runnable_t& on_hidden = nullptr
    );

    void confirm(
        Engine& engine,
        const std::wstring& text,
        const runnable_t& on_confirm = nullptr,
        const runnable_t& on_deny = nullptr,
        std::wstring yestext = L"",
        std::wstring notext = L""
    );

    void confirm_with_memo(
        Engine& engine,
        const std::wstring& text,
        const std::wstring& memo,
        const runnable_t& on_confirm = nullptr,
        std::wstring yestext = L"",
        std::wstring notext = L""
    );
}
