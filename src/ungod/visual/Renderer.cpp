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

#include "ungod/visual/Renderer.h"
#include "ungod/base/World.h"
#include "ungod/physics/Physics.h"
#include "ungod/content/particle_system/ParticleSystem.h"

namespace ungod
{
    Renderer::Renderer(World& world, VisualsManager& visualsmanager) :
        mWorld(&world), mVisualsManager(&visualsmanager), mFirstNonePlane(0) {}


    void Renderer::renewRenderlist(sf::RenderTarget& target, quad::PullResult<Entity>& pull) const
    {
        //first step: retrieve the entities that may collide with the window
        sf::Vector2f windowPosition = target.mapPixelToCoords(sf::Vector2i(0,0), target.getView());
		windowPosition = mWorld->getContainer()->mapToLocalPosition(windowPosition);
        sf::Vector2f windowSize = target.getView().getSize();
        mWorld->getQuadTree().retrieve(pull, {windowPosition.x, windowPosition.y, windowSize.x, windowSize.y} );

        //second step: sort out the entities, that are not on the screen
        auto removalCondition = [windowPosition, windowSize](Entity entity)
        {
             const TransformComponent& t = entity.get<TransformComponent>();
             return !(t.getPosition().x <= windowPosition.x + windowSize.x &&
                      t.getPosition().x + t.getSize().x >= windowPosition.x &&
                      t.getPosition().y <= windowPosition.y + windowSize.y &&
                      t.getPosition().y + t.getSize().y >= windowPosition.y);
        };
        pull.getList().erase(std::remove_if( pull.getList().begin(),
                                               pull.getList().end(),
                                               removalCondition), pull.getList().end());

        //third step: depth sorting
        auto depthSorting = [](Entity l, Entity r)
        {
            //TRUE == "left is rendered FIRST, right after"

            const TransformComponent& left = l.get<TransformComponent>();
            const TransformComponent& right = r.get<TransformComponent>();

            //test if right.rightAnchor is RIGHT of the line between the two anchors of left
            sf::Vector2 lla = left.getLeftAnchor();
            sf::Vector2 lra = left.getRightAnchor();
            sf::Vector2 rla = right.getLeftAnchor();
            sf::Vector2 rra = right.getRightAnchor();

            if (sign(lra, rra ,lla) >= 0 && sign(lra, rla ,lla) >= 0)
                return true;

            if (sign(rra, lra ,rla) >= 0 && sign(rra, lla ,rla) >= 0)
                return false;

            return std::max(rra.y, rla.y) > std::max(lla.y, lra.y);
        };

        pull.getList().sort(depthSorting);
    }



    //todo rewrite the method, last 2 arguments feel to hacky
    void Renderer::renderEntity(Entity e, TransformComponent& transf, VisualsComponent& vis, sf::RenderTarget& target, sf::RenderStates states, bool flip, float offsety)
    {
      states.transform *= transf.getTransform();
     if (flip)
        states.transform.scale(1.0f, -1.0f);
      states.transform.translate(0.0f, offsety);
      states.texture = vis.getTexture();

      if (e.has<WaterComponent>())
      {
          e.modify<WaterComponent>().mWater.render(target, states, e.getWorld());
      }
      if (e.has<TileMapComponent>())
      {
          e.modify<TileMapComponent>().mTileMap.render(target, states);
      }

      if (vis.isVisible() && vis.isLoaded())
      {
          if (e.has<VertexArrayComponent>())
          {
             e.get<VertexArrayComponent>().mVertices.render(target, states);
          }
          if (e.has<SpriteComponent>())
          {
             e.get<SpriteComponent>().mSprite.render(target, states);
          }
          if (e.has<MultiSpriteComponent>())
          {
             const MultiSpriteComponent& multisprite = e.get<MultiSpriteComponent>();
             for (unsigned i = 0; i < multisprite.getComponentCount(); ++i)
             {
                 multisprite.getComponent(i).mSprite.render(target, states);
             }
          }
          if (e.has<ParticleSystemComponent>())
          {
              e.modify<ParticleSystemComponent>().mParticleSystem->render(vis.getTexture(), target, states);
          }
      }
    }


