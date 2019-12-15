#include "LightEditState.h"
#include "EntityPreview.h"
#include "EntityLightWindow.h"

namespace uedit
{
    LightEditState::LightEditState(EntityPreview& preview, const EntityLightWindow& lightWindow) :
        mPreview(preview), mMouseDown(false), mCtrlDown(false), mShiftDown(false),
        mPointSet(false), mLightSelected(false), mColliderSelected(false), mEntityLightWindow(lightWindow), mShadowEmitterDraggers(preview.getWorldAction())
    {
        mShadowEmitterDraggers.setupPointDraggers(mPreview.getEntity());
        mLink = mPreview.getEntity().getWorld().getLightSystem().onContentsChanged([this](ungod::Entity e, const sf::FloatRect&)
            {
                mShadowEmitterDraggers.notifyChange(mPreview.getEntity());
            });
    }

    void LightEditState::selectLight()
    {
        mLightSelected = true;
    }

    void LightEditState::deselectLight()
    {
        mLightSelected = false;
    }

    void LightEditState::selectMultiLight(std::size_t i)
    {
        mSelectedMultiLights.emplace_back(i);
    }

    void LightEditState::deselectMultiLight(std::size_t i)
    {
        mSelectedMultiLights.erase(std::remove(mSelectedMultiLights.begin(), mSelectedMultiLights.end(), i), mSelectedMultiLights.end());
    }

    void LightEditState::selectCollider()
    {
        mColliderSelected = true;
    }

    void LightEditState::deselectCollider()
    {
        mColliderSelected = false;
    }

    void LightEditState::selectMultiCollider(std::size_t i)
    {
        mSelectedMultiColliders.emplace_back(i);
    }

    void LightEditState::deselectMultiCollider(std::size_t i)
    {
        mSelectedMultiColliders.erase(std::remove(mSelectedMultiColliders.begin(), mSelectedMultiColliders.end(), i), mSelectedMultiColliders.end());
    }

    void LightEditState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        if (!preview.mEntity.has<ungod::TransformComponent>())
            return;

        switch (event.type)
        {
            case sf::Event::MouseButtonPressed:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mMouseDown = true;
                    mLastMouse = sf::Mouse::getPosition();

                    sf::Vector2f mousePos = preview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(preview.mWindow), preview.mCamera.getView());
                    //normalize to the position of the entity
                    mousePos = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(mousePos);

