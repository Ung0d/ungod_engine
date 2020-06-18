#ifndef UEDIT_LIGHT_EDIT_STATE_H
#define UEDIT_LIGHT_EDIT_STATE_H

#include "EntityEditState.h"
#include "ungod/visual/Light.h"
#include "PointDragger.h"

namespace uedit
{
    class EntityLightWindow;

    template<>
    struct CompTraits<ungod::ShadowEmitterComponent>
    {
        static unsigned numPoints(const ungod::ShadowEmitterComponent& c)
        {
            return c.getCollider().getPointCount();
        }

        static void setup(ungod::Entity e, ungod::ShadowEmitterComponent& c, ActionManager& actionManager, std::list<PointDragger>& draggers)
        {
            for (unsigned i = 0; i < c.getCollider().getPointCount(); i++)
            {
                auto setter = [e, &c, i, &actionManager](const sf::Vector2f& p) mutable { actionManager.lightActions().setPoint(e, c, p, i); };
                auto getter = [&c, i]() { return c.getCollider().getPoint(i); };
                draggers.emplace_back(setter, getter);
            }
        }
    };


    /** \brief A state for visual editing. */
    class LightEditState : public EditState
    {
    public:
        LightEditState(EntityPreview& preview,const EntityLightWindow& lightWindow);

        void selectLight();
        void deselectLight();

        void selectMultiLight(std::size_t i);
        void deselectMultiLight(std::size_t i);

        void selectCollider();
        void deselectCollider();

        void selectMultiCollider(std::size_t i);
        void deselectMultiCollider(std::size_t i);


        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;

        ~LightEditState();

    private:
        EntityPreview& mPreview;
        bool mMouseDown;
        bool mCtrlDown;
        bool mShiftDown;
        bool mPointSet;
        sf::Vector2i mLastMouse;
        bool mLightSelected;
        std::vector< std::size_t > mSelectedMultiLights;
        bool mColliderSelected;
        std::vector< std::size_t > mSelectedMultiColliders;
        const EntityLightWindow& mEntityLightWindow;
        PointDraggerSet<ungod::ShadowEmitterComponent> mShadowEmitterDraggers;
        owls::SignalLink<void, ungod::Entity, const sf::FloatRect&> mLink;

        static constexpr float DRAG_DISTANCE = 20.0f;

    private:
        void renderLightSelection(sf::RenderWindow& window, sf::RenderStates states, const sf::FloatRect& bounds);
        void renderColliderSelection(sf::RenderWindow& window, sf::RenderStates states, sf::ConvexShape shape);
    };

}

#endif // UEDIT_LIGHT_EDIT_STATE_H
