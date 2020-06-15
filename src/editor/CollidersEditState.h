#ifndef UEDIT_COLLIDERS_EDIT_STATE_H
#define UEDIT_COLLIDERS_EDIT_STATE_H

#include "ungod/base/Transform.h"
#include "ungod/physics/CollisionHandler.h"
#include "EntityEditState.h"
#include "EntityPreview.h"
#include "PointDragger.h"
#include "EntityCollidersWindow.h"

namespace uedit
{
    template<std::size_t CONTEXT>
    struct CompTraits<ungod::RigidbodyComponent<CONTEXT>>
    {
        static unsigned numPoints(const ungod::RigidbodyComponent<CONTEXT>& c)
        {
            switch (c.getCollider().getType())
            {
            case ungod::ColliderType::ROTATED_RECT:
                return 4u;
            case ungod::ColliderType::CONVEX_POLYGON: case ungod::ColliderType::EDGE_CHAIN:
                return ungod::PointSetConstAggregator{ c.getCollider() }.getNumberOfPoints();
            default:
                return 0u;
            }
        }

        static void setup(ungod::Entity e, ungod::RigidbodyComponent<CONTEXT>& c, WorldActionWrapper& waw, std::list<PointDragger>& draggers)
        {
            switch (c.getCollider().getType())
            {
            case ungod::ColliderType::ROTATED_RECT:
            {
                ungod::RotatedRectConstAggregator rrca{ c.getCollider() };
                auto ulsetter = [e, &c, rrca, &waw](const sf::Vector2f& p) mutable
                { 
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    waw.setRectUpLeft(e, c, t.getInverse().transformPoint(p)); 
                };
                auto ulgetter = [rrca]() 
                { 
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    return t.transformPoint({rrca.getUpLeftX(), rrca.getUpLeftY()}); 
                };
                draggers.emplace_back(ulsetter, ulgetter);
                auto ursetter = [e, rrca, &c, &waw](const sf::Vector2f& p) mutable 
                {
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    waw.setRectUpLeft(e, c, t.getInverse().transformPoint({ rrca.getUpLeftX(), p.y }));
                    waw.setRectDownRight(e, c, t.getInverse().transformPoint({ p.x, rrca.getDownRightY() }));
                };
                auto urgetter = [rrca]() 
                {
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    return t.transformPoint({ rrca.getDownRightX(), rrca.getUpLeftY() });
                };
                draggers.emplace_back(ursetter, urgetter);
                auto drsetter = [e, rrca, &c, &waw](const sf::Vector2f& p) mutable
                {
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    waw.setRectDownRight(e, c, t.getInverse().transformPoint(p));
                };
                auto drgetter = [rrca]() 
                {
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    return t.transformPoint({ rrca.getDownRightX(), rrca.getDownRightY() });
                };
                draggers.emplace_back(drsetter, drgetter);
                auto dlsetter = [e, rrca, &c, &waw](const sf::Vector2f& p) mutable
                {
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    waw.setRectUpLeft(e, c, t.getInverse().transformPoint({ p.x, rrca.getUpLeftY() }));
                    waw.setRectDownRight(e, c, t.getInverse().transformPoint({ rrca.getDownRightX(), p.y }));
                };
                auto dlgetter = [rrca]() 
                {
                    sf::Transform t;
                    t.rotate(rrca.getRotation(), rrca.getCenter());
                    return t.transformPoint({ rrca.getUpLeftX(), rrca.getDownRightY() });
                };
                draggers.emplace_back(dlsetter, dlgetter);
                break;
            }
            case ungod::ColliderType::CONVEX_POLYGON: case ungod::ColliderType::EDGE_CHAIN:
            {
                ungod::PointSetConstAggregator psca{ c.getCollider() };
                for (unsigned i = 0; i < psca.getNumberOfPoints(); i++)
                {
                    auto setter = [e, &c, i, &waw](const sf::Vector2f& p) mutable { waw.setColliderPoint(e, c, p, i); };
                    auto getter = [psca, i]() { return sf::Vector2f{ psca.getPointX(i), psca.getPointY(i) }; };
                    draggers.emplace_back(setter, getter);
                }
                break;
            }
            default:
                break;
            }
        }
    };

