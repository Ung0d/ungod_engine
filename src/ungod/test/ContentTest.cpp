#include <boost/test/unit_test.hpp>
#include "ungod/content/tilemap/TileMap.h"
#include "ungod/content/tilemap/FloodFill.h"
#include "ungod/content/tilemap/TilemapBrush.h"
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
    ungod::Image tex{ "test_data/tilemap_test.png" };
    ungod::MetaMap meta{ "test_data/tilemap_test.xml" };
    tilemap.setMetaMap(meta);
    tilemap.setTileDims(32, 32, {"geen", "red", "blue", "yellow"});
    ungod::TileData tiledata;
    tiledata.ids = std::make_shared<std::vector<int>>(std::initializer_list<int>{ 0,1,2,3,0,1,2,3,0,1,2,3 });
    tilemap.setTiles(tiledata, 4, 3);


    BOOST_CHECK_EQUAL(tilemap.getTileID(0,0), 0);
    BOOST_CHECK_EQUAL(tilemap.getTileID(1,2), 1);
    BOOST_CHECK_EQUAL(tilemap.getTileID(3,1), 2);

    BOOST_CHECK_EQUAL(tilemap.getTileID(sf::Vector2f{50,50}), 0);

    tilemap.setTile(3, 1, 1);

    BOOST_CHECK_EQUAL(tilemap.getTileID(sf::Vector2f{50,50}), 3);

    {  //test correct output with the default view
        tilemap.render(rendertex, &tex.get(), {});

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

        tilemap.render(rendertex, &tex.get(), {});

        sf::Image result = rendertex.getTexture().copyToImage();
        BOOST_CHECK_EQUAL(result.getPixel(10, 10).b, sf::Color::Yellow.b);
        BOOST_CHECK_EQUAL(result.getPixel(10, 40).r, sf::Color::Green.r);
        BOOST_CHECK_EQUAL(result.getPixel(10, 40).b, sf::Color::Green.b);
        BOOST_CHECK_EQUAL(result.getPixel(10, 70).g, sf::Color::Red.g);
        BOOST_CHECK_EQUAL(result.getPixel(10, 70).b, sf::Color::Red.b);
    }
}

BOOST_AUTO_TEST_CASE( tilemap_brush_test )
{
    ungod::TileMap tilemap;
    //ungod::Image tex{ "test_data/ground.png" };
    ungod::MetaMap meta{ "test_data/ground.xml" };
    tilemap.setMetaMap(meta);
    tilemap.setTileDims(128, 128, {"purple_stone", "brown_stone", "brown_stone_iso",
                                                                                "brown_stone_pathend_down", "brown_stone_pathend_up", "brown_stone_pathend_left", "brown_stone_pathend_right",
                                                                                "brown_stone_pathcurve_1", "brown_stone_pathcurve_2", "brown_stone_pathcurve_3", "brown_stone_pathcurve_4",
                                                                                "brown_stone_corner_1", "brown_stone_corner_2", "brown_stone_corner_3", "brown_stone_corner_4"});
    ungod::TileData tiledata;
    tiledata.ids = std::make_shared<std::vector<int>>(std::initializer_list<int>{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 });
    tilemap.setTiles(tiledata, 5, 5);
    ungod::TilemapBrush brush("brown_stone", tilemap);

    brush.paintTile(2,2);
    BOOST_CHECK_EQUAL( tilemap.getTileID(2,2), 2 );
    brush.paintTile(3,2);
    BOOST_CHECK_EQUAL( tilemap.getTileID(2,2), 6 );
    BOOST_CHECK_EQUAL( tilemap.getTileID(3,2), 5 );
    brush.paintTile(3,3);
    BOOST_CHECK_EQUAL( tilemap.getTileID(3,2), 10 );
    BOOST_CHECK_EQUAL( tilemap.getTileID(3,3), 4 );
    brush.paintTile(2,3);
    BOOST_CHECK_EQUAL( tilemap.getTileID(2,2), 13 );
    BOOST_CHECK_EQUAL( tilemap.getTileID(3,2), 14 );
    BOOST_CHECK_EQUAL( tilemap.getTileID(3,3), 11 );
    BOOST_CHECK_EQUAL( tilemap.getTileID(2,3), 12 );
}

BOOST_AUTO_TEST_CASE( floodfill_test )
{
    {
        ungod::TileMap tilemap;
        //ungod::Image tex{ "test_data/tilemap_test.png" };
        ungod::MetaMap meta{ "test_data/tilemap_test.xml" };
        tilemap.setMetaMap(meta);
        tilemap.setTileDims(32, 32, {"geen", "red", "blue", "yellow"});
        ungod::TileData tiledata;
        tiledata.ids = std::make_shared<std::vector<int>>(std::initializer_list<int>{ 0,0,0,0,0,0,0,0,0,0,0,0 });
        tilemap.setTiles(tiledata, 4, 3);

        ungod::floodFill(tilemap, 1, 1, {1});

        BOOST_REQUIRE(tilemap.getTileID(0,0) != -1);
        BOOST_CHECK_EQUAL(1, tilemap.getTileID(2,2));
        BOOST_CHECK_EQUAL(1, tilemap.getTileID(0,2));
        BOOST_CHECK_EQUAL(1, tilemap.getTileID(2,1));
        BOOST_CHECK_EQUAL(1, tilemap.getTileID(0,0));
        BOOST_CHECK_EQUAL(1, tilemap.getTileID(3,2));
    }
}

BOOST_AUTO_TEST_CASE(tilemap_extend_test)
{
    ungod::TileMap tilemap;
    ungod::TileData tiledata;
    tiledata.ids = std::make_shared<std::vector<int>>(std::initializer_list<int>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
    tilemap.setTiles(tiledata, 4, 3);
    tilemap.extend(1, 1, 1, 1);
    BOOST_CHECK_EQUAL(tilemap.getMapSizeX(), 6);
    BOOST_CHECK_EQUAL(tilemap.getMapSizeY(), 5);
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(0,0));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(1,0));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(4,0));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(0,1));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(0,4));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(5,0));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(5,3));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(2,4));
    BOOST_CHECK_EQUAL(-1, tilemap.getTileID(3,4));
    BOOST_CHECK_EQUAL(0, tilemap.getTileID(2, 2));
    BOOST_CHECK_EQUAL(0, tilemap.getTileID(2, 1));
    BOOST_CHECK_EQUAL(0, tilemap.getTileID(3, 2));
}


BOOST_AUTO_TEST_SUITE_END()
