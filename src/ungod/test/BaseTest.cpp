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


BOOST_AUTO_TEST_CASE(iterate_over_component_test)
{
	ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
    node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();
	std::list<ungod::Entity> entities;
	world->create(ungod::BaseComponents<ungod::TransformComponent>(), 
		ungod::OptionalComponents<>(), 10000, [&entities](ungod::Entity e) {entities.push_back(e); });
	int counter = 0;
	world->iterateOverComponents<ungod::TransformComponent>([&counter](ungod::TransformComponent& transf)
		{
			counter++;
		});
	BOOST_CHECK_EQUAL(10000, counter);
	for (const auto& e : entities)
		world->destroy(e);
	world->update(20.0f, {}, {});
}

BOOST_AUTO_TEST_CASE( asset_image_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({800,600});
	ungod::World* world = node.addWorld();
	ungod::Entity e = world->create(ungod::BaseComponents<ungod::TransformComponent, ungod::VisualsComponent>(), ungod::OptionalComponents<>());
	BOOST_CHECK(e);
    world->getVisualsHandler().loadTexture(e, "test_data/test_sheet.png", ungod::LoadPolicy::ASYNC);
    world->getVisualsHandler().waitForLoading(e);
	BOOST_CHECK(e.get<ungod::VisualsComponent>().isLoaded());
	world->destroy(e); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( transform_system_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();
    ungod::Entity e1 = world->create(ungod::BaseComponents<ungod::TransformComponent>(), ungod::OptionalComponents<>());
    ungod::Entity e2 = world->create(ungod::BaseComponents<ungod::TransformComponent>(), ungod::OptionalComponents<>());
    world->getTransformHandler().setPosition(e1, {100,0});
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().x, 100);
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().y, 0);
    world->getTransformHandler().move(e1, {10,10});
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().x, 110);
    BOOST_CHECK_EQUAL(e1.get<ungod::TransformComponent>().getPosition().y, 10);

    //test position changed signal
    world->getTransformHandler().onPositionChanged([world, e1, e2] (ungod::Entity e, const sf::Vector2f& position)
                                                  {
                                                    //automatically move e2 when e1 is moved
                                                    if (e == e1)
                                                    {
                                                        world->getTransformHandler().setPosition(e2, position);
                                                    }
                                                  });
    world->getTransformHandler().setPosition(e2, {50,50});
    world->getTransformHandler().move(e1, {10,10});
    BOOST_CHECK_EQUAL(e2.get<ungod::TransformComponent>().getPosition().x, 120);
    BOOST_CHECK_EQUAL(e2.get<ungod::TransformComponent>().getPosition().y, 20);
	world->destroy(e1); //queue entity for destruction
	world->destroy(e2); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}
