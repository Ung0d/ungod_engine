#include <boost/test/unit_test.hpp>
#include <chrono>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/utility/Graph.h"
#include "ungod/utility/DelaunayTriangulation.h"
#include "ungod/utility/Vec2fTraits.h"
#include "ungod/utility/DisjointSets.h"
#include "ungod/test/mainTest.h"

BOOST_AUTO_TEST_SUITE(BaseTest)

BOOST_AUTO_TEST_CASE( asset_image_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate(EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,800,600);
    ungod::Entity e = world.create(ungod::BaseComponents<ungod::TransformComponent, ungod::VisualsComponent>(), ungod::OptionalComponents<>());
    world.getVisualsManager().loadTexture(e, "test_data/test_sheet.png", ungod::ASYNC);
}

BOOST_AUTO_TEST_CASE( transform_system_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,800,600);
    ungod::Entity e1 = world.create(ungod::BaseComponents<ungod::TransformComponent>(), ungod::OptionalComponents<>());
    ungod::Entity e2 = world.create(ungod::BaseComponents<ungod::TransformComponent>(), ungod::OptionalComponents<>());
    world.getTransformManager().setPosition(e1, {100,0});
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().x, 100);
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().y, 0);
    world.getTransformManager().move(e1, {10,10});
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().x, 110);
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().y, 10);

    //test position changed signal
    world.getTransformManager().onPositionChanged([&world, e1, e2] (ungod::Entity e, const sf::Vector2f& position)
                                                  {
                                                    //automatically move e2 when e1 is moved
                                                    if (e == e1)
                                                    {
                                                        world.getTransformManager().setPosition(e2, position);
                                                    }
                                                  });
    world.getTransformManager().setPosition(e2, {50,50});
    world.getTransformManager().move(e1, {10,10});
    BOOST_CHECK_EQUAL(e2.get<ungod::TransformComponent>().getPosition().x, 120);
    BOOST_CHECK_EQUAL(e2.get<ungod::TransformComponent>().getPosition().y, 20);
}

BOOST_AUTO_TEST_CASE( movement_system_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,800,600);
    ungod::Entity e1 = world.create(ungod::BaseComponents<ungod::TransformComponent, ungod::MovementComponent>(), ungod::OptionalComponents<>());
    world.getTransformManager().setPosition(e1, {0,0});
    world.getMovementManager().accelerate(e1, {1,1});
    world.getMovementManager().update({e1}, 20);
    BOOST_CHECK(  e1.get<ungod::TransformComponent>().getPosition().x > 0);
}

BOOST_AUTO_TEST_CASE( render_system_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,800,600);
    ungod::Entity e = world.create(ungod::BaseComponents<ungod::TransformComponent, ungod::VisualsComponent>(), ungod::OptionalComponents<>());
    world.getVisualsManager().loadTexture(e, "test_data/test.png", ungod::SYNC);
    BOOST_REQUIRE( e.get<ungod::VisualsComponent>().isLoaded() );
}

