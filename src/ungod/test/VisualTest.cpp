#include <boost/test/unit_test.hpp>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/content/tilemap/TileMap.h"
#include "ungod/test/mainTest.h"

BOOST_AUTO_TEST_SUITE(VisualTest)

BOOST_AUTO_TEST_CASE( animation_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
	node.setSize({ 3000,3000 });
	ungod::World* world = node.addWorld();
    ungod::Entity e = world->create(ungod::BaseComponents< ungod::TransformComponent, ungod::VisualsComponent, ungod::SpriteComponent, ungod::SpriteMetadataComponent, ungod::AnimationComponent >(),
                                   ungod::OptionalComponents<>());

    std::list<ungod::Entity> entities;   //need a list of entities to call visualmanager.update(...)
    entities.push_back(e);

    bool normalInitCheck = false;
    bool normalEndCheck = false;
    bool loopedInitCheck = false;
    bool otoEndCheck = false;

    world->getVisualsHandler().onAnimationStart([&] (ungod::Entity e, const std::string& key)
                                               {
                                                 if (key == "normal")
                                                 {
                                                     normalInitCheck = true;
                                                 }
                                                 else if (key == "looped")
                                                 {
                                                     loopedInitCheck = true;
                                                 }
                                               });

    world->getVisualsHandler().onAnimationStop([&] (ungod::Entity e, const std::string& key)
                                               {
                                                 if (key == "normal")
                                                 {
                                                     normalEndCheck = true;
                                                 }
                                                 else if (key == "oneTimeOnly")
                                                 {
                                                    otoEndCheck = true;
                                                 }
                                               });

    world->getVisualsHandler().loadTexture(e, "test_data/test.png");
    world->getVisualsHandler().loadMetadata(e, "test_data/animation_test.xml");
    world->getVisualsHandler().bindSpriteToAnimation(e);

    BOOST_CHECK(!e.get<ungod::AnimationComponent>().getAnimation().isRunning());

    world->getVisualsHandler().setAnimationState(e, "normal");

    BOOST_CHECK(e.get<ungod::AnimationComponent>().getAnimation().isRunning());
    BOOST_CHECK(normalInitCheck);
    BOOST_CHECK_EQUAL(e.get<ungod::AnimationComponent>().getAnimation().getCurrentIndex(), 0u);

    state.getRenderer().update(entities, 21, world->getVisualsHandler());

    BOOST_CHECK_EQUAL(e.get<ungod::AnimationComponent>().getAnimation().getCurrentIndex(), 1u);

    world->getVisualsHandler().setAnimationState(e, "looped");

    BOOST_CHECK(normalEndCheck);
    BOOST_CHECK(loopedInitCheck);

    state.getRenderer().update(entities, 21, world->getVisualsHandler());
    state.getRenderer().update(entities, 21, world->getVisualsHandler());

    BOOST_CHECK_EQUAL(e.get<ungod::AnimationComponent>().getAnimation().getCurrentIndex(), 2u);

    state.getRenderer().update(entities, 21, world->getVisualsHandler());

    BOOST_CHECK_EQUAL(e.get<ungod::AnimationComponent>().getAnimation().getCurrentIndex(), 1u); //check looping

    world->getVisualsHandler().setAnimationState(e, "oneTimeOnly");

    state.getRenderer().update(entities, 21, world->getVisualsHandler());
    state.getRenderer().update(entities, 21, world->getVisualsHandler());

    BOOST_CHECK(!e.get<ungod::AnimationComponent>().getAnimation().isRunning());

    world->getVisualsHandler().setAnimationState(e, "loopedOneTimeOnly");

    BOOST_CHECK(e.get<ungod::AnimationComponent>().getAnimation().isRunning());

    state.getRenderer().update(entities, 21, world->getVisualsHandler());
    state.getRenderer().update(entities, 21, world->getVisualsHandler());

    BOOST_CHECK(e.get<ungod::AnimationComponent>().getAnimation().isRunning());

    state.getRenderer().update(entities, 21, world->getVisualsHandler());

    BOOST_CHECK(!e.get<ungod::AnimationComponent>().getAnimation().isRunning());

	world->destroy(e); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( multivisual_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();
    ungod::Entity e = world->create(ungod::BaseComponents< ungod::VisualsComponent, ungod::MultiSpriteComponent >(),
                                   ungod::OptionalComponents<>());

    e.initMulti<ungod::MultiSpriteComponent>(6);
    BOOST_REQUIRE( e.get<ungod::MultiSpriteComponent>().getComponentCount() == 6u );
    BOOST_CHECK_EQUAL( e.get<ungod::MultiSpriteComponent>().getComponent(0).getSprite().getScale().x, 1.0f );
    BOOST_CHECK_EQUAL( e.get<ungod::MultiSpriteComponent>().getComponent(1).getSprite().getRotation(), 0.0f );
    BOOST_CHECK_EQUAL( e.get<ungod::MultiSpriteComponent>().getComponent(2).getSprite().getOrigin().x, 0.0f );

	world->destroy(e); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( render_order_test )
{
    //set up window and world
    sf::RenderWindow window(sf::VideoMode(800, 600), "render order test");
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();

    //create entities in the world and set their baselines
    ungod::Entity e[6];
    for (int i = 0; i < 6; i++)
    {
        e[i] = world->create(ungod::BaseComponents< ungod::VisualsComponent, ungod::TransformComponent, ungod::RigidbodyComponent<> >(),
                                        ungod::OptionalComponents<>());
        world->getQuadTree().insert(e[i]);
    }

    BOOST_REQUIRE_EQUAL(world->getQuadTree().size(), 6u);

    world->getTransformHandler().setPosition(e[0], sf::Vector2f{ 10, 10 });
    world->getMovementRigidbodyHandler().addCollider(e[0], 
			ungod::makeRotatedRect(sf::Vector2f{0,0}, sf::Vector2f{30,50}));  //giving a collider just to set the size of the entity to 30x50 pixel
    world->getTransformHandler().setPosition(e[1], sf::Vector2f{ 40, 20 });
    world->getMovementRigidbodyHandler().addCollider(e[1], 
			ungod::makeRotatedRect(sf::Vector2f{0,0}, sf::Vector2f{30,50}));

    world->getTransformHandler().setPosition(e[2], sf::Vector2f{ 120, 150 });
    world->getMovementRigidbodyHandler().addCollider(e[2], 
			ungod::makeRotatedRect(sf::Vector2f{0,0}, sf::Vector2f{50,70}));
    world->getTransformHandler().setBaseLineOffsets(e[2], sf::Vector2f{ 0, 70 });
    world->getTransformHandler().setPosition(e[3], sf::Vector2f{ 145, 195 });
    world->getMovementRigidbodyHandler().addCollider(e[3], 
			ungod::makeRotatedRect(sf::Vector2f{0,0}, sf::Vector2f{20,20}));

    world->getTransformHandler().setPosition(e[4], sf::Vector2f{ 320, 350 });
    world->getMovementRigidbodyHandler().addCollider(e[4], 
			ungod::makeRotatedRect(sf::Vector2f{0,0}, sf::Vector2f{50,70}));
    world->getTransformHandler().setBaseLineOffsets(e[4], sf::Vector2f{ 70, 0 });
    world->getTransformHandler().setPosition(e[5], sf::Vector2f{ 330, 370 });
    world->getMovementRigidbodyHandler().addCollider(e[5], 
			ungod::makeRotatedRect(sf::Vector2f{0,0}, sf::Vector2f{20,20}));

    world->render(window, sf::RenderStates{});

    auto iter = world->getEntitiesInRenderRange().getList().begin();
    BOOST_REQUIRE_EQUAL(world->getEntitiesInRenderRange().getList().size(), 6u);
    //BOOST_CHECK(e[0] == *(iter));
    //std::advance(iter, 1);
    //BOOST_CHECK(e[1] == *(iter));
    //std::advance(iter, 1);
    //BOOST_CHECK(e[2] == *(iter));
    //std::advance(iter, 1);
    //BOOST_CHECK(e[3] == *(iter));
    //std::advance(iter, 1);
    //BOOST_CHECK(e[5] == *(iter));
    //std::advance(iter, 1);
    //BOOST_CHECK(e[4] == *(iter));

	for (int i = 0; i < 6; i++)
		world->destroy(e[i]); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_SUITE_END()
