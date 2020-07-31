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

#ifndef UNGOD_RENDER_LAYER_H
#define UNGOD_RENDER_LAYER_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "ungod/serialization/Serializable.h"
#include "ungod/serialization/SerialRenderLayer.h"
#include "ungod/script/CustomEvent.h"
#include "owls/Signal.h"
#include "quadtree/Quadtree.h"

namespace ungod
{
    class Camera;
    class Entity;
    struct DeserialMemory;

    /** \brief A layer where different kinds of 2d contents are rendered on.
    * A layer has a depth value, that controls its virtual z-position in 3d space. */
    class RenderLayer : public PolymorphicSerializable<RenderLayer>
    {
        friend struct SerialBehavior<RenderLayer>;
        friend struct DeserialBehavior<RenderLayer, DeserialMemory&>;

        friend class RenderLayerContainer;

    public:
        RenderLayer(float renderdepth = 1.0f);

        void setRenderDepth(float renderdepth);

        float getRenderDepth() const;

        virtual bool render(sf::RenderTarget& target, sf::RenderStates states) { return false;  }

        virtual bool renderDebug(sf::RenderTarget& target, sf::RenderStates states,
            bool bounds = true, bool texrects = true, bool colliders = true, bool audioemitters = true, bool lights = true) const {
            return true;
        }

        /** \brief Updates the layer for the given amount of delta time and within the specified rectangular area
        * of the game world. */
        virtual void update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize) {}

        /** \brief Handles the given input event. */
        virtual void handleInput(const sf::Event& event, const sf::RenderTarget& target) {}

        /** \brief Handles the given custom event. */
        virtual void handleCustomEvent(const CustomEvent& event) {}

        /** \brief Returns the boundaries of the render layer not affected by render depth and transform. */
        virtual sf::Vector2f getSize() const { return {}; }

        /** \brief Returns the boundaries of the render layer not affected by render depth. */
        sf::Vector2f getTransformedSize() const;

        virtual ~RenderLayer() {}

        /** \brief Sets the string identifier for the layer. */
        void setName(const std::string& identifier);

        /** \brief Returns the string identifier for the layer. */
        const std::string& getName() const;

		/** \brief Returns the container, this layer is attached to or nullptr. */
		inline RenderLayerContainer* getContainer() const { return mContainer; }

    private:
        float mRenderDepth;
        std::string mName;
		RenderLayerContainer* mContainer;

    public:
        virtual void serialize(ungod::MetaNode serializer, ungod::SerializationContext& context) const override
        {
            deferredSerialize<RenderLayer>(*this, serializer, context);
        }
        virtual std::string getSerialIdentifier() const override
        {
            return deferredGetIdentifier<RenderLayer>();
        }

	private:
		/** \brief Sets the render layer to a new size. */
        virtual void setSize(const sf::Vector2f& layersize) {}

        /** \brief Changes layer size, but can extend into specific directions. Alters transforms of all
        * entities if requried. */
        virtual void extend(const sf::Vector2f& leftTopExtensions, const sf::Vector2f& rightBotExtensions) {}

    };

    using RenderLayerPtr = std::shared_ptr<RenderLayer>;

    /** \brief A utility class that bundles a set of RenderLayer-pointers and provides convenient operations on them. */
    class RenderLayerContainer : public Serializable<RenderLayerContainer>
    {
    friend struct DeserialBehavior<RenderLayerContainer, DeserialMemory&>;
    public:
        RenderLayerContainer() = default;
        RenderLayerContainer(const RenderLayerContainer&) = delete;

        void update(float delta, const sf::Vector2f& areaPosition, const sf::Vector2f& areaSize);

        void handleInput(const sf::Event& event, const sf::RenderTarget& target);

        void handleCustomEvent(const CustomEvent& event);

		/** \brief Registers a render layer in the container. 
		* This method also resizes the layer to the container size. */
        RenderLayer* registerLayer(const RenderLayerPtr& layer, std::size_t i);

        void removeLayer(RenderLayer* layer);

        const std::vector<std::pair<RenderLayerPtr, bool>>& getVector() const { return mRenderLayers; }

        void moveLayerUp(std::size_t i);

        void moveLayerDown(std::size_t i);

        void setActive(std::size_t i, bool active= true);

        bool isActive(std::size_t i) const;

        RenderLayer* getLayer(const std::string& name) const;

        void clearEverything();

		/** \brief Resizes all internal layers. This may be a costly operation if the layers already have content since attached layers may have to reorganize their entire content. */
		void setSize(const sf::Vector2f& size);
        void extend(const sf::Vector2f& leftTopExtensions, const sf::Vector2f& rightBotExtensions);

        const sf::Vector2f& getSize() const { return mSize; }

    private:
        std::vector<std::pair<RenderLayerPtr, bool>> mRenderLayers;
        std::queue<std::pair<std::size_t, bool>> mToMove;
        sf::Vector2f mSize;
    };
}

#endif // UNGOD_RENDER_LAYER_H