BOOST_AUTO_TEST_CASE( movement_system_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();
    ungod::Entity e1 = world->create(ungod::BaseComponents<ungod::TransformComponent, ungod::MovementComponent>(), ungod::OptionalComponents<>());
    world->getTransformHandler().setPosition(e1, {0,0});
    world->getMovementHandler().accelerate(e1, {1,1});
    world->getMovementHandler().update({e1}, 20);
    BOOST_CHECK(  e1.get<ungod::TransformComponent>().getPosition().x > 0);
	world->destroy(e1); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( render_system_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();
    ungod::Entity e = world->create(ungod::BaseComponents<ungod::TransformComponent, ungod::VisualsComponent>(), ungod::OptionalComponents<>());
    world->getVisualsHandler().loadTexture(e, "test_data/test.png", ungod::LoadPolicy::SYNC);
    BOOST_REQUIRE( e.get<ungod::VisualsComponent>().isLoaded() );
	world->destroy(e); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( config_property_test )
{
    bool changed1 = false;
    bool changed2 = false;

    EmbeddedTestApp::getApp().getConfig().onConfigurationChanged([&changed1] (ungod::Configuration& config, const std::string& item)
                                                                 { if(item == "testgroup/floatTest") changed1 = true; });

    EmbeddedTestApp::getApp().getConfig().set("testgroup/floatTest", 1.0f);
    EmbeddedTestApp::getApp().getConfig().set("testgroup/stringTest", std::string{"hello"});
    EmbeddedTestApp::getApp().getConfig().set("testgroup/boolTest", true);
    EmbeddedTestApp::getApp().getConfig().set("testgroup/intTest", 24);

    BOOST_CHECK(changed1);

    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<float>("testgroup/floatTest"));
    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<std::string>("testgroup/stringTest"));
    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<bool>("testgroup/boolTest"));
    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<int>("testgroup/intTest"));

    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<float>("testgroup/floatTest").value(), 1.0f);
    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<std::string>("testgroup/stringTest").value(), "hello");
    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<bool>("testgroup/boolTest").value(), true);
    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<int>("testgroup/intTest").value(), 24);

    EmbeddedTestApp::getApp().getConfig().set("testgroup/floatTest", 12.0f);
    EmbeddedTestApp::getApp().getConfig().set<std::string>("testgroup/stringTest", "bye");
    EmbeddedTestApp::getApp().getConfig().set("testgroup/boolTest", false);
    EmbeddedTestApp::getApp().getConfig().set("testgroup/intTest", 199);

    EmbeddedTestApp::getApp().getConfig().onConfigurationChanged([&changed2] (ungod::Configuration& config, const std::string& item)
                                                                 { if(item == "") changed2 = true; });

    EmbeddedTestApp::getApp().getConfig().save();
    EmbeddedTestApp::getApp().loadConfig();

    BOOST_CHECK(changed2);

    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<float>("testgroup/floatTest"));
    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<std::string>("testgroup/stringTest"));
    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<bool>("testgroup/boolTest"));
    BOOST_REQUIRE(EmbeddedTestApp::getApp().getConfig().get<int>("testgroup/intTest"));

    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<float>("testgroup/floatTest").value(), 12.0f);
    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<std::string>("testgroup/stringTest").value(), "bye");
    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<bool>("testgroup/boolTest").value(), false);
    BOOST_CHECK_EQUAL(EmbeddedTestApp::getApp().getConfig().get<int>("testgroup/intTest").value(), 199);
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



