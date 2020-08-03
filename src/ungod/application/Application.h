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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include "owls/Signal.h"
#include "ungod/application/State.h"
#include "ungod/serialization/MetaData.h"
#include "ungod/ressource_management/AssetHandler.h"
#include "ungod/script/Script.h"
#include "ungod/application/ConfigFile.h"
#include "ungod/visual/Image.h"
#include "ungod/visual/Camera.h"
#include "ungod/audio/MusicPlayer.h"
#include "ungod/audio/Audio.h"
#include "ungod/base/Input.h"
#include "ungod/visual/Font.h"

namespace ungod
{

    enum class ErrorCode  : int   {QUIT_STATUS_OK = 0,
                             QUIT_STATUS_NO_ACTIVE_STATE_FOUND = 1,
                             ERROR_DATA_MISSING = 100,
                             COULD_NOT_CREATE_CONFIG_FILE = 101,
                             CORRUPTED_SAVE_FILE = 102,
                             INIT_FAILED = 103};

    /** \brief The main application class. Usually the only thing one has to deal with in the
    * main() method of the program. Runs the main loop and manages the states. Also resposible for
    * things a different cursors. */
    class Application
    {
    public:
        /** \brief Constructs a new application, but does no start it. The application title
        * will be also the window title. If wished, the user can deliver a herzt value for the
        * update rate of the game. This value describes the count of cycles per second. Default is 50.
        * The user can also specify the number of maximum updates performed during a single cycle of the mainloop. */
        Application(unsigned hertz = 50, unsigned maxUpdates = 3);

        /** \brief Initialises the application but does not run it. */
        void initApplication();

        /**
        * \brief Runs the mainloop of the engine. The application must be intialized before and there must be at least one state added.
        * \return Error code.
        */
        ErrorCode runApplication();

        /** \brief Returns true if the application is currently running. */
        bool isRunning() const;

        /** \brief Terminates the application with the given error code. */
        void quitApplication(ErrorCode error = ErrorCode::QUIT_STATUS_OK);

        /** \brief Loads a new cursor and tags it with a key. */
        void initCursor(const std::string& id, const std::string& path);

        /** \brief Activates a previously loaded cursor. */
        void setCursor(const std::string& id);

        /** \brief Returns the current build version: */
        static const std::string getBuildVersion() {return "Version: 0.01 - pre alpha build";}

        /** \brief Check if the debug mode is currentlyConfigGroup active. */
        bool debugModeActive();

        /** \brief Toggles the debug mode. */
        void toggleDebugMode();

        /** \brief Returns a reference to the underlying sfml window. */
        const sf::RenderWindow& getWindow() const;
        sf::RenderWindow& getWindow();

        /** \brief Accesses the config file. */
        Configuration& getConfig() {return mConfig;}
        const Configuration& getConfig() const {return mConfig;}

        /** \brief Loads a config file. If none exists yet, a default file is generated. */
        void loadConfig();

        /** \brief Defines the background color of the window. */
        void setBackgroundColor(const sf::Color& color);

        /** \brief Accesses the state manager. */
        StateManager& getStateManager();

        /** \brief Returns a handle to the underlying script state, so it can be shared along gamestates. */
        const script::SharedState& getScriptState() const;

        /** \brief Accesses the global script environment of the application that holds data shared between all gamestates. */
        const script::Environment& getGlobalScriptEnv() const;

        /** \brief Forwards a custom event to all transcendent states. */
        void emitCustomEvent(const std::string& type, script::Environment data, float delay = 0.0f);

        /** \brief Connects a callback to the on target size changed signal.*/
        owls::SignalLink<void, const sf::Vector2u&> onTargetSizeChanged(const std::function<void(const sf::Vector2u&)>& callback);

        /** \brief Connects a callback to the on target size changed signal.*/
        owls::SignalLink<void> onScriptStateChanged(const std::function<void()>& callback);

        /** \brief Restores the default value of a config property. */
        void restoreDefaultConfigProperty(const std::string& name);

        /** \brief Returns true if and only if fullscreen is enabled. */
        bool isFullscreen() const { return mWindowStyle == sf::Style::Fullscreen; }

        /** \brief Returns true if and only if vsync is enabled. */
        bool vsyncEnabled() const { return mVsync; }

        /** \brief Accesses the music manager to load and play music tracks or playlists. */
        MusicManager& getMusicManager() { return mMusicManager; }

