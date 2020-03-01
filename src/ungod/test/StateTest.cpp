#include <boost/test/unit_test.hpp>
#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/base/World.h"
#include "ungod/test/mainTest.h"

class TestState : public ungod::State
{
public:
    TestState(ungod::Application& app, ungod::StateID id)
        : ungod::State(app, id), updateCounter(100), renderCounter(100) {}

    virtual void update(const float delta) override
    {
        if (updateCounter == 3)
            mApp->quitApplication();
        updateCounter++;
    }

    virtual void render(sf::RenderTarget& target, sf::RenderStates states) override
    {
        renderCounter++;
    }

protected:

    virtual void init() override
    {
        updateCounter = 0;
        renderCounter = 0;
    }

    virtual void close() override
    {

    }

public:
    std::size_t updateCounter;
    std::size_t renderCounter;
};

BOOST_AUTO_TEST_CASE( state_test )
{
    using namespace ungod;


    {
        EmbeddedTestApp::getApp().getStateManager().addInactiveState<TestState>(1);
        EmbeddedTestApp::getApp().getStateManager().addState<TestState>(2);
        EmbeddedTestApp::getApp().getStateManager().addState<TestState>(3);
        EmbeddedTestApp::getApp().getStateManager().addState<TestState>(4);

        EmbeddedTestApp::getApp().getStateManager().getState(3)->setTranscendency(false);
        EmbeddedTestApp::getApp().getStateManager().getState(3)->setTransparency(false);

        ErrorCode err = EmbeddedTestApp::getApp().runApplication();

        BOOST_CHECK(err == ungod::ErrorCode::QUIT_STATUS_OK);

        BOOST_CHECK(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(4)->updateCounter >= 3u);
        BOOST_CHECK(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(4)->renderCounter > 0u);
        BOOST_CHECK(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(3)->updateCounter >= 3u);
        BOOST_CHECK(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(3)->renderCounter > 0u);
        BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(2)->updateCounter, 0u);
        BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(2)->renderCounter, 0u);
        BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(1)->updateCounter, 100u);
        BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getStateManager().getState<TestState>(1)->renderCounter, 100u);

        EmbeddedTestApp::getApp().getStateManager().getState(1)->expire();
        EmbeddedTestApp::getApp().getStateManager().getState(2)->expire();
        EmbeddedTestApp::getApp().getStateManager().getState(3)->expire();
        EmbeddedTestApp::getApp().getStateManager().getState(4)->expire();

        EmbeddedTestApp::getApp().getStateManager().cleanup();
    }

    {
        EmbeddedTestApp::getApp().getStateManager().addState<ScriptedMenuState>(1, "test_data/menu_state_test.lua");
        ErrorCode err = EmbeddedTestApp::getApp().runApplication();

        BOOST_CHECK(err == ungod::ErrorCode::QUIT_STATUS_OK);
        BOOST_REQUIRE(EmbeddedTestApp::getApp().getStateManager().getState(1));

        auto check_init = EmbeddedTestApp::getApp().getStateManager().getState<ScriptedMenuState>(1)->getEnvironment().get<Optional<bool>>("check_init");

        BOOST_REQUIRE(check_init);
        BOOST_REQUIRE(*check_init);

        auto check_update = EmbeddedTestApp::getApp().getStateManager().getState<ScriptedMenuState>(1)->getEnvironment().get<Optional<bool>>("check_update");

        BOOST_REQUIRE(check_update);
        BOOST_REQUIRE(*check_update);

        BOOST_REQUIRE(EmbeddedTestApp::getApp().getStateManager().getState(2)); //successfully added a state with id 1

        EmbeddedTestApp::getApp().getStateManager().getState(1)->expire();
        EmbeddedTestApp::getApp().getStateManager().getState(2)->expire();

        EmbeddedTestApp::getApp().getStateManager().cleanup();
    }
    {
        EmbeddedTestApp::getApp().getStateManager().addState<ScriptedGameState>(1, "test_data/game_state_test.lua");
        ErrorCode err = EmbeddedTestApp::getApp().runApplication();

        BOOST_CHECK(err == ungod::ErrorCode::QUIT_STATUS_OK);

        BOOST_REQUIRE(EmbeddedTestApp::getApp().getStateManager().getState(1));

        auto check_init = EmbeddedTestApp::getApp().getStateManager().getState<ScriptedGameState>(1)->getEnvironment().get<Optional<bool>>("check_init");
        auto check_custom_event = EmbeddedTestApp::getApp().getStateManager().getState<ScriptedGameState>(1)->getEnvironment().get<Optional<bool>>("check_custom_event");
        auto check_custom_event_type = EmbeddedTestApp::getApp().getStateManager().getState<ScriptedGameState>(1)->getEnvironment().get<Optional<bool>>("check_custom_event_type");
        auto check_custom_event_value = EmbeddedTestApp::getApp().getStateManager().getState<ScriptedGameState>(1)->getEnvironment().get<Optional<bool>>("check_custom_event_value");

        BOOST_REQUIRE(check_init);
        BOOST_REQUIRE(*check_init);
        BOOST_REQUIRE(check_custom_event);
        BOOST_REQUIRE(*check_custom_event);
        BOOST_REQUIRE(check_custom_event_type);
        BOOST_REQUIRE(*check_custom_event_type);
        BOOST_REQUIRE(check_custom_event_value);
        BOOST_REQUIRE(*check_custom_event_value);

        EmbeddedTestApp::getApp().getStateManager().getState(1)->expire();

        EmbeddedTestApp::getApp().getStateManager().cleanup();
    }
}