BOOST_AUTO_TEST_CASE( graph_test )
{
    //test this graph
    //
    //   a --2-- b --1-- c ---2-- d -1-- e
    //     \      \          ____/
    //    2 \    2 \   __9__/
    //       f -1-- g /
    //

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
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();
    world->onComponentAdded<ungod::TransformComponent>([&transfAdded] (ungod::Entity e) { transfAdded = true; });
    world->onComponentAdded<ungod::MovementComponent>([&mvmAdded] (ungod::Entity e) { mvmAdded = true; });
    world->onComponentRemoved<ungod::MovementComponent>([&mvmRemoved] (ungod::Entity e) { mvmRemoved = true; });

    ungod::Entity e = world->create(ungod::BaseComponents<ungod::TransformComponent>(), ungod::OptionalComponents<ungod::MovementComponent>());

    BOOST_CHECK(transfAdded);

    e.add<ungod::MovementComponent>();

    BOOST_CHECK(mvmAdded);

    e.rem<ungod::MovementComponent>();

    BOOST_CHECK(mvmRemoved);

	world->destroy(e); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( entity_copy_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 8000,6000 });
	ungod::World* world = node.addWorld();
    ungod::Entity e = world->create(ungod::BaseComponents<ungod::VisualsComponent, ungod::TransformComponent, ungod::SpriteMetadataComponent>(), ungod::OptionalComponents<ungod::SpriteComponent, ungod::MovementComponent>());
    e.add<ungod::SpriteComponent>();
    world->getTransformHandler().setPosition(e, {150,40});
    world->getVisualsHandler().loadTexture(e, "resource/penumbraTexture.png");
    world->getVisualsHandler().setSpriteTextureRect(e, sf::FloatRect{60,60,60,60});
    ungod::Entity ecopy = e.getWorld().makeCopy(e);
    world->getQuadTree().insert(e);
    world->getQuadTree().insert(ecopy);
    world->getTransformHandler().setPosition(ecopy, {150,770});

    BOOST_CHECK(ecopy.has<ungod::TransformComponent>());
    BOOST_CHECK(ecopy.has<ungod::VisualsComponent>());
    BOOST_CHECK(ecopy.has<ungod::SpriteMetadataComponent>());
    BOOST_CHECK(ecopy.has<ungod::SpriteComponent>());
    BOOST_CHECK(!ecopy.has<ungod::MovementComponent>());
    BOOST_CHECK_EQUAL(2u, world->getQuadTree().size());
    BOOST_CHECK_EQUAL(ecopy.get<ungod::TransformComponent>().getPosition().x, e.get<ungod::TransformComponent>().getPosition().x);
    BOOST_CHECK(ecopy.get<ungod::TransformComponent>().getPosition().y != e.get<ungod::TransformComponent>().getPosition().y);
    BOOST_CHECK_EQUAL(ecopy.get<ungod::VisualsComponent>().getFilePath(), e.get<ungod::VisualsComponent>().getFilePath());
    BOOST_CHECK_EQUAL(ecopy.get<ungod::SpriteComponent>().getSprite().getTextureRect().left, e.get<ungod::SpriteComponent>().getSprite().getTextureRect().left);

	world->destroy(e); //queue entity for destruction
	world->destroy(ecopy); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( entity_instantiation_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();

    ungod::Entity wo = world->create(ungod::WorldObjectBaseComponents(), ungod::WorldObjectOptionalComponents());
    ungod::Entity ac = world->create(ungod::ActorBaseComponents(), ungod::ActorOptionalComponents());
    ungod::Entity me = world->create(ungod::AudioEmitterBaseComponents(), ungod::AudioEmitterOptionalComponents());
    ungod::Entity ps = world->create(ungod::ParticleSystemBaseComponents(), ungod::ParticleSystemOptionalComponents());

    BOOST_CHECK_EQUAL(world->getComponentCount<ungod::TransformComponent>(), 4u);
    BOOST_CHECK_EQUAL(world->getComponentCount<ungod::VisualsComponent>(), 3u);
    BOOST_CHECK_EQUAL(world->getComponentCount<ungod::ParticleSystemComponent>(), 1u);
    BOOST_CHECK_EQUAL(world->getComponentCount<ungod::MusicEmitterComponent>(), 1u);
    BOOST_CHECK_EQUAL(world->getComponentCount<ungod::MovementComponent>(), 1u);

    BOOST_CHECK(!wo.get<ungod::VisualsComponent>().isLoaded());
    BOOST_CHECK(!ac.get<ungod::VisualsComponent>().isLoaded());
    BOOST_CHECK(!ps.get<ungod::VisualsComponent>().isLoaded());

	world->destroy(wo); //queue entity for destruction
	world->destroy(ac); //queue entity for destruction
	world->destroy(me); //queue entity for destruction
	world->destroy(ps); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}


BOOST_AUTO_TEST_CASE( parent_child_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();

    ungod::Entity parent = world->create(ungod::EntityBaseComponents(), ungod::EntityOptionalComponents());
    ungod::Entity child = world->create(ungod::EntityBaseComponents(), ungod::EntityOptionalComponents());

    world->getTransformHandler().setPosition(parent, {100.0f, 100.f});

    world->getParentChildHandler().addChild(parent, child);

    BOOST_CHECK_EQUAL(child.get<ungod::TransformComponent>().getPosition().x , 100.0f);

    world->getParentChildHandler().setChildPosition(child, {20.0f, 20.0f});

    BOOST_CHECK_EQUAL(child.get<ungod::TransformComponent>().getPosition().x , 120.0f);

	world->destroy(parent); //queue entity for destruction
	world->destroy(child); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( world_graph_test )
{
    constexpr int NUM_TRYS = 1;
    for (int i = 0; i < NUM_TRYS; i++) //we can try the following a number of times to have a good chance of detecting race conditions or cpu stalls
    {
        ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
        ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "test_output/nodefile");
        node.wait();
        ungod::World* world1;
        ungod::World* world2;
        if (node.getNumWorld() == 0)
        {
            world1 = node.addWorld();
            world2 = node.addWorld();
        }
        else
        {
            world1 = node.getWorld(0);
            world2 = node.getWorld(1);
        }
        BOOST_CHECK_EQUAL(ungod::WorldGraphNode::DEFAULT_SIZE, world1->getSize().x);
        BOOST_CHECK_EQUAL(ungod::WorldGraphNode::DEFAULT_SIZE, world1->getSize().y);
        BOOST_CHECK_EQUAL(ungod::WorldGraphNode::DEFAULT_SIZE, world2->getSize().x);
        BOOST_CHECK_EQUAL(ungod::WorldGraphNode::DEFAULT_SIZE, world2->getSize().y);
        BOOST_CHECK_EQUAL(state.getWorldGraph().getActiveNode(), &node);
        node.setSize({ 800,600 });
        BOOST_CHECK_EQUAL(state.getWorldGraph().getActiveNode(), &node);
        node.setPosition({ 100,100 });
        BOOST_CHECK(!state.getWorldGraph().getActiveNode());
        BOOST_CHECK(!node.isLoaded());
        state.getWorldGraph().activateNode("nodeid");
        BOOST_CHECK_EQUAL(state.getWorldGraph().getActiveNode(), &node);
        node.wait();
        world1 = node.getWorld(0);
        world2 = node.getWorld(1);
        BOOST_REQUIRE(world1);
        BOOST_REQUIRE(world2);
        BOOST_CHECK(node.isLoaded());
        BOOST_CHECK_EQUAL(800.0f, node.getBounds().width);
        BOOST_CHECK_EQUAL(600.0f, node.getBounds().height);
        BOOST_CHECK_EQUAL(800.0f, world1->getSize().x);
        BOOST_CHECK_EQUAL(600.0f, world1->getSize().y);
        BOOST_CHECK_EQUAL(800.0f, world2->getSize().x);
        BOOST_CHECK_EQUAL(600.0f, world2->getSize().y);
        node.setPosition({ 400,400 });
        ungod::WorldGraphNode* nodeptr = state.getWorldGraph().getNode(sf::Vector2f{ 0,0 });
        BOOST_CHECK(!nodeptr);
        nodeptr = state.getWorldGraph().getNode(sf::Vector2f{ 500,500 });
        BOOST_CHECK(nodeptr);
    }
}