    void Renderer::renderBounds(const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states)
    {
      sf::Vertex line[2];
      line[0].position = transf.getLeftAnchor();
      line[1].position = transf.getRightAnchor();
      line[0].color = sf::Color::Cyan;
      line[1].color = sf::Color::Cyan;
      target.draw(line, 2, sf::Lines, states);

      states.transform *= transf.getTransform();  //apply the transform of the entity

      sf::RectangleShape rect(transf.getLowerBounds() - transf.getUpperBounds());
      rect.setPosition(transf.getUpperBounds());
      rect.setFillColor(sf::Color::Transparent);
      rect.setOutlineThickness(1);
      rect.setOutlineColor(sf::Color::Green);
      target.draw(rect, states);
    }


    void Renderer::renderTextureRects(Entity e, const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states)
    {
      sf::Color spriteCol = sf::Color::Red;
      sf::Color vertexCol = sf::Color::Yellow;
      states.transform *= transf.getTransform();  //apply the transform of the entity
      if (e.has<SpriteComponent>())
      {
          const Sprite& sprite = e.get<SpriteComponent>().getSprite();
          sf::RenderStates localstates = states;
          localstates.transform *= sprite.getTransform();
          sf::RectangleShape rect( {sprite.getTextureRect().width, sprite.getTextureRect().height} );
          rect.setFillColor(sf::Color::Transparent);
          rect.setOutlineThickness(1);
          rect.setOutlineColor(spriteCol);
          target.draw(rect, localstates);
      }
      if (e.has<MultiSpriteComponent>())
      {
          for (std::size_t i = 0; i < e.get<MultiSpriteComponent>().getComponentCount(); ++i)
          {
              const Sprite& sprite = e.get<MultiSpriteComponent>().getComponent(i).getSprite();
              sf::RenderStates localstates = states;
              localstates.transform *= sprite.getTransform();
              sf::RectangleShape rect( {sprite.getTextureRect().width, sprite.getTextureRect().height} );
              rect.setFillColor(sf::Color::Transparent);
              rect.setOutlineThickness(1);
              rect.setOutlineColor(spriteCol);
              target.draw(rect, localstates);
          }
      }
      if (e.has<VertexArrayComponent>())
      {
          const VertexArray& vertices = e.get<VertexArrayComponent>().getVertices();
          for (std::size_t i = 0; i < vertices.textureRectCount(); ++i)
          {
              sf::ConvexShape shape(4);
              for (int j = 0; j < 4; ++j)
                shape.setPoint(j, vertices.getPoint(i, j));
              shape.setFillColor(sf::Color::Transparent);
              shape.setOutlineThickness(1);
              shape.setOutlineColor(vertexCol);
              target.draw(shape, states);
          }
      }
    }


    void Renderer::renderParticleSystemBounds(Entity e, const TransformComponent& transf, const ParticleSystemComponent& ps, sf::RenderTarget& target, sf::RenderStates states)
    {
          sf::Color col = sf::Color::Blue;
          states.transform *= transf.getTransform();  //apply the transform of the entity
          sf::FloatRect bounds = ps.mParticleSystem->getBounds();
          sf::RectangleShape rect( {bounds.left, bounds.top} );
          rect.setSize( {bounds.width, bounds.height} );
          rect.setFillColor(sf::Color::Transparent);
          rect.setOutlineThickness(1);
          rect.setOutlineColor(col);
          target.draw(rect, states);
    }


    void Renderer::renderAudioEmitter(Entity e, const TransformComponent& transf, MusicEmitterComponent& emitter, sf::RenderTarget& target, sf::RenderStates states)
    {
        states.transform *= transf.getTransform();  //apply the transform of the entity
        sf::RectangleShape rect( {50, 50} );
          rect.setPosition( {-25, -25} );
          rect.setFillColor(sf::Color::Magenta);
          rect.setOutlineThickness(2);
          rect.setOutlineColor(sf::Color::Red);
          target.draw(rect, states);
    }


