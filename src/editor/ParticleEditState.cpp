#include "ParticleEditState.h"
#include "ungod/visual/Renderer.h"
#include "ungod/base/Transform.h"
#include "EntityPreview.h"
#include "EditorFrame.h"
#include "sfml_ext/EllipseShape.h"

namespace uedit
{
    ParticleEditState::ParticleEditState(EntityPreview& preview, ungod::Entity e) : mMouseDown(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        refresh(preview, e);
    }

    void ParticleEditState::handleInput(EntityPreview& preview, const sf::Event& event)
    {
        sf::Vector2f mouseRelatedToEntity = preview.getWindow().mapPixelToCoords(sf::Mouse::getPosition(preview.getWindow()), preview.mCamera.getView());
        mouseRelatedToEntity = preview.mEntity.get<ungod::TransformComponent>().getTransform().getInverse().transformPoint(mouseRelatedToEntity);

        switch (event.type)
        {
        case sf::Event::MouseButtonPressed:
            {
                if (!mMouseDown)
                {
                    mMouseDown = true;
                    mPositionDistEditor->mouseClick(preview, mouseRelatedToEntity);
                }
                break;
            }
        case sf::Event::MouseMoved:
            {
                mPositionDistEditor->mouseMove(preview, mouseRelatedToEntity);
                break;
            }
        case sf::Event::MouseButtonReleased:
            {
                mMouseDown = false;
                mPositionDistEditor->mouseUp(preview, mouseRelatedToEntity);
                break;
            }
        default:
            break;
        }
    }

    void ParticleEditState::update(EntityPreview& preview, float delta)
    {
    }

    void ParticleEditState::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
    {
        preview.mCanvas.getEditorState()->getRenderer().renderBounds(preview.mEntity.get<ungod::TransformComponent>(), window, states);

        states.transform *= preview.mEntity.get<ungod::TransformComponent>().getTransform();

        mPositionDistEditor->render(preview, window, states);
    }

    void ParticleEditState::refresh(EntityPreview& preview, ungod::Entity e)
    {
        //reset to null objects
        mPositionDistEditor = std::make_unique<ParticleEditor>();

        const ungod::ParticleSystem& ps = preview.mEntity.get<ungod::ParticleSystemComponent>().getSystem();

        if (ps.getEmitterKey() == ungod::PS_UNIVERSAL_EMITTER)
        {
            if (ps.getEmitter<ungod::UniversalEmitter>().getPositionDistKey() == ungod::PS_FIXED_POSITION)
            {
                mPositionDistEditor = std::unique_ptr<ParticleEditor>(new detail::FixedPositionEditor());
            }
            else if (ps.getEmitter<ungod::UniversalEmitter>().getPositionDistKey() == ungod::PS_ELLIPSE_DIST)
            {
                mPositionDistEditor = std::unique_ptr<ParticleEditor>(new detail::EllipseDistEditor());
            }
            else if (ps.getEmitter<ungod::UniversalEmitter>().getPositionDistKey() == ungod::PS_RECTANGLE_DIST)
            {
                mPositionDistEditor = std::unique_ptr<ParticleEditor>(new detail::RectangleDistEditor());
            }
            else if (ps.getEmitter<ungod::UniversalEmitter>().getPositionDistKey() == ungod::PS_LINE_SEGMENT_DIST)
            {
                mPositionDistEditor = std::unique_ptr<ParticleEditor>(new detail::LineSegmentDistEditor());
            }
        }
    }


    namespace detail
    {
        void FixedPositionEditor::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();

