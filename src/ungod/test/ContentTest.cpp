#include <boost/test/unit_test.hpp>
#include "ungod/content/TileMap.h"
#include "ungod/content/FloodFill.h"
#include "ungod/content/TilemapBrush.h"
#include "ungod/base/World.h"
#include "ungod/test/mainTest.h"
#include <boost/filesystem.hpp>

BOOST_AUTO_TEST_SUITE(ContentTest)

BOOST_AUTO_TEST_CASE( tilemap_test )
{
    //manually create a test png if it does not exist
    if (!boost::filesystem::exists("test_data/tilemap_test.png"))
    {
        sf::Image img;
        img.create(64, 64);
        for (unsigned i = 0; i < 32; ++i)
            for (unsigned j = 0; j < 32; ++j)
        {
            img.setPixel(i, j, sf::Color::Green);
        }
        for (unsigned i = 32; i < 64; ++i)
            for (unsigned j = 0; j < 32; ++j)
        {
            img.setPixel(i, j, sf::Color::Red);
        }
        for (unsigned i = 0; i < 32; ++i)
            for (unsigned j = 32; j < 64; ++j)
        {
            img.setPixel(i, j, sf::Color::Blue);
        }
        for (unsigned i = 32; i < 64; ++i)
            for (unsigned j = 32; j < 64; ++j)
        {
            img.setPixel(i, j, sf::Color::Yellow);
        }
        img.saveToFile("test_data/tilemap_test.png");
    }

    sf::RenderTexture rendertex{};
    rendertex.create(800,600);
    ungod::TileMap tilemap;
    tilemap.loadTiles("test_data/tilemap_test.png", "test_data/tilemap_test.xml", 32, 32, {"geen", "red", "blue", "yellow"});
    tilemap.setTiles({0,1,2,3,0,1,2,3,0,1,2,3}, {0,1,2,3,0,1,2,3,0,1,2,3}, 4, 3);


    BOOST_CHECK_EQUAL(tilemap.getTiledata(0,0)->getTileID(), 0);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(0,0)->getMaterialID(), 0u);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(1,2)->getTileID(), 1);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(1,2)->getMaterialID(), 1u);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(3,1)->getTileID(), 2);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(3,1)->getMaterialID(), 2u);

    BOOST_CHECK_EQUAL(tilemap.getTiledata(sf::Vector2f{50,50})->getTileID(), 0);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(sf::Vector2f{50,50})->getMaterialID(), 0u);

    tilemap.getTiledata(1,1)->setTileID(3);
    tilemap.getTiledata(3,1)->setMaterialID(3);

    BOOST_CHECK_EQUAL(tilemap.getTiledata(sf::Vector2f{50,50})->getTileID(), 3);
    BOOST_CHECK_EQUAL(tilemap.getTiledata(sf::Vector2f{25,100})->getMaterialID(), 3u);

    {  //test correct output with the default view
        tilemap.render(rendertex, {});

        sf::Image result = rendertex.getTexture().copyToImage();
        BOOST_CHECK_EQUAL(result.getPixel(10, 10).r, sf::Color::Green.r);
        BOOST_CHECK_EQUAL(result.getPixel(10, 10).b, sf::Color::Green.b);
        BOOST_CHECK_EQUAL(result.getPixel(10, 40).g, sf::Color::Red.g);
        BOOST_CHECK_EQUAL(result.getPixel(10, 40).b, sf::Color::Red.b);
        BOOST_CHECK_EQUAL(result.getPixel(10, 70).r, sf::Color::Blue.r);
        BOOST_CHECK_EQUAL(result.getPixel(10, 70).g, sf::Color::Blue.g);
    }

    {   //test correct output with a moved view
        sf::View view = rendertex.getView();
        view.move(32,0);
        rendertex.setView(view);

        tilemap.render(rendertex, {});

        sf::Image result = rendertex.getTexture().copyToImage();
        BOOST_CHECK_EQUAL(result.getPixel(10, 10).b, sf::Color::Yellow.b);
        BOOST_CHECK_EQUAL(result.getPixel(10, 40).r, sf::Color::Green.r);
        BOOST_CHECK_EQUAL(result.getPixel(10, 40).b, sf::Color::Green.b);
        BOOST_CHECK_EQUAL(result.getPixel(10, 70).g, sf::Color::Red.g);
        BOOST_CHECK_EQUAL(result.getPixel(10, 70).b, sf::Color::Red.b);
    }
}

