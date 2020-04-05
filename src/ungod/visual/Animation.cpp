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


#include "ungod/visual/Animation.h"
#include "ungod/serialization/MetaData.h"

namespace ungod
{
    Animation::Animation() :
        mTimer(0), mLoop(false), mReverse(false), mRunning(false), mOneTimeOnly(false),
        mCurrentIndex(0), mCurrentNode(nullptr), mSpeed(1.0f) {}

    bool Animation::update(float delta, sf::Vertex* vertices)
    {
        if (mRunning)
        {
            mTimer -= delta*mSpeed;
            if (mTimer <= 0)
            {
                return nextImage(vertices);
            }
        }
        return false;
    }

    bool Animation::isLooping() const
    {
        return mLoop;
    }

    bool Animation::isRunning() const
    {
        return mRunning;
    }

    void Animation::setRunning(bool running)
    {
        mRunning = running;
    }

    std::string Animation::getKey() const
    {
        return mCurrentNode.parent().name();
    }

    unsigned Animation::getCurrentIndex() const
    {
        return mCurrentIndex;
    }

    unsigned Animation::getFrameCount() const
    {
        unsigned num = 0;
        forEachSubnode(mCurrentNode.parent(), [&num] (MetaNode) {num++;});
        return num;
    }

    void Animation::setFrame(unsigned i, sf::Vertex* vertices)
    {
        MetaNode remcur = mCurrentNode;
        mCurrentNode = mCurrentNode.parent().firstNode();
        for (unsigned k = 0; k < i; k++)
            mCurrentNode = mCurrentNode.next();
        if (mRunning)
            setupFrame(remcur, vertices);
    }

    float Animation::getSpeed() const
    {
        return mSpeed;
    }

    void Animation::setSpeed(float speed)
    {
        mSpeed = speed;
    }

    bool Animation::startAnimation(const MetaMap& meta, const std::string& key, sf::Vertex* vertices)
    {
        auto node = meta.getNodeWithKey(key);
        if (node)
        {
            startAnimation(node, vertices);
            return true;
        }
        else
        {
            Logger::warning("Attempt to set an animation state that does not exist.");
            Logger::endl();
            Logger::warning("Key: ");
            Logger::warning(key);
            Logger::endl();
            return false;
        }
    }

    void Animation::setupFrame(MetaNode previous, sf::Vertex* vertices)
    {
        if (previous)
        {
            auto global = previous.parent().getAttributes<float, float>( {"global_offset_x", 0}, {"global_offset_y", 0} );
            auto local = previous.getAttributes<float, float>( {"offset_x", std::get<0>(global)}, {"offset_y", std::get<1>(global)} );
            vertices[0].position -= sf::Vector2f(std::get<0>(local), std::get<1>(local));
        }

        auto global = mCurrentNode.parent().getAttributes<float, float, float, float, float>(
                    {"global_width", 0}, {"global_height", 0}, {"global_offset_x", 0}, {"global_offset_y", 0}, {"global_duration", 100} );
        auto local = mCurrentNode.getAttributes<float, float, float, float, float, float, float>(
                    {"pos_x", 0}, {"pos_y", 0}, {"width", std::get<0>(global)}, {"height", std::get<1>(global)},
                    {"offset_x", std::get<2>(global)}, {"offset_y", std::get<3>(global)}, {"duration", std::get<4>(global)} );

        //set the vertices
        vertices[0].texCoords = sf::Vector2f(std::get<0>(local), std::get<1>(local));
        vertices[1].texCoords = sf::Vector2f(std::get<0>(local) + std::get<2>(local), std::get<1>(local));
        vertices[2].texCoords = sf::Vector2f(std::get<0>(local) + std::get<2>(local), std::get<1>(local) + std::get<3>(local));
        vertices[3].texCoords = sf::Vector2f(std::get<0>(local), std::get<1>(local) + std::get<3>(local));
        sf::Vector2f curPos = vertices[0].position;
        vertices[0].position += sf::Vector2f(std::get<4>(local), std::get<5>(local));
        vertices[1].position = curPos + sf::Vector2f(std::get<4>(local) + std::get<2>(local), std::get<5>(local));
        vertices[2].position = curPos + sf::Vector2f(std::get<4>(local) + std::get<2>(local), std::get<5>(local) + std::get<3>(local));
        vertices[3].position = curPos + sf::Vector2f(std::get<4>(local), std::get<5>(local) + std::get<3>(local));

        mTimer = std::get<6>(local);
    }

    bool Animation::nextImage(sf::Vertex* vertices)
    {
        MetaNode previous = mCurrentNode;
        if (mReverse)
        {
            if (mCurrentNode.prev())
            {
                mCurrentNode = mCurrentNode.prev();
                mCurrentIndex --;
            }
            else if (mOneTimeOnly)
            {
                mRunning = false;
            }
            else
            {
                mReverse = false;
                if (mCurrentNode.next())
                {
                    mCurrentNode = mCurrentNode.next();
                    mCurrentIndex ++;
                }
            }
        }
        else
        {
            if (mCurrentNode.next())
            {
                mCurrentNode = mCurrentNode.next();
                mCurrentIndex ++;
            }
            else if (mLoop)
            {
                mReverse = true;
                if (mCurrentNode.prev())
                {
                    mCurrentNode = mCurrentNode.prev();
                    mCurrentIndex --;
                }
            }
            else if (mOneTimeOnly)
            {
                mRunning = false;
            }
            else
            {
                mCurrentNode = mCurrentNode.parent().firstNode();
                mCurrentIndex = 0;
            }
        }
        if (mRunning)
            setupFrame(previous, vertices);
        return mRunning;
    }

    void Animation::startAnimation(MetaNode node, sf::Vertex* vertices)
    {
        mCurrentNode = node.firstNode();
        if (mCurrentNode)
        {
            mCurrentIndex = 0;
            auto res = node.getAttributes<bool, bool>({"loop", false}, {"one_time_only", false});
            mLoop = std::get<0>(res);
            mOneTimeOnly = std::get<1>(res);
            mRunning = true;
            mReverse = false;
            setupFrame(MetaNode(), vertices);
        }
        else
            mRunning = false;
    }

    sf::FloatRect Animation::getBounds() const
    {
        MetaNode parent = mCurrentNode.parent();
        auto global = parent.getAttributes<int, int, float, float>(
                    {"global_width", 0}, {"global_height", 0}, {"global_offset_x", 0}, {"global_offset_y", 0} );
        sf::Vector2f upperBounds;
        sf::Vector2f lowerBounds;
        upperBounds.x = std::get<2>(global);
        upperBounds.y = std::get<3>(global);
        lowerBounds.x = std::get<2>(global) + std::get<0>(global);
        lowerBounds.y = std::get<3>(global) + std::get<1>(global);

        forEachSubnode(parent, [&] (MetaNode sub)
           {
                auto local = sub.getAttributes<int, int, float, float>(
                            {"width", std::get<0>(global)}, {"height", std::get<1>(global)}, {"offset_x", std::get<2>(global)}, {"offset_y", std::get<3>(global)} );
                upperBounds.x = std::min(upperBounds.x, std::get<2>(local));
                upperBounds.y = std::min(upperBounds.y, std::get<3>(local));
                lowerBounds.x = std::max(lowerBounds.x, std::get<2>(local) + std::get<0>(local));
                lowerBounds.y = std::max(lowerBounds.y, std::get<3>(local) + std::get<1>(local));
           });

        return {upperBounds.x, upperBounds.y, lowerBounds.x - upperBounds.x, lowerBounds.y - upperBounds.y};
    }
}