        /** \brief Returns the audio manager. */
        SoundProfileManager& getSoundProfileManager() { return mSoundProfileManager; }

        /** \brief Returns a reference to the input manager. */
        InputManager& getInputManager() { return mInputManager; }

        /** \brief Clears the script state. */
        void resetScriptState();

        /** \brief Get a default font for debugging and simple text. */
        const Font& getDefaultFont() const { return mDefaultFont; }

    private:
        //const
        const float mDelta;
        const unsigned mMaxUpdates;
        const std::string mTitle;
        //management
        AssetManager mAssetmanager;
        StateManager mStatemanager;
        script::SharedState mScriptState;
        script::Environment mGlobalScriptEnvironment;
        //video
        sf::RenderWindow mWindow;
        sf::VideoMode mVideoMode;
        unsigned long mWindowStyle;
        sf::ContextSettings mContextSettings;
        bool mVsync;
        Font mDefaultFont;
        //audio
        MusicManager mMusicManager;
        SoundProfileManager mSoundProfileManager;
        //input
        InputManager mInputManager;
        //configuration
        Configuration mConfig;
        //behavior
        bool mIsInit;
        bool mRunning;
        ErrorCode mExitcode;
        bool mDebugModeToggled;
        float mUpdateCounter;
        float mAccumulator;
        sf::Clock mUpdateTimer;
        std::unordered_map<std::string, Image> mCursors;
        sf::Sprite mActiveCursor;
        sf::Color mBackgroundColor;
        Camera mCursorCamera;
        //signals
        owls::Signal<const sf::Vector2u&> mTargetSizeChanged;
        owls::Signal<> mScriptStateChanged;
        //statics
        static const std::string CONFIG_LOAD_PATH;
        static const std::string DEFAULT_CONFIG_XML;
        static const unsigned DEFAULT_VIDEO_WIDTH;
        static const unsigned DEFAULT_VIDEO_HEIGHT;
        static const unsigned DEFAULT_VIDEO_BPP;

    /**
    * These methods are accessable from derived classes, useable when the
    * application functionality must be invoked indirectly (see editor implementation)
    */
    protected:
        void init(bool initStateFromConfig = true);
        void createWindow();
        void createWindow(sf::WindowHandle handle);
        void mainloop();
        template<typename STATE, typename ... PARAM>
        STATE* addState(PARAM&& ... param);
        template<typename STATE, typename ... PARAM>
        STATE* addInactiveState(PARAM&& ... param);
        void processEvents();
        bool update();
        void render();
    };