BOOST_AUTO_TEST_CASE( attach_on_create_test )
{
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
    ungod::World world(&state);
    world.instantiate(EmbeddedTestApp::getApp(), "", "", "", "", "");
    ungod::Entity e = world.create(ungod::TileMapBaseComponents(), ungod::TileMapOptionalComponents());

    BOOST_CHECK_EQUAL(1u, world.getTileMapRenderer().getTileMapEntities().size());

	world.destroy(e); //queue entity for destruction
	world.update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_CASE( tilemap_brush_test )
{
    ungod::TileMap tilemap;
    tilemap.loadTiles("test_data/ground.png", "test_data/ground.xml", 128, 128, {"purple_stone", "brown_stone", "brown_stone_iso",
                                                                                "brown_stone_pathend_down", "brown_stone_pathend_up", "brown_stone_pathend_left", "brown_stone_pathend_right",
                                                                                "brown_stone_pathcurve_1", "brown_stone_pathcurve_2", "brown_stone_pathcurve_3", "brown_stone_pathcurve_4",
                                                                                "brown_stone_corner_1", "brown_stone_corner_2", "brown_stone_corner_3", "brown_stone_corner_4"});
    tilemap.setTiles({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 5, 5);
    ungod::TilemapBrush brush("brown_stone", tilemap);

    brush.paintTile(2,2);
    BOOST_CHECK_EQUAL( tilemap.getTiledata(2,2)->getTileID(), 2 );
    brush.paintTile(3,2);
    BOOST_CHECK_EQUAL( tilemap.getTiledata(2,2)->getTileID(), 6 );
    BOOST_CHECK_EQUAL( tilemap.getTiledata(3,2)->getTileID(), 5 );
    brush.paintTile(3,3);
    BOOST_CHECK_EQUAL( tilemap.getTiledata(3,2)->getTileID(), 10 );
    BOOST_CHECK_EQUAL( tilemap.getTiledata(3,3)->getTileID(), 4 );
    brush.paintTile(2,3);
    BOOST_CHECK_EQUAL( tilemap.getTiledata(2,2)->getTileID(), 13 );
    BOOST_CHECK_EQUAL( tilemap.getTiledata(3,2)->getTileID(), 14 );
    BOOST_CHECK_EQUAL( tilemap.getTiledata(3,3)->getTileID(), 11 );
    BOOST_CHECK_EQUAL( tilemap.getTiledata(2,3)->getTileID(), 12 );
}

BOOST_AUTO_TEST_CASE( floodfill_test )
{
    {
        ungod::TileMap tilemap;
        tilemap.loadTiles("test_data/tilemap_test.png", "test_data/tilemap_test.xml", 32, 32, {"geen", "red", "blue", "yellow"});
        tilemap.setTiles({0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0}, 4, 3);

        ungod::floodFill(tilemap, 1, 1, {1}, true);

        BOOST_REQUIRE(tilemap.getTiledata(0,0));
        BOOST_CHECK_EQUAL(1, tilemap.getTiledata(2,2)->getTileID());
        BOOST_CHECK_EQUAL(1, tilemap.getTiledata(0,2)->getTileID());
        BOOST_CHECK_EQUAL(1, tilemap.getTiledata(2,1)->getTileID());
        BOOST_CHECK_EQUAL(1, tilemap.getTiledata(0,0)->getTileID());
        BOOST_CHECK_EQUAL(1, tilemap.getTiledata(3,2)->getTileID());
    }
}

BOOST_AUTO_TEST_SUITE_END()
