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

#ifndef VISUAL_H
#define VISUAL_H

#include "owls/Signal.h"
#include "quadtree/QuadTree.h"
#include "ungod/visual/Image.h"
#include "ungod/visual/Sprite.h"
#include "ungod/visual/VertexArray.h"
#include "ungod/visual/Animation.h"
#include "ungod/serialization/MetaData.h"
#include "ungod/base/Utility.h"
#include "ungod/base/Transform.h"
#include "ungod/base/Entity.h"
#include "ungod/base/MultiComponent.h"
#include <sfml_ext/BigTexture.hpp>
#include <sfml_ext/BigSprite.hpp>
#include "ungod/physics/Collision.h"

namespace ungod
{
    class World; class VisualsHandler; class Renderer; class Camera;

    /**
    * \ingroup Components
    * \brief A component that wraps a single texture that can be attached to an entity.
    * This texture can be futher used by Visuals, Sprite and Animation components. */
    class VisualsComponent : public Serializable<VisualsComponent>
    {
    friend class VisualsHandler;
    friend struct SerialBehavior<VisualsComponent, Entity>;
    public:
        VisualsComponent();

        /** \brief Returns a reference to the underlying texture if it is loaded. Else it returns a reference to the default texture. */
        const sf::Texture& getTexture();

        /** \brief Returns true if a texture is successfully loaded. */
        bool isLoaded() const;

        /** \brief Returns the visibility status of the VIsuals-component. */
        bool isVisible() const;

        /** \brief Returns the current opacity value of the entity in range [0,1]. */
        float getOpacity() const;

        /** \brief If set to true, the renderer smoothly lowers opacity to zero, when the camera center intersects the bounds of the corresponding entity. */
        bool isHiddenForCamera() const;

        /** \brief Returns the filepath of the texture if it is loaded. */
        std::string getFilePath() const;

    private:
        Image mImage;
        bool mVisible;
        float mOpacity;
        bool mHideForCamera; //smoothly lowers opacity to zero, when the camera center intersects the bounds of the corresponding entity
    };

    /**
    * \ingroup Components
    * \brief A component that models the visual appearence of an entity.
    * The vertex-array can contain one or multiple texture rects.
    * The only restriction is a single texture per entity. */
    class VertexArrayComponent : public Serializable<VertexArrayComponent>
    {
    friend class VisualsHandler;
    friend class Renderer;
     friend struct SerialBehavior<VertexArrayComponent, Entity>;
    public:
        /** \brief Default constructs the visual component. */
        VertexArrayComponent() = default;

        /** \brief Accesses the underlying vertex array. */
        const VertexArray& getVertices() const { return mVertices; }

    private:
        VertexArray mVertices;
        std::vector<std::string> mKeys;
    };


    /**
    * \ingroup Components
    * \brief A component that models a single sprite. */
    class SpriteComponent : public Serializable<SpriteComponent>
    {
    friend class VisualsHandler;
    friend class Renderer;
    friend struct SerialBehavior<SpriteComponent, Entity>;
    friend struct DeserialBehavior<SpriteComponent, Entity, DeserialMemory&>;
    public:
        SpriteComponent() = default;

        /** \brief Accesses the underlying sprite. */
        const Sprite& getSprite() const { return mSprite; }

    private:
        Sprite mSprite;
        std::string mKey;
    };

    /**
    * \ingroup Components
    * \brief MultiVisualTransform is only useful in combination with MultiVisuals. */
    using MultiSpriteComponent = MultiComponent<SpriteComponent>;

    using VisualAffectorComponentCallback = std::function<void(Entity, float, VisualsHandler&, VisualsComponent&)>;

    /**
    * \ingroup Components
    *\brief A component that wraps behavior that is applied to a Visuals-Component (and optionally
    * a VisualsTransform) every frame. */
    class VisualAffectorComponent : public Serializable<VisualAffectorComponent>
    {
    friend class VisualsHandler;
    friend class Renderer;
    public:
        VisualAffectorComponent();

        void setActive(bool active);

        bool isActive() const;

    private:
        VisualAffectorComponentCallback mCallback;
        bool mActive;
    };

    /**
    * \ingroup Components
    * \brief MultiComponent version of VisualAffectorComponent. */
    using MultiVisualAffectorComponent = MultiComponent<VisualAffectorComponent>;


