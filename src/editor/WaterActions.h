#ifndef UEDIT_WATER_ACTIONS_H
#define UEDIT_WATER_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/content/water/WaterHandler.h"

namespace uedit
{
    class ActionManager;

    class WaterActions
    {
    public:
        WaterActions(ActionManager& am) : mActionManager(am) {}

        void loadWaterShaders(ungod::Entity e, const std::string& distortionMap,
            const std::string& fragmentShader, const std::string& vertexShader);

        void setWaterReflections(ungod::Entity e, bool flag);

        void setWaterShaders(ungod::Entity e, bool flag);

        void setWaterDistortionFactor(ungod::Entity e, float distortion);

        void setWaterFlowFactor(ungod::Entity e, float flow);

        void setWaterReflectionOpacity(ungod::Entity e, float opacity);

    private:
        ActionManager& mActionManager;
        std::unordered_map<ungod::Entity, sf::Vector2f> mEntityMovements;
    };
}

#endif