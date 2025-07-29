#pragma once

#include "typedefs.hpp"

#include <string>
#include <memory>
#include <unordered_map>

#include "io/fwd.hpp"

namespace gui {
    class GUI;
    class UINode;
}

struct uidocscript {
    bool onopen : 1;
    bool onprogress : 1;
    bool onclose : 1;
};

using UINodesMap = std::unordered_map<std::string, std::shared_ptr<gui::UINode>>;

class UiDocument {
    std::string id;
    uidocscript script;
    UINodesMap map;
    std::shared_ptr<gui::UINode> root;
    script_env_t env;
public:
    UiDocument(
        std::string id, 
        uidocscript script, 
        const std::shared_ptr<gui::UINode> &root,
        script_env_t env
    );

    void rebuildIndices();

    const std::string& getId() const;
    const UINodesMap& getMap() const;
    UINodesMap& getMapWriteable();
    std::shared_ptr<gui::UINode> getRoot() const;
    std::shared_ptr<gui::UINode> get(const std::string& id) const;
    const uidocscript& getScript() const;
    script_env_t getEnvironment() const;

    static std::unique_ptr<UiDocument> read(
        gui::GUI&,
        const script_env_t& parent_env,
        const std::string& name,
        const io::Path& file,
        const std::string& fileName
    );
    static std::shared_ptr<gui::UINode> readElement(
        gui::GUI&, const io::Path& file, const std::string& fileName
    );
};