            //draw a red cycle around the emitter origin
            sf::CircleShape circle;
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::Red);
            circle.setOutlineThickness(2);
            circle.setRadius(10);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::FixedPosition>().position);
            window.draw(circle, states);
        }

        void FixedPositionEditor::mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            if (mBound)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::FixedPosition>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::FixedPosition>& f)
                                               {
                                                    f->position = mouseRelativeToEntity;
                                               });
            }
        }

        void FixedPositionEditor::mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();
            mBound = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::FixedPosition>().position);
        }

        void FixedPositionEditor::mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            mBound = false;
        }




        void EllipseDistEditor::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();

            //draw the ellipse
            sf::EllipseShape ellipse;
            ellipse.setFillColor(sf::Color::Transparent);
            ellipse.setOutlineColor(sf::Color::Red);
            ellipse.setOutlineThickness(1);
            ellipse.setRadius(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius);
            ellipse.setOrigin(ellipse.getHorizontalRadius(), ellipse.getVerticalRadius());
            ellipse.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center);
            window.draw(ellipse, states);

            sf::CircleShape circle;
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::Red);
            circle.setOutlineThickness(2);
            circle.setRadius(10);
            circle.setOrigin(circle.getRadius(), circle.getRadius());

            //draw a red circle at the center position of the ellipse
            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center);
            window.draw(circle, states);

            //draw red circles at the intersection points of the main axes with the ellipse
            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center +
                               sf::Vector2f{ 0, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.y});
            window.draw(circle, states);
            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center +
                               sf::Vector2f{ ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.x, 0});
            window.draw(circle, states);

            //draw lines for the main axes of the ellipse
            sf::Vertex line[2];
            line[0].color = sf::Color::Red;
            line[1].color = sf::Color::Red;

            line[0].position = ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center -
                               sf::Vector2f{ ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.x, 0};
            line[1].position = ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center +
                               sf::Vector2f{ ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.x, 0};
            window.draw(line, 2, sf::Lines, states);

            line[0].position = ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center -
                               sf::Vector2f{ 0, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.y};
            line[1].position = ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center +
                               sf::Vector2f{ 0, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.y};
            window.draw(line, 2, sf::Lines, states);
        }

        void EllipseDistEditor::mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();
            mBoundCenter = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center);
            mBoundRadX = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center +
                                                                        sf::Vector2f{ ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.x, 0});
            mBoundRadY = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center +
                                                                        sf::Vector2f{ 0, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.y});
        }

        void EllipseDistEditor::mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            if (mBoundCenter)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::EllipseDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->center = mouseRelativeToEntity;
                                               });
            }
            if (mBoundRadX)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::EllipseDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->radius.x = mouseRelativeToEntity.x - f->center.x;
                                               });
            }
            if (mBoundRadY)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::EllipseDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->radius.y = mouseRelativeToEntity.y - f->center.y;
                                               });
            }
        }

        void EllipseDistEditor::mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            mBoundCenter = false;
            mBoundRadX = false;
            mBoundRadY = false;
        }




        void RectangleDistEditor::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();

            //draw the rectangle
            sf::RectangleShape rect;
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Red);
            rect.setOutlineThickness(2);
            rect.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft);
            rect.setSize(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().sizes);
            window.draw(rect, states);

            //draw circles for the modificable points
            sf::CircleShape circle;
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::Red);
            circle.setOutlineThickness(2);
            circle.setRadius(10);
            circle.setOrigin(circle.getRadius(), circle.getRadius());

            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft);
            window.draw(circle, states);

            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft +
                               ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().sizes);
            window.draw(circle, states);
        }

        void RectangleDistEditor::mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();
            mBoundTopleft = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft);
            mBoundSizes = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft +
                                                                      ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().sizes);
        }

        void RectangleDistEditor::mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            if (mBoundTopleft)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::RectangleDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::RectangleDist>& f)
                                               {
                                                    f->topleft = mouseRelativeToEntity;
                                               });
            }
            if (mBoundSizes)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::RectangleDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::RectangleDist>& f)
                                               {
                                                    f->sizes = mouseRelativeToEntity - f->topleft;
                                               });
            }
        }

        void RectangleDistEditor::mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            mBoundTopleft = false;
            mBoundSizes = false;
        }




        void LineSegmentDistEditor::render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();

            //draw the line segment
            sf::Vertex line[2];
            line[0].color = sf::Color::Red;
            line[1].color = sf::Color::Red;

            line[0].position = ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point1;
            line[1].position = ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point2;
            window.draw(line, 2, sf::Lines, states);

            //draw circles for the modificable points
            sf::CircleShape circle;
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::Red);
            circle.setOutlineThickness(2);
            circle.setRadius(10);
            circle.setOrigin(circle.getRadius(), circle.getRadius());

            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point1);
            window.draw(circle, states);

            circle.setPosition(ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point2);
            window.draw(circle, states);
        }

        void LineSegmentDistEditor::mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            const ungod::ParticleSystem& ps = preview.getEntity().get<ungod::ParticleSystemComponent>().getSystem();
            mBound1 = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point1);
            mBound2 = 10 >= ungod::distance(mouseRelativeToEntity, ps.getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point2);
        }

        void LineSegmentDistEditor::mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            if (mBound1)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::LineSegmentDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::LineSegmentDist>& f)
                                               {
                                                    f->point1 = mouseRelativeToEntity;
                                               });
            }
            if (mBound2)
            {
                preview.getActionManager().particleSystemActions().alterPositionDist<ungod::LineSegmentDist>(preview.getEntity(), [mouseRelativeToEntity] (ungod::ScopedAccessor<ungod::LineSegmentDist>& f)
                                               {
                                                    f->point2 = mouseRelativeToEntity;
                                               });
            }
        }

        void LineSegmentDistEditor::mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity)
        {
            mBound1 = false;
            mBound2 = false;
        }
    }
}