    void Renderer::renderLightDebug(Entity e, const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states)
    {
        states.transform *= transf.getTransform();  //apply the transform of the entity

        auto renderLightEmitter = [&](const LightEmitterComponent& light)
        {
            auto bb = light.getLight().getBoundingBox();
            sf::RectangleShape rect( {bb.width, bb.height} );
            rect.setPosition( {bb.left, bb.top} );
            rect.setOutlineThickness(2);
            rect.setOutlineColor(sf::Color::Yellow);
            rect.setFillColor(sf::Color::Transparent);
            sf::RectangleShape center( {20, 20} );
            center.setPosition( {bb.left + bb.width/2, bb.top + bb.height/2} );
            center.setFillColor(sf::Color::Yellow);
            target.draw(rect, states);
            target.draw(center, states);
        };

        if (e.has<LightEmitterComponent>())
            renderLightEmitter(e.get<LightEmitterComponent>());
        if (e.has<MultiLightEmitter>())
        {
            for (unsigned i = 0; i < e.get<MultiLightEmitter>().getComponentCount(); i++)
                renderLightEmitter(e.get<MultiLightEmitter>().getComponent(i));
        }

        auto renderLightCollider = [&](const ShadowEmitterComponent& shadow)
        {
            sf::ConvexShape shape = shadow.getCollider().getShape();
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineThickness(2);
            shape.setOutlineColor(sf::Color::Blue);
            target.draw(shape, states);
        };

        if (e.has<ShadowEmitterComponent>())
            renderLightCollider(e.get<ShadowEmitterComponent>());
        if (e.has<MultiShadowEmitter>())
        {
            for (unsigned i = 0; i < e.get<MultiShadowEmitter>().getComponentCount(); i++)
                renderLightCollider(e.get<MultiShadowEmitter>().getComponent(i));
        }
    }


    void Renderer::render(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
		sf::Vector2f camCenter = target.mapPixelToCoords(sf::Vector2i(target.getSize().x / 2, target.getSize().y / 2));
		camCenter = mWorld->getContainer()->mapToLocalPosition(camCenter);
        //iterate over all entities with both Transform and Visuals-component
        dom::Utility<Entity>::iterate<TransformComponent, VisualsComponent>(pull.getList(),
          [this, &target, &states, camCenter] (Entity e, TransformComponent& transf, VisualsComponent& vis)
          {
            if (vis.isHiddenForCamera())
            {
                if (transf.getBounds().contains(camCenter))
                {
                    sf::FloatRect innerRect = transf.getBounds();
                    innerRect.left += transf.getSize().x*INNER_RECT_PERCENTAGE;
                    innerRect.top += transf.getSize().y*INNER_RECT_PERCENTAGE;
                    innerRect.width -= 2*transf.getSize().x*INNER_RECT_PERCENTAGE;
                    innerRect.height -= 2*transf.getSize().y*INNER_RECT_PERCENTAGE;
                    if (innerRect.contains(camCenter))
                    {
                    mVisualsManager->componentOpacitySet(e, 0.0f);
                    }
                    else
                    {
                        sf::Vector2f centerDist = transf.getCenterPosition() - camCenter;
                        float dx = std::abs(centerDist.x) - transf.getSize().x*(1.0f-2*INNER_RECT_PERCENTAGE)*0.5f;
                        float dy = std::abs(centerDist.y) - transf.getSize().y*(1.0f-2*INNER_RECT_PERCENTAGE)*0.5f;
                        float reducedOpac = std::max( dx/(transf.getSize().x*INNER_RECT_PERCENTAGE), dy/(transf.getSize().y*INNER_RECT_PERCENTAGE) );
                        ungod::Logger::info(reducedOpac);
                        ungod::Logger::endl();
                        mVisualsManager->componentOpacitySet(e, reducedOpac);
                    }
                }
            }

            renderEntity(e, transf, vis, target, states);

            if (vis.isHiddenForCamera())
            {
                mVisualsManager->componentOpacitySet(e, vis.getOpacity());
            }
          });

        //iterate over all entities with both Transform and BigSprite-component
        dom::Utility<Entity>::iterate<TransformComponent, BigSpriteComponent>(pull.getList(),
          [this, &target, &states] (Entity e, TransformComponent& transf, BigSpriteComponent& bs)
          {
              if (bs.isVisible() && bs.isLoaded())
              {
                states.transform *= transf.getTransform();
                target.draw(bs.mBigSprite, states);
              }
          });
    }

