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

#ifndef GRASS_H
#define GRASS_H

#include "ungod/base/World.h"

namespace ungod
{
    class GrassPatch;

    using GrassBase = BaseComponents< TransformComponent, VertexArray, VisualsComponent, VisualAffectorComponent, RigidbodyComponent<>, Initializer, GrassPatch >;

    /** \brief A component that defines a patch of grass that reacts on actors trampling over it.
    * It also enables swaing in the wind. */
    class GrassPatch : Serializable<GrassPatch>
    {
    friend class GrassManager;
    public:
        GrassPatch() {}

    private:
        struct GrassData
        {
            std::size_t rectIndex;  ///index of the corresponding texture rect
            sf::Vector2f position;  ///local position with index of the corresponding tex rect in the vertex array
            std::string sheetKey;   //the key of the grass-sprite
            bool swayDirection;

            GrassData() : rectIndex(0), position(0,0), sheetKey(), swayDirection(false) {}

            GrassData(std::size_t cindex, const sf::Vector2f& cpos, const std::string& ckey) :
                rectIndex(cindex), position(cpos), sheetKey(ckey) {}
        };

    private:
        std::vector< GrassData > mGrassData;
        std::string mTrampledKey;
    };


    /** \brief A manager object of grass patches that handles collisions. */
    class GrassManager
    {
    public:
        GrassManager(World& world) : mWorld(&world) {}

        /** \brief Makes grass swaying in the wind. */
        void update(const std::list<Entity>& entities, float delta, float windForce);

        /** \brief Collision reaction callback. Sets the "trampled" texture. */
        void handleCollision(Entity trampling, Entity grass, const sf::Vector2f& vec, const Collider& collider, const Collider&);

        /** \brief Collision end reaction callback. Switches back to the "normal" texture. */
        void handleCollisionEnd(Entity trampling, Entity grass);

        /** \brief Inits a GrassPatch with uniformly distributed grasses inside the given bounds. */
        void initUniform(Entity grass, std::size_t grassDensity, float patchWidth, float patchHeight,
                         const std::string& imagePath, const std::vector<std::string>& grassKeys, const std::string& trampeledKey);

        /** \brief Inits a GrassPatch with normally distributed grasses around a center point with the
        * given distance.*/
        void initNormal(Entity grass, std::size_t grassDensity, float distance,
                         const std::string& imagePath, const std::vector<std::string>& grassKeys, const std::string& trampeledKey);

    private:
        World* mWorld;

    private:
        void init(Entity grass, const std::string& imagePath, const std::vector<std::string>& grassKeys,
                  const std::string& trampeledKey, std::size_t grassDensity);
    };
}

#endif // GRASS_H
