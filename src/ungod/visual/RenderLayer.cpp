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

    bool RenderLayerContainer::render(sf::RenderTarget& target, Camera& camera, sf::RenderStates states) const
    {
        sf::Vector2f layerWorldPos{ mBounds.left, mBounds.top };
		states.transform.translate(layerWorldPos);
        bool check = true;
        for (const auto& layer : mRenderLayers)
        {
            if (layer.second)
            {
                camera.renderBegin(layer.first.get());
				check = check && layer.first->render(target, states);
                camera.renderEnd();
            }
        }
        return check;
    }

    bool RenderLayerContainer::renderDebug(sf::RenderTarget& target, Camera& camera, sf::RenderStates states, bool bounds, bool texrects, bool colliders, bool audioemitters, bool lights) const
    {
		states.transform.translate({ mBounds.left, mBounds.top });
        bool check = true;
        for (const auto& layer : mRenderLayers)
        {
            if (layer.second)
            {
                camera.renderBegin(layer.first.get());
                check = check && layer.first->renderDebug(target, states, bounds, texrects, colliders, audioemitters, lights);
                camera.renderEnd();
            }
        }
        return check;
    }

    void RenderLayerContainer::update(float delta, const Camera& camera)
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
        {
            if (layer.second)
            {
                sf::View camview = camera.getView();
				sf::Vector2f viewpos = mapToLocalPosition(sf::Vector2f{ camview.getCenter().x - camview.getSize().x / 2,camview.getCenter().y - camview.getSize().y / 2 });
				layer.first->update(delta, viewpos*layer.first->getRenderDepth(), camview.getSize());
            }
        }
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
        auto pred = [layer](const std::pair<RenderLayerPtr, bool>& p) { p.first.get() == layer;  }
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

	void RenderLayerContainer::setPosition(const sf::Vector2f& position)
	{
		mBounds.left = position.x;
		mBounds.top = position.y;
	}

	void RenderLayerContainer::setSize(const sf::Vector2f& size)
	{
		mBounds.width = size.x;
		mBounds.height = size.y;
		for (const auto& layer : mRenderLayers)
			layer.first->setSize(size);
	}

	sf::Vector2f RenderLayerContainer::mapToGlobalPosition(const sf::Vector2f& position) const
	{
		return { position.x + mBounds.left, position.y + mBounds.top };
	}

	sf::Vector2f RenderLayerContainer::mapToLocalPosition(const sf::Vector2f& position) const
	{
		return { position.x - mBounds.left, position.y - mBounds.top };
	}
}
