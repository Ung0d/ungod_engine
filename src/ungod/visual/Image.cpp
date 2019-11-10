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
    ImageQuality Image::quality = IMAGE_QUALITY_NORMAL;

    Image::Image(const std::string& filePath, const LoadPolicy policy) : Asset<sf::Texture>(filePath, policy) {}

    Image::Image() : Asset<sf::Texture>() {}

    bool LoadBehavior<sf::Texture>::loadFromFile(const std::string& filepath, sf::Texture& data)
    {
        //sf::Context context;
        bool success=false;
        if (Image::quality == IMAGE_QUALITY_NORMAL)
        {
            success = data.loadFromFile(filepath);
        }
        else
        {
            boost::filesystem::path normalPath(filepath);
            boost::filesystem::path lowPath = normalPath.parent_path() / boost::filesystem::path(normalPath.stem().string() + "_low" + normalPath.extension().string());
            if ( boost::filesystem::exists(lowPath) )
                success = data.loadFromFile(lowPath.string());
            else
                success = data.loadFromFile(filepath);  //give the normal version a try
        }
        return success;
    }




    BigImage::BigImage(const std::string& filePath, const LoadPolicy policy) : Asset<sf::BigTexture>(filePath, policy) {}

    BigImage::BigImage() : Asset<sf::BigTexture>() {}

    bool LoadBehavior<sf::BigTexture>::loadFromFile(const std::string& filepath, sf::BigTexture& data)
    {
        //sf::Context context;
        bool success;
        if (Image::quality == IMAGE_QUALITY_NORMAL)
        {
            success = data.loadFromFile(filepath);
        }
        else
        {
            boost::filesystem::path normalPath(filepath);
            boost::filesystem::path lowPath = normalPath.parent_path() / boost::filesystem::path(normalPath.stem().string() + "_low" + normalPath.extension().string());
            if ( boost::filesystem::exists(lowPath) )
                success = data.loadFromFile(lowPath.string());
            else
                success = data.loadFromFile(filepath);  //give the normal version a try
        }
        //glFlush();
        return success;
    }
}