BOOST_AUTO_TEST_CASE( metadata_test )
{
    ungod::MetaMap metamap;
    BOOST_REQUIRE(!metamap.isLoaded());


    metamap.load("test_data/test.xml");
    BOOST_REQUIRE(metamap.isLoaded());
    ungod::MetaNode node = metamap.getNodeWithKey("this_is_no_key");
    BOOST_REQUIRE(!node);
    node = metamap.getNodeWithKey("key1");
    BOOST_REQUIRE(node);

    auto val1 = node.getAttribute<float>("some");
    BOOST_CHECK_EQUAL(val1, 2.5f);
    auto val2 = node.getAttribute<bool>("random");
    BOOST_CHECK_EQUAL(val2, true);
    auto val3 = node.getAttribute<std::string>("attributes");
    BOOST_CHECK_EQUAL(val3, "test");

    auto result = node.getAttributes<float, bool, std::string>( {"some"}, {"random"}, {"attributes"} );
    BOOST_CHECK_EQUAL(std::get<0>(result), 2.5f);
    BOOST_CHECK_EQUAL(std::get<1>(result), true);
    BOOST_CHECK_EQUAL(std::get<2>(result), "test");


    ungod::MetaList metalist;
    BOOST_REQUIRE(!metalist.isLoaded());
    metalist.load("test_data/test.xml");
    BOOST_REQUIRE(metalist.isLoaded());
    node = metalist.getNodeAt(0);
    BOOST_REQUIRE(node);

    val1 = node.getAttribute<float>("some");
    BOOST_CHECK_EQUAL(val1, 2.5f);
    val2 = node.getAttribute<bool>("random");
    BOOST_CHECK_EQUAL(val2, true);
    val3 = node.getAttribute<std::string>("attributes");
    BOOST_CHECK_EQUAL(val3, "test");


    ungod::MetaTable metatable;
    BOOST_REQUIRE(!metatable.isLoaded());
    metatable.load("test_data/test2.xml");
    BOOST_REQUIRE(metatable.isLoaded());
    node = metatable.getNodeAt(1, 0);
    BOOST_REQUIRE(node);

    val1 = node.getAttribute<float>("some");
    BOOST_CHECK_EQUAL(val1, 2.5f);
    val2 = node.getAttribute<bool>("random");
    BOOST_CHECK_EQUAL(val2, true);
    val3 = node.getAttribute<std::string>("attributes");
    BOOST_CHECK_EQUAL(val3, "test");
}
/*
void InitMe(ungod::Entity e, ungod::World& world)
{
    world.getTransformManager().setPosition(e, {10.0f, 10.0f});
}

BOOST_AUTO_TEST_CASE( initialization_test )
{
    enum Inits { INIT_ME, NUM  };

    ungod::World world;

    ungod::Entity e = world.create(ungod::BaseComponents<ungod::Transform, ungod::Initializer>());

    world.getInitializerManager().initialize(e, INIT_ME, world);

    BOOST_CHECK_EQUAL(0.0f, e.get<ungod::Transform>().getPosition().x);

    world.getInitializerManager().performInitialization<&InitMe>(world);

    BOOST_CHECK_EQUAL(10.0f, e.get<ungod::Transform>().getPosition().x);
} */



BOOST_AUTO_TEST_CASE( graph_test )
{
    //test this graph
    /*
    *   a --2-- b --1-- c ---2-- d -1-- e
    *     \      \          ____/
    *    2 \    2 \   __9__/
    *       f -1-- g /
    */

    std::vector<ungod::graph::EdgeInstantiator> edges = { {0,1}, {1,2}, {2,3}, {3,4}, {0,5}, {1,6}, {5,6}, {6,3} };
    std::vector< float > weights = {2,1,1,1,2,2,1,9};
    std::vector< std::pair<float, float> > positions = { {0,0}, {-3,2}, {5,2}, {8,1}, {11,1}, {2,7}, {8,13} };  /// imaginary 2d world positions of the vertices to have something to estimate (use euclidean distance)

    auto weightGetter = [&weights] (std::size_t i) { return weights[i]; };
    auto costEstimator = [&positions] (std::size_t a, std::size_t b) { return std::sqrt( (positions[a].first - positions[b].first)*(positions[a].first - positions[b].first) + (positions[a].second - positions[b].second)*(positions[a].second - positions[b].second) ); };

    ungod::graph::UndirectedAdjacencyLists graph {7, edges.begin(), edges.end()};
    ungod::graph::AStarAlg astar {graph};
    astar.run(0, 4, weightGetter, costEstimator);

    BOOST_CHECK_EQUAL(astar.getProperties(4).origin, 3u);
    BOOST_CHECK_EQUAL(astar.getProperties(3).origin, 2u);
    BOOST_CHECK_EQUAL(astar.getProperties(2).origin, 1u);
    BOOST_CHECK_EQUAL(astar.getProperties(1).origin, 0u);
}


