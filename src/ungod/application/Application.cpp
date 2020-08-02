/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/application/ScriptedMenuState.h"
#include <boost/filesystem.hpp>
#include "ungod/script/Registration.h"
#include <numeric>

namespace ungod
{
    const std::string Application::CONFIG_LOAD_PATH = "data/config.xml";
    const std::string Application::DEFAULT_CONFIG_XML = "<config>"\
                                                       "<base title=\"UngodApp\"/>"\
                                                       "<initial_state type=\"GameState\" script_file=\"init_state.lua\"/>"\
                                                       "<resolution width=\"800\" height=\"600\" fullscreen=\"0\" vsync=\"0\" framerate_limit=\"120\"/>"\
                                                       "<graphics_settings image_quality=\"0\"/>"\
                                                       "<light unshadow_vertex_shader=\"data/shaders/light/unshadowShader.vert\" unshadow_frag_shader=\"data/shaders/light/unshadowShader.frag\"\
                                                       light_vertex_shader=\"data/shaders/light/lightOverShapeShader.vert\" light_frag_shader=\"data/shaders/light/lightOverShapeShader.frag\"\
                                                       default_penumbra_texture=\"data/shaders/light/penumbraTexture.png\"/>"\
                                                       "</config>";
    const unsigned Application::DEFAULT_VIDEO_WIDTH = 800;
    const unsigned Application::DEFAULT_VIDEO_HEIGHT = 600;
    const unsigned Application::DEFAULT_VIDEO_BPP = 32;

    Application::Application(unsigned hertz, unsigned maxUpdates) :
        mDelta(1000.0f/hertz),
        mMaxUpdates(maxUpdates),
        mAssetmanager(),
        mStatemanager(*this),
        mScriptState(),
        mWindow(),
        mVideoMode(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT, DEFAULT_VIDEO_BPP),
        mWindowStyle(sf::Style::Close | sf::Style::Resize),
        mContextSettings(),
        mVsync(false),
        mConfig(),
        mIsInit(false),
        mRunning(false),
        mExitcode(ErrorCode::QUIT_STATUS_OK),
        mDebugModeToggled(false),
        mUpdateCounter(0),
        mAccumulator(0),
        mUpdateTimer(),
        mCursors(),
        mActiveCursor(),
        mBackgroundColor(sf::Color::Black)
    {
        ungod::BaseAsset::setManager(mAssetmanager);
    }

    void Application::initApplication()
    {
        if (mIsInit)
            return;

        Logger::info("Now initializing application", mTitle, "...");

        mIsInit = true;
        loadConfig();
        createWindow();
        init();
    }

    ErrorCode Application::runApplication()
    {
        Logger::info("Now running application", mTitle, "...");
        mRunning = true;
        mainloop();
        mConfig.save();
        return mExitcode;
    }

    bool  Application::isRunning() const
    {
        return mRunning;
    }


    void Application::quitApplication(ErrorCode error)
    {
        mExitcode = error;
        mRunning = false;
    }


    void Application::loadConfig()
    {
        if ( boost::filesystem::exists(CONFIG_LOAD_PATH) )
        {
            mConfig.load(CONFIG_LOAD_PATH);
        }
        else
        {
            boost::filesystem::path configPath(CONFIG_LOAD_PATH);
            boost::system::error_code err;
            boost::filesystem::create_directories(configPath.parent_path(), err);
            if (!err)
            {
                std::ofstream file(CONFIG_LOAD_PATH);
                file << DEFAULT_CONFIG_XML;
                file.close();
                mConfig.load(CONFIG_LOAD_PATH);
            }
            else quitApplication(ErrorCode::COULD_NOT_CREATE_CONFIG_FILE);
        }
    }


    void Application::initCursor(const std::string& id, const std::string& path)
    {
        Image img{ path };
        if (img.isLoaded())
            auto c = mCursors.emplace(id, img);
    }


    void Application::setCursor(const std::string& id)
    {
        auto result = mCursors.find(id);
        if (result != mCursors.end())
        {
            mActiveCursor.setTexture(result->second.get(), true);
            mWindow.setMouseCursorVisible(false);
        }
    }


    bool Application::debugModeActive()
    {
        return mDebugModeToggled;
    }


