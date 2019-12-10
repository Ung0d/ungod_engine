#include <boost/test/unit_test.hpp>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/test/mainTest.h"

using namespace ungod;

BOOST_AUTO_TEST_SUITE(CollisionTest)

BOOST_AUTO_TEST_CASE( collision_events_test )
{
    /*ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
	node.setSize({ 8000,6000 });
	ungod::World* world = node.addWorld();

    bool collisionDetected = false;
    bool collisionDetectedBegin = false;
    bool collisionDetectedEnd = false;
    Entity e1 = world->create(BaseComponents<TransformComponent, RigidbodyComponent<>, MovementComponent>());
    Entity e2 = world->create(BaseComponents<TransformComponent, RigidbodyComponent<>, MovementComponent>());
    Entity e3 = world->create(BaseComponents<TransformComponent, RigidbodyComponent<>, MovementComponent>());
    world->getMovementRigidbodyManager().addCollider(e1, ungod::makeRotatedRect({0,0}, {10.0f, 10.0f}));
    world->getMovementRigidbodyManager().addCollider(e2, ungod::makeRotatedRect({0.0f,0.0f}, {40.0f, 40.0f}));

    world->getMovementCollisionManager().onCollision([&collisionDetected] (Entity e, Entity other, const sf::Vector2f& mdv,
                                                const Collider& collider, const Collider& otherCollider)
                                            {
                                                collisionDetected = true;
                                            });
    world->getMovementCollisionManager().onBeginCollision([&collisionDetectedBegin] (Entity e, Entity other)
                                            {
                                                collisionDetectedBegin = true;
                                            });
    world->getMovementCollisionManager().onEndCollision([&collisionDetectedEnd] (Entity e, Entity other)
                                            {
                                                collisionDetectedEnd = true;
                                            });


    world->getQuadTree().insert(e1);
    world->getQuadTree().insert(e2);
    world->getQuadTree().insert(e3);

    world->update(0.1f, {0,0}, {800, 600});

    BOOST_CHECK(collisionDetected);
    BOOST_CHECK(collisionDetectedBegin);

    world->getTransformManager().setPosition(e1, {1000, 1000});

    world->update(0.1f, {0,0}, {800, 600});

    BOOST_CHECK(collisionDetectedEnd);

	world->destroy(e1); //queue entity for destruction
	world->destroy(e2); //queue entity for destruction
	world->destroy(e3); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue*/
}

BOOST_AUTO_TEST_CASE( point_inside_collider_test )
{
	{
		sf::Vector2f isInside{ 10.0f, 10.0f };
		sf::Vector2f isNotInside{ 0.0f, 0.0f };

		ungod::Collider rect;
		rect.initRotatedRect({ 8.0f, 8.0f }, { 16.0f, 16.0f }, 0.0f);
		ungod::Collider orRect;
		orRect.initRotatedRect({ 8.0f, 8.0f }, { 16.0f, 16.0f }, 30.0f);

		ungod::TransformComponent transf;

		BOOST_CHECK(ungod::containsPoint(rect, transf, isInside));
		BOOST_CHECK(!ungod::containsPoint(rect, transf, isNotInside));
		BOOST_CHECK(ungod::containsPoint(orRect, transf, isInside));
		BOOST_CHECK(!ungod::containsPoint(orRect, transf, isNotInside));
	}
	{
		sf::Vector2f isInside1{ 1, 0 };
		sf::Vector2f isInside2{ 1.5f, 0.5f };
		sf::Vector2f isNotInside1{ 0, 1 };
		sf::Vector2f isNotInside2{ 3, 1 };

		ungod::Collider poly;
		poly.initConvexPolygon({ { 0,0 }, { 1,1 }, {2,1}, {-1,2} });

		ungod::TransformComponent transf;

		BOOST_CHECK(ungod::containsPoint(poly, transf, isInside1));
		BOOST_CHECK(ungod::containsPoint(poly, transf, isInside2));
		BOOST_CHECK(!ungod::containsPoint(poly, transf, isNotInside1));
		BOOST_CHECK(!ungod::containsPoint(poly, transf, isNotInside2));
	}
	{
		sf::Vector2f isInside{ 10.0f, 10.0f };
		sf::Vector2f isNotInside{ 0.0f, 0.0f };
		ungod::Collider circle;
		circle.initCircle({ 8.0f, 8.0f }, 6.0f);
		ungod::TransformComponent transf;
		BOOST_CHECK(ungod::containsPoint(circle, transf, isInside));
		BOOST_CHECK(!ungod::containsPoint(circle, transf, isNotInside));
	}
}

BOOST_AUTO_TEST_SUITE_END()
