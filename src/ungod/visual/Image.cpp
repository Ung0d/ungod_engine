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

#include "boost/filesystem.hpp"
#include "ungod/visual/Image.h"
#include <SFML/OpenGL.hpp>

namespace ungod
{
    Image::Image(const std::string& filePath, const LoadPolicy policy, bool smooth, bool repeated) : Asset<sf::Texture>(filePath, policy, smooth, repeated) {}

    Image::Image() : Asset<sf::Texture>() {}

    void Image::load(const std::string filePath, const LoadPolicy policy, bool smooth, bool repeated)
    {
        Asset<sf::Texture>::load(filePath, policy, smooth, repeated);
    }

    bool LoadBehavior<sf::Texture, bool, bool>::loadFromFile(const std::string& filepath, sf::Texture& data, bool smooth, bool repeated)
    {
        bool success = data.loadFromFile(filepath);
        data.setSmooth(smooth);
        data.setRepeated(repeated);
        return success;
    }


    BigImage::BigImage(const std::string& filePath, const LoadPolicy policy, bool smooth, bool repeated) : Asset<sf::BigTexture>(filePath, policy, smooth, repeated) {}

    BigImage::BigImage() : Asset<sf::BigTexture>() {}

    void BigImage::load(const std::string filePath, const LoadPolicy policy, bool smooth, bool repeated)
    {
        Asset<sf::BigTexture>::load(filePath, policy, smooth, repeated);
    }

    bool LoadBehavior<sf::BigTexture, bool, bool>::loadFromFile(const std::string& filepath, sf::BigTexture& data, bool smooth, bool repeated)
    {
        bool success = data.loadFromFile(filepath);
        data.setSmooth(smooth);
        data.setRepeated(repeated);
        return success;
    }
}
