#include <boost/test/unit_test.hpp>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/content/TileMap.h"
#include "ungod/serialization/SerialGraph.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/test/mainTest.h"

BOOST_AUTO_TEST_SUITE(SerializationTest)

BOOST_AUTO_TEST_CASE( serializsation_test )
{
   {
        sf::RenderWindow window;
        ungod::TileMap tilemap;
        tilemap.loadTiles("tilemap_tiles.png", "tilemap_tiles.xml", 128, 128, { "planks", "stones", "dirt", "grass" });
        tilemap.setTiles({ 0,1,2,3,0,0,
                          0,1,2,3,0,0,
                          0,1,2,3,0,0,
                          0,1,2,3,0,0,
                          0,1,2,3,0,0 },
                        { 0,1,2,3,0,0,
                          0,1,2,3,0,0,
                          0,1,2,3,0,0,
                          0,1,2,3,0,0,
                          0,1,2,3,0,0 }, 5, 6);
        ungod::SerializationContext context;
        context.serializeRootObject(tilemap);
        context.save("test_output/tilemap_sav.xml");
    }
    //tilemap is deleted...
    {
        sf::RenderWindow window;
        ungod::TileMap tilemap; //we want to restore it now
        ungod::DeserializationContext context;

        context.read("test_output/tilemap_sav.xml");
        context.deserializeRootObject(tilemap);

        BOOST_CHECK_EQUAL(tilemap.getTileWidth(), 128u);
        BOOST_CHECK_EQUAL(tilemap.getMapSizeX(), 5u);
        BOOST_REQUIRE(tilemap.getTiledata(2,2));
        BOOST_CHECK_EQUAL(tilemap.getTiledata(2,2)->getTileID(), 2);
        BOOST_CHECK_EQUAL(tilemap.getTiledata(2,2)->getMaterialID(), 2u);
    }

    //serialize world
    {
        sf::RenderWindow window;
        ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
        ungod::World world(&state);
        world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
        world.initSpace(0,0,800,600);

        ungod::Entity e = world.create(ungod::BaseComponents<ungod::SoundEmitterComponent, ungod::TransformComponent>(), ungod::OptionalComponents<>());
        ungod::Entity e2 = world.create(ungod::BaseComponents<ungod::VisualsComponent, ungod::TransformComponent>(),
                                        ungod::OptionalComponents<ungod::RigidbodyComponent<>>());

        e2.add<ungod::RigidbodyComponent<>>();

        auto profile = world.getAudioManager().initSoundProfile("test");
        world.getAudioManager().initSounds(profile, 1);
        world.getAudioManager().loadSound(profile, "test_data/sound.wav", 0);
        world.getAudioManager().connectProfile(e, profile);

        world.getTransformManager().setPosition(e, {10.0f, 10.0f});
        world.getTransformManager().setPosition(e2, {100.0f, 100.0f});
        world.getVisualsManager().loadTexture(e2, "test_data/test.png");

        world.getRigidbodyManager().addCollider(e2, {0,0}, {10,10}, 0);

        world.getQuadTree().insert(e);
        world.getQuadTree().insert(e2);

        world.tagWithName(e, "dog");
        world.tagWithName(e2, "cat");

        ungod::SerializationContext context;
        context.serializeRootObject(world, static_cast<const sf::RenderTarget&>(window));
        context.save("test_output/world_sav.xml");

		world.destroy(e); //queue entity for 
		world.destroy(e2); //queue entity for destruction
		world.update(20.0f, {}, {}); //destroys entity in queue
    }
    {
        sf::RenderWindow window;
        ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
        ungod::World world(&state);

        typedef ungod::BaseComponents<ungod::SoundEmitterComponent, ungod::TransformComponent> Base1;
        typedef ungod::OptionalComponents<> Opt1;
        typedef ungod::BaseComponents<ungod::VisualsComponent, ungod::TransformComponent> Base2;
        typedef ungod::OptionalComponents<ungod::RigidbodyComponent<>> Opt2;

        world.registerInstantiation(Base1(), Opt1());
        world.registerInstantiation(Base2(), Opt2());

        ungod::DeserializationContext context;
        context.changeStorageSemantics<ungod::deserial_ref_semantics::ByValue<ungod::Entity>>(
                        ungod::SerialIdentifier< ungod::EntityInstantiation< Base1, Opt1 > >::get());
        context.changeStorageSemantics<ungod::deserial_ref_semantics::ByValue<ungod::Entity>>(
                        ungod::SerialIdentifier< ungod::EntityInstantiation< Base2, Opt2 > >::get());
        context.read("test_output/world_sav.xml");

        context.deserializeRootObject(world, static_cast<const sf::RenderTarget&>(window));

		std::this_thread::sleep_for(std::chrono::milliseconds(500)); //make sure image data can be loaded async in time

        BOOST_CHECK_EQUAL(2u, world.getQuadTree().size());

        quad::PullResult<ungod::Entity> pull;
        world.getQuadTree().getContent(pull);
        for (const auto& e : pull.getList())
        {
            if ( e.getInstantiation()->getSerialIdentifier() ==
                 ungod::SerialIdentifier< ungod::EntityInstantiation< ungod::BaseComponents<ungod::SoundEmitterComponent, ungod::TransformComponent>, ungod::OptionalComponents<> > >::get())
            {
                BOOST_CHECK_EQUAL(10.0f, e.get<ungod::TransformComponent>().getPosition().x);
            }
            else if ( e.getInstantiation()->getSerialIdentifier() ==
                 ungod::SerialIdentifier< ungod::EntityInstantiation< ungod::BaseComponents<ungod::VisualsComponent, ungod::TransformComponent>, ungod::OptionalComponents<ungod::RigidbodyComponent<>> > >::get())
             {
                 BOOST_CHECK(e.has<ungod::RigidbodyComponent<>>());
                 BOOST_CHECK_EQUAL(10.0f, e.get<ungod::RigidbodyComponent<>>().getColliders()[0].getDownright().y);
                 BOOST_CHECK(e.get<ungod::VisualsComponent>().isLoaded());
                 BOOST_CHECK_EQUAL(100.0f, e.get<ungod::TransformComponent>().getPosition().y);
             }
        }

        BOOST_CHECK(world.getEntityByName("dog"));
        BOOST_CHECK(world.getEntityByName("cat"));
        BOOST_CHECK_EQUAL(world.getEntityByName("dog").get<ungod::TransformComponent>().getPosition().x, 10.0f);
        BOOST_CHECK_EQUAL(world.getEntityByName("cat").get<ungod::TransformComponent>().getPosition().x, 100.0f);

		world.destroyNamed(world.getEntityByName("dog")); //queue entity for 
		world.destroyNamed(world.getEntityByName("cat")); //queue entity for destruction
		world.update(20.0f, {}, {}); //destroys entity in queue
    }
    {
        ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);

        ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "test_node", "test_output/renderlayers_sav.xml");

		ungod::World* world1 = node.addWorld();
		world1->initSpace(0, 0, 800, 600);

		ungod::World* world2 = node.addWorld();
		world2->initSpace(800, 600, 800, 600);

        BOOST_CHECK(!node.isLoaded());
        BOOST_CHECK(state.getWorldGraph().updateReferencePosition({1000.0f,1000.0f}));
        BOOST_CHECK(node.isLoaded());

        //state.save("test_output/renderlayers_state_sav.xml", true);
    }
    {
        ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
        state.load("test_output/renderlayers_state_sav.xml");

		BOOST_CHECK(state.getWorldGraph().updateReferencePosition({ 1000.0f,1000.0f }));
		ungod::WorldGraphNode* node = state.getWorldGraph().getNode(sf::Vector2f{ 1000.0f,1000.0f });
		BOOST_REQUIRE(node);
        BOOST_CHECK(node->isLoaded());

        ungod::World* world = node->getWorld(0);
        BOOST_REQUIRE(world);
        ungod::World* world2 = node->getWorld(1);
        BOOST_REQUIRE(world2);

        BOOST_CHECK_EQUAL(800, world->getBounds().width);
		BOOST_CHECK_EQUAL(0, world->getBounds().left);
        BOOST_CHECK_EQUAL(800, world2->getBounds().width);
		BOOST_CHECK_EQUAL(600, world2->getBounds().top);
    }
}

