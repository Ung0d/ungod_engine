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

#include "ungod/visual/RenderLayer.h"
#include "ungod/visual/Camera.h"
#include "ungod/serialization/DeserialMemory.h"
#include <algorithm>

namespace ungod
{
    RenderLayer::RenderLayer(float renderdepth) : mRenderDepth(renderdepth), mContainer(nullptr) {}

    void RenderLayer::setRenderDepth(float renderdepth)
    {
        mRenderDepth = renderdepth;
    }

    float RenderLayer::getRenderDepth() const
    {
        return mRenderDepth;
    }

    sf::Vector2f RenderLayer::getTransformedSize() const
    {
        auto layersize = getSize();
		layersize.x *= getRenderDepth();
		layersize.y *= getRenderDepth();
        return layersize;
    }

    void RenderLayer::setName(const std::string& name)
    {
        mName = name;
    }

    const std::string& RenderLayer::getName() const
    {
        return mName;
    }

    void RenderLayerContainer::update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize)
    {
        while (!mToMove.empty())
        {
            auto m = mToMove.front();
            mToMove.pop();
            if (m.second)
            {
                if (m.first < mRenderLayers.size()-1)
                    std::swap( mRenderLayers[m.first], mRenderLayers[m.first + 1] );
            }
            else
            {
                if (m.first > 0)
                    std::swap( mRenderLayers[m.first], mRenderLayers[m.first - 1] );
            }
        }

        for (const auto& layer : mRenderLayers)
            if (layer.second)
				layer.first->update(delta, areaPosition* layer.first->getRenderDepth(), areaSize);
    }


    void RenderLayerContainer::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        for (const auto& layer : mRenderLayers)
        {
            if (layer.second)
                layer.first->handleInput(event, target);
        }
    }


    void RenderLayerContainer::handleCustomEvent(const CustomEvent& event)
    {
        for (const auto& layer : mRenderLayers)
        {
            if (layer.second)
                layer.first->handleCustomEvent(event);
        }
    }

    RenderLayer* RenderLayerContainer::registerLayer(const RenderLayerPtr& layer, std::size_t i)
    {
        RenderLayer* rl = layer.get();
		rl->setSize(getSize());
		rl->mContainer = this;
        if (i < mRenderLayers.size())
        {
            mRenderLayers.emplace(mRenderLayers.begin() + i, layer, true);
        }
        else
        {
            mRenderLayers.emplace_back(layer, true);
        }
        return rl;
    }

    void RenderLayerContainer::removeLayer(RenderLayer* layer)
    {
        auto pred = [layer](const std::pair<RenderLayerPtr, bool>& p) { return p.first.get() == layer;  };
        mRenderLayers.erase(std::remove_if(mRenderLayers.begin(), mRenderLayers.end(), pred), mRenderLayers.end());
    }

    void RenderLayerContainer::moveLayerUp(std::size_t i)
    {
        mToMove.emplace(i, true);
    }

    void RenderLayerContainer::moveLayerDown(std::size_t i)
    {
        mToMove.emplace(i, false);
    }

    void RenderLayerContainer::setActive(std::size_t i, bool active)
    {
        mRenderLayers[i].second = active;
    }

    bool RenderLayerContainer::isActive(std::size_t i) const
    {
        return mRenderLayers[i].second;
    }

    RenderLayer* RenderLayerContainer::getLayer(const std::string& name) const
    {
        auto res = std::find_if(mRenderLayers.begin(), mRenderLayers.end(), [&name] (const auto& layer) { return layer.first->getName() == name; });
        if (res != mRenderLayers.end())
            return res->first.get();
        else
            return nullptr;
    }

    void RenderLayerContainer::clearEverything()
    {
        mRenderLayers.clear();
        while (!mToMove.empty())
            mToMove.pop();
    }

    void RenderLayerContainer::setSize(const sf::Vector2f& size)
    {
        mSize = size;
        for (const auto& layer : mRenderLayers)
            layer.first->setSize(mSize);
    }

    void RenderLayerContainer::extend(const sf::Vector2f& leftTopExtensions, const sf::Vector2f& rightBotExtensions)
    {
        mSize += leftTopExtensions + rightBotExtensions;
        for (const auto& layer : mRenderLayers)
            layer.first->extend(leftTopExtensions, rightBotExtensions);
    }
}