    /**
    * \ingroup Components
    * \brief A component that can be used in combination with a Visuals component
    * that allows an entity to load metadata that allows a mapping from string keys
    * to texture rects.
    */
    class SpriteMetadataComponent : public Serializable<SpriteMetadataComponent>
    {
    friend class VisualsHandler;
     friend struct SerialBehavior<SpriteMetadataComponent, Entity>;
    public:
        /** \brief Default constructs a SpriteMetadta. */
        SpriteMetadataComponent() {}

        /** \brief Accesses the underlying meta map. */
        const MetaMap& getMetaMap() const {return mMeta; }

        /** \brief Returns the filepath of the metadata if it is loaded. */
        std::string getFilePath() const;

        /** \brief Returns the texrect of the given key or {0,0,0,0} if not found. */
        sf::IntRect getTexrect(const std::string& key) const;

    private:
        MetaMap mMeta;
    };


    /**
    * \ingroup Components
    * \brief A component that can be used in combination with a Visuals and a SpriteMetadataComponent-
    * component. Manages one animation. Animation components are limited to be used with Visuals
    * components only (non MultiVisuals-).
    */
    class AnimationComponent : public Serializable<AnimationComponent>
    {
    friend class VisualsHandler;
    friend class Renderer;
     friend struct SerialBehavior<AnimationComponent, Entity>;
    public:
        /** \brief Default constructs an Animation. */
        AnimationComponent() : mAnimation(), mVertices(nullptr) {}

        /** \brief Accesses the underlying animation. */
        const Animation& getAnimation() const { return mAnimation; }

    private:
        Animation mAnimation;
        sf::Vertex* mVertices;
    };

    /**
    * \ingroup Components
    * \brief Animation-components with multipe animations. */
    using MultiAnimationComponent = MultiComponent<AnimationComponent>;


    /**
    * \ingroup Components
    * \brief A component managing a BigSprite that renders textures that are (or can be) bigger than
    * the internal texture size limit.
    */
    class BigSpriteComponent : public Serializable<BigSpriteComponent>
    {
    friend class VisualsHandler;
    friend class Renderer;
     friend struct SerialBehavior<BigSpriteComponent, Entity>;
    public:
        /** \brief Default constructs a BigSpriteComponent. */
        BigSpriteComponent() : mVisible(false) {}

        /** \brief Accesses the underlying BigSprite. */
        const sf::BigSprite& getBigSprite() const { return mBigSprite; }

        /** \brief Returns true if a texture is successfully loaded. */
        bool isLoaded() const;

        /** \brief Returns the visibility status of the VIsuals-component. */
        bool isVisible() const;

        /** \brief Returns the filepath of the texture if it is loaded. */
        std::string getFilePath() const;

    private:
        sf::BigSprite mBigSprite;
        BigImage mBigImage;
        bool mVisible;
    };


    /** \brief Helper class that manages all actions performed on Visuals-components. Emits events. */
    class VisualsHandler
    {
    friend class Renderer;
    public:
        VisualsHandler() = default;

        /** \brief Inits a given number of texture rects that have to be defined afterwards.
        * Requires VertexArray component. */
        void initTextureRects(Entity e, std::size_t num);

        /** \brief Sets the texture rect for the sprite component of the given entity. Requires
        * a Sprite or a MultiSprite component. */
        inline void setSpriteTextureRect(Entity e, const sf::FloatRect& rect) { setSpriteTextureRect(e, e.modify<SpriteComponent>(), rect); }
        inline void setSpriteTextureRect(Entity e, const sf::FloatRect& rect, std::size_t multiIndex) { setSpriteTextureRect(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex), rect); }
        void setSpriteTextureRect(Entity e, SpriteComponent& sprite, const sf::FloatRect& rect);

