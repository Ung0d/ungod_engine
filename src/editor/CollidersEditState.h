#ifndef UEDIT_COLLIDERS_EDIT_STATE_H
#define UEDIT_COLLIDERS_EDIT_STATE_H

#include "ungod/base/Transform.h"
#include "ungod/physics/Collision.h"
#include "EntityEditState.h"
#include "EntityPreview.h"

namespace uedit
{
    /** \brief A state for colliders editing. */
    template<std::size_t CONTEXT = 0>
    class CollidersEditState : public EditState
    {
    public:
        CollidersEditState(EntityPreview& preview);

        void selectCollider(std::size_t i);
        void deselectCollider(std::size_t i);

        virtual void handleInput(EntityPreview& preview, const sf::Event& event) override;
        virtual void update(EntityPreview& preview, float delta) override;
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;

    private:
        EntityPreview& mPreview;
        bool mMouseDown;
        bool mCtrlDown;
        bool mShiftDown;
        bool mUpleftGrabbed;
        bool mDownrightGrabbed;
        sf::Vector2i mLastMouse;
        bool mSingleSelected;
        std::vector< std::size_t > mSelectedMultiColliders;

        static constexpr float CORNERDIST = 20.0f;
    };


    template<std::size_t CONTEXT>
    CollidersEditState<CONTEXT>::CollidersEditState(EntityPreview& preview) :
        mPreview(preview), mMouseDown(false), mCtrlDown(false), mShiftDown(false), mSingleSelected(false)
    {
        mLastMouse = sf::Mouse::getPosition(preview.mWindow);
    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::selectCollider(std::size_t i)
    {
        mSelectedColliders.emplace_back(i);
    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::deselectCollider(std::size_t i)
    {
        mSelectedColliders.erase(std::remove(mSelectedColliders.begin(), mSelectedColliders.end(), i), mSelectedColliders.end());
    }

    template<std::size_t CONTEXT>
    void CollidersEditState<CONTEXT>::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        switch (event.type)
        {
            case sf::Event::MouseButtonPressed:
            {
                if (!preview.mEntity.has<ungod::TransformComponent>())
                    return;
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mMouseDown = true;
                    mLastMouse = sf::Mouse::getPosition(preview.mWindow);

                    ungod::RigidbodyComponent<CONTEXT>& body = preview.mEntity.modify<ungod::RigidbodyComponent<CONTEXT>>();
                    sf::Vector2f mouseWorldPos = preview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(preview.mWindow), preview.mCamera.getView());
                    if (mCtrlDown)
                    {
                        std::size_t i = 0;
                        for (const auto& c : body.getColliders())
                        {
                            if (ungod::containsPoint(c, preview.mEntity.get<ungod::TransformComponent>(), mouseWorldPos))
                            {
                                if (std::find(mSelectedColliders.begin(), mSelectedColliders.end(), i) == mSelectedColliders.end())
                                    selectCollider(i);
                                else
                                    deselectCollider(i);
                            }
                            ++i;
                        }
                    }
                    else
                    {
                        std::size_t i = 0;
                        for (const auto& c : body.getColliders())
                        {
                            sf::Transform transf = preview.mEntity.get<ungod::TransformComponent>().getTransform();
                            transf.rotate(c.getRotation(), c.getCenter());
                            sf::Vector2f upleft = transf.transformPoint(c.getUpleft());
                            sf::Vector2f downright = transf.transformPoint(c.getDownright());

                            if (ungod::distance(upleft, mouseWorldPos) < CORNERDIST)
                                mUpleftGrabbed = true;
                            else if (ungod::distance(downright, mouseWorldPos) < CORNERDIST)
                                mDownrightGrabbed = true;
                            ++i;
                        }
                    }
                }

                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mMouseDown = false;
                    mUpleftGrabbed = false;
                    mDownrightGrabbed = false;
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                if (mMouseDown)
                {
                    sf::Vector2f worldPos = preview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(preview.mWindow), preview.mCamera.getView());
                    sf::Vector2f worldPosLast = preview.mWindow.mapPixelToCoords(mLastMouse, preview.mCamera.getView());
                    worldPos = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldPos);
                    worldPosLast = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(worldPosLast);

                    sf::Vector2f offset = worldPos - worldPosLast;

                    //shift down? yes -> move no? -> rotate
                    if (!mShiftDown)
                    {
                        if (mUpleftGrabbed)
                        {
                            for (const auto& c : mSelectedColliders)
                            {
                                auto ul = preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[c].getUpleft();
                                ul.x += offset.x;
                                ul.y += offset.y;
                                preview.mWorldAction.setColliderUpLeft( preview.mEntity, c, ul );
                            }
                        }
                        else if (mDownrightGrabbed)
                        {
                            for (const auto& c : mSelectedColliders)
                            {
                                auto dr = preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[c].getDownright();
                                dr.x += offset.x;
                                dr.y += offset.y;
                                preview.mWorldAction.setColliderDownRight( preview.mEntity, c, dr );
                            }
                        }
                        else
                        {
                            for (const auto& c : mSelectedColliders)
                            {
                                auto dr = preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[c].getDownright();
                                dr.x += offset.x;
                                dr.y += offset.y;
                                preview.mWorldAction.setColliderDownRight( preview.mEntity, c, dr );

                                auto ul = preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[c].getUpleft();
                                ul.x += offset.x;
                                ul.y += offset.y;
                                preview.mWorldAction.setColliderUpLeft( preview.mEntity, c, ul );
                            }
                        }
                    }
                    else 
                    {
                        if (mSingleSelected)
                        {
                            if (offset.y > 0)
                                preview.mWorldAction.rotateRect(preview.mEntity, -ungod::magnitude(offset));
                            else
                                preview.mWorldAction.rotateRect(preview.mEntity, ungod::magnitude(offset));
                        }
                        for (const auto& c : mSelectedColliders)
                        {
                            if (offset.y > 0)
                                preview.mWorldAction.rotateRect( preview.mEntity, c, -ungod::magnitude(offset) );
                            else
                                preview.mWorldAction.rotateRect( preview.mEntity, c, ungod::magnitude(offset) );
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
            case sf::Event::KeyPressed:
            {
                if (event.key.code == sf::Keyboard::LControl)
                    mCtrlDown = true;
                else if (event.key.code == sf::Keyboard::LShift)
                    mShiftDown = true;
                break;
            }
            case sf::Event::KeyReleased:
            {
                if (event.key.code == sf::Keyboard::LControl)
                    mCtrlDown = false;
                else if (event.key.code == sf::Keyboard::LShift)
                    mShiftDown = false;
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

        ungod::Renderer::renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);
        ungod::Renderer::renderColliders<CONTEXT>(preview.mEntity.get<ungod::TransformComponent>(), preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>(), window, states, sf::Color::Red);

        states.transform *= preview.mEntity.get<ungod::TransformComponent>().getTransform();

        for (const auto& i : mSelectedColliders)
        {
          sf::RenderStates locstates = states;
          locstates.transform.rotate(preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getRotation(),
                                     preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getCenter());   //apply the rotation of the collider
          sf::RectangleShape rect( preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getDownright() -
                                   preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getUpleft() );
          rect.setPosition( preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getUpleft() );
          rect.setFillColor(sf::Color(255, 0, 0, 70));
          rect.setOutlineThickness(3);
          rect.setOutlineColor(sf::Color::Red);
          window.draw(rect, locstates);

          sf::CircleShape upleft(CORNERDIST);
          upleft.setPosition(preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getUpleft()-sf::Vector2f{CORNERDIST, CORNERDIST});
          upleft.setFillColor(sf::Color(255, 0, 0, 70));
          upleft.setOutlineThickness(3);
          upleft.setOutlineColor(sf::Color::Red);
          window.draw(upleft, locstates);

          sf::CircleShape downright(CORNERDIST);
          downright.setPosition(preview.mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[i].getDownright()-sf::Vector2f{CORNERDIST, CORNERDIST});
          downright.setFillColor(sf::Color(255, 0, 0, 70));
          downright.setOutlineThickness(3);
          downright.setOutlineColor(sf::Color::Red);
          window.draw(downright, locstates);
        }
    }
}

#endif // UEDIT_COLLIDERS_EDIT_STATE_H