    void Renderer::renderBounds(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
        dom::Utility<Entity>::iterate<TransformComponent>(pull.getList(),
          [&target, &states] (Entity e, TransformComponent& transf)
          {
              renderBounds(transf, target, states);
          });
    }

    void Renderer::renderTextureRects(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
        dom::Utility<Entity>::iterate<TransformComponent, VisualsComponent>(pull.getList(),
          [&target, &states] (Entity e, TransformComponent& transf, VisualsComponent& vis)
          {
              renderTextureRects(e, transf, target, states);
          });
    }

    void Renderer::renderParticleSystemBounds(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
        dom::Utility<Entity>::iterate<TransformComponent, VisualsComponent, ParticleSystemComponent>(pull.getList(),
          [&target, &states] (Entity e, TransformComponent& transf, VisualsComponent& vis, ParticleSystemComponent& ps)
          {
              renderParticleSystemBounds(e, transf, ps, target, states);
          });
    }


    void Renderer::renderAudioEmitters(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
        dom::Utility<Entity>::iterate<TransformComponent, MusicEmitterComponent>(pull.getList(),
          [&target, &states] (Entity e, TransformComponent& transf, MusicEmitterComponent& emitter)
          {
              renderAudioEmitter(e, transf, emitter, target, states);
          });
    }


    void Renderer::renderLightRanges(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
        //single light emitter
        dom::Utility<Entity>::iterate<TransformComponent>(pull.getList(),
          [&target, &states] (Entity e, TransformComponent& transf)
          {
              renderLightDebug(e, transf, target, states);
          });
    }


    void Renderer::update(const std::list<Entity>& entities, float delta)
    {
        dom::Utility<Entity>::iterate< VisualsComponent >(entities,
          [delta, this] (Entity e, VisualsComponent& visuals)
          {
              //handle animations
                if (e.has<AnimationComponent>())
                {
                    updateAnimation(e, e.modify<AnimationComponent>(), delta);
                }
                else if(e.has<MultiAnimationComponent>())
                {
                    for (std::size_t i = 0; i < e.modify<MultiAnimationComponent>().getComponentCount(); ++i)
                    {
                        updateAnimation(e, e.modify<MultiAnimationComponent>().getComponent(i), delta);
                    }
                }

             //handle affectors
             if (e.has<VisualAffectorComponent>())
             {
                 VisualAffectorComponent& affector = e.modify<VisualAffectorComponent>();
                 if (affector.isActive())
                 {
                    affector.mCallback(e, delta, *mVisualsManager, visuals);
                 }
             }
          });

        dom::Utility<Entity>::iterate< VisualsComponent, MultiVisualAffectorComponent >(entities,
          [delta, this] (Entity e, VisualsComponent& visuals, MultiVisualAffectorComponent& affector)
          {
                for (std::size_t i = 0; i < affector.getComponentCount(); ++i)
                {
                     if (affector.getComponent(i).isActive())
                     {
                        affector.getComponent(i).mCallback(e, delta, *mVisualsManager, visuals);
                     }
                }
          });
    }


    void Renderer::updateAnimation(Entity e, AnimationComponent& animation, float delta)
    {
        if (!animation.mVertices)
            return;
        bool runningBefore = animation.mAnimation.isRunning();
        animation.mAnimation.update(delta, animation.mVertices);
        if (runningBefore && !animation.mAnimation.isRunning())
        {
            mVisualsManager->mAnimationStopSignal(e, animation.getAnimation().getKey());
        }
    }
}
