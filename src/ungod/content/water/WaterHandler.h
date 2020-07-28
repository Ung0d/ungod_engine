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

#ifndef UNGOD_WATER_HANDLER_H
#define UNGOD_WATER_HANDLER_H

#include "ungod/content/water/Water.h"
#include "ungod/base/Entity.h"
#include "owls/Signal.h"

namespace ungod
{
    class World;
    class Camera;

    /**
    * \ingroup Components
    * \brief A component that manages a water-field.
    */
    class WaterComponent : public Serializable<WaterComponent>
    {
    friend class WaterHandler;
    friend class Renderer;
    friend struct DeserialBehavior<WaterComponent, Entity, DeserialMemory&>;

    public:
        WaterComponent() = default;

        const Water& getWater() const { return mWater; }
    private:
        Water mWater;
    };

    /** \brief A class that manages water-entities. */
    class WaterHandler
    {
    public:
        WaterHandler() = default;

        void init(World & world);

        void update(const std::list<Entity>& entities);

        inline static void initWater(Entity e, const std::string& distortionTex, const std::string& fragmentShader, const std::string& vertexShader)
        { initWater(e.modify<WaterComponent>(), distortionTex, fragmentShader, vertexShader); }
        static void initWater(WaterComponent& water, const std::string& distortionTex, const std::string& fragmentShader, const std::string& vertexShader);

        /** \brief Adds a world of which nearby world-objects are pulled to draw reflections. */
        inline static bool addReflectionWorld(Entity e, World* world)
        { return addReflectionWorld(e.modify<WaterComponent>(), world); }
        static bool addReflectionWorld(WaterComponent& water, World* world);

        /** \brief Removes a previously added reflection world. */
        inline static bool removeReflectionWorld(Entity e, World* world)
        {
            return removeReflectionWorld(e.modify<WaterComponent>(), world);
        }
        static bool removeReflectionWorld(WaterComponent& water, World* world);

        /** \brief Activates or deactivates the rendering of the water shaders. */
        inline static void setWaterShaders(Entity e, bool flag)
        { setWaterShaders(e.modify<WaterComponent>(), flag); }
        static void setWaterShaders(WaterComponent& water, bool flag);

        /** \brief Sets the distortion-factor for the water shader. Controls the severity of the effect */
        inline static void setWaterDistortionFactor(Entity e, float distortion)
        { setWaterDistortionFactor(e.modify<WaterComponent>(), distortion); }
        static void setWaterDistortionFactor(WaterComponent& water, float distortion);

        /** \brief Sets the rise-factor for the water shader. Controls the animation speed. */
        inline static void setWaterFlowFactor(Entity e, float flow)
        { setWaterFlowFactor(e.modify<WaterComponent>(), flow); }
        static void setWaterFlowFactor(WaterComponent& water, float flow);

        /** \brief Sets the opacity decrease-factor for the reflections of entitys. Default value is 0.5f. */
        inline static void setWaterReflectionOpacity(Entity e, float opacity)
        { setWaterReflectionOpacity(e.modify<WaterComponent>(), opacity); }
        static void setWaterReflectionOpacity(WaterComponent& water, float opacity);

        void targetSizeChanged(const World& world, const sf::Vector2u& targetsize);

        ~WaterHandler();

    private:
        owls::SignalLink<void, const sf::Vector2u&> mTargetSizeLink; 

    private:
    };
}

#endif // UNGOD_WATER_HANDLER_H

