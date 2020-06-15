#include "WaterActions.h"
#include "ActionManager.h"

namespace uedit
{
    void WaterActions::loadWaterShaders(ungod::Entity e, const std::string& distortionMap,
        const std::string& fragmentShader, const std::string& vertexShader)
    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        std::string oldDist = w.getDistortionMapID();
        std::string oldFrag = w.getFragmentShaderID();
        std::string oldVert = w.getVertexShaderID();

        mEFrame->action(std::function([this, distortionMap, fragmentShader, vertexShader](ungod::Entity e)
            { e.getWorld().getTileMapRenderer().loadWaterShaders(e, distortionMap, fragmentShader, vertexShader); }),
            std::function([this, oldDist, oldFrag, oldVert](ungod::Entity e)
                { e.getWorld().getTileMapRenderer().loadWaterShaders(e, oldDist, oldFrag, oldVert); }),
            e);
    }

    void WaterActions::setWaterReflections(ungod::Entity e, bool flag)
    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        bool old = w.isShowingReflections();
        mActionManager.action(std::function([this, flag](ungod::Entity e)
            { e.getWorld().getWaterHandler().setWaterReflections(e, flag); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getWaterHandler().setWaterReflections(e, old); }),
            e);
    }

    void WaterActions::setWaterShaders(ungod::Entity e, bool flag)
    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        bool old = w.isShowingShaders();
        mActionManager.action(std::function([this, flag](ungod::Entity e)
            { e.getWorld().getWaterHandler().setWaterShaders(e, flag); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getWaterHandler().setWaterShaders(e, old); }),
            e);
    }

    void WaterActions::setWaterDistortionFactor(ungod::Entity e, float distortion)
    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        float old = w.getDistortionFactor();
        mActionManager.action(std::function([this, distortion](ungod::Entity e)
            { e.getWorld().getWaterHandler().setWaterDistortionFactor(e, distortion); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getWaterHandler().setWaterDistortionFactor(e, old); }),
            e);
    }

    void WaterActions::setWaterFlowFactor(ungod::Entity e, float flow)
    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        float old = w.getFlowFactor();
        mActionManager.action(std::function([this, flow](ungod::Entity e)
            { e.getWorld().getWaterHandler().setWaterFlowFactor(e, flow); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getWaterHandler().setWaterFlowFactor(e, old); }),
            e);
    }

    void WaterActions::setWaterReflectionOpacity(ungod::Entity e, float opacity)
    {
        const ungod::Water& w = e.get<ungod::WaterComponent>().getWater();
        float old = w.getReflectionOpacity();
        mActionManager.action(std::function([this, opacity](ungod::Entity e)
            { e.getWorld().getWaterHandler().setWaterReflectionOpacity(e, opacity); }),
            std::function([this, old](ungod::Entity e)
                { e.getWorld().getWaterHandler().setWaterReflectionOpacity(e, old); }),
            e);
    }
}