    void Application::toggleDebugMode()
    {
        mDebugModeToggled = !mDebugModeToggled;
    }


    const sf::RenderWindow& Application::getWindow() const
    {
        return mWindow;
    }

    sf::RenderWindow& Application::getWindow()
    {
        return mWindow;
    }


    void Application::setBackgroundColor(const sf::Color& color)
    {
        mBackgroundColor = color;
    }


    StateManager& Application::getStateManager()
    {
        return mStatemanager;
    }


    const script::SharedState& Application::getScriptState() const
    {
        return mScriptState;
    }


    const script::Environment& Application::getGlobalScriptEnv() const
    {
        return mGlobalScriptEnvironment;
    }


    void Application::createWindow()
    {
        //createWindow
        mVideoMode.width = mConfig.getOr<unsigned>("resolution/width", 800u);
        mVideoMode.height = mConfig.getOr<unsigned>("resolution/height", 600u);
        if (mConfig.getOr<bool>("resolution/fullscreen", false))
            mWindowStyle = sf::Style::Fullscreen;
        mVideoMode.bitsPerPixel = DEFAULT_VIDEO_BPP;
        std::string title = mConfig.getOr<std::string>("base/title", "UngodApp");
        mWindow.create(mVideoMode, title, mWindowStyle, mContextSettings);
        mVsync = mConfig.getOr<bool>("resolution/vsync", false);
        if (mVsync)
            mWindow.setVerticalSyncEnabled(true);
        else
            mWindow.setFramerateLimit(mConfig.getOr<unsigned>("resolution/framerate_limit", 120));
        mWindow.setIcon(icon.width, icon.height, icon.pixel_data);
        mWindow.clear(mBackgroundColor);
        mWindow.display();
        mCursorCamera.init(mWindow);
        if (mActiveCursor.getTexture())
            mWindow.setMouseCursorVisible(false);
    }


    void Application::createWindow(sf::WindowHandle handle)
    {
        mWindow.create(handle, mContextSettings);
        mWindow.setVerticalSyncEnabled(true);
        mWindow.setIcon(icon.width, icon.height, icon.pixel_data);
        mWindow.clear(sf::Color::Black);
        mWindow.display();
        mCursorCamera.init(mWindow);
        if (mActiveCursor.getTexture())
            mWindow.setMouseCursorVisible(false);
    }


    void Application::init(bool initStateFromConfig)
    {
        mRunning = true;

        resetScriptState();

        mConfig.onConfigurationChanged([this] (Configuration& config, const std::string& item)
           {
               if (item == "resolution/width" || item == "resolution/height" || item == "resolution/fullscreen" || item == "resolution/vsync" || item == "resolution/framerate_limit")
                    createWindow();
           });

        mDefaultFont.load(mConfig.getOr<std::string>("gui/default_font", "data/gui/fonts/arial.ttf"), LoadPolicy::SYNC);
        if (!mDefaultFont.isLoaded())
            Logger::warning("Could not load the default font. Disabled debug text info.");

        //setup initial state
        std::string type = mConfig.getOr<std::string>("initial_state/type", "");
        std::string scriptFile = mConfig.getOr<std::string>("initial_state/script_file", "init.lua");

        if (initStateFromConfig)
        {
            if (type == "GameState")
                mStatemanager.addState<ScriptedGameState>(0, scriptFile);
            else if (type == "MenuState")
                mStatemanager.addState<ScriptedMenuState>(0, scriptFile);
        }
    }


    void Application::mainloop()
    {
        while(mRunning && mStatemanager.hasState() && mWindow.isOpen())
        {
            processEvents();
            if (update())
                render();
        }
    }

    void Application::processEvents()
    {
        sf::Event event;
        while(mWindow.pollEvent(event))
        {
            mInputManager.handleEvent(event);
            mStatemanager.handleEvent(event);
            mCursorCamera.handleEvent(event);

            //do own Event evaluation
            switch(event.type)
            {
            case sf::Event::Closed:
            {
                quitApplication();
                break;
            }
            case sf::Event::Resized:
            {
                mTargetSizeChanged(sf::Vector2u(event.size.width, event.size.height));
                break;
            }
            case sf::Event::GainedFocus:
                break;
            case sf::Event::LostFocus:
                break;
            case sf::Event::MouseMoved:
                break;
            default:
                break;
            }
        }
    }

