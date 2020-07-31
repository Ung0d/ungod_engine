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

#include "ungod/base/WorldGraph.h"
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/serialization/SerialGraph.h"
#include "ungod/serialization/SerialRenderLayer.h"


namespace ungod
{
    WorldGraphNode::WorldGraphNode(WorldGraph& wg, unsigned index, ScriptedGameState& gamestate, const std::string& identifier, const std::string& datafile)
        : mWorldGraph(wg), 
        mIndex(index), 
        mLoadingInProcess(false),
        mIsLoaded(false), 
        mGamestate(gamestate), 
        mData(),
        mLayers(), 
        mIdentifier(identifier), 
        mDataFile(datafile),
        mBounds(0.0f, 0.0f, 0.0f, 0.0f),
        mSaveContents(true),
        mPriority(0)
    {
        mBounds.width = DEFAULT_SIZE;
        mBounds.height = DEFAULT_SIZE;
        mLayers.setSize({ DEFAULT_SIZE , DEFAULT_SIZE });
    }

    void WorldGraphNode::load()
    {
        wait();
        if (mIsLoaded)
            return;
        mData.load(mDataFile, LoadPolicy::ASYNC, this);
        mLoadingInProcess = true;
    }

    void WorldGraphNode::unload()
    {
        //save();
        mLayers.clearEverything();
        mIsLoaded = false;
		Logger::info("Node unloaded:", getIdentifier());
    }

	void WorldGraphNode::save()
	{
        wait();
        if (!mSaveContents)
            return;
		SerializationContext context;
		context.serializeRootObject(mLayers);
		context.save(mDataFile);
	}

	sf::FloatRect WorldGraphNode::getBounds() const
	{
		return mBounds;
	}

    World* WorldGraphNode::addWorld(unsigned i, bool init)
    {
        World* world = new World(*this);
        if (init)
            world->init(mGamestate);
        return static_cast<World*>(mLayers.registerLayer(RenderLayerPtr{ world }, i));
    }

    World* WorldGraphNode::addWorld(bool init)
    {
        return addWorld((unsigned)mLayers.getVector().size(), init);
    }

    World* WorldGraphNode::getWorld(unsigned i) const
    {
        if (i < mLayers.getVector().size())
            return static_cast<World*>(mLayers.getVector()[i].first.get());
        else
            return nullptr;
    }

    World* WorldGraphNode::getWorld(const std::string& identifier) const
    {
        World* result = nullptr;
        for (const auto& layer : mLayers.getVector())
        {
            if (layer.first->getName() == identifier)
            {
                result = static_cast<World*>(layer.first.get());
                break;
            }
        }
        return result;
    }

    void WorldGraphNode::update(float delta)
    {
        if (mLoadingInProcess)
            mLoadingInProcess = !tryInit();
        sf::View camview = mWorldGraph.getCamera().getView();
        sf::Vector2f campos{ camview.getCenter().x - camview.getSize().x / 2,camview.getCenter().y - camview.getSize().y / 2 };
        sf::Vector2f viewpos = mapToLocalPosition(campos);
        mLayers.update(delta, viewpos, camview.getSize());
    }

