#include "Engine.hpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include "debug/Logger.hpp"
#include "assets/AssetsLoader.hpp"
#include "audio/audio.hpp"
#include "coders/GLSLExtension.hpp"
#include "coders/imageio.hpp"
#include "coders/json.hpp"
#include "coders/toml.hpp"
#include "coders/commons.hpp"
#include "devtools/Editor.hpp"
#include "devtools/Project.hpp"
#include "content/ContentControl.hpp"
#include "core_defs.hpp"
#include "io/io.hpp"
#include "frontend/locale.hpp"
#include "frontend/menu.hpp"
#include "frontend/screens/Screen.hpp"
#include "graphics/render/ModelsGenerator.hpp"
#include "graphics/core/DrawContext.hpp"
#include "graphics/core/ImageData.hpp"
#include "graphics/core/Shader.hpp"
#include "graphics/ui/GUI.hpp"
#include "objects/rigging.hpp"
#include "logic/EngineController.hpp"
#include "logic/CommandsInterpreter.hpp"
#include "logic/scripting/scripting.hpp"
#include "logic/scripting/scripting_hud.hpp"
#include "network/Network.hpp"
#include "util/platform.hpp"
#include "window/Camera.hpp"
#include "window/input.hpp"
#include "window/Window.hpp"
#include "world/Level.hpp"
#include "Mainloop.hpp"
#include "ServerMainloop.hpp"

#include <iostream>
#include <assert.h>
#include <glm/glm.hpp>
#include <unordered_set>
#include <functional>
#include <utility>

static debug::Logger logger("engine");

static std::unique_ptr<ImageData> load_icon() {
    try {
        auto file = "res:textures/misc/icon.png";
        if (io::exists(file)) {
            return imageio::read(file);
        }
    } catch (const std::exception& err) {
        logger.error() << "could not load window icon: " << err.what();
    }
    return nullptr;
}

Engine::Engine() = default;
Engine::~Engine() = default;

static std::unique_ptr<Engine> instance = nullptr;

Engine& Engine::getInstance() {
    if (!instance) {
        instance = std::make_unique<Engine>();
    }
    return *instance;
}

void Engine::initialize(CoreParameters coreParameters) {
    m_params = std::move(coreParameters);
    m_settingsHandler = std::make_unique<SettingsHandler>(m_settings);

    logger.info() << "engine version: " << ENGINE_VERSION_STRING;
    if (m_params.headless) {
        logger.info() << "headless mode is enabled";
    }
    if (m_params.projectFolder.empty()) {
        m_params.projectFolder = m_params.resFolder;
    }
    m_paths.setResourcesFolder(m_params.resFolder);
    m_paths.setUserFilesFolder(m_params.userFolder);
    m_paths.setProjectFolder(m_params.projectFolder);
    m_paths.prepare();
    loadProject();

    m_editor = std::make_unique<devtools::Editor>(*this);
    m_cmd = std::make_unique<cmd::CommandsInterpreter>();
    m_network = network::Network::create(m_settings.network);

    if (!m_params.scriptFile.empty()) {
        m_paths.setScriptFolder(m_params.scriptFile.parent_path());
    }
    loadSettings();

    m_controller = std::make_unique<EngineController>(*this);
    if (!m_params.headless) {
        std::string title = m_project->title;
        if (title.empty()) {
            title = "VoxelCore v" +
                            std::to_string(ENGINE_VERSION_MAJOR) + "." +
                            std::to_string(ENGINE_VERSION_MINOR);
        }
        if (ENGINE_DEBUG_BUILD) {
            title += " [debug]";
        }
        auto [window, input] = Window::initialize(&m_settings.display, title);
        if (!window || !input){
            throw initialize_error("could not initialize window");
        }
        window->setFramerate(m_settings.display.framerate.get());

        m_time.set(window->time());
        if (auto icon = load_icon()) {
            icon->flipY();
            window->setIcon(icon.get());
        }
        this->m_window = std::move(window);
        this->m_input = std::move(input);

        loadControls();

        m_gui = std::make_unique<gui::GUI>(*this);
        if (ENGINE_DEBUG_BUILD) {
            menus::create_version_label(*m_gui);
        }
        keepAlive(m_settings.display.fullscreen.observe(
            [this](bool value) {
                if (value != this->m_window->isFullscreen()) {
                    this->m_window->toggleFullscreen();
                }
            },
            true
        ));
    }
    audio::initialize(!m_params.headless, m_settings.audio);

    bool langNotSet = m_settings.ui.language.get() == "auto";
    if (langNotSet) {
        m_settings.ui.language.set(
            langs::locale_by_envlocale(platform::detect_locale())
        );
    }

    m_content = std::make_unique<ContentControl>(*m_project, m_paths, *m_input, [this]() {
        m_editor->loadTools();
        langs::setup(langs::get_current(), m_paths.resPaths.collectRoots());
        if (!isHeadless()) {
            for (auto& pack : m_content->getAllContentPacks()) {
                auto configFolder = pack.folder / "config";
                auto bindsFile = configFolder / "bindings.toml";
                if (io::is_regular_file(bindsFile)) {
                    m_input->getBindings().read(
                        toml::parse(
                            bindsFile.string(), io::read_string(bindsFile)
                        ),
                        BindType::BIND
                    );
                }
            }
            loadAssets();
        }
    });
    scripting::initialize(this);
    if (!isHeadless()) {
        m_gui->setPageLoader(scripting::create_page_loader());
    }
    keepAlive(m_settings.ui.language.observe([this](auto lang) {
        langs::setup(lang, m_paths.resPaths.collectRoots());
    }, true));
}

