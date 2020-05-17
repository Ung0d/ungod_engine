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

#ifndef WATER_H
#define WATER_H

#include <SFML/Graphics.hpp>
#include "ungod/serialization/SerialWater.h"

namespace ungod
{
    class TileMap;

    /** \brief A manager class for water-fields and their rendering. */
    class Water : public Serializable<Water>
    {
    friend struct SerialBehavior<Water>;
    friend struct DeserialBehavior<Water>;

    public:
        /** \brief Default constructs the water object for later initialization. */
        Water();

        /** \brief Renders shaders and reflections (if activated) on top of the given tilemap. */
        bool render(sf::RenderTarget& target, sf::RenderTexture& rendertex, const TileMap& tilemap, const sf::Texture* tilemapTex, sf::RenderStates states, World& world) const;

        /** \brief Provides appropriate resources to set up all internals of the water. */
        void init(const std::string& distortionTex, const std::string& fragmentShader, const std::string& vertexShader);

        /** \brief Fits to a new target size. */
        void targetsizeChanged(const sf::Vector2u& targetsize);

        /** \brief Activates or deactivates the rendering of reflections of nearby world-objects. */
        void setReflections(bool flag);

        /** \brief Activates or deactivates the rendering of the water shaders. */
        void setShaders(bool flag);

        /** \brief Sets the distortion-factor for the water shader. Controls the severity of the effect */
        void setDistortionFactor(float distortion);

        /** \brief Sets the rise-factor for the water shader. Controls the animation speed. */
        void setFlowFactor(float flow);

        /** \brief Sets the opacity decrease-factor for the reflections of entitys. Default value is 0.5f. */
        void setReflectionOpacity(float opacity);

        /** \brief Returns the filepath of the currently loaded distortion map. */
        const std::string& getDistortionMapID() const { return mDistortionTextureFile; }

        /** \brief Returns the filepath of the currently loaded fragment shader. */
        const std::string& getFragmentShaderID() const { return mFragmentShaderID; }

        /** \brief Returns the filepath of the currently loaded vertex shader. */
        const std::string& getVertexShaderID() const { return mVertexShaderID; }

    private:
        sf::Texture mDistortionTexture;
        std::string mDistortionTextureFile;
        sf::Shader mWaterShader;
        std::string mVertexShaderID;
        std::string mFragmentShaderID;

        sf::Vector2u mTargetSize;

        sf::Clock mDistortionTimer;

        bool mShowReflections;
        bool mShowShaders;

        float mDistortionFactor;
        float mFlowFactor;
        float mReflectionOpacity;

        //default values
        static constexpr float DEFAULT_DISTORTION = 0.01f;
        static constexpr float DEFAULT_FLOW = 0.1f;
        static constexpr float DEFAULT_REFLECTION_OPACITY = 0.5f;
        static constexpr float BOUNDING_BOX_SCALING = 3.0f;  ///< indicates how high the imaginary bounding box for the quad tree search is
        static constexpr float BOUNDS_OVERLAP = 0.95f;  ///< indicates how much the visuals and the reflected visuals of an entity overlap in y-direction, smaller <-> more overlap

    private:
        void buildDistortionTexture(const sf::Vector2u& texsize);
    };
}

#endif //WATER_H