        inline void setSpritePosition(Entity e, const sf::Vector2f& position) { setSpritePosition(e, e.modify<SpriteComponent>(), position); }
        inline void setSpritePosition(Entity e, const sf::Vector2f& position, std::size_t multiIndex) { setSpritePosition(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex), position);}
        void setSpritePosition(Entity e, SpriteComponent& sprite, const sf::Vector2f& position);

        /** \brief Sets the texture rect for the given index. Emits a "Contents-Changed" signal.
        * Requires a VertexArray component. */
        inline void setArrayTextureRect(Entity e, const sf::FloatRect& rect, std::size_t index) {setArrayTextureRect(e.modify<VertexArrayComponent>(), rect, index);}
        void setArrayTextureRect(VertexArrayComponent& vertices, const sf::FloatRect& rect, std::size_t index);

        /** \brief Sets the position of a texture rect in a vertex array. Requires VertexArray components. */
        void setTextureRectPosition(Entity e, const sf::Vector2f& position, std::size_t index);

        /** \brief Sets points for the representation explicit. */
        void setPoints(Entity e, std::size_t index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4)
        { setPoints(e, e.modify<VertexArrayComponent>(), index, p1, p2, p3, p4); }
        void setPoints(Entity e, VertexArrayComponent& vertices, std::size_t index, const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4);

        /** \brief Allows examination of the points of the given texture rect and point.
        * Point indexing is clockwise 0 to 3. Requires VertexArray-component. */
        const sf::Vector2f& getPoint(Entity e, std::size_t rectIndex, std::size_t pointIndex);

        /** \brief Accomodates a new texture rect on a new index. The index is returned. Requires a VertexArray component. */
        inline std::size_t newTextureRect(Entity e, const sf::FloatRect& rect) { return newTextureRect(e.modify<VertexArrayComponent>(), rect); }
        std::size_t newTextureRect(VertexArrayComponent& vertices, const sf::FloatRect& rect);

        /** \brief Creates a new texture rect with the bounds of the texture, if a texture is loaded. */
        inline std::size_t newTextureRect(Entity e) { return newTextureRect(e, e.modify<VertexArrayComponent>(), e.modify<VisualsComponent>()); }
        std::size_t newTextureRect(Entity e, VertexArrayComponent& vertices, VisualsComponent& vis);

        /** \brief Initializes async loading of the internal texture and invokes the given callback when the loading is done. */
        inline void loadTexture(Entity e, const std::string& imageID, std::function<void(VisualsComponent&)> callback)
         { loadTexture(e.modify<VisualsComponent>(), imageID, callback); }
        void loadTexture(VisualsComponent& visuals, const std::string& imageID, std::function<void(VisualsComponent&)> callback);

        /** \brief Waits for the loading of the image of the given entity. */
        void waitForLoading(Entity e);

        /** \brief Initializes sync or async loading of the internal texture. */
        inline void loadTexture(Entity e, const std::string& imageID, const LoadPolicy policy = LoadPolicy::SYNC)
        { loadTexture(e.modify<VisualsComponent>(), imageID, policy); }
        void loadTexture(VisualsComponent& visuals, const std::string& imageID, const LoadPolicy policy = LoadPolicy::SYNC);

        /** \brief Loads a new metadata for the given entity. Requires a SpriteMetadataComponent-component. */
        void loadMetadata(Entity e, const std::string& metaID);

        /** \brief Sets up the texture rect with given index for the given entity e and the given key. Requires a
        * SpriteMetadataComponent-component. */
        inline void setSpriteTextureRect(Entity e, const std::string& key)
        { setSpriteTextureRect(e, e.modify<SpriteComponent>(), e.get<SpriteMetadataComponent>(), key); }
        inline void setSpriteTextureRect(Entity e, const std::string& key, std::size_t multiIndex)
        { setSpriteTextureRect(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex), e.get<SpriteMetadataComponent>(), key); }
        void setSpriteTextureRect(Entity e, SpriteComponent& sprite, const SpriteMetadataComponent& data, const std::string& key);

        /** \brief Sets up the texture rect with given index for the given entity e and the given key. Requires a
        * SpriteMetadataComponent-component and a VertexArray component. */
        void setArrayTextureRect(Entity e, std::size_t index, const std::string& key);

        /** \brief Sets up anew texture rect for the given entity e and the given key. Requires a
        * SpriteMetadataComponent-component. */
        std::size_t newTextureRect(Entity e, const std::string& key);

        /** \brief Sets the visibility status of the given entity. Requires a visuals component. */
        void setVisible(Entity e, bool visible);

        /** \brief If set to true, entities with bounds that contain the current camera center, will be hidden. */
        void setHideForCamera(Entity e, bool hideForCamera);

        /** \brief Binds the vertices of a Sprite component to an animation. */
        inline static void bindSpriteToAnimation(Entity e)
        { bindSpriteToAnimation(e.modify<SpriteComponent>(), e.modify<AnimationComponent>()); }
        inline static void bindSpriteToAnimation(Entity e, std::size_t multiSpriteIndex, std::size_t multiAnimationIndex)
        { bindSpriteToAnimation(e.modify<MultiSpriteComponent>().getComponent(multiSpriteIndex), e.modify<MultiAnimationComponent>().getComponent(multiAnimationIndex));}
        static void bindSpriteToAnimation(SpriteComponent& sprite, AnimationComponent& animation);

        /** \brief Binds the vertices of a specific subrect of a vertexarry-component to an animation. */
        inline static void bindArrayToAnimation(Entity e, std::size_t index)
        {  bindArrayToAnimation(e.modify<VertexArrayComponent>(), e.modify<AnimationComponent>(), index); }
        inline static void bindArrayToAnimation(Entity e, std::size_t index, std::size_t multiAnimationIndex)
        { bindArrayToAnimation(e.modify<VertexArrayComponent>(), e.modify<MultiAnimationComponent>().getComponent(multiAnimationIndex), index);  }
        static void bindArrayToAnimation(VertexArrayComponent& vertices, AnimationComponent& animation, std::size_t index);

        /** \brief Sets up a new animation state for entity e with Animation- and SpriteMetadataComponent-components.
        * The state is identified by the given string key. Running flag will be true after this call. */
        bool setAnimationState(Entity e, const std::string& key);
        bool setAnimationState(Entity e, const std::string& key, std::size_t multiAnimationIndex);
        bool setAnimationState(Entity e, const SpriteMetadataComponent& meta, AnimationComponent& anim, const std::string& key);

        /** \brief Starts a new animation state for the given entity. This requires an VertexArray component and
        * an Animation component. The method appends a new texture-rect to the vertexarray and automatically connects
        * the new vertices with the animation component. It also sets up a new animation state.
        * The new animation state is identified by the given string key. Running flag will be true after this call. */
        bool newAnimationState(Entity e, const std::string& key);

        /** \brief Sets restarts or stops an animation. */
        void setRunning(Entity e, bool running);

        /** \brief Sets restarts or stops an animation. */
        void setRunning(Entity e, bool running, std::size_t animationIndex);


        /** \brief Sets the running speed of an animation. */
        void setAnimationSpeed(Entity e, float speed);

        /** \brief Sets the running speed of an multi-animation. */
        void setAnimationSpeed(Entity e, float speed, std::size_t animationIndex);


        /** \brief Registers new callback for the ContentsChanged signal. */
        void onContentsChanged(const std::function<void(Entity, const sf::FloatRect&)>& callback);

        /** \brief Registers new callback for the VisibilityChanged signal. */
        void onVisibilityChanged(const std::function<void(Entity, bool)>& callback);

        /** \brief Registers new callback for the AnimationStart signal. */
        void onAnimationStart(const std::function<void(Entity, const std::string&)>& callback);

        /** \brief Registers new callback for the AnimastionStop signal. */
        void onAnimationStop(const std::function<void(Entity, const std::string&)>& callback);

        /** \brief Registers new callback for the AnimastionStop signal. */
        void onAnimationFrame(const std::function<void(Entity, const std::string&, int)>& callback);

        /** \brief Returns the lower bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getLowerBound(Entity e);

        /** \brief Returns the upper bound of the bounding rect around all contents of the given entity. */
        sf::Vector2f getUpperBound(Entity e);

        /** \brief Returns the lower bound of the bounding rect around all contents of the given entity, but
        * without taking the local transforms of these contents into account. */
        sf::Vector2f getUntransformedLowerBound(Entity e);

        /** \brief Returns the upper bound of the bounding rect around all contents of the given entity, but
        * without taking the local transforms of these contents into account. */
        sf::Vector2f getUntransformedUpperBound(Entity e);

        /** \brief A method that moves the visuals attached to the given entity. This method is usually only
        * used internally by the transform-manager. */
        void moveVisuals(Entity e, const sf::Vector2f& vec);

        /** \brief Sets the rotation of the sprite. Requires Sprite component. */
        inline void setRotation(Entity e, float rotation) {setRotation(e, e.modify<SpriteComponent>(), rotation);}
        inline void setRotation(Entity e, float rotation, std::size_t multiIndex) {setRotation(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex), rotation);}
        void setRotation(Entity e, SpriteComponent& sprite, float rotation);

        /** \brief Sets the scale of the sprite. Requires Sprite component.*/
        inline void setScale(Entity e, float scalex, float scaley) {setScale(e, e.modify<SpriteComponent>(), {scalex, scaley});}
        inline void setScale(Entity e, const sf::Vector2f& scale) {setScale(e, e.modify<SpriteComponent>(), scale);}
        inline void setScale(Entity e, float scalex, float scaley, std::size_t multiIndex) { setScale(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex),  {scalex, scaley});}
        inline void setScale(Entity e, const sf::Vector2f& scale, std::size_t multiIndex) { setScale(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex), scale);}
        void setScale(Entity e, SpriteComponent& sprite, const sf::Vector2f& scale);

        /** \brief Sets the origin of the sprite. Requires Sprite component. */
        inline void setOrigin(Entity e, const sf::Vector2f& origin) {setOrigin(e, e.modify<SpriteComponent>(), origin);}
        inline void setOrigin(Entity e, const sf::Vector2f& origin, std::size_t multiIndex) {setOrigin(e, e.modify<MultiSpriteComponent>().getComponent(multiIndex), origin);}
        void setOrigin(Entity e, SpriteComponent& sprite, const sf::Vector2f& origin);

        /** \brief Sets a affector callback. Requires a VisualAffectorComponent (MultiVisualAffectorComponent) component. */
        inline void setAffectorCallback(Entity e, const VisualAffectorComponentCallback& callback) {setAffectorCallback(e.modify<VisualAffectorComponent>(), callback);}
        inline void setAffectorCallback(Entity e, const VisualAffectorComponentCallback& callback, std::size_t multiIndex) {setAffectorCallback(e.modify<MultiVisualAffectorComponent>().getComponent(multiIndex), callback);}
        void setAffectorCallback(VisualAffectorComponent& affector, const VisualAffectorComponentCallback& callback);

        /** \brief Sets the color of a sprite component. */
        inline void setSpriteColor(Entity e, const sf::Color& color) {setSpriteColor(e.modify<SpriteComponent>(), color);}
        inline void setSpriteColor(Entity e, const sf::Color& color, std::size_t multiIndex) {setSpriteColor(e.modify<MultiSpriteComponent>().getComponent(multiIndex), color);}
        void setSpriteColor(SpriteComponent& sprite, const sf::Color& color);

        /** \brief Sets the color of an array-texture-rect. */
        inline void setArrayRectColor(Entity e, const sf::Color& color, std::size_t index) {setArrayRectColor(e.modify<VertexArrayComponent>(), color, index);}
        void setArrayRectColor(VertexArrayComponent& vertices, const sf::Color& color, std::size_t index);

        /** \brief Sets the opacity of the entity in range [0,1]. */
        static void setOpacity(Entity e, float opacity);

        /** \brief Flips the vertex-array of the given entity in x direction. */
        inline static void flipVertexX(Entity e) {flipVertexX(e.modify<VertexArrayComponent>());}
        static void flipVertexX(VertexArrayComponent& vertices);

        /** \brief Flips the vertex-array of the given entity in y direction. */
        inline static void flipVertexY(Entity e) {flipVertexY(e.modify<VertexArrayComponent>());}
        static void flipVertexY(VertexArrayComponent& vertices);

        /** \brief Flips the i-th rect of the vertex-array of the given entity in x direction. */
        inline static void flipVertexX(Entity e, unsigned i) { flipVertexX(e.modify<VertexArrayComponent>(), i); }
        static void flipVertexX(VertexArrayComponent& vertices, unsigned i);

        /** \brief Flips the i-th rect of the the vertex-array of the given entity in y direction. */
        inline static void flipVertexY(Entity e, unsigned i) { flipVertexY(e.modify<VertexArrayComponent>(), i); }
        static void flipVertexY(VertexArrayComponent& vertices, unsigned i);

        /** \brief Flips the sprite of the given entity in x direction. */
        inline static void flipSpriteX(Entity e) {flipSpriteX(e.modify<SpriteComponent>());}
        inline static void flipSpriteX(Entity e, std::size_t multiIndex) {flipSpriteY(e.modify<MultiSpriteComponent>().getComponent(multiIndex));}
        static void flipSpriteX(SpriteComponent& sprite);

        /** \brief Flips the sprite of the given entity in y direction. */
        inline static void flipSpriteY(Entity e) {flipSpriteY(e.modify<SpriteComponent>());}
        inline static void flipSpriteY(Entity e, std::size_t multiIndex) {flipSpriteY(e.modify<MultiSpriteComponent>().getComponent(multiIndex));}
        static void flipSpriteY(SpriteComponent& sprite);


        /** \brief Loads a texture that can be bigger than the hardware limitations into memory and displays it in a BigSpriteComponent. */
        inline void loadBigTexture(Entity e, const std::string& filepath, LoadPolicy policy = LoadPolicy::ASYNC) { loadBigTexture(e, e.modify<BigSpriteComponent>(), filepath, policy); }
        void loadBigTexture(Entity e, BigSpriteComponent& bigSprite, const std::string& filepath, LoadPolicy policy);

        /** \brief Loads a texture that can be bigger than the hardware limitations into memory and displays it in a BigSpriteComponent. */
        inline void setBigSpriteTexture(Entity e) { setBigSpriteTexture(e, e.modify<BigSpriteComponent>()); }
        void setBigSpriteTexture(Entity e, BigSpriteComponent& bigSprite);

        /** \brief Sets the visibility flag of the BigTextureComponent. */
        inline void setBigSpriteVisibility(Entity e, bool visible = true) { setBigSpriteVisibility(e.modify<BigSpriteComponent>(), visible); }
        void setBigSpriteVisibility(BigSpriteComponent& bigSprite, bool visible = true);

        /** \brief Sets the position of the BigTextureComponent. */
        inline void setBigSpritePosition(Entity e, const sf::Vector2f& position) { setBigSpritePosition(e, e.modify<BigSpriteComponent>(), position); }
        void setBigSpritePosition(Entity e, BigSpriteComponent& bigSprite, const sf::Vector2f& position);

        /** \brief Sets the color of the BigTextureComponent. */
        inline void setBigSpriteColor(Entity e, const sf::Color& color) { setBigSpriteColor(e.modify<BigSpriteComponent>(), color); }
        void setBigSpriteColor(BigSpriteComponent& bigSprite, const sf::Color& color);

        /** \brief Sets the scale of the BigTextureComponent. */
        inline void setBigSpriteScale(Entity e, const sf::Vector2f& scale) { setBigSpriteScale(e, e.modify<BigSpriteComponent>(), scale); }
        void setBigSpriteScale(Entity e, BigSpriteComponent& bigSprite, const sf::Vector2f& scale);

        /** \brief Sets the origin of the BigTextureComponent. */
        inline void setBigSpriteOrigin(Entity e, const sf::Vector2f& origin) { setBigSpriteOrigin(e, e.modify<BigSpriteComponent>(), origin); }
        void setBigSpriteOrigin(Entity e, BigSpriteComponent& bigSprite, const sf::Vector2f& origin);

        /** \brief Sets the scale of the BigTextureComponent. */
        inline void setBigSpriteRotation(Entity e, float angle) { setBigSpriteRotation(e, e.modify<BigSpriteComponent>(), angle); }
        void setBigSpriteRotation(Entity e, BigSpriteComponent& bigSprite, const float angle);

    private:
        sf::Vector2f getLowerBoundInternal(Entity e, const std::function<sf::FloatRect(const Sprite&)>& boundsGetter);
        sf::Vector2f getUpperBoundInternal(Entity e, const std::function<sf::FloatRect(const Sprite&)>& boundsGetter);
        static void componentOpacitySet(Entity e, float opacity);

    private:
        owls::Signal<Entity, const sf::FloatRect&> mContentsChangedSignal;
        owls::Signal<Entity, bool> mVisibilityChangedSignal;
        owls::Signal<Entity, const std::string&> mAnimationStartSignal;
        owls::Signal<Entity, const std::string&, int> mAnimationFrameSignal;
        owls::Signal<Entity, const std::string&> mAnimationStopSignal;
    };
}

#endif // VISUAL_H