void Engine::loadSettings() {
    io::path settings_file = EnginePaths::SETTINGS_FILE;
    if (io::is_regular_file(settings_file)) {
        logger.info() << "loading settings";
        std::string text = io::read_string(settings_file);
        try {
            toml::parse(*m_settingsHandler, settings_file.string(), text);
        } catch (const parsing_error& err) {
            logger.error() << err.errorLog();
            throw;
        }
    }
}

void Engine::loadControls() {
    io::path controls_file = EnginePaths::CONTROLS_FILE;
    if (io::is_regular_file(controls_file)) {
        logger.info() << "loading controls";
        std::string text = io::read_string(controls_file);
        m_input->getBindings().read(
            toml::parse(controls_file.string(), text), BindType::BIND
        );
    }
}

void Engine::updateHotkeys() {
    if (m_input->jpressed(Keycode::F2)) {
        saveScreenshot();
    }
    if (m_input->pressed(Keycode::LEFT_CONTROL) && m_input->pressed(Keycode::F3) &&
        m_input->jpressed(Keycode::U)) {
        m_gui->toggleDebug();
    }
    if (m_input->jpressed(Keycode::F11)) {
        m_settings.display.fullscreen.toggle();
    }
}

void Engine::saveScreenshot() {
    auto image = m_window->takeScreenshot();
    image->flipY();
    io::path filename = m_paths.getNewScreenshotFile("png");
    imageio::write(filename.string(), image.get());
    logger.info() << "saved screenshot as " << filename.string();
}

void Engine::run() {
    if (m_params.headless) {
        ServerMainloop(*this).run();
    } else {
        Mainloop(*this).run();
    }
}

void Engine::postUpdate() {
    m_network->update();
    postRunnables.run();
    scripting::process_post_runnables();
}

void Engine::updateFrontend() {
    double delta = m_time.getDelta();
    updateHotkeys();
    audio::update(delta);
    m_gui->act(delta, m_window->getSize());
    m_screen->update(delta);
    m_gui->postAct();
}

void Engine::nextFrame() {
    m_window->setFramerate(
        m_window->isIconified() && m_settings.display.limitFpsIconified.get()
            ? 20
            : m_settings.display.framerate.get()
    );
    m_window->swapBuffers();
    m_input->pollEvents();
}