                    if (mShiftDown && !mPointSet)
                    {
                        int mult = mEntityLightWindow.multiColliderActive();
                        if (mEntityLightWindow.singleColliderActive())
                        {
                            mPreview.mWorldAction.setPointCount(mPreview.mEntity, preview.mEntity.get<ungod::ShadowEmitterComponent>().getCollider().getPointCount()+1);
                            mPreview.mWorldAction.setPoint(mPreview.mEntity, mousePos, preview.mEntity.get<ungod::ShadowEmitterComponent>().getCollider().getPointCount()-1);
                            mPointSet = true;
                        }
                        else if (mult != wxNOT_FOUND)
                        {
                            const ungod::MultiShadowEmitter& ms = preview.mEntity.get<ungod::MultiShadowEmitter>();
                            mPreview.mWorldAction.setMultiPointCount(mPreview.mEntity, ms.getComponent(mult).getCollider().getPointCount()+1, mult);
                            mPreview.mWorldAction.setMultiPoint(mPreview.mEntity, mousePos, ms.getComponent(mult).getCollider().getPointCount()-1, mult);
                            mPointSet = true;
                        }
                    }
                    else if (mCtrlDown)
                    {

                        //check whether a light or a multilight
                        if (preview.mEntity.has<ungod::LightEmitterComponent>())
                        {
                            sf::FloatRect bounds = preview.mEntity.get<ungod::LightEmitterComponent>().getLight().getBoundingBox();
                            if (bounds.contains(mousePos))
                            {
                                if (!mLightSelected)
                                    selectLight();
                                else
                                    deselectLight();
                            }
                        }
                        if (preview.mEntity.has<ungod::MultiLightEmitter>())
                        {
                            ungod::MultiLightEmitter& ml = preview.mEntity.modify<ungod::MultiLightEmitter>();
                            for (std::size_t i = 0; i < ml.getComponentCount(); ++i)
                            {
                                sf::FloatRect bounds = ml.getComponent(i).getLight().getBoundingBox();
                                if (bounds.contains(mousePos))
                                {
                                    if (std::find(mSelectedMultiLights.begin(), mSelectedMultiLights.end(), i) == mSelectedMultiLights.end())
                                        selectMultiLight(i);
                                    else
                                        deselectMultiLight(i);
                                }
                            }
                        }
                        if (preview.mEntity.has<ungod::ShadowEmitterComponent>())
                        {
                            sf::FloatRect bounds = preview.mEntity.get<ungod::ShadowEmitterComponent>().getCollider().getBoundingBox();
                            if (bounds.contains(mousePos))
                            {
                                if (!mColliderSelected)
                                    selectCollider();
                                else
                                    deselectCollider();
                            }
                        }
                        if (preview.mEntity.has<ungod::MultiShadowEmitter>())
                        {
                            ungod::MultiShadowEmitter& ms = preview.mEntity.modify<ungod::MultiShadowEmitter>();
                            for (std::size_t i = 0; i < ms.getComponentCount(); ++i)
                            {
                                sf::FloatRect bounds = ms.getComponent(i).getCollider().getBoundingBox();
                                if (bounds.contains(mousePos))
                                {
                                    if (std::find(mSelectedMultiColliders.begin(), mSelectedMultiColliders.end(), i) == mSelectedMultiColliders.end())
                                        selectMultiCollider(i);
                                    else
                                        deselectMultiCollider(i);
                                }
                            }
                        }
                    }
                }

                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                mMouseDown = false;
                mPointSet = false;
                break;
            }
            case sf::Event::MouseMoved:
            {
                if (mMouseDown)
                {
                    sf::Vector2i offseti = sf::Mouse::getPosition() - mLastMouse;
                    sf::Vector2f offset = sf::Vector2f{(float) offseti.x, (float) offseti.y};

                    if (mLightSelected)
                    {
                        preview.mWorldAction.setLightPosition(preview.mEntity, preview.mEntity.get<ungod::LightEmitterComponent>().getLight().getPosition() + offset);
                    }

                    for (const auto& i : mSelectedMultiLights)
                    {
                        preview.mWorldAction.setMultiLightPosition(preview.mEntity, preview.mEntity.get<ungod::MultiLightEmitter>().getComponent(i).getLight().getPosition() + offset, i);
                    }

                    if (mColliderSelected)
                    {
                        auto& comp = preview.mEntity.get<ungod::ShadowEmitterComponent>();
                        for (unsigned i = 0; i < comp.getCollider().getPointCount(); i++)
                            preview.mWorldAction.setPoint(preview.mEntity, comp.getCollider().getPoint(i) + offset, i);
                    }

                    for (const auto& i : mSelectedMultiLights)
                    {
                        auto& comp = preview.mEntity.get<ungod::MultiShadowEmitter>().getComponent(i);
                        for (unsigned j = 0; j < comp.getCollider().getPointCount(); j++)
                            preview.mWorldAction.setMultiPoint(preview.mEntity, comp.getCollider().getPoint(j) + offset, j, i);
                    }

                    mLastMouse = sf::Mouse::getPosition();
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
                {
                    mPointSet = false;
                    mShiftDown = true;
                }
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

        mShadowEmitterDraggers.handleEvent(preview, event);
    }

    void LightEditState::update(EntityPreview& preview, float delta)
    {

    }

    void LightEditState::renderLightSelection(sf::RenderWindow& window, sf::RenderStates states, const sf::FloatRect& bounds)
    {
          sf::RectangleShape rect( {bounds.width, bounds.height} );
          rect.setPosition( {bounds.left, bounds.top} );
          rect.setFillColor(sf::Color(255, 0, 0, 70));
          rect.setOutlineThickness(2);
          rect.setOutlineColor(sf::Color::Red);
          window.draw(rect, states);
    }

    void LightEditState::renderColliderSelection(sf::RenderWindow& window, sf::RenderStates states, sf::ConvexShape shape)
    {
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color::Red);
        shape.setFillColor(sf::Color(255, 0, 0, 70));
        window.draw(shape, states);
    }

    void LightEditState::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        ungod::Renderer::renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);
        ungod::Renderer::renderLightDebug(preview.mEntity, preview.mEntity.get<ungod::TransformComponent>(), window, states);

        if (!preview.mEntity.has<ungod::TransformComponent>())
            return;

        states.transform *= preview.mEntity.get<ungod::TransformComponent>().getTransform();

        //mark the selected stuff
        if (mLightSelected)
            renderLightSelection(window, states, preview.mEntity.get<ungod::LightEmitterComponent>().getLight().getBoundingBox());

        for (const auto& i : mSelectedMultiLights)
            renderLightSelection(window, states, preview.mEntity.get<ungod::MultiLightEmitter>().getComponent(i).getLight().getBoundingBox());

        if (mColliderSelected)
            renderColliderSelection(window, states, preview.mEntity.get<ungod::ShadowEmitterComponent>().getCollider().getShape());

        for (const auto& i : mSelectedMultiColliders)
            renderColliderSelection(window, states, preview.mEntity.get<ungod::MultiShadowEmitter>().getComponent(i).getCollider().getShape());

        mShadowEmitterDraggers.render(window, states);
    }

    LightEditState::~LightEditState()
    {
        mLink.disconnect();
    }
}