    bool Application::update()
    {
        bool updated = false;
        mAccumulator += mUpdateTimer.restart().asMicroseconds()/1000.0f;
        mUpdateCounter = 0;
        while (mAccumulator >= mDelta && ++mUpdateCounter <= mMaxUpdates)
        {
             mAssetmanager.update();
             mInputManager.update();
             mStatemanager.update(mDelta);
             mMusicManager.update(mDelta);
             mAccumulator -= mDelta;
             updated = true;
        }
        return updated;
    }

    void Application::render()
    {
        mWindow.clear(mBackgroundColor);
        mStatemanager.render(mWindow, sf::RenderStates());
        mCursorCamera.renderBegin();
        auto cursorPos = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow));
        mActiveCursor.setPosition(cursorPos);
        mWindow.draw(mActiveCursor);
        mCursorCamera.renderEnd();
        if (mDebugModeToggled && mDefaultFont.isLoaded())
        {
            float fps = 1.0f / mRenderTimer.restart().asSeconds();
            float smoothFPS = FPS_WINDOW_LAMBDA * fps + (1- FPS_WINDOW_LAMBDA) * std::accumulate(mLastFps.begin(), mLastFps.end(), 0) / mLastFps.size();
            mWindow.draw(sf::Text("FPS: " + std::to_string((int)std::floor(smoothFPS)), mDefaultFont.get(), DEBUG_TEXT_SIZE));
            mLastFps.push_back(fps);
            if (mLastFps.size() > FPS_AVG_WINDOW_SIZE)
                mLastFps.pop_front();
        }
        mWindow.display();
    }

    void Application::emitCustomEvent(const std::string& type, script::Environment data, float delay)
    {
        CustomEvent event = makeCustomEvent(mScriptState, type, data, delay);
        mStatemanager.onCustomEvent(event);
    }

    void Application::resetScriptState()
    {
        //setup script stuff
        mScriptState = std::make_shared<script::State>();
        mGlobalScriptEnvironment = mScriptState->create_named_table("ungod");

        ScriptStateBase scriptBase{ mScriptState, getGlobalScriptEnv() };

        //register functionality
        scriptRegistration::registerWorld(scriptBase);
        scriptRegistration::registerAssets(scriptBase);
        scriptRegistration::registerUtility(scriptBase);
        scriptRegistration::registerGui(scriptBase);
        scriptRegistration::registerInput(scriptBase);
        scriptRegistration::registerAudio(scriptBase);
        scriptRegistration::registerMenuState(scriptBase);
        scriptRegistration::registerGameState(scriptBase);
        scriptRegistration::registerApplication(scriptBase, *this);
        scriptRegistration::registerWater(scriptBase);
        scriptRegistration::registerTileMap(scriptBase);
        scriptRegistration::registerWorldGraph(scriptBase);
        scriptRegistration::registerEntity(scriptBase);
        scriptRegistration::registerSerialization(scriptBase);
        scriptRegistration::registerTransform(scriptBase);
        scriptRegistration::registerMovement(scriptBase);
        scriptRegistration::registerVisuals(scriptBase, *this);
        scriptRegistration::registerCollision<MOVEMENT_COLLISION_CONTEXT>(scriptBase);
        scriptRegistration::registerCollision<SEMANTICS_COLLISION_CONTEXT>(scriptBase);
        scriptRegistration::registerRigidbody(scriptBase);
        scriptRegistration::registerLight(scriptBase);
        scriptRegistration::registerBehavior(scriptBase);
        scriptRegistration::registerParticleSystem(scriptBase);
        scriptRegistration::registerParentChild(scriptBase);
        scriptRegistration::registerCutscene(scriptBase);
        scriptRegistration::registerCutsceneState(scriptBase);

        mScriptStateChanged();
    }

    owls::SignalLink<void, const sf::Vector2u&> Application::onTargetSizeChanged(const std::function<void(const sf::Vector2u&)>& callback)
    {
        return mTargetSizeChanged.connect(callback);
    }

    owls::SignalLink<void> Application::onScriptStateChanged(const std::function<void()>& callback)
    {
        return mScriptStateChanged.connect(callback);
    }
}
