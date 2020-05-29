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

#ifndef UNGOD_PARENT_CHILD_H
#define UNGOD_PARENT_CHILD_H

#include <list>
#include <SFML/Graphics/Transformable.hpp>
#include "ungod/base/Entity.h"

namespace ungod
{
    /**
    * \ingroup Components
    * \brief A component that contains a list of child entities. Child entities transform are relative to the parents transform.
    * Custom events for the parent entity are also forwarded to the children. */
    class ParentComponent : public Serializable<ParentComponent>
    {
    friend class ParentChildHandler;
    public:
        /** \brief Returns the number of children. */
        unsigned getNumberOfChildren() const;

    private:
        std::list<Entity> mChildren;
    };

     /**
    * \ingroup Components
    * \brief A component for child entities. Is automatically attached, when added to a parent. */
    class ChildComponent : public Serializable<ChildComponent>
    {
    friend class ParentChildHandler;
    public:
        const sf::Vector2f& getPosition() const;

        const sf::Vector2f& getScale() const;

    private:
        Entity mParent;
        sf::Transformable mChildTransform; //< defines local coordinate system for the child, is applied on top of the parents transform
    };

    /** \brief A manager structure that handles all operations on ParentChildComponents. */
    class ParentChildHandler
    {
    public:
        ParentChildHandler();

        void init(World& world);

        /** \brief Adds a child entity to the parent. Adds a ChildComponent to the child entity
        * if it has none. Adds a ParentComponent to the parent entity if it has none.
        * Both parent and child require a TransformComponent. */
        void addChild(Entity parent, Entity child);

        /** \brief Sets the local child position. */
        void setChildPosition(Entity child, const sf::Vector2f& position);

        /** \brief Sets the local child scale. */
        void setChildScale(Entity child, const sf::Vector2f& scale);

    private:
        World* mWorld;

    private:
        void updateAllChildPositions(Entity e);
        void updateAllChildScales(Entity e);
        void updateChildPosition(Entity e, Entity child);
        void updateChildScale(Entity e, Entity child);
    };
}


#endif // UNGOD_PARENT_CHILD_H