BOOST_AUTO_TEST_CASE(copy_between_worlds_test)
{
	ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
    node.setSaveContents(false); //do not serialize any changes we make to this node
	ungod::World* world1 = node.addWorld();
	ungod::World* world2 = node.addWorld();
	ungod::Entity e1 = world1->create(ungod::EntityBaseComponents(), ungod::EntityOptionalComponents());
	world1->getTransformHandler().setPosition(e1, { 100,100 });
	e1.add<ungod::SpriteComponent>();
	world1->getVisualsHandler().setSpritePosition(e1, { 20,20 });
	ungod::Entity e2 = world2->accomodateForeign(e1);
	BOOST_CHECK(e2.has<ungod::SpriteComponent>());
	BOOST_CHECK(e2.has<ungod::TransformComponent>());
	BOOST_CHECK_EQUAL(e2.get<ungod::TransformComponent>().getPosition().x, 100.0f);
	BOOST_CHECK_EQUAL(e2.get<ungod::SpriteComponent>().getSprite().getPosition().x, 20.0f);
	world1->addEntity(e1);
	world2->addEntity(e2); //adding to the quadtree automatically destroys entities when world is destroyed
}

BOOST_AUTO_TEST_CASE(multiple_threads_request_asset_test)
{
    //same asset
    {
        auto loader = []()
        {
            ungod::Image img("test_data/test_sheet.png", ungod::LoadPolicy::ASYNC);
            return img;
        };
        auto r1 = std::async(loader);
        auto r2 = std::async(loader);
        r1.wait();
        r2.wait();
        ungod::Image img1 = r1.get();
        ungod::Image img2 = r2.get();
        img1.getWait();
        img2.getWait();
        BOOST_CHECK(img1.isLoaded());
        BOOST_CHECK(img2.isLoaded());
    }
    //different assets
    {
        auto loader = [](const std::string& file)
        {
            ungod::Image img(file, ungod::LoadPolicy::ASYNC);
            return img;
        };
        auto r1 = std::async(std::launch::async, loader, "test_data/test_sheet.png");
        auto r2 = std::async(std::launch::async, loader, "test_data/blech_warrior.png");
        r1.wait();
        r2.wait();
        ungod::Image img1 = r1.get();
        ungod::Image img2 = r2.get();
        img1.getWait();
        img2.getWait();
        BOOST_CHECK(img1.isLoaded());
        BOOST_CHECK(img2.isLoaded());
    }
}

BOOST_AUTO_TEST_SUITE_END() 


















