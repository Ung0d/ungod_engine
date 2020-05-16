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

#include "ungod/content/Tree.h"
#include "ungod/visual/Visual.h"

namespace ungod
{

    void TreeGenerator::initKeyset( const std::string& texture, const std::string& meta,
                                     std::initializer_list< std::pair<std::string, sf::Vector2f> > smallStumb,
                                     std::initializer_list< std::pair<std::string, sf::Vector2f> > mediumStumb,
                                     std::initializer_list< std::pair<std::string, sf::Vector2f> > bigStumb,
                                     std::initializer_list<std::string> smallleaves,
                                     std::initializer_list<std::string> mediumleaves,
                                     std::initializer_list<std::string> bigleaves )
     {
        mKeyset.texture = texture;
        mKeyset.meta = meta;
        mKeyset.smallStumbKeys = smallStumb;
        mKeyset.mediumStumbKeys = mediumStumb;
        mKeyset.bigStumbKeys = bigStumb;
        mKeyset.smallLeavesKeys = smallleaves;
        mKeyset.mediumLeavesKeys = mediumleaves;
        mKeyset.bigLeavesKeys = bigleaves;

        if (mKeyset.smallStumbKeys.size() == 0 ||
            mKeyset.mediumStumbKeys.size() == 0 ||
            mKeyset.bigStumbKeys.size() == 0 ||
            mKeyset.smallLeavesKeys.size() == 0 ||
            mKeyset.mediumLeavesKeys.size() == 0 ||
            mKeyset.bigLeavesKeys.size() == 0)
        {
            Logger::warning("A key list in a tree-keyset is empty.");
            Logger::endl();
        }
     }

    void TreeGenerator::buildBigTree(Entity e, World& world) const
    {
        static const std::size_t COMP_COUNT_BIG = 9u;
        prepare(e, world, COMP_COUNT_BIG);
        const auto& bigstumb = mKeyset.bigStumbKeys.at( NumberGenerator::getRandBetw(0, (unsigned)mKeyset.bigStumbKeys.size()-1) );
        build(e, world, bigstumb.first, bigstumb.second, 1, 3, 4);
    }

    void TreeGenerator::buildMediumTree(Entity e, World& world) const
    {
        static const std::size_t COMP_COUNT_MEDIUM = 6u;
        prepare(e, world, COMP_COUNT_MEDIUM);
        const auto& mediumstumb = mKeyset.mediumStumbKeys.at( NumberGenerator::getRandBetw(0, (unsigned)mKeyset.mediumStumbKeys.size()-1) );
        build(e, world, mediumstumb.first, mediumstumb.second, 0, 2, 3);
    }

    void TreeGenerator::buildSmallTree(Entity e, World& world) const
    {
        static const std::size_t COMP_COUNT_SMALL = 4u;
        prepare(e, world, COMP_COUNT_SMALL);
        const auto& smallstumb = mKeyset.smallStumbKeys.at( NumberGenerator::getRandBetw(0, (unsigned)mKeyset.smallStumbKeys.size()-1) );
        build(e, world, smallstumb.first, smallstumb.second, 0, 1, 2);
    }