BOOST_AUTO_TEST_CASE( polymorphic_test )
{

}

BOOST_AUTO_TEST_CASE( serial_graph_test )
{
    {
        //(de)serialize this graph
        /*
        *   a --2-- b --1-- c ---2-- d -1-- e
        *     \      \          ____/
        *    2 \    2 \   __9__/
        *       f -1-- g /
        */

        std::vector<ungod::graph::EdgeInstantiator> edges = { {0,1}, {1,2}, {2,3}, {3,4}, {0,5}, {1,6}, {5,6}, {6,3} };

        ungod::graph::UndirectedAdjacencyLists graph {7, edges.begin(), edges.end()};

        ungod::SerializationContext context;
        context.serializeRootObject(graph);
        context.save("test_output/graph_sav.xml");
    }

    {
        ungod::DeserializationContext context;
        context.read("test_output/graph_sav.xml");

        ungod::graph::UndirectedAdjacencyLists graph {};

        context.deserializeRootObject(graph);

        BOOST_CHECK_EQUAL(7u, graph.getVertexCount());
        BOOST_CHECK_EQUAL(8u, graph.getEdgeCount());


        for (const auto& e : graph.getNeighbors(0))
            BOOST_CHECK(e.destination == 1 || e.destination == 5);

        for (const auto& e : graph.getNeighbors(2))
            BOOST_CHECK(e.destination == 1 || e.destination == 3);

        for (const auto& e : graph.getNeighbors(4))
            BOOST_CHECK(e.destination == 3);
    }
}


BOOST_AUTO_TEST_SUITE_END()
