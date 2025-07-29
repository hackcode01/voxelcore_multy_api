#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <optional>

#include "typedefs.hpp"
#include "content_fwd.hpp"
#include "io/io.hpp"

class EnginePaths;

class contentpack_error : public std::runtime_error {
    std::string packId;
    io::Path folder;
public:
    contentpack_error(
        std::string packId,
        io::Path folder,
        const std::string& message
    );

    std::string getPackId() const;
    io::Path getFolder() const;
};

enum class DependencyLevel {
    required,  // dependency must be installed
    optional,  // dependency will be installed if found
    weak,      // only affects packs order
};

/// @brief Content-pack that should be installed earlier the dependent
struct DependencyPack {
    DependencyLevel level;
    std::string id;
};

struct ContentPackStats {
    size_t totalBlocks;
    size_t totalItems;
    size_t totalEntities;

    inline bool hasSavingContent() const {
        return totalBlocks + totalItems + totalEntities > 0;
    }
};

struct ContentPack {
    std::string id = "none";
    std::string title = "untitled";
    std::string version = "0.0";
    std::string creator = "";
    std::string description = "no description";
    io::Path folder;
    std::vector<DependencyPack> dependencies;
    std::string source = "";

    io::Path getContentFile() const;

    std::optional<ContentPackStats> loadStats() const;

    static inline const std::string PACKAGE_FILENAME = "package.json";
    static inline const std::string CONTENT_FILENAME = "content.json";
    static inline const io::Path BLOCKS_FOLDER = "blocks";
    static inline const io::Path ITEMS_FOLDER = "items";
    static inline const io::Path ENTITIES_FOLDER = "entities";
    static inline const io::Path GENERATORS_FOLDER = "generators";
    static const std::vector<std::string> RESERVED_NAMES;

    static bool is_pack(const io::Path& folder);
    static ContentPack read(const io::Path& folder);

    static void scanFolder(
        const io::Path& folder, std::vector<ContentPack>& packs
    );

    static std::vector<std::string> worldPacksList(
        const io::Path& folder
    );

    static io::Path findPack(
        const EnginePaths* paths,
        const io::Path& worldDir,
        const std::string& name
    );

    static ContentPack createCore();

    static inline io::Path getFolderFor(ContentType type) {
        switch (type) {
            case ContentType::BLOCK: return ContentPack::BLOCKS_FOLDER;
            case ContentType::ITEM: return ContentPack::ITEMS_FOLDER;
            case ContentType::ENTITY: return ContentPack::ENTITIES_FOLDER;
            case ContentType::GENERATOR: return ContentPack::GENERATORS_FOLDER;
            case ContentType::NONE: return "";
            default: return "";
        }
    }
};

struct WorldFuncsSet {
    bool onblockplaced;
    bool onblockreplaced;
    bool onblockbreaking;
    bool onblockbroken;
    bool onblockinteract;
    bool onplayertick;
    bool onchunkpresent;
    bool onchunkremove;
    bool oninventoryopen;
    bool oninventoryclosed;
};

class ContentPackRuntime {
    ContentPack info;
    ContentPackStats stats {};
    script_env_t env;
public:
    WorldFuncsSet worldfuncsset {};

    ContentPackRuntime(ContentPack info, script_env_t env);
    ~ContentPackRuntime();

    inline const ContentPackStats& getStats() const {
        return stats;
    }

    inline ContentPackStats& getStatsWriteable() {
        return stats;
    }

    inline const std::string& getId() {
        return info.id;
    }

    inline const ContentPack& getInfo() const {
        return info;
    }

    inline script_env_t getEnvironment() const {
        return env;
    }
};
