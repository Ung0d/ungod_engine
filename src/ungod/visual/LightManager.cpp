/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This is a modified version of the Let There Be Light 2 framework.
* See https://github.com/222464/LTBL2
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

#include "ungod/visual/LightManager.h"
#include "ungod/application/Application.h"

namespace ungod
{
    LightManager::LightManager(Application& app) 
    {
        setImageSize(app.getWindow().getSize());
        mAppSignalLink = app.onTargetSizeChanged([this](const sf::Vector2u& targetsize)
            {
                setImageSize(targetsize);
            });

        auto unshadowVertex = app.getConfig().get<std::string>("light/unshadow_vertex_shader");
        auto unshadowFrag = app.getConfig().get<std::string>("light/unshadow_frag_shader");
        auto lightVertex = app.getConfig().get<std::string>("light/light_vertex_shader");
        auto lightFrag = app.getConfig().get<std::string>("light/light_frag_shader");
        auto penumbraTexture = app.getConfig().get<std::string>("light/default_penumbra_texture");
        if (!(unshadowVertex && unshadowFrag && lightVertex && lightFrag && penumbraTexture))
            Logger::error("Can not instantiate the light system, because the config file seems to be corrupted. \n Consider deleting it and let the application generate a fresh one.");
        else
        {
            mUnshadowShader.loadFromFile(unshadowVertex.value(), unshadowFrag.value());
            mLightOverShapeShader.loadFromFile(lightVertex.value(), lightFrag.value());


            mPenumbraTexture.load(penumbraTexture.value(), LoadPolicy::ASYNC, true);
            if (mPenumbraTexture.isLoaded())
                mUnshadowShader.setUniform("penumbraTexture", mPenumbraTexture.get());
            else
                ungod::Logger::warning("No valid penumbra texture loaded!");
        }
    }


    void LightManager::setImageSize(const sf::Vector2u& imageSize)
    {
        mLightTexture.create(imageSize.x, imageSize.y);
        mEmissionTexture.create(imageSize.x, imageSize.y);
        mAntumbraTexture.create(imageSize.x, imageSize.y);
        mCompositionTexture.create(imageSize.x, imageSize.y);
        mLightOverShapeShader.setUniform("targetSizeInv", sf::Glsl::Vec2(1.0f / imageSize.x, 1.0f / imageSize.y));
        mLightOverShapeShader.setUniform("emissionTexture", mEmissionTexture.getTexture());
    }


    LightManager::~LightManager()
    {
        mAppSignalLink.disconnect();
    }
}
