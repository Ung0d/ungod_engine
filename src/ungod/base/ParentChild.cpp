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

#include "ungod/base/ParentChild.h"
#include "ungod/base/World.h"

namespace ungod
{
    unsigned ParentComponent::getNumberOfChildren() const
    {
        return mChildren.size();
    }


    const sf::Vector2f& ChildComponent::getPosition() const
    {
        return mChildTransform.getPosition();
    }


    const sf::Vector2f& ChildComponent::getScale() const
    {
        return mChildTransform.getScale();
    }



    ParentChildManager::ParentChildManager(World& world) : mWorld(world)
    {
        //set child positions to child_local + parent_global
        world.getTransformManager().onPositionChanged([this] (Entity e, const sf::Vector2f& pos)
          {
              if (e.has<ParentComponent>())
              {
                updateAllChildPositions(e);
              }
          });

        //set child scales to child_local * parent_global
        world.getTransformManager().onScaleChanged([this] (Entity e, const sf::Vector2f& scale)
          {
              if (e.has<ParentComponent>())
              {
                updateAllChildScales(e);
              }
          });
    }


    void ParentChildManager::addChild(Entity parent, Entity child)
    {
        if (!child.has<ChildComponent>())
            child.add<ChildComponent>();
        if (!parent.has<ParentComponent>())
            parent.add<ParentComponent>();
        ParentComponent& pc = parent.modify<ParentComponent>();
        ChildComponent& cc = child.modify<ChildComponent>();
        pc.mChildren.emplace_back(child);
        cc.mParent = parent;
        updateChildPosition(parent, child);
        updateChildScale(parent, child);
    }


    void ParentChildManager::setChildPosition(Entity child, const sf::Vector2f& position)
    {
        ChildComponent& cc = child.modify<ChildComponent>();
        cc.mChildTransform.setPosition(position);
        updateChildPosition(cc.mParent, child);
    }


    void ParentChildManager::setChildScale(Entity child, const sf::Vector2f& scale)
    {
        ChildComponent& cc = child.modify<ChildComponent>();
        cc.mChildTransform.setScale(scale);
        updateChildScale(cc.mParent, child);
    }


    void ParentChildManager::updateAllChildPositions(Entity e)
    {
        const ParentComponent& pc = e.get<ParentComponent>();
        for (const auto& child : pc.mChildren)
            updateChildPosition(e, child);
    }


    void ParentChildManager::updateAllChildScales(Entity e)
    {
        const ParentComponent& pc = e.get<ParentComponent>();
        for (const auto& child : pc.mChildren)
            updateChildScale(e, child);
    }


    void ParentChildManager::updateChildPosition(Entity e, Entity child)
    {
        const TransformComponent& tc = e.get<TransformComponent>();
        const ChildComponent& cc = child.get<ChildComponent>();
        mWorld.getTransformManager().setPosition(child, cc.getPosition() + tc.getPosition());
    }


    void ParentChildManager::updateChildScale(Entity e, Entity child)
    {
        const TransformComponent& tc = e.get<TransformComponent>();
        const ChildComponent& cc = child.get<ChildComponent>();
        mWorld.getTransformManager().setScale(child, {cc.getScale().x * tc.getScale().x, cc.getScale().y * tc.getScale().y});
    }
}
