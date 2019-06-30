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
#include <boost/filesystem.hpp>

namespace ungod
{
    const std::string Application::CONFIG_LOAD_PATH = "databases/config.xml";
    const std::string Application::DEFAULT_CONFIG_XML = "<meta>"\
                                                       "<resolution width=\"800\" height=\"600\" fullscreen=\"0\"/>"\
                                                       "<graphics_settings image_quality=\"0\"/>"\
                                                       "<light unshadow_vertex_shader=\"resource/unshadowShader.vert\" unshadow_frag_shader=\"resource/unshadowShader.frag\"\
                                                       light_vertex_shader=\"resource/lightOverShapeShader.vert\" light_frag_shader=\"resource/lightOverShapeShader.frag\"\
                                                       default_penumbra_texture=\"resource/penumbraTexture.png\"/>"\
                                                       "</meta>";
    const unsigned Application::DEFAULT_VIDEO_WIDTH = 800;
    const unsigned Application::DEFAULT_VIDEO_HEIGHT = 600;
    const unsigned Application::DEFAULT_VIDEO_BPP = 32;

    Application::Application(const std::string title, unsigned hertz, unsigned maxUpdates) :
        mDelta(1000.0f/hertz),
        mMaxUpdates(maxUpdates),
        mTitle(title),
        mAssetmanager(),
        mStatemanager(*this),
        mScriptState(),
        mWindow(),
        mVideoMode(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT, DEFAULT_VIDEO_BPP),
        mWindowStyle(sf::Style::Close | sf::Style::Resize),
        mContextSettings(),
        mConfig(),
        mIsInit(false),
        mRunning(false),
        mExitcode(QUIT_STATUS_OK),
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

        Logger::info("Now initializing application ");
        Logger::info(mTitle);
        Logger::info("...");
        Logger::endl();

        mIsInit = true;
        loadConfig();
        createWindow();
        init();
    }

    ErrorCode Application::runApplication()
    {
        Logger::info("Now running application ");
        Logger::info(mTitle);
        Logger::info("...");
        Logger::endl();
        mRunning = true;
        mainloop();
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


    void Application::initCursor(const std::string& id, sf::Texture& texture)
    {
        mCursors.emplace(id, &texture);
    }


    void Application::setCursor(const std::string& id)
    {
        auto result = mCursors.find(id);
        if (result != mCursors.end())
        {
            mActiveCursor.setTexture(*result->second, true);
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


    const MetaMap& Application::getConfig() const
    {
        return mConfig;
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
            else quitApplication(COULD_NOT_CREATE_CONFIG_FILE);
        }
    }


    void Application::createWindow()
    {
        //createWindow
        auto resolutionConfig = mConfig.getNodeWithKey("resolution");
        auto values = resolutionConfig.getAttributes<unsigned, unsigned, bool, bool, unsigned>(
                    {"width", DEFAULT_VIDEO_WIDTH}, {"height", DEFAULT_VIDEO_HEIGHT}, {"fullscreen", false}, {"vsync", false}, {"framerate_limit", 120});
        mVideoMode.width = std::get<0>(values);
        mVideoMode.height = std::get<1>(values);
        if (std::get<2>(values))
            mWindowStyle = sf::Style::Fullscreen;
        mVideoMode.bitsPerPixel = DEFAULT_VIDEO_BPP;
        mWindow.create(mVideoMode, mTitle, mWindowStyle, mContextSettings);
        if (std::get<3>(values))
            mWindow.setVerticalSyncEnabled(true);
        else
            mWindow.setFramerateLimit(std::get<4>(values));
        mWindow.setIcon(icon.width, icon.height, icon.pixel_data);
        mWindow.clear(mBackgroundColor);
        mWindow.display();
    }


    void Application::createWindow(sf::WindowHandle handle)
    {
        mWindow.create(handle, mContextSettings);
        mWindow.setVerticalSyncEnabled(true);
        mWindow.setIcon(icon.width, icon.height, icon.pixel_data);
        mWindow.clear(sf::Color::Black);
        mWindow.display();
    }


    void Application::init()
    {
        mRunning = true;

        //setup script stuff
        mScriptState = std::make_shared<script::State>();
        mGlobalScriptEnvironment = mScriptState->create_named_table("ungod");

        //load images in normal or low quality?
        auto graphicsConfig = mConfig.getNodeWithKey("graphics_settings");
        if (graphicsConfig)
        {
            /*Image::quality = (ImageQuality)ungod::getAttribute<std::underlying_type<ImageQuality>::type>(
                                        "image_quality", graphics_config, IMAGE_QUALITY_NORMAL);*/
        }
    }


    void Application::mainloop()
    {
        while(mRunning && mStatemanager.hasState() && mWindow.isOpen())
        {
            processEvents();
            update();
            render();
        }
    }

    void Application::processEvents()
    {
        sf::Event event;
        while(mWindow.pollEvent(event))
        {
            mStatemanager.handleEvent(event);

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
            {
                mActiveCursor.setPosition(event.mouseMove.x, event.mouseMove.y);
                break;
            }
            default:
                break;
            }
        }
    }

    void Application::update()
    {
        mAccumulator += mUpdateTimer.restart().asMicroseconds()/1000.0f;
        mUpdateCounter = 0;
        while (mAccumulator >= mDelta && ++mUpdateCounter <= mMaxUpdates)
        {
             mAssetmanager.update();
             mStatemanager.update(mDelta);
             mAccumulator -= mDelta;
        }
    }

    void Application::render()
    {
        mWindow.clear(mBackgroundColor);
        mStatemanager.render(mWindow, sf::RenderStates());
        mWindow.draw(mActiveCursor);
        mWindow.display();
    }

    void Application::emitCustomEvent(const std::string& type, script::Environment data)
    {
        CustomEvent event = makeCustomEvent(mScriptState, type, data);
        mStatemanager.onCustomEvent(event);
    }

    void Application::onTargetSizeChanged(const std::function<void(const sf::Vector2u&)>& callback)
    {
        mTargetSizeChanged.connect(callback);
    }
}