BOOST_AUTO_TEST_CASE( disjoint_sets_test )
{
    ungod::DisjointSets<std::string> ds;
    ds.reserve(5);
    ds.insertElement("hi");
    ds.insertElement("I");
    ds.insertElement("am");
    ds.insertElement("a");
    ds.insertElement("lamer");

    BOOST_CHECK_EQUAL(ds.setCount(), 5u);
    BOOST_CHECK_EQUAL(ds.elementCount(), 5u);

    ds.merge("hi", "I");
    ds.merge("a", "lamer");

    BOOST_CHECK_EQUAL(ds.setCount(), 3u);
    BOOST_REQUIRE(ds.findSet("hi"));
    BOOST_REQUIRE(ds.findSet("I"));
    BOOST_REQUIRE(ds.findSet("a"));
    BOOST_REQUIRE(ds.findSet("lamer"));
    BOOST_CHECK(((*(ds.findSet("hi")->clist.begin()))->data == "hi" && (*(std::next(ds.findSet("hi")->clist.begin())))->data  == "I") ||
                ((*(ds.findSet("hi")->clist.begin()))->data  == "I" && (*(std::next(ds.findSet("hi")->clist.begin())))->data  == "hi"));
    BOOST_CHECK(((*(ds.findSet("I")->clist.begin()))->data  == "hi" && (*(std::next(ds.findSet("I")->clist.begin())))->data  == "I") ||
                ((*(ds.findSet("I")->clist.begin()))->data  == "I" && (*(std::next(ds.findSet("I")->clist.begin())))->data  == "hi"));
    BOOST_CHECK(((*(ds.findSet("a")->clist.begin()))->data  == "a" && (*(std::next(ds.findSet("a")->clist.begin())))->data  == "lamer") ||
                ((*(ds.findSet("a")->clist.begin()))->data  == "lamer" && (*(std::next(ds.findSet("a")->clist.begin())))->data  == "a"));
    BOOST_CHECK(((*(ds.findSet("lamer")->clist.begin()))->data  == "a" && (*(std::next(ds.findSet("lamer")->clist.begin())))->data  == "lamer") ||
                ((*(ds.findSet("lamer")->clist.begin()))->data  == "lamer" && (*(std::next(ds.findSet("lamer")->clist.begin())))->data  == "a"));

    ds.merge("hi", "lamer");
    ds.merge("a", "am");

    BOOST_CHECK_EQUAL(ds.setCount(), 1u);
}

BOOST_AUTO_TEST_CASE( triangulation_test )
{
    //simple test with just a single triangle
    {
        ungod::DelaunayTriangulation<sf::Vector2f> triag;

        sf::Vector2f upperBound = {0,0};
        sf::Vector2f lowerBound = {100,100};
        std::vector<sf::Vector2f> points = { {10,10}, {30,14}, {20,50} };

        triag.run(points, upperBound, lowerBound);

        BOOST_CHECK_EQUAL(1u, triag.getData().getTriangleContainer().size());
    }
    //more advanced test
    {
        ungod::DelaunayTriangulation<sf::Vector2f> triag;

        sf::Vector2f upperBound = {0,0};
        sf::Vector2f lowerBound = {100,100};
        std::vector<sf::Vector2f> points = { {25,5}, {60,5}, {5,40}, {40, 50}, {48, 22} , {53, 48}  };

        triag.run(points, upperBound, lowerBound);

        BOOST_CHECK_EQUAL(5u, triag.getData().getTriangleContainer().size());
    }
}

BOOST_AUTO_TEST_CASE( component_signal_test )
{
    bool transfAdded = false;
    bool mvmAdded = false;
    bool mvmRemoved = false;

    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,800,600);
    world.onComponentAdded<ungod::TransformComponent>([&transfAdded] (ungod::Entity e) { transfAdded = true; });
    world.onComponentAdded<ungod::MovementComponent>([&mvmAdded] (ungod::Entity e) { mvmAdded = true; });
    world.onComponentRemoved<ungod::MovementComponent>([&mvmRemoved] (ungod::Entity e) { mvmRemoved = true; });

    ungod::Entity e = world.create(ungod::BaseComponents<ungod::TransformComponent>(), ungod::OptionalComponents<ungod::MovementComponent>());

    BOOST_CHECK(transfAdded);

    e.add<ungod::MovementComponent>();

    BOOST_CHECK(mvmAdded);

    e.rem<ungod::MovementComponent>();

    BOOST_CHECK(mvmRemoved);
}