    void TreeGenerator::build(Entity e, World& world, const std::string& stumb, const sf::Vector2f& pivotPos,
                              std::size_t bigCount, std::size_t mediumCount, std::size_t smallCount) const
    {
        //select leaves
        std::vector< std::string > leaves;
        std::vector< sf::Vector2f > offsets;

        //select big leaves
        for (unsigned i = 0; i < bigCount; ++i)
        {
            leaves.push_back( mKeyset.bigLeavesKeys.at( NumberGenerator::getRandBetw(0, (unsigned)mKeyset.bigLeavesKeys.size()-1) ) );
            offsets.emplace_back ( std::max(-0.6f, std::min(0.5f, NumberGenerator::getNormRand(0,0.3f))),
                                     std::max(-0.5f, std::min(0.3f, NumberGenerator::getNormRand(0,0.15f))) );
        }

        //select medium leaves
        for (unsigned i = 0; i < mediumCount; ++i)
        {
            leaves.push_back( mKeyset.mediumLeavesKeys.at( NumberGenerator::getRandBetw(0, (unsigned)mKeyset.mediumLeavesKeys.size()-1) ) );
            offsets.emplace_back ( std::max(-0.6f, std::min(0.5f, NumberGenerator::getNormRand(0,0.3f))),
                                     std::max(-0.5f, std::min(0.3f, NumberGenerator::getNormRand(0,0.15f))) );
        }

        //select small leaves
        for (unsigned i = 0; i < smallCount; ++i)
        {
            leaves.push_back( mKeyset.smallLeavesKeys.at( NumberGenerator::getRandBetw(0, (unsigned)mKeyset.smallLeavesKeys.size()-1) ) );
            offsets.emplace_back ( std::max(-0.6f, std::min(0.5f, NumberGenerator::getNormRand(0,0.3f))),
                                     std::max(-0.5f, std::min(0.3f, NumberGenerator::getNormRand(0,0.15f))) );
        }

        //build whole tree
        world.getVisualsManager().setSpriteTextureRect(e, stumb, 0);
        sf::FloatRect bounds = e.get<MultiSpriteComponent>().getComponent(0).getSprite().getTextureRect();
        for (unsigned i = 0; i < leaves.size(); ++i)
        {
            sf::Vector2f defPos{ pivotPos.x + bounds.width * offsets[i].x, pivotPos.y + bounds.height * offsets[i].y };
            world.getVisualsManager().setSpriteTextureRect(e, leaves[i], i);
            world.getVisualsManager().setSpritePosition(e, defPos, i);
            sf::FloatRect leavesbounds = e.get<MultiSpriteComponent>().getComponent(i+1).getSprite().getTextureRect();
            world.getVisualsManager().setOrigin(e, {leavesbounds.width*0.5f, leavesbounds.height*0.5f}, i+1);
            if (mRandomColor)
            {
                int red = mBaseColor.r;
                int green = mBaseColor.g;
                int blue = mBaseColor.b;
                //idea: the greate a color value, the more variation is possible
                red = std::min( 255, std::max(0, red - NumberGenerator::getRandBetw(-mVarianceColor.r, mVarianceColor.r)));
                green = std::min( 255, std::max(0, green - NumberGenerator::getRandBetw(-mVarianceColor.g, mVarianceColor.g)));
                blue = std::min( 255, std::max(0, blue - NumberGenerator::getRandBetw(-mVarianceColor.b, mVarianceColor.b)));
                world.getVisualsManager().setSpriteColor(e.modify<MultiSpriteComponent>().getComponent(i+1), sf::Color(red, green, blue));
            }
        }
    }


    void TreeGenerator::setRandomBaseColor(const sf::Color& color, float randVariance)
    {
        mRandomColor = true;
        mVarianceColor.r = (sf::Uint8)(color.r*randVariance);
        mVarianceColor.g = (sf::Uint8)(color.g*randVariance);
        mVarianceColor.b = (sf::Uint8)(color.b*randVariance);
        mBaseColor = color;
    }


    void TreeGenerator::prepare(Entity e, World& world, std::size_t compCount) const
    {
        e.initMulti<MultiSpriteComponent>(compCount);
        e.initMulti<MultiVisualAffectorComponent>(compCount-1); //strump

        world.getVisualsManager().loadTexture(e, mKeyset.texture, LoadPolicy::ASYNC);
        world.getVisualsManager().loadMetadata(e, mKeyset.meta);

        //world.getRigidbodyManager().addCollider(e, )
    }


    void SerialBehavior< TreeGenerator >::serialize(const TreeGenerator& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("texture", data.mKeyset.texture, serializer);
        context.serializeProperty("meta", data.mKeyset.meta, serializer);

        MetaNode smallStumbNode = context.appendSubnode(serializer, "small_trunks");
        MetaNode mediumStumbNode = context.appendSubnode(serializer, "medium_trunks");
        MetaNode bigStumbNode = context.appendSubnode(serializer, "big_trunks");
        MetaNode smallLeavesNode = context.appendSubnode(serializer, "small_leaves");
        MetaNode mediumLeavesNode = context.appendSubnode(serializer, "medium_leaves");
        MetaNode bigLeavesNode = context.appendSubnode(serializer, "big_leaves");

        //helper lambdas, no depper meaning
        const static auto stumbHelper = [] (const std::pair<std::string, sf::Vector2f>& e, MetaNode father, SerializationContext& context)
        {
            MetaNode stumbNode = context.appendSubnode(father, e.first);
            context.serializeProperty("pivot_x", e.second.x, stumbNode);
            context.serializeProperty("pivot_y", e.second.y, stumbNode);
        };

        for (const auto& e : data.mKeyset.smallStumbKeys) stumbHelper(e, smallStumbNode, context);
        for (const auto& e : data.mKeyset.mediumStumbKeys) stumbHelper(e, mediumStumbNode, context);
        for (const auto& e : data.mKeyset.bigStumbKeys) stumbHelper(e, bigStumbNode, context);
        for (const auto& e : data.mKeyset.smallLeavesKeys) context.appendSubnode(smallLeavesNode, e);
        for (const auto& e : data.mKeyset.mediumLeavesKeys) context.appendSubnode(mediumLeavesNode, e);
        for (const auto& e : data.mKeyset.bigLeavesKeys) context.appendSubnode(bigLeavesNode, e);
    }