    /** \brief A state for colliders editing. */
    template<std::size_t CONTEXT = 0>
    class CollidersEditState : public EditState
    {
    public:
        CollidersEditState(EntityPreview& preview, EntityCollidersWindow<CONTEXT>& colliderWindow);

        void selectMultiCollider(std::size_t i);

        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;

        ~CollidersEditState();

    private:
        EntityPreview& mPreview;
        sf::Vector2i mLastMouse;
        bool mSingleSelected;
        std::vector< std::size_t > mSelectedMultiColliders;
        bool mMousePressed;
        PointDraggerSet<ungod::RigidbodyComponent<CONTEXT>> mColliderPointDraggers;
        owls::SignalLink<void, ungod::Entity, const sf::FloatRect&> mLink;
        EntityCollidersWindow<CONTEXT>& mCollidersWindow;
    };


    template<std::size_t CONTEXT>
    CollidersEditState<CONTEXT>::CollidersEditState(EntityPreview& preview, EntityCollidersWindow<CONTEXT>& colliderWindow) :
        mPreview(preview), mSingleSelected(false), mMousePressed(false), mColliderPointDraggers(mPreview.getWorldAction()), mCollidersWindow(colliderWindow)
    {
        mLastMouse = sf::Mouse::getPosition(preview.mWindow);

        mColliderPointDraggers.setupPointDraggers(mPreview.getEntity());
        if constexpr (CONTEXT == ungod::MOVEMENT_COLLISION_CONTEXT)
            mLink = mPreview.getEntity().getWorld().getMovementRigidbodyHandler().onContentsChanged([this](ungod::Entity e, const sf::FloatRect&)
                {
                    mColliderPointDraggers.notifyChange(mPreview.getEntity());
                });
        else if constexpr (CONTEXT == ungod::SEMANTICS_COLLISION_CONTEXT)
            mLink = mPreview.getEntity().getWorld().getSemanticsRigidbodyHandler().onContentsChanged([this](ungod::Entity e, const sf::FloatRect&)
                {
                    mColliderPointDraggers.notifyChange(mPreview.getEntity());
                });
    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::selectMultiCollider(std::size_t i)
    {
        if (std::find(mSelectedMultiColliders.begin(), mSelectedMultiColliders.end(), i) == mSelectedMultiColliders.end())
            mSelectedMultiColliders.emplace_back(i);
        else
            mSelectedMultiColliders.erase(std::remove(mSelectedMultiColliders.begin(), mSelectedMultiColliders.end(), i), mSelectedMultiColliders.end());
    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        if (mColliderPointDraggers.handleEvent(preview, event))
            return;

        switch (event.type)
        {
            case sf::Event::MouseButtonPressed:
            {
                if (!preview.mEntity.has<ungod::TransformComponent>())
                    return;
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mLastMouse = sf::Mouse::getPosition(preview.mWindow);
                    if (!mMousePressed)
                    {
                        sf::Vector2f mouseWorldPos = preview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(preview.mWindow), preview.mCamera.getView());
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                        {
                            if (preview.mEntity.has<ungod::RigidbodyComponent<CONTEXT>>())
                            {
                                auto& body = preview.mEntity.modify<ungod::RigidbodyComponent<CONTEXT>>();
                                if (ungod::containsPoint(body.getCollider(), preview.mEntity.get<ungod::TransformComponent>(), mouseWorldPos))
                                    mSingleSelected = !mSingleSelected;
                            }
                            if (preview.mEntity.has<ungod::MultiRigidbodyComponent<CONTEXT>>())
                            {
                                auto& mbody = preview.mEntity.modify<ungod::MultiRigidbodyComponent<CONTEXT>>();
                                for (unsigned i = 0; i < mbody.getComponentCount(); i++)
                                {
                                    if (ungod::containsPoint(mbody.getComponent(i).getCollider(), preview.mEntity.get<ungod::TransformComponent>(), mouseWorldPos))
                                        selectMultiCollider(i);
                                }
                            }
                        }
                        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        {
                            sf::Vector2f mousePosLocal = preview.getEntity().get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(mouseWorldPos);
                            if (mCollidersWindow.getActiveComponent() && 
                                (mCollidersWindow.getActiveComponent()->getCollider().getType() == ungod::ColliderType::CONVEX_POLYGON ||
                                mCollidersWindow.getActiveComponent()->getCollider().getType() == ungod::ColliderType::EDGE_CHAIN))
                                preview.mWorldAction.addColliderPoint(preview.mEntity, *mCollidersWindow.getActiveComponent(), mousePosLocal);
                        }
                    }
                    mMousePressed = true;
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                    mMousePressed = false;
            }
            case sf::Event::MouseMoved:
            {
                if (mMousePressed)
                {
                    sf::Vector2f worldPos = preview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(preview.mWindow), preview.mCamera.getView());
                    sf::Vector2f worldPosLast = preview.mWindow.mapPixelToCoords(mLastMouse, preview.mCamera.getView());
                    worldPos = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldPos);
                    worldPosLast = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldPosLast);

                    sf::Vector2f offset = worldPos - worldPosLast;

                    //shift down? yes -> move no? -> rotate
                    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    {
                        if (mSingleSelected)
                            preview.mWorldAction.moveCollider(preview.mEntity, offset);
                        for (auto c : mSelectedMultiColliders)
                            preview.mWorldAction.moveCollider(preview.mEntity, c, offset);
                    }
                    else 
                    {
                        if (mSingleSelected && 
                            preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider().getType() == ungod::ColliderType::ROTATED_RECT)
                        {
                            if (offset.y > 0)
                                preview.mWorldAction.rotateRect(preview.mEntity, -ungod::magnitude(offset));
                            else
                                preview.mWorldAction.rotateRect(preview.mEntity, ungod::magnitude(offset));
                        }
                        for (auto c : mSelectedMultiColliders)
                        {
                            if (preview.mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(c).getCollider().getType() == ungod::ColliderType::ROTATED_RECT)
                            {
                                if (offset.y > 0)
                                    preview.mWorldAction.rotateRect(preview.mEntity, c, -ungod::magnitude(offset));
                                else
                                    preview.mWorldAction.rotateRect(preview.mEntity, c, ungod::magnitude(offset));
                            }
                        }

                    }

                    mLastMouse = sf::Mouse::getPosition(preview.mWindow);
                }
                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                preview.mCamera.setZoom( preview.mCamera.getZoom() - event.mouseWheelScroll.delta * 0.1f );
                break;
            }
            default: break;
        }
    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::update(EntityPreview& preview, float delta)
    {

    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        if (!preview.mEntity.has<ungod::TransformComponent>())
            return;

        preview.getCanvas().getEditorState()->getRenderer().renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);
        if (preview.mEntity.has<ungod::RigidbodyComponent<CONTEXT>>())
            preview.getCanvas().getEditorState()->getRenderer().renderCollider<CONTEXT>(preview.mEntity.get<ungod::TransformComponent>(),
                                                    preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>(), window, states, sf::Color::Blue);
        if (preview.mEntity.has<ungod::MultiRigidbodyComponent<CONTEXT>>())
            for (unsigned i = 0; i < preview.mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponentCount(); i++)
                preview.getCanvas().getEditorState()->getRenderer().renderCollider<CONTEXT>(preview.mEntity.get<ungod::TransformComponent>(),
                    preview.mEntity.get < ungod::MultiRigidbodyComponent<CONTEXT >> ().getComponent(i), window, states, sf::Color::Blue);

        if (mSingleSelected)
            preview.getCanvas().getEditorState()->getRenderer().renderCollider<CONTEXT>(preview.mEntity.get<ungod::TransformComponent>(),
                preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>(), window, states, sf::Color::Red, sf::Color(255, 0, 0, 70));

        for (const auto& i : mSelectedMultiColliders)
            preview.getCanvas().getEditorState()->getRenderer().renderCollider<CONTEXT>(preview.mEntity.get<ungod::TransformComponent>(),
                preview.mEntity.get < ungod::MultiRigidbodyComponent<CONTEXT >>().getComponent(i), window, states, sf::Color::Red, sf::Color(255, 0, 0, 70));

        states.transform *= preview.mEntity.get<ungod::TransformComponent>().getTransform();
        mColliderPointDraggers.render(window, states);
    }

    template<std::size_t CONTEXT>
    CollidersEditState<CONTEXT>::~CollidersEditState()
    {
        mLink.disconnect();
    }
}


#endif // UEDIT_COLLIDERS_EDIT_STATE_H
