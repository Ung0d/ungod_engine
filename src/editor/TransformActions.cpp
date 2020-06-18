#include "TransformActions.h"
#include "ActionManager.h"

namespace uedit
{

    void TransformActions::setEntityPosition(ungod::Entity e, const sf::Vector2f& pos)
    {
        sf::Vector2f ecurpos = e.get<ungod::TransformComponent>().getPosition();
        mActionManager.action(std::function([this, e, pos]() { e.getWorld().getTransformHandler().setPosition(e, pos); }),
            std::function([this, e, ecurpos]() { e.getWorld().getTransformHandler().setPosition(e, ecurpos); }));
    }

    void TransformActions::setEntityScale(ungod::Entity e, const sf::Vector2f& scale)
    {
        sf::Vector2f ecurscale = e.get<ungod::TransformComponent>().getScale();
        mActionManager.action(std::function([this, e, scale]() { e.getWorld().getTransformHandler().setScale(e, scale); }),
            std::function([this, e, ecurscale]() { e.getWorld().getTransformHandler().setScale(e, ecurscale); }));
    }

    void TransformActions::setEntityBaseLineOffset(ungod::Entity e, const sf::Vector2f& offset)
    {
        sf::Vector2f curoffset = e.get<ungod::TransformComponent>().getScale();
        mActionManager.action(std::function([this, e, offset]() { e.getWorld().getTransformHandler().setBaseLineOffsets(e, offset); }),
            std::function([this, e, curoffset]() { e.getWorld().getTransformHandler().setBaseLineOffsets(e, curoffset); }));
    }


    void TransformActions::startEntityMoveSession(ungod::Entity e)
    {
        mEntityMovements.emplace(e, e.get<ungod::TransformComponent>().getPosition());
    }

    void TransformActions::moveEntity(ungod::Entity e, const sf::Vector2f& mv)
    {
        if (mEntityMovements.find(e) != mEntityMovements.end())
            e.getWorld().getTransformHandler().move(e, mv);
    }

    void TransformActions::endEntityMoveSession(ungod::Entity e)
    {
        auto res = mEntityMovements.find(e);
        if (res != mEntityMovements.end())
        {
            sf::Vector2f startPos = res->second;
            sf::Vector2f endPos = e.get<ungod::TransformComponent>().getPosition();
            mActionManager.action<>(std::function([this, e, endPos]()
                {
                    e.getWorld().getTransformHandler().setPosition(e, endPos);
                }),
                std::function([this, e, startPos]()
                {
                    e.getWorld().getTransformHandler().setPosition(e, startPos);
                }));
            mEntityMovements.erase(res);
        }
    }
}