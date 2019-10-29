#include "VisualsEditState.h"
#include "EntityPreview.h"

namespace uedit
{
    VisualsEditState::VisualsEditState(EntityPreview& preview) :
        mPreview(preview), mMouseDown(false), mCtrlDown(false), mSpriteSelected(false)
    {
        mLastMouse = sf::Mouse::getPosition();
    }

    void VisualsEditState::selectSprite()
    {
        mSpriteSelected = true;
    }

    void VisualsEditState::deselectSprite()
    {
        mSpriteSelected = false;
    }

    void VisualsEditState::selectMultiSprite(std::size_t i)
    {
        mSelectedMultiSprites.emplace_back(i);
    }

    void VisualsEditState::deselectMultiSprite(std::size_t i)
    {
        mSelectedMultiSprites.erase(std::remove(mSelectedMultiSprites.begin(), mSelectedMultiSprites.end(), i), mSelectedMultiSprites.end());
    }

    void VisualsEditState::selectVertices(std::size_t i)
    {
        mSelectedVertices.emplace_back(i);
    }

    void VisualsEditState::deselectVertices(std::size_t i)
    {
        mSelectedVertices.erase(std::remove(mSelectedVertices.begin(), mSelectedVertices.end(), i), mSelectedVertices.end());
    }

    void VisualsEditState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        switch (event.type)
        {
            case sf::Event::MouseButtonPressed:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    mMouseDown = true;
                    mLastMouse = sf::Mouse::getPosition();

                    if (mCtrlDown)
                    {
                        sf::Vector2f mouseWorldPos = preview.mWindow.mapPixelToCoords(sf::Mouse::getPosition(preview.mWindow), preview.mCamera.getView());
                        //normalize to the position of the entity
                        mouseWorldPos = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(mouseWorldPos);

                        //check whether a sprite or texrect was clicked
                        if (preview.mEntity.has<ungod::SpriteComponent>())
                        {
                            sf::FloatRect bounds = preview.mEntity.get<ungod::SpriteComponent>().getSprite().getUntransformedBounds();
                            if (bounds.contains(preview.mEntity.get<ungod::SpriteComponent>().getSprite().getTransform().getInverse().transformPoint(mouseWorldPos)))
                            {
                                if (!mSpriteSelected)
                                    selectSprite();
                                else
                                    deselectSprite();
                            }
                        }
                        if (preview.mEntity.has<ungod::MultiSpriteComponent>())
                        {
                            ungod::MultiSpriteComponent& ms = preview.mEntity.modify<ungod::MultiSpriteComponent>();
                            for (std::size_t i = 0; i < ms.getComponentCount(); ++i)
                            {
                                sf::Vector2f corrpos = mouseWorldPos - ms.getComponent(i).getSprite().getPosition();
                                sf::FloatRect bounds = ms.getComponent(i).getSprite().getUntransformedBounds();
                                bounds = ms.getComponent(i).getSprite().getTransform().transformRect(bounds);
                                if (bounds.contains(ms.getComponent(i).getSprite().getTransform().transformPoint(corrpos)))
                                {
                                    if (std::find(mSelectedMultiSprites.begin(), mSelectedMultiSprites.end(), i) == mSelectedMultiSprites.end())
                                        selectMultiSprite(i);
                                    else
                                        deselectMultiSprite(i);
                                }
                            }
                        }
                        if (preview.mEntity.has<ungod::VertexArrayComponent>())
                        {
                            ungod::VertexArrayComponent& vert = preview.mEntity.modify<ungod::VertexArrayComponent>();
                            for (std::size_t i = 0; i < vert.getVertices().textureRectCount(); ++i)
                            {
                                sf::FloatRect bounds = vert.getVertices().getBounds(i);
                                if (bounds.contains(mouseWorldPos))
                                {
                                    if (std::find(mSelectedVertices.begin(), mSelectedVertices.end(), i) == mSelectedVertices.end())
                                        selectVertices(i);
                                    else
                                        deselectVertices(i);
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
                break;
            }
            case sf::Event::MouseMoved:
            {
                if (mMouseDown)
                {
                    sf::Vector2i offseti = sf::Mouse::getPosition() - mLastMouse;
                    sf::Vector2f offset = sf::Vector2f{(float) offseti.x, (float) offseti.y};

                    if (mSpriteSelected)
                    {
                        preview.mWorldAction.setSpritePosition(preview.mEntity, preview.mEntity.get<ungod::SpriteComponent>().getSprite().getPosition() + offset);
                    }

                    for (const auto& i : mSelectedMultiSprites)
                    {
                        preview.mWorldAction.setMultiSpritePosition(preview.mEntity, preview.mEntity.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getPosition() + offset, i);
                    }

                    for (const auto& i : mSelectedVertices)
                    {
                        preview.mWorldAction.setVertexarrayPosition(preview.mEntity, preview.mEntity.get<ungod::VertexArrayComponent>().getVertices().getPosition(i) + offset, i);
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
                break;
            }
            case sf::Event::KeyReleased:
            {
                if (event.key.code == sf::Keyboard::LControl)
                    mCtrlDown = false;
                break;
            }
            default: break;
        }
    }

    void VisualsEditState::update(EntityPreview& preview, float delta)
    {

    }

    void VisualsEditState::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        ungod::Renderer::renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);
        ungod::Renderer::renderTextureRects(preview.mEntity, preview.mEntity.get<ungod::TransformComponent>(), window, states);


        if (!preview.mEntity.has<ungod::TransformComponent>())
            return;

        states.transform *= preview.mEntity.get<ungod::TransformComponent>().getTransform();

        //mark the selected stuff
        if (mSpriteSelected)
        {
              sf::RenderStates local = states;
              local.transform *= preview.mEntity.get<ungod::SpriteComponent>().getSprite().getTransform();
              sf::RectangleShape rect( {preview.mEntity.get<ungod::SpriteComponent>().getSprite().getTextureRect().width, preview.mEntity.get<ungod::SpriteComponent>().getSprite().getTextureRect().height} );
              rect.setFillColor(sf::Color(255, 0, 0, 70));
              rect.setOutlineThickness(2);
              rect.setOutlineColor(sf::Color::Red);
              window.draw(rect, local);
        }

        for (const auto& i : mSelectedMultiSprites)
        {
          sf::RenderStates local = states;
          local.transform *= preview.mEntity.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getTransform();
          sf::RectangleShape rect( {preview.mEntity.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getTextureRect().width,
                                    preview.mEntity.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getTextureRect().height} );
          rect.setFillColor(sf::Color(255, 0, 0, 70));
          rect.setOutlineThickness(2);
          rect.setOutlineColor(sf::Color::Red);
          window.draw(rect, local);
        }

        for (const auto& i : mSelectedVertices)
        {
          sf::RectangleShape rect( {preview.mEntity.get<ungod::VertexArrayComponent>().getVertices().getBounds(i).width,
                                    preview.mEntity.get<ungod::VertexArrayComponent>().getVertices().getBounds(i).height} );
          rect.setPosition( sf::Vector2f{preview.mEntity.get<ungod::VertexArrayComponent>().getVertices().getBounds(i).left,
                                         preview.mEntity.get<ungod::VertexArrayComponent>().getVertices().getBounds(i).top} );
          rect.setFillColor(sf::Color(255, 0, 0, 70));
          rect.setOutlineThickness(2);
          rect.setOutlineColor(sf::Color::Red);
          window.draw(rect, states);
        }
    }
}
