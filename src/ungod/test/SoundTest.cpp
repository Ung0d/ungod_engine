#include <boost/test/unit_test.hpp>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"

BOOST_AUTO_TEST_SUITE(AudioTest)

BOOST_AUTO_TEST_CASE( sound_test )
{
    /*ungod::AssetManager assetmanager;
    ungod::BaseAsset::setManager(assetmanager);

    {
    ungod::Sound sound;
    sound.load("sound.wav");

    ungod::World world(0,0,800,600, 800, 600);

    world.getAudioManager().initMusic(1);
    world.getAudioManager().loadMusic("music.wav", 0);
    world.getAudioManager().playMusic(0);

    world.getAudioManager().onSoundBegin([] (std::string profile, std::size_t index)
                                         {
                                            ungod::Logger::info("Playing sound: ");
                                            ungod::Logger::info(profile);
                                            ungod::Logger::info(" ");
                                            ungod::Logger::info(index);
                                            ungod::Logger::endl();
                                         });

    world.getAudioManager().onSoundEnd([] (std::string profile, std::size_t index)
                                         {
                                            ungod::Logger::info("End of sound: ");
                                            ungod::Logger::info(profile);
                                            ungod::Logger::info(" ");
                                            ungod::Logger::info(index);
                                            ungod::Logger::endl();
                                         });

    auto profile = world.getAudioManager().initSoundProfile("test");

    ungod::Entity e = world.create(ungod::BaseComponents<ungod::Transform>(), ungod::OptionalComponents<ungod::SoundEmitter>());
    e.add( world.getUniverse().instantiate<ungod::SoundEmitter>(profile) );

    world.getAudioManager().initSounds(profile, 1);
    world.getAudioManager().loadSound(profile, "sound.wav", 0);

    BOOST_CHECK_EQUAL(sound.getRefCount(), 2u);

    world.getQuadTree().insert(e);

    sf::RenderWindow window(sf::VideoMode(800, 600), "sound_text press '1' and '2' for music fading, press '3' to emit sound");

    //register some input callbacks
    world.getInputManager().addBinding("1", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Num1);
    world.getInputManager().addBinding("2", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Num2);
    world.getInputManager().addBinding("3", ungod::InputManager::KEYBOARD_INPUT, sf::Keyboard::Num3);

    world.getInputManager().onPressed([&world, e] (const std::string& key)
      {
            if (key == "1")
            {
                ungod::Logger::info("Music fadein");
                ungod::Logger::endl();
                world.getAudioManager().fadeinMusic(500.0f, 0);
            }
            else if (key == "2")
            {
                ungod::Logger::info("Music fadeout");
                ungod::Logger::endl();
                world.getAudioManager().fadeoutMusic(500.0f, 0);
            }
            else
            {
                ungod::Logger::info("Soundeffect");
                ungod::Logger::endl();
                world.getAudioManager().playSound(e, 0);
            }
      });

    sf::Clock timer;
    float delta = 0;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            world.handleEvent(event);
        }

        delta = timer.restart().asMicroseconds()/1000.0f;

        world.update(delta, window.mapPixelToCoords(sf::Vector2i(0,0)), {(float)window.getSize().x, (float)window.getSize().y});

        world.render(window);

        window.display();
        window.clear(sf::Color::Black);
    }

    world.getAudioManager().expireSounds(profile);

    BOOST_CHECK_EQUAL(sound.getRefCount(), 1u);
    } */
}

BOOST_AUTO_TEST_SUITE_END()