BOOST_AUTO_TEST_CASE( entity_copy_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,8000,6000);
    ungod::Entity e = world.create(ungod::BaseComponents<ungod::VisualsComponent, ungod::TransformComponent, ungod::SpriteMetadataComponent>(), ungod::OptionalComponents<ungod::SpriteComponent, ungod::MovementComponent>());
    e.add<ungod::SpriteComponent>();
    world.getTransformManager().setPosition(e, {150,40});
    world.getVisualsManager().loadTexture(e, "resource/penumbraTexture.png");
    world.getVisualsManager().setSpriteTextureRect(e, sf::FloatRect{60,60,60,60});
    ungod::Entity ecopy = e.getWorld().makeCopy(e);
    world.getQuadTree().insert(e);
    world.getQuadTree().insert(ecopy);
    world.getTransformManager().setPosition(ecopy, {150,770});

    BOOST_CHECK(ecopy.has<ungod::TransformComponent>());
    BOOST_CHECK(ecopy.has<ungod::VisualsComponent>());
    BOOST_CHECK(ecopy.has<ungod::SpriteMetadataComponent>());
    BOOST_CHECK(ecopy.has<ungod::SpriteComponent>());
    BOOST_CHECK(!ecopy.has<ungod::MovementComponent>());
    BOOST_CHECK_EQUAL(2u, world.getQuadTree().size());
    BOOST_CHECK_EQUAL(ecopy.get<ungod::TransformComponent>().getPosition().x, e.get<ungod::TransformComponent>().getPosition().x);
    BOOST_CHECK(ecopy.get<ungod::TransformComponent>().getPosition().y != e.get<ungod::TransformComponent>().getPosition().y);
    BOOST_CHECK_EQUAL(ecopy.get<ungod::VisualsComponent>().getFilePath(), e.get<ungod::VisualsComponent>().getFilePath());
    BOOST_CHECK_EQUAL(ecopy.get<ungod::SpriteComponent>().getSprite().getTextureRect().left, e.get<ungod::SpriteComponent>().getSprite().getTextureRect().left);
}

BOOST_AUTO_TEST_CASE( entity_instantiation_test )
{
    ungod::Logger::info("HI");
    ungod::Logger::endl();

    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
    world.initSpace(0,0,800,600);

    ungod::Entity wo = world.create(ungod::WorldObjectBaseComponents(), ungod::WorldObjectOptionalComponents());
    ungod::Entity ac = world.create(ungod::ActorBaseComponents(), ungod::ActorOptionalComponents());
    ungod::Entity me = world.create(ungod::AudioEmitterBaseComponents(), ungod::AudioEmitterOptionalComponents());
    ungod::Entity ps = world.create(ungod::ParticleSystemBaseComponents(), ungod::ParticleSystemOptionalComponents());

    BOOST_CHECK_EQUAL(world.getComponentCount<ungod::TransformComponent>(), 4u);
    BOOST_CHECK_EQUAL(world.getComponentCount<ungod::VisualsComponent>(), 3u);
    BOOST_CHECK_EQUAL(world.getComponentCount<ungod::ParticleSystemComponent>(), 1u);
    BOOST_CHECK_EQUAL(world.getComponentCount<ungod::MusicEmitterComponent>(), 1u);
    BOOST_CHECK_EQUAL(world.getComponentCount<ungod::MovementComponent>(), 1u);

    BOOST_CHECK(!wo.get<ungod::VisualsComponent>().isLoaded());
    BOOST_CHECK(!ac.get<ungod::VisualsComponent>().isLoaded());
    BOOST_CHECK(!ps.get<ungod::VisualsComponent>().isLoaded());
}

BOOST_AUTO_TEST_SUITE_END()


