    void DeserialBehavior< TreeGenerator >::deserialize(TreeGenerator& data, MetaNode deserializer, DeserializationContext& context)
    {
        //get texture and meta paths and store them
        auto result = deserializer.getAttributes<std::string, std::string>( {"texture", ""}, {"meta", ""} );
        data.mKeyset.texture = std::get<0>(result);
        data.mKeyset.meta = std::get<1>(result);

        //helper lamdas
        const static auto stumbHelper = [] (std::vector< std::pair<std::string, sf::Vector2f> >& trunkKeys, MetaNode node)
        {
            forEachSubnode(node, [&trunkKeys] (MetaNode trunk)
            {
                auto result = trunk.getAttributes<float, float>( {"pivot_x", 0.0f}, {"pivot_y", 0.0f} );
                trunkKeys.emplace_back( trunk.name(), sf::Vector2f(std::get<0>(result), std::get<1>(result)) );
            });
        };
        const static auto leavesHelper = [] (std::vector<std::string>& leavesKeys, MetaNode node)
        {
            forEachSubnode(node, [&leavesKeys] (MetaNode leaves)
            {
                leavesKeys.emplace_back( leaves.name() );
            });
        };

        //get the subnodes with the keys, skip if something goes wrong
        auto nodes = getExplicitSubnodes(deserializer, { "small_trunks", "medium_trunks", "big_trunks", "small_leaves", "medium_leaves", "big_leaves" });
        if (!nodes.empty())
        {
            stumbHelper(data.mKeyset.smallStumbKeys, nodes[0]);
            stumbHelper(data.mKeyset.mediumStumbKeys, nodes[1]);
            stumbHelper(data.mKeyset.bigStumbKeys, nodes[2]);
            leavesHelper(data.mKeyset.smallLeavesKeys, nodes[3]);
            leavesHelper(data.mKeyset.mediumLeavesKeys, nodes[4]);
            leavesHelper(data.mKeyset.bigLeavesKeys, nodes[5]);
        }

        //print error stuff
        Logger::error("Something went wront while deserializing a tree keyset.");
    }


    void initTree(Entity e, World& world)
    {
        static const float LEAVE_ROTATION = 0.005f;

        MultiVisualAffectorComponent& affector = e.modify<MultiVisualAffectorComponent>();
        for (unsigned i = 0; i < affector.getComponentCount(); ++i)
        {
            bool clockwise = NumberGenerator::getRandBool();
            world.getVisualsManager().setRotation(e, NumberGenerator::getFloatRandBetw(0.0f, 360.0f), i+1);
            world.getVisualsManager().setAffectorCallback(e,
              [i, clockwise] (Entity e, float delta, VisualsHandler& mgr, VisualsComponent& vis) mutable
              {
                  float current = e.get<MultiSpriteComponent>().getComponent(i+1).getSprite().getRotation();
                  float acum = LEAVE_ROTATION * delta;
                  if (clockwise)
                  {
                    if (current - acum >= 0.0f)
                        mgr.setRotation(e, current - acum, i+1);
                    else
                    {
                        mgr.setRotation(e, 0.0f, i+1);
                        clockwise = false;
                    }
                  }
                  else
                  {
                    if (current + acum <= 360.0f)
                        mgr.setRotation(e, current + acum, i+1);
                    else
                    {
                        mgr.setRotation(e, 360.0f, i+1);
                        clockwise = true;
                    }
                  }
              }, i);
        }
    }
}
