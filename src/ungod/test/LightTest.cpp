#include <boost/test/unit_test.hpp>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"

BOOST_AUTO_TEST_SUITE(LightTest)

//define a test state, that is runned in the application to perform world testings
/*class LightTestState : public ungod::State
{
public:
    LightTestState(ungod::Application& app) : ungod::State(app), mWorld()
    {
        mWorld.instantiate( app.getWindow().getSize());
        mWorld.initSpace(-1000, -1000, 2000, 2000);
    }

    virtual void handleEvent(const sf::Event& curEvent) override
    {
        mWorld.handleEvent(curEvent, mApp->getWindow());
    }

    virtual void update(const float delta) override
    {
        mWorld.update(delta, mApp->getWindow().mapPixelToCoords(sf::Vector2i(0,0)),
                          {(float)mApp->getWindow().getSize().x,
                           (float)mApp->getWindow().getSize().y});

        if (mTransition)
        {
            mWorld.getLightSystem().interpolateAmbientLight(sf::Color::Black, 5000.0f / delta);
            if (mTimer.getElapsedTime().asMilliseconds() >= 5000)
            {
                mTimer.restart();
                mTransition = false;
            }
        }
        else
        {
            mWorld.getLightSystem().interpolateAmbientLight(sf::Color(96,123,136), 5000.0f / delta);
            if (mTimer.getElapsedTime().asMilliseconds() >= 5000)
            {
                mTimer.restart();
                mTransition = true;
            }
        }
    }

    virtual void render(sf::RenderTarget& target, sf::RenderStates states) override
    {
        mWorld.render(target, states, false);
    }
protected:
    virtual void init() override
    {
        mWorld.getInputManager().addBinding("move_left", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Left);
        mWorld.getInputManager().addBinding("move_right", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Right);
        mWorld.getInputManager().addBinding("move_up", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Up);
        mWorld.getInputManager().addBinding("move_down", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Down);

        mEntity = mWorld.create(ungod::BaseComponents<ungod::MovementComponent, ungod::MultiLightEmitter, ungod::TransformComponent>(),
                                ungod::OptionalComponents<>(),
                                mWorld.getUniverse().instantiate<ungod::MovementComponent>(),
                                mWorld.getUniverse().instantiate< ungod::MultiLightEmitter >(2, mWorld.getUniverse()),
                                mWorld.getUniverse().instantiate< ungod::TransformComponent >());
        mWall = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::ShadowEmitterComponent>(), ungod::OptionalComponents<>());
        mWall2 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::ShadowEmitterComponent>(), ungod::OptionalComponents<>());
        mWall3 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::ShadowEmitterComponent>(), ungod::OptionalComponents<>());
        mWall4 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::ShadowEmitterComponent>(), ungod::OptionalComponents<>());
        mWall5 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::ShadowEmitterComponent>(), ungod::OptionalComponents<>());
        mLight1 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::LightEmitterComponent, ungod::LightAffectorComponent>(), ungod::OptionalComponents<>());
        mLight2 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::LightEmitterComponent, ungod::LightAffectorComponent>(), ungod::OptionalComponents<>());
        mLight3 = mWorld.create(ungod::BaseComponents<ungod::TransformComponent, ungod::LightEmitterComponent, ungod::LightAffectorComponent>(), ungod::OptionalComponents<>());

        mWorld.getLightSystem().setLightScale(mEntity, {2.0f, 2.0f}, 0);
        mWorld.getLightSystem().setLightScale(mEntity, {2.0f, 2.0f}, 1);
        mWorld.getLightSystem().setLocalLightPosition(mEntity, {50, 0}, 0);
        mWorld.getLightSystem().setLocalLightPosition(mEntity, {0, 50}, 0);
        mWorld.getLightSystem().setLightScale(mLight1, {9.0f, 9.0f});
        mWorld.getLightSystem().setLightScale(mLight2, {7.0f, 7.0f});
        mWorld.getLightSystem().setLightScale(mLight3, {11.0f, 11.0f});
        mWorld.getLightSystem().setPoints(mWall, { {0, 0}, {40,0}, {40,40}, {0, 40} });
        mWorld.getLightSystem().setPoints(mWall2, { {0, 0}, {40,0}, {40,40}, {0, 40} });
        mWorld.getLightSystem().setPoints(mWall3, { {0, 0}, {40,0}, {40,40}, {0, 40} });
        mWorld.getLightSystem().setPoints(mWall4, { {0, 0}, {40,0}, {40,40}, {0, 40} });
        mWorld.getLightSystem().setPoints(mWall5, { {0, 0}, {40,0}, {40,40}, {0, 40} });

        mWorld.getLightSystem().setAffectorCallback(mLight1, ungod::LightFlickering(800.0f, 0.2f));
        mWorld.getLightSystem().setAffectorCallback(mLight2, ungod::LightFlickering(500.0f, 0.15f));
        mWorld.getLightSystem().setAffectorCallback(mLight3, ungod::LightFlickering(1000.0f, 0.5f));

        mWorld.getTransformManager().setPosition(mEntity, {50,50});
        mWorld.getTransformManager().setPosition(mWall, {150,150});
        mWorld.getTransformManager().setPosition(mWall2, {50,50});
        mWorld.getTransformManager().setPosition(mWall3, {250,250});
        mWorld.getTransformManager().setPosition(mWall4, {350,350});
        mWorld.getTransformManager().setPosition(mWall5, {450,450});
        mWorld.getTransformManager().setPosition(mLight1, {50,100});
        mWorld.getTransformManager().setPosition(mLight2, {200,450});
        mWorld.getTransformManager().setPosition(mLight3, {650,350});

        mWorld.getQuadTree().insert(mEntity);
        mWorld.getQuadTree().insert(mWall);
        mWorld.getQuadTree().insert(mWall2);
        mWorld.getQuadTree().insert(mWall3);
        mWorld.getQuadTree().insert(mWall4);
        mWorld.getQuadTree().insert(mWall5);
        mWorld.getQuadTree().insert(mLight1);
        mWorld.getQuadTree().insert(mLight2);
        mWorld.getQuadTree().insert(mLight3);

        mWorld.getInputManager().onDown([this] (const std::string& key)
           {
                if (key == "move_up")
                {
                    mWorld.getMovementManager().directMovement(mEntity, {0,-1});
                }
                else if (key == "move_down")
                {
                    mWorld.getMovementManager().directMovement(mEntity, {0,1});
                }
                else if (key == "move_left")
                {
                    mWorld.getMovementManager().directMovement(mEntity, {-1,0});
                }
                else if (key == "move_right")
                {
                    mWorld.getMovementManager().directMovement(mEntity, {1,0});
                }
           });
    }

private:
    ungod::World mWorld;
    ungod::Entity mEntity;
    ungod::Entity mWall;
    ungod::Entity mWall2;
    ungod::Entity mWall3;
    ungod::Entity mWall4;
    ungod::Entity mWall5;
    ungod::Entity mLight1;
    ungod::Entity mLight2;
    ungod::Entity mLight3;
    bool mTransition;
    sf::Clock mTimer;
};*/

BOOST_AUTO_TEST_CASE( light_test )
{
    //ungod::Application app("light_test");
    //app.setBackgroundColor(sf::Color::White);
    //BOOST_CHECK_EQUAL(app.runApplication<LightTestState>(), ungod::QUIT_STATUS_OK);
}

BOOST_AUTO_TEST_SUITE_END()