    void WorldGraphNode::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        mLayers.handleInput(event, mGamestate.getApp().getWindow());
    }

    void WorldGraphNode::handleCustomEvent(const CustomEvent& event)
    {
        mLayers.handleCustomEvent(event);
    }

	void WorldGraphNode::setPosition(const sf::Vector2f& pos)
	{
        mBounds.left = pos.x;
        mBounds.top = pos.y;
		mWorldGraph.notifyBoundsChanged(this);
        mNodeChangedSignal();
	}

	void WorldGraphNode::move(const sf::Vector2f& offset)
	{
		setPosition(getPosition() + offset);
		mWorldGraph.notifyBoundsChanged(this);
        mNodeChangedSignal();
	}

    void WorldGraphNode::setSize(const sf::Vector2f& size)
    {
        mBounds.width = size.x;
        mBounds.height = size.y;
        mLayers.setSize(size);
        mWorldGraph.notifyBoundsChanged(this);
        mNodeChangedSignal();
    }

    void WorldGraphNode::extend(const sf::Vector2f & leftTopExtensions, const sf::Vector2f & rightBotExtensions)
    {
        mBounds.left -= leftTopExtensions.x;
        mBounds.top -= leftTopExtensions.y;
        mBounds.width += leftTopExtensions.x + rightBotExtensions.x;
        mBounds.height += leftTopExtensions.y + rightBotExtensions.y;
        mLayers.extend(leftTopExtensions, rightBotExtensions);
        mWorldGraph.notifyBoundsChanged(this);
        mNodeChangedSignal();
    }

    void WorldGraphNode::activateLayer(unsigned i, bool active)
    {
        mLayers.setActive(i, active);
    }


	void WorldGraphNode::moveLayerUp(unsigned i)
	{
		mLayers.moveLayerUp(i);
	}

	void WorldGraphNode::moveLayerDown(unsigned i)
	{
		mLayers.moveLayerDown(i);
	}

	void WorldGraphNode::setActive(unsigned i, bool active)
	{
		mLayers.setActive(i, active);
	}

    sf::Vector2f WorldGraphNode::mapToGlobalPosition(const sf::Vector2f& position) const
    {
        return position + getPosition() - mWorldGraph.getActiveNode()->getPosition();
    }

    sf::Vector2f WorldGraphNode::mapToLocalPosition(const sf::Vector2f& position) const
    {
        return position - getPosition() + mWorldGraph.getActiveNode()->getPosition();
    }

    void WorldGraphNode::wait() 
    {
        while (mLoadingInProcess)
            mLoadingInProcess = !tryInit();
    }

    owls::SignalLink<void> WorldGraphNode::onNodeChanged(const std::function<void()>& callback)
    {
        return mNodeChangedSignal.connect(callback);
    }

    WorldGraphNode::~WorldGraphNode()
    {
        while (mData.isLoading())
            continue;
    }

    bool WorldGraphNode::tryInit()
    {
        if (!mData.isLoading())
        {
            if (mData.isLoaded())
            {
                mIsLoaded = true;
                //init loaded worlds
                for (unsigned i = 0; i < mData.get().container.getVector().size(); i++)
                {
                    World* world = static_cast<World*>(mLayers.registerLayer(mData.get().container.getVector()[i].first, mLayers.getVector().size()));
                    world->init(mGamestate, &mData.get().memory);
                }
                mData.drop(); //we can drop the asset, it is no longer required
                Logger::info("Loaded node:", getIdentifier());
            }
            else
                Logger::info("Failed to load node:", getIdentifier());
            return true;
        }
        else
            return false;
    }

    void SerialBehavior<WorldGraphNode>::serialize(const WorldGraphNode& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("id", data.mIdentifier, serializer);
        context.serializeProperty("file", data.mDataFile, serializer);
		context.serializeProperty("x", data.getPosition().x, serializer);
		context.serializeProperty("y", data.getPosition().y, serializer);
		context.serializeProperty("w", data.getSize().x, serializer);
		context.serializeProperty("h", data.getSize().y, serializer);
        context.serializeProperty("priority", data.getPriority(), serializer);

    }

    void DeserialBehavior<WorldGraphNode>::deserialize(WorldGraphNode& data, MetaNode deserializer, DeserializationContext& context)
    {
        //world graph nodes are deserialized in sleeping state
        auto result = deserializer.getAttributes<std::string, std::string, float, float, float, float, int>(
            { "id", "" }, { "file", "" }, { "x", 0.0f }, { "y", 0.0f }, { "w", 0.0f }, { "h", 0.0f }, { "priority", 0 });
        data.mIdentifier = std::get<0>(result);
        data.mDataFile = std::get<1>(result);
        data.mBounds.left = std::get<2>(result);
        data.mBounds.top = std::get<3>(result);
        sf::Vector2f size{ std::get<4>(result) , std::get<5>(result) };
        data.mBounds.width = size.x;
        data.mBounds.height = size.y;
        data.mLayers.setSize(size);
        data.setPriority(std::get<6>(result));
    }
}

namespace quad
{
    Vector2f ElementTraits<ungod::WorldGraphNode*>::getPosition(ungod::WorldGraphNode const* w)
    {
        return Vector2f(w->getBounds().left, w->getBounds().top);
    }

    Vector2f ElementTraits<ungod::WorldGraphNode*>::getSize(ungod::WorldGraphNode const* w)
    {
        return Vector2f(w->getBounds().width, w->getBounds().height);
    }

	uint64_t ElementTraits<ungod::WorldGraphNode*>::getID(ungod::WorldGraphNode const* e)
    {
		return e->getIndex();
    }
}
