#pragma once

#include <unordered_map>
#include <stdexcept>
#include <optional>
#include <string>
#include <vector>
#include <tuple>

#include "io.hpp"
#include "data/dv.hpp"

struct PathsRoot {
    std::string name;
    io::Path path;

    PathsRoot(std::string name, io::Path path)
        : name(std::move(name)), path(std::move(path)) {
    }
};

class ResPaths {
public:
    ResPaths() = default;

    ResPaths(std::vector<PathsRoot> roots);

    io::Path find(const std::string& filename) const;
    std::string findRaw(const std::string& filename) const;
    std::vector<io::Path> listdir(const std::string& folder) const;
    std::vector<std::string> listdirRaw(const std::string& folder) const;

    /// @brief Read all found list versions from all packs and combine into a
    /// single list. Invalid versions will be skipped with logging a warning
    /// @param file *.json file path relative to entry point 
    dv::value readCombinedList(const std::string& file) const;

    dv::value readCombinedObject(const std::string& file, bool deep=false) const;

    std::vector<io::Path> collectRoots();
private:
    std::vector<PathsRoot> roots;
};

class EnginePaths {
public:
    ResPaths resPaths;

    void prepare();

    void setUserFilesFolder(std::filesystem::path folder);
    const std::filesystem::path& getUserFilesFolder() const;

    void setResourcesFolder(std::filesystem::path folder);
    const std::filesystem::path& getResourcesFolder() const;

    void setScriptFolder(std::filesystem::path folder);

    void setProjectFolder(std::filesystem::path folder);

    io::Path getWorldFolderByName(const std::string& name);
    io::Path getWorldsFolder() const;

    void setCurrentWorldFolder(io::Path folder);
    io::Path getCurrentWorldFolder();
    io::Path getNewScreenshotFile(const std::string& ext);

    std::string mount(const io::Path& file);
    void unmount(const std::string& name);

    std::string createWriteableDevice(const std::string& name);

    void setEntryPoints(std::vector<PathsRoot> entryPoints);

    std::vector<io::Path> scanForWorlds() const;

    static std::tuple<std::string, std::string> parsePath(std::string_view view);

    static inline io::Path CONFIG_DEFAULTS = "config/defaults.toml";
    static inline io::Path CONTROLS_FILE = "user:controls.toml";
    static inline io::Path SETTINGS_FILE = "user:settings.toml";

private:
    std::filesystem::path userFilesFolder {"."};
    std::filesystem::path resourcesFolder {"res"};
    std::filesystem::path projectFolder = resourcesFolder;
    io::Path currentWorldFolder;
    std::optional<std::filesystem::path> scriptFolder;
    std::vector<PathsRoot> entryPoints;
    std::unordered_map<std::string, std::string> writeables;
    std::vector<std::string> mounted;

    void cleanup();
};