    static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 pixel_data[32 * 32 * 4 + 1];
} icon = {
  32, 32, 4,
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\1\277\277\257\20\301\274\2622\300\271\263Q\274\267"
  "\262g\273\271\264f\275\266\262F\271\261\252!\266\266\266\7\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\226\231\233\2\366\347\334\40\327\320\310m\300\273\271"
  "\270\306\301\274\333\317\312\310\347\316\307\303\344\311\303\275\315\301"
  "\277\272\235\343\330\320N\356\360\321\20\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\1\325"
  "\314\302%\312\311\301\206\332\332\325\333\335\334\331\371\332\326\321\375"
  "\333\335\331\376\334\333\330\375\334\333\330\374\330\326\321\362\307\303"
  "\300\300onj\202\0\0\0_\0\0\0\25\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\351\20\265\257\257w\325\323\320\340"
  "\334\333\326\374\270\264\256\376\250\243\231\376\241\233\225\377\245\237"
  "\233\377\261\254\245\377\300\272\265\376\304\301\272\371\230\222\215\361"
  "\31\30\32\333\0\0\0K\0\0\0\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\363\366\3462\303\276\272\275\322\316\310\373\254\247"
  "\237\375\211\204}\376\203\201x\377yso\377{vl\377{vo\377\203}v\377\226\221"
  "\214\377\253\246\233\373c`[\337\10\11\6s\0\0\0\37\0\0\0\11\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\37\203}x\201\244\235\226\350\237"
  "\233\222\374rlf\375^VQ\377TSN\377PMK\377URO\377[XU\377VSQ\377ba]\377\201"
  "}v\376ic^\365\1\0\2\322\0\0\0\240\0\0\0Y\0\0\0\22\0\0\0\1\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\6\0\0\0\3\0\0\0\7"
  "\0\0\0\34\0\0\0\14\0\0\0Rea\\\264\207\206\177\360UPK\375=95\376<96\37776"
  "4\377666\377101\3772.0\377\37\37\37\377-+)\377MKE\377MIF\377\3\4\3\374\0"
  "\0\0\354\0\0\0\223\0\0\0-\0\0\0""4\0\0\0\33\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\7\0\0\0E\0\0\0L\0\0\0Q\0\0\0\214\0\0\0X\0\0\0\202"
  "862\273LEA\360*(%\376\37\36\37\376\35\36\40\377\17\20\20\377\16\16\16\377"
  "\20\21\21\377\7\7\7\377\0\0\0\377\24\25\25\377FGF\377976\377\1\1\0\376\0"
  "\0\0\365\0\0\0\263\0\0\0o\0\0\0\240\0\0\0K\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\5\0\0\0i\0\0\0\253\0\0\0\257\0\0\0\345\0\0\0\322"
  "\0\0\0\335\1\0\1\352\15\10\4\366\27\25\27\376\4\4\3\377\0\0\0\377\0\0\0\377"
  "\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\36\40\40\377YYY\377\"\"\"\377\0"
  "\0\0\376\0\0\0\375\0\0\0\350\0\0\0\326\0\0\0\313\0\0\0I\0\0\0\10\0\0\0\21"
  "\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\7\0\0\0V\0\0\0\317\0\0\0\302\0\0\0"
  "\332\0\0\0\373\0\0\0\375\0\0\0\376\0\0\0\375\0\0\0\376\0\0\0\377\5\4\3\377"
  "$$#\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\"!\"\377\0\0\0"
  "\377\0\0\0\376\0\0\0\375\0\0\0\361\0\0\0\365\0\0\0\341\0\0\0\240\0\0\0W\0"
  "\0\0u\0\0\0""6\0\0\0\1\0\0\0\0\0\0\0\12\0\0\0J\0\0\0\204\0\0\0\344\0\0\0"
  "\315\0\0\0\332\0\0\0\376\0\0\0\377\0\0\0\377\0\0\0\377A@>\377JGF\377+*(\377"
  "\200{x\377\0\0\0\377\0\0\0\377\36\35\36\377\0\0\0\377ec_\377\204\177|\377"
  "\12\11\11\377\0\0\0\376\0\0\0\376\0\0\0\374\0\0\0\375\0\0\0\361\0\0\0\344"
  "\0\0\0\213\0\0\0\247\0\0\0s\0\0\0\10\0\0\0\0\0\0\0\17\0\0\0\213\0\0\0\330"
  "\0\0\0\370\0\0\0\355\0\0\0\366\0\0\0\376\0\0\0\377\0\0\0\377\0\0\0\377a]"
  "Z\377\336\331\322\377c^\\\377]ZU\377\0\0\0\377\13\12\13\377\177yv\377B?>"
  "\377\245\235\232\377=:8\377\13\10\11\377\0\0\0\377\0\0\0\377\0\0\0\377\0"
  "\0\0\376\0\0\0\372\0\0\0\355\0\0\0\320\0\0\0\314\0\0\0\205\0\0\0\11\0\0\0"
  "\0\0\0\0\22\0\0\0\252\0\0\0\367\0\0\0\376\0\0\0\376\0\0\0\376\0\0\0\377\0"
  "\0\0\377\0\0\0\377\0\2\1\377.,+\377\257\251\243\377\232\224\220\377eb^\377"
  "\32\32\32\377\0\0\0\377\225\217\214\377\233\224\216\377\246\240\234\377p"
  "lh\377HFC\377\0\0\0\377\11\10\10\377\24\24\24\377\31\30\31\377!\35\33\373"
  "\25\25\22\354\0\0\0\360\0\0\0\345\0\0\0\221\0\0\0\35\0\0\0\0\0\0\0\32\0\0"
  "\0\275\0\0\0\3753/-\377>94\377$$#\377\26\27\27\377\0\0\0\377\0\0\0\377\6"
  "\5\5\377OLG\377,,*\377\200\177y\377yss\377NJH\3770..\377\"!\40\377\201}x"
  "\377\277\271\264\377\231\225\217\377\30\30\26\377\0\0\0\377\0\0\1\377\27"
  "\32\32\377234\377QOI\376sqi\366GBC\353\0\0\0\343\0\0\0\272\0\0\0a\0\0\0\7"
  "\0\0\0I\5\7\5\321][Y\376\206\201~\377URK\377021\377\16\20\20\377'&\"\377"
  "\"#\37\377}yt\377\277\270\261\377\10\5\5\377\202|y\377][X\377\35\35\33\377"
  "nig\377\0\0\0\377853\377\251\242\235\377VSP\377\0\0\0\377\35\34\33\377\35"
  "\33\32\377\25\24\24\377467\377NKF\377}{v\374\261\253\252\363HFA\360\0\0\0"
  "\336\0\0\0\244\0\0\0>\0\0\0\310TRP\370\272\264\260\376\202|x\377KHD\377:"
  "<=\377\12\11\13\377WRP\377\0\0\0\377rml\377\306\274\267\377\0\0\0\377\243"
  "\235\230\377}yt\377a\\Y\377\315\310\301\377&$\"\377\12\12\10\377upn\377>"
  ";;\377\0\0\0\377\30\26\27\377VRN\377+*)\377BDH\377NJF\377jf`\376\275\271"
  "\263\374\244\241\236\366\5\5\6\353\0\0\0\225\0\0\0U\0\2\1\352\242\236\230"
  "\375\310\304\277\376vpj\377LGD\377PQS\377)((\377\247\241\232\377*)&\377C"
  "B?\377\307\277\271\377\0\0\0\377\202{x\377\312\303\277\377\235\231\222\377"
  "0/-\377\0\0\0\377uqn\377\265\256\247\377\202~y\377tqn\377yur\377\265\260"
  "\252\377HFF\377KJM\377LHE\377lib\377\274\266\260\376\313\306\303\37642.\334"
  "\6\6\6N\3\3\3R\40\37\40\352\313\305\303\375\316\312\305\376xto\377JGC\377"
  "KMK\377!\"%\377\242\234\225\377\270\262\254\377xqp\377\312\304\275\377\13"
  "\13\12\377A>;\377\267\256\251\377\30\27\26\377\0\0\0\377tom\377\370\362\351"
  "\377\261\253\245\377\235\227\223\377rnk\377\265\257\253\377\276\270\261\377"
  "234\377>=:\377OLF\377uum\377\316\313\303\377\326\323\320\377ife\272\40\40"
  "\40\37\7\7\7E/..\350\324\321\313\375\342\340\331\376\213\212\202\377QNI\377"
  "DC8\377\26\27\32\377\0\0\0\377LKG\377hc`\377\346\340\331\377\211\206\177"
  "\377\27\26\25\377\222\216\210\377;99\377\17\17\17\377\251\242\236\377\255"
  "\251\243\377\0\0\0\377\3\5\5\377\26\26\24\377QNI\377&$%\377(+,\377><2\377"
  "RNE\377\212\205\201\377\343\336\327\377\321\313\305\375xvr\236NNN\15\0\0"
  "\0""1!\"\"\324\274\267\264\376\354\346\337\377\245\242\233\377XRQ\377F@8"
  "\377CCA\377\16\13\16\377\0\0\0\377\0\0\0\377CB=\377\223\217\213\377YVS\377"
  "uqm\377\340\330\320\377\265\257\251\377\231\224\220\377=<9\377\0\0\0\377"
  "\10\6\6\377\4\5\3\377\11\12\11\377.-/\377JKF\377GD7\377ZVT\377\254\250\245"
  "\376\351\345\337\376\263\256\253\371RPR\210\37\37\37\10\0\0\0&\5\5\4\304"
  "\210\206\204\375\351\346\340\376\313\307\304\377rqk\377IB8\377JH>\377==@"
  "\377&%$\377rkj\377|xu\377\300\272\263\377\327\314\306\377\201{w\377\320\307"
  "\300\377\331\321\313\377\177|v\377<99\377\\ZW\377\226\220\215\377\201~y\377"
  "USQ\377UUV\377862\377IB:\377\200|v\376\320\314\306\375\341\336\327\376rp"
  "k\366\17\14\15y\0\0\0\6\0\0\0\30\0\0\0\2610.-\374\277\274\266\375\336\332"
  "\325\377\240\234\221\377[VQ\37763,\377GGC\377\201\200~\377\271\262\254\377"
  "\204\201|\377EBB\377XTR\377\262\256\250\377\346\337\330\377\357\347\337\377"
  "\275\265\257\377xsq\377\177|w\377tqm\377xsp\377vvr\377YXR\377LG:\377rmf\376"
  "\264\255\243\375\333\326\315\371\217\215\211\375\22\21\24\351\0\0\0V\0\0"
  "\0\3\0\0\0\16\0\0\0\225\0\0\0\371FDB\374\262\256\253\377\305\300\267\377"
  "\230\224\216\377icY\377JF;\377!\"#\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0"
  "\0\377A=;\377\344\335\326\377\371\357\346\377VQO\376\0\0\0\376\2\2\1\377"
  "\11\10\7\377\2\0\0\376\"\37\40\377OLC\377ysj\377\243\235\230\376\257\253"
  "\246\370xvs\361\20\17\20\366\0\0\0\265\0\0\0\"\0\0\0\0\0\0\0\1\0\0\0A\0\0"
  "\0\265\0\0\0\350\"!\37\373iee\376\211\204\177\377urj\377JE?\377\24\22\22"
  "\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\30\30\27\377\340\327\320\377"
  "\371\357\350\377($#\376\0\0\0\376\0\0\0\377\0\0\0\376\0\0\0\375\0\0\0\376"
  "\16\16\14\377**'\377-++\376\16\15\14\374\0\0\0\362\0\0\0\322\0\0\0v\0\0\0"
  "\16\0\0\0\0\0\0\0\0\0\0\0\4\0\0\0H\0\0\0\227\0\0\0\327\0\0\0\372\0\0\0\377"
  "\2\5\4\377\0\0\0\377\0\0\0\377\6\7\7\377\7\7\6\377\0\0\1\377\0\0\0\377'$"
  "#\377\350\337\330\377\377\365\357\377PMJ\376\0\0\0\376\0\0\0\377\0\0\0\377"
  "\11\12\10\376\0\0\0\376\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\372\0\0\0\326"
  "\0\0\0\243\0\0\0C\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\32\0\0\0D\0\0\0\200"
  "\0\0\0\312\0\0\0\362\0\0\0\376\0\0\0\377\0\0\0\377QML\377,+)\377\12\10\11"
  "\377\0\0\0\377YVT\377\362\350\342\377\371\360\347\377\206\201}\377\13\11"
  "\11\377\7\6\10\377'%$\377db]\377\26\25\23\377\3\3\2\377\0\0\0\374\0\0\0\361"
  "\0\0\0\325\0\0\0\255\0\0\0u\0\0\0\30\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\3\0\0\0\7\0\0\0,\0\0\0\205\0\0\0\302\7\7\7\362YVS\377>>=\377\215\210\204"
  "\377%#!\377\31\30\27\377kgc\377\276\267\262\377\352\343\332\377\353\341\333"
  "\377\322\311\306\377}yu\377+*)\377%\"\"\377\242\234\227\377SQN\377lge\375"
  "\33\32\31\342\0\0\0\261\0\0\0\226\0\0\0\201\0\0\0""7\0\0\0\1\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\3\0\0\0>\0\0\0\216\11\11\11\315"
  "IHD\370\34\33\32\377\217\212\205\377sol\377\233\225\220\377\363\352\343\377"
  "\371\360\351\377\365\356\346\377\360\351\342\377\372\357\350\377\363\353"
  "\342\377\260\250\244\377tol\377\261\254\246\377;96\377TRO\374\35\34\30\323"
  "\0\0\0\206\0\0\0]\0\0\0/\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\6\0\0\0-\0\0\0n\0\0\0\265\0\0\0\334'&\"\352"
  "643\364A<;\366VPN\371tpm\373USP\371OJJ\364yro\361USO\362LKG\360EA>\357JF"
  "D\352\2\1\0\345\0\0\0\324\0\0\0\233\0\0\0A\0\0\0\24\0\0\0\1\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\2\0\0\0!\0\0\0@\0\0\0]\0\0\0\202\0\0\0\226\0\0\0\244\0\0\0\255\0\0\0\265"
  "\0\0\0\254\0\0\0\234\0\0\0\232\0\0\0\241\0\0\0\232\0\0\0\213\1\2\3k\0\0\0"
  "P\0\0\0""4\0\0\0\24\0\0\0\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\5\0\0\0"
  "\17\0\0\0\21\0\0\0$\0\0\0-\0\0\0""3\0\0\0""7\0\0\0""5\0\0\0""1\0\0\0""4\0"
  "\0\0""6\0\0\0""3\0\0\0$\0\0\0\25\0\0\0\7\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\2"
  "\0\0\0\3\0\0\0\5\0\0\0\3\0\0\0\2\0\0\0\2\0\0\0\2\0\0\0\1\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0",
};
}

#endif // APPLICATION_H
