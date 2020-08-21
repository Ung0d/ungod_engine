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

#include "ungod/content/particle_system/Affector.h"
#include "ungod/content/particle_system/ParticleSystem.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/Utility.h"
#include <cmath>

namespace ungod
{
        void directionalForce(DirectionalForce& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                accelerate(particles.getParticleData(i).mov, data.force, 1.0f);
            }
        }

        void displaceForce(DisplaceForce& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                displace(particles.getParticleData(i).mov, data.speed, data.circle, data.angle);
            }
        }

        void fadeOut(FadeOut&, ParticleSystem& particles, float)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                float rel = 4*particles.getParticleData(i).lifetime / particles.getParticleData(i).maxlifetime;
                if (rel <= 1.0f)
                    particles.getParticleData(i).color.a = static_cast<sf::Uint8>(255 * rel);
            }
        }

        void fadeIn(FadeIn& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                float rel = 4 * (1 - particles.getParticleData(i).lifetime / particles.getParticleData(i).maxlifetime);
                if (rel <= 1.0f)
                    particles.getParticleData(i).color.a = static_cast<sf::Uint8>(255 * rel);
            }
        }


        void AnimatedParticles::init(const std::string& metaID, const std::string& keyInit, std::size_t numAnim)
        {
            key = keyInit;

            meta.load(metaID);
            if (meta.isLoaded())
            {
                animations.resize(numAnim);
                for (auto& a : animations)
                {
                    a.animation.startAnimation(meta, key, a.vertices);
                    a.animation.setFrame( NumberGenerator::getRandBetw(0, a.animation.getFrameCount()-1), a.vertices);
                }
            }
        }

        void animatedParticles(AnimatedParticles& apdata, ParticleSystem& particles, float delta)
        {
            if (!apdata.meta.isLoaded())
                return;

            for (auto& a : apdata.animations)
            {
                a.animation.update(delta, a.vertices);
            }

            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                sf::Vertex* vert = particles.getVertices(i);
                detail::ParticleData& data = particles.getParticleData(i);
                //copy texrect
                vert[0].texCoords =  apdata.animations[data.animIndex].vertices[0].texCoords;
                vert[1].texCoords =  apdata.animations[data.animIndex].vertices[1].texCoords;
                vert[2].texCoords =  apdata.animations[data.animIndex].vertices[2].texCoords;
                vert[3].texCoords =  apdata.animations[data.animIndex].vertices[3].texCoords;
            }
        }

        void colorShift(ColorShift& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                float rel = particles.getParticleData(i).lifetime / particles.getParticleData(i).maxlifetime;
                particles.getParticleData(i).color.r = sf::Uint8(data.colorBegin.r*rel + data.colorEnd.r*(1-rel));
                particles.getParticleData(i).color.g = sf::Uint8(data.colorBegin.g*rel + data.colorEnd.g*(1-rel));
                particles.getParticleData(i).color.b = sf::Uint8(data.colorBegin.b*rel + data.colorEnd.b*(1-rel));
            }
        }


        void rotateParticle(RotateParticle& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                particles.getParticleData(i).rotation += data.speed * delta / 20.0f;
            }
        }


        void scaleParticle(ScaleParticle& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                float rel = particles.getParticleData(i).lifetime / particles.getParticleData(i).maxlifetime;
                particles.getParticleData(i).scale.x = ( data.scalesBegin.x*rel + data.scalesEnd.x*(1-rel)) / 2;
                particles.getParticleData(i).scale.y = ( data.scalesBegin.y*rel + data.scalesEnd.y*(1-rel)) / 2;
            }
        }


        void velocityBasedRotation(VelocityBasedRotation& data, ParticleSystem& particles, float delta)
        {
            for (std::size_t i = 0; i < particles.getParticleCount(); i++)
            {
                if (particles.getParticleData(i).mov.velocity.x <= 0.0f)
                    particles.getParticleData(i).rotation = std::acos( (particles.getParticleData(i).mov.velocity.y)/magnitude(particles.getParticleData(i).mov.velocity) ) * 180.0f / PI;
                else
                    particles.getParticleData(i).rotation = 360 - std::acos((particles.getParticleData(i).mov.velocity.y) / magnitude(particles.getParticleData(i).mov.velocity)) * 180.0f / PI;
            }
        }


        sf::IntRect explicitTexrect(ExplicitTexrect& data)
        {
            return data.rect;
        }


        void TexrectByKey::init(const std::string& metaID, const std::string& k)
        {
            key = k;
            meta.load(metaID);
            if (meta.isLoaded())
            {
                node = meta.getNodeWithKey(key);
            }
        }

        sf::IntRect texrectByKey(TexrectByKey& data)
        {
            if (!data.node)
                return {};
            auto result = data.node.getAttributes<int, int, int, int>(
                    {"pos_x", 0}, {"pos_y", 0}, {"width", 0}, {"height", 0} );
            return sf::IntRect{std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result)};
        }


        void MultipleTexrectsByKey::init(const std::string& metaID, const std::vector<std::string>& k)
        {
            keys = k;
            meta.load(metaID);
            if (meta.isLoaded())
            {
                nodes.clear();
                nodes.reserve(keys.size());
                for (const auto& key : keys)
                    nodes.emplace_back(meta.getNodeWithKey(key));
            }
        }

        sf::IntRect multipleTexrectsByKey(MultipleTexrectsByKey& data)
        {
            if (data.keys.empty())
                return {};
            unsigned keyid = (unsigned)NumberGenerator::getRandBetw(0, (unsigned)(data.keys.size()-1));
            if (!data.nodes[keyid])
                return {};
            auto result = data.nodes[keyid].getAttributes<int, int, int, int>(
                    {"pos_x", 0}, {"pos_y", 0}, {"width", 0}, {"height", 0} );
            return sf::IntRect{std::get<0>(result), std::get<1>(result), std::get<2>(result), std::get<3>(result)};
        }
}
