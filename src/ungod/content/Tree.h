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

#ifndef UNGOD_TREE_H
#define UNGOD_TREE_H

#include "ungod/base/EntityUtility.h"
#include "ungod/base/BehaviorIDs.h"
#include "ungod/base/World.h"

namespace ungod
{
    using TreeBase = BaseComponents< TransformComponent, SpriteMetadataComponent, VisualsComponent, MultiSpriteComponent, MultiVisualAffectorComponent, RigidbodyComponent<>, Initializer >;

    /** \brief A class that handles the creation of trees. Each tree is based on a texture and pools of
    * keys, that map on trunk and leaves in different sizes. A tree can be created with explicit parameters or
    * as a result of a random generation. */
    class TreeGenerator : public Serializable<TreeGenerator>
    {
     friend struct SerialBehavior< TreeGenerator >;
    friend struct DeserialBehavior< TreeGenerator >;

    public:
        TreeGenerator() : mRandomColor(false), mBaseColor(sf::Color::White) {}

        /** \brief Inits a key set for this tree generator. */
        void initKeyset( const std::string& texture, const std::string& meta,
                         std::initializer_list< std::pair<std::string, sf::Vector2f> > smalltrunk,  //pairs key with pivot point
                         std::initializer_list< std::pair<std::string, sf::Vector2f> > mediumtrunk,
                         std::initializer_list< std::pair<std::string, sf::Vector2f> > bigtrunk,
                         std::initializer_list<std::string> smallleaves,
                         std::initializer_list<std::string> mediumleaves,
                         std::initializer_list<std::string> bigleaves );

        /** \brief A method that randomly builds a tree given an entity with 'TreeBase'-components attached. */
        void buildBigTree(Entity e, World& world) const;

        /** \brief A method that randomly builds a tree given an entity with 'TreeBase'-components attached. */
        void buildMediumTree(Entity e, World& world) const;

        /** \brief A method that randomly builds a tree given an entity with 'TreeBase'-components attached. */
        void buildSmallTree(Entity e, World& world) const;

        /** \brief A method that gets explicit keys and a leave positions and builds a tree. */
        void build(Entity e, World& world, const std::string& stumb, const sf::Vector2f& pivotPos,
                   std::size_t bigCount, std::size_t mediumCount, std::size_t smallCount) const;

        /** \brief Sets a base color for random leaves-coloration. */
        void setRandomBaseColor(const sf::Color& color, float randVariance = 0.4f);

    private:
        //a set of keys for stumb and leaves, random generation graps keys out of the pools randomly
        struct KeySet
        {
            std::string texture;
            std::string meta;
            std::vector< std::pair<std::string, sf::Vector2f> > smallStumbKeys;
            std::vector< std::pair<std::string, sf::Vector2f> > mediumStumbKeys;
            std::vector< std::pair<std::string, sf::Vector2f> > bigStumbKeys;
            std::vector<std::string> smallLeavesKeys;
            std::vector<std::string> mediumLeavesKeys;
            std::vector<std::string> bigLeavesKeys;
        };

        void prepare(Entity e, World& world, std::size_t compCount) const;

    private:
        KeySet mKeyset;
        bool mRandomColor;
        sf::Color mBaseColor;
        sf::Color mVarianceColor;
    };


    template<>
    struct SerialIdentifier< TreeGenerator >
    {
        static std::string get() { return "TreeGenerator"; }
    };

    template <>
    struct SerialBehavior< TreeGenerator >
    {
        static void serialize(const TreeGenerator& data, MetaNode serializer, SerializationContext& context);
    };
    template <>
    struct DeserialBehavior< TreeGenerator >
    {
        static void deserialize(TreeGenerator& data, MetaNode deserializer, DeserializationContext& context);
    };

    void initTree(Entity e, World& world);
}

#endif // UNGOD_TREE_H
