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

#include "ungod/base/Entity.h"
#include "ungod/base/World.h"
#include "ungod/base/Transform.h"
#include "ungod/physics/Movement.h"

namespace ungod
{
    bool Entity::isStatic() const
    {
        return !mHandle.has<MovementComponent>();
    }

    bool Entity::isSolid() const
    {
        return mHandle.has<RigidbodyComponent<>>();
    }

    bool Entity::valid() const
    {
        return mHandle.valid();
    }

    bool Entity::operator==(const Entity& other) const { return mHandle == other.mHandle; }

    bool Entity::operator!=(const Entity& other) const { return mHandle != other.mHandle;  }

    EntityID Entity::getID() const { return mHandle.getID(); }

    const EntityData* Entity::getData() const { return &mHandle.getEntityData(); }

    InstantiationBase* Entity::getInstantiation() const { return mInstantiation; }

    ungod::World& Entity::getWorld() const
    {
        return static_cast<ungod::World&>(mHandle.getUniverse());
    }
}

namespace quad
{
    bool ElementTraits<ungod::Entity>::isStatic(const ungod::Entity& e)
    {
        return e.isStatic();
    }

    Vector2f ElementTraits<ungod::Entity>::getPosition(const ungod::Entity& e)
    {
        return Vector2f(e.get<ungod::TransformComponent>().getGlobalUpperBounds().x,
                        e.get<ungod::TransformComponent>().getGlobalUpperBounds().y);
    }

    Vector2f ElementTraits<ungod::Entity>::getSize(const ungod::Entity& e)
    {
        return Vector2f(e.get<ungod::TransformComponent>().getSize().x,
                        e.get<ungod::TransformComponent>().getSize().y);
    }

    std::size_t ElementTraits<ungod::Entity>::getID(const ungod::Entity& e)
    {
        return e.getID();
    }
}
