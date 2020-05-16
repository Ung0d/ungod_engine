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

#ifndef UNGOD_LIGHT_MANAGER_H
#define UNGOD_LIGHT_MANAGER_H

#include "ungod/visual/Light.h"
#include "ungod/visual/Image.h"
#include "owls/Signal.h"

namespace ungod
{
    /** \brief Manages shared resources for all LightHandlers. */
    class LightManager : sf::NonCopyable
    {
    public:
        LightManager(Application& app);

        /** \brief Updates the size of the underlying render-textures (e.g. if the window was resized). */
        void setImageSize(const sf::Vector2u& imageSize); 

        sf::RenderTexture& getLightTexture() { return mLightTexture; }
        sf::RenderTexture& getEmissionTexture() { return mEmissionTexture; }
        sf::RenderTexture& getAntumbraTexture() { return mAntumbraTexture; }
        sf::RenderTexture& getCompositionTexture() { return mCompositionTexture; }
        
        ~LightManager();

    private:
        sf::RenderTexture mLightTexture, mEmissionTexture, mAntumbraTexture, mCompositionTexture;
        sf::Shader mUnshadowShader, mLightOverShapeShader;
        ungod::Image mPenumbraTexture;
        owls::SignalLink<void, const sf::Vector2u&> mAppSignalLink;
    };
}

#endif //UNGOD_LIGHT_MANAGER_H