void Engine::renderFrame() {
    m_screen->draw(m_time.getDelta());

    DrawContext ctx(nullptr, *m_window, nullptr);
    m_gui->draw(ctx, *m_assets);
}

void Engine::saveSettings() {
    logger.info() << "saving settings";
    io::write_string(EnginePaths::SETTINGS_FILE, toml::stringify(*m_settingsHandler));
    if (!m_params.headless) {
        logger.info() << "saving bindings";
        io::write_string(EnginePaths::CONTROLS_FILE, m_input->getBindings().write());
    }
}

void Engine::close() {
    saveSettings();
    logger.info() << "shutting down";
    if (m_screen) {
        m_screen->onEngineShutdown();
        m_screen.reset();
    }
    m_content.reset();
    m_assets.reset();
    m_cmd.reset();

    if (m_gui) {
        m_gui.reset();
        logger.info() << "gui finished";
    }
    audio::close();
    m_network.reset();
    clearKeepedObjects();
    scripting::close();
    logger.info() << "scripting finished";
    if (!m_params.headless) {
        m_window.reset();
        logger.info() << "window closed";
    }
    logger.info() << "engine finished";
}

void Engine::terminate() {
    instance->close();
    instance.reset();
}

EngineController* Engine::getController() {
    return m_controller.get();
}

void Engine::setLevelConsumer(OnWorldOpen levelConsumer) {
    this->m_levelConsumer = std::move(levelConsumer);
}

void Engine::loadAssets() {
    logger.info() << "loading assets";
    Shader::preprocessor->setPaths(&m_paths.resPaths);

    auto content = this->m_content->get();

    auto new_assets = std::make_unique<Assets>();
    AssetsLoader loader(*this, *new_assets, m_paths.resPaths);
    AssetsLoader::addDefaults(loader, content);

    // no need
    // correct log messages order is more useful
    bool threading = false; // look at two upper lines
    if (threading) {
        auto task = loader.startTask([=](){});
        task->waitForEnd();
    } else {
        while (loader.hasNext()) {
            loader.loadNext();
        }
    }

    m_assets = std::move(new_assets);
    if (content) {
        ModelsGenerator::prepare(*content, *m_assets);
    }
    m_assets->setup();
    m_gui->onAssetsLoad(m_assets.get());
}

void Engine::loadProject() {
    io::path projectFile = "project:project.toml";
    m_project = std::make_unique<Project>();
    m_project->deserialize(io::read_object(projectFile));
    logger.info() << "loaded project " << util::quote(m_project->name);
}

void Engine::setScreen(std::shared_ptr<Screen> screen) {
    // reset audio channels (stop all sources)
    audio::reset_channel(audio::get_channel_index("regular"));
    audio::reset_channel(audio::get_channel_index("ambient"));
    this->m_screen = std::move(screen);
}

void Engine::onWorldOpen(std::unique_ptr<Level> level, int64_t localPlayer) {
    logger.info() << "world open";
    m_levelConsumer(std::move(level), localPlayer);
}

void Engine::onWorldClosed() {
    logger.info() << "world closed";
    m_levelConsumer(nullptr, -1);
}

void Engine::quit() {
    m_quitSignal = true;

    if (!isHeadless()) {
        m_window->setShouldClose(true);
    }
}

bool Engine::isQuitSignal() const {
    return m_quitSignal;
}

EngineSettings& Engine::getSettings() {
    return m_settings;
}

Assets* Engine::getAssets() {
    return m_assets.get();
}

EnginePaths& Engine::getPaths() {
    return m_paths;
}

ResPaths& Engine::getResPaths() {
    return m_paths.resPaths;
}

std::shared_ptr<Screen> Engine::getScreen() {
    return m_screen;
}

SettingsHandler& Engine::getSettingsHandler() {
    return *m_settingsHandler;
}

Time& Engine::getTime() {
    return m_time;
}

const CoreParameters& Engine::getCoreParameters() const {
    return m_params;
}

bool Engine::isHeadless() const {
    return m_params.headless;
}

ContentControl& Engine::getContentControl() {
    return *m_content;
}
