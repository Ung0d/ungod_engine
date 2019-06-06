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

#ifndef UNGOD_RENDERER_H
#define UNGOD_RENDERER_H

#include "ungod/base/Entity.h"
#include "ungod/base/Transform.h"
#include "ungod/physics/Collision.h"

namespace ungod
{
    class VisualsManager;
    class VisualsComponent;
    class AnimationComponent;
    class ParticleSystemComponent;
    class MusicEmitterComponent;

    /** \brief A "system" class that renders entities to a render target.
    * Also updates entities with animation components. */
    class Renderer
    {
    public:
        Renderer(World& world, VisualsManager& visualsmanager);

        /** \brief Computes a new list of entities that are on the given given render target. */
        void renewRenderlist(sf::RenderTarget& target, quad::PullResult<Entity>& pull) const;

        /** \brief Draws the internal list of entities that must have a Transform and a Visual component. */
        void render(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the bounding boxes of all entities in the internal render-list. */
        void renderBounds(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the texture rects of all entities in the internal render-list, that have a Visuals-component and
        * either sprite-, vertex-array or multisprite-components. */
        void renderTextureRects(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the bounds of any attached particle-system. */
        void renderParticleSystemBounds(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the colliders of all entities in the internal render-list, that have a Rigidbody-component. */
        template<std::size_t CONTEXT = 0>
        void renderColliders(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws audio emitters. */
        void renderAudioEmitters(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Updates the internal list of entities. Selects out the entities with Animation-components automatically.
        * Entities with no animation component will be skipped. */
        void update(const std::list<Entity>& entities, float delta);

        /** \brief Renders an entity to the render target. The flip flag indicates whether the entity is rendering
        * is mirrored in y direction. This is used in water reflection-rendering. */
        static void renderEntity(Entity e, TransformComponent& transf, VisualsComponent& vis, sf::RenderTarget& target, sf::RenderStates states, bool flip = false, float offsety = 0.0f);

        /** \brief Draws the bounds the given entity. */
        static void renderBounds(const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states);

        /** \brief Draws the texture rects the given entity. */
        static void renderTextureRects(Entity e, const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states);

        /** \brief Draws the texture rects the given entity. */
        static void renderParticleSystemBounds(Entity e, const TransformComponent& transf, const ParticleSystemComponent& ps, sf::RenderTarget& target, sf::RenderStates states);

        /** \brief Draws the collider-bounds the given entity. */
        template<std::size_t CONTEXT = 0>
        static void renderColliders(const TransformComponent& transf, const RigidbodyComponent<CONTEXT>& body, sf::RenderTarget& target, sf::RenderStates states);

        /** \brief Renders a audio emitter entity. */
        static void renderAudioEmitter(Entity e, const TransformComponent& transf, MusicEmitterComponent& emitter, sf::RenderTarget& target, sf::RenderStates states);

    private:
        World* mWorld;
        VisualsManager* mVisualsManager;

        static constexpr float INNER_RECT_PERCENTAGE = 0.1f;

    private:
        void updateAnimation(Entity e, AnimationComponent& animation, float delta);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<std::size_t CONTEXT>
    void Renderer::renderColliders(const TransformComponent& transf, const RigidbodyComponent<CONTEXT>& body, sf::RenderTarget& target, sf::RenderStates states)
    {
        states.transform *= transf.getTransform();  //apply the transform of the entity
        for (const auto& c : body.getColliders())
          {
              sf::RenderStates locstates = states;
              locstates.transform.rotate(c.getRotation(), c.getCenter());   //apply the rotation of the collider
              sf::RectangleShape rect(c.getDownright() - c.getUpleft());
              rect.setPosition(c.getUpleft());
              rect.setFillColor(sf::Color::Transparent);
              rect.setOutlineThickness(2);
              rect.setOutlineColor(sf::Color::Cyan);
              target.draw(rect, locstates);
          }
    }

    template<std::size_t CONTEXT>
    void Renderer::renderColliders(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const
    {
          dom::Utility<Entity>::iterate<TransformComponent, RigidbodyComponent<CONTEXT>>(pull.getList(),
          [&target, &states] (Entity e, TransformComponent& transf, RigidbodyComponent<CONTEXT>& body)
          {
            Renderer::renderColliders(transf, body, target, states);
          });
    }
}

#endif // UNGOD_RENDERER_H
