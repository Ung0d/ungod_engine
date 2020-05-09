/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#ifndef UNGOD_IMAGE_H
#define UNGOD_IMAGE_H

#include "ungod/ressource_management/Asset.h"
#include <SFML/Graphics.hpp>
#include <sfml_ext/BigTexture.hpp>

namespace ungod
{
    class Image : public Asset<sf::Texture, bool, bool>
    {
    public:
        Image(const std::string& filePath, const LoadPolicy policy = LoadPolicy::SYNC, bool smooth = false, bool repeated = false);

        Image();

        void load(const std::string filePath, const LoadPolicy policy = LoadPolicy::SYNC, bool smooth = false, bool repeated = false);
    };

    template<>
    struct LoadBehavior<sf::Texture, bool, bool>
    {
        static bool loadFromFile(const std::string& filepath, sf::Texture& data, bool smooth, bool repeated);
        static std::string getIdentifier() { return "Image"; }
    };


    /** \brief For images that do not fit into the hardwares internal texture size. */
    class BigImage : public Asset<sf::BigTexture, bool, bool>
    {
    public:
        BigImage(const std::string& filePath, const LoadPolicy policy = LoadPolicy::SYNC, bool smooth = false, bool repeated = false);

        BigImage();

        void load(const std::string filePath, const LoadPolicy policy = LoadPolicy::SYNC, bool smooth = false, bool repeated = false);
    };

    template<>
    struct LoadBehavior<sf::BigTexture, bool, bool>
    {
        static bool loadFromFile(const std::string& filepath, sf::BigTexture& data, bool smooth, bool repeated);
        static std::string getIdentifier() { return "BigImage"; }
    };
}

#endif // UNGOD_IMAGE_H
