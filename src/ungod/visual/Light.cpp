/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This is a modified version of the Let There Be Light 2 framework.
* See https://github.com/222464/LTBL2
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

#include "ungod/visual/Light.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/World.h"
#include "ungod/application/Application.h"

namespace ungod
{
    BaseLight::BaseLight() : mActive(true) {}

    void BaseLight::setActive(bool active)
    {
        mActive = active;
    }

    bool BaseLight::isActive() const
    {
        return mActive;
    }

    void BaseLight::toggleActive()
    {
        mActive = !mActive;
    }

    void BaseLight::unmaskWithPenumbras(sf::RenderTexture& renderTexture,
                                        sf::RenderStates states,
                                        sf::Shader& unshadowShader,
                                        const std::vector<Penumbra>& penumbras,
                                        float shadowExtension) const
    {
        sf::VertexArray vertexArray;
        vertexArray.setPrimitiveType(sf::PrimitiveType::Triangles);
        vertexArray.resize(3);

        states.shader = &unshadowShader;

        for (std::size_t i = 0; i < penumbras.size(); ++i)
        {
            unshadowShader.setUniform("lightBrightness", penumbras[i].lightBrightness);
            unshadowShader.setUniform("darkBrightness", penumbras[i].darkBrightness);
            vertexArray[0].position = penumbras[i].source;
            vertexArray[1].position = penumbras[i].source + normalizeVector(penumbras[i].lightEdge) * shadowExtension;
            vertexArray[2].position = penumbras[i].source + normalizeVector(penumbras[i].darkEdge) * shadowExtension;
            vertexArray[0].texCoords = sf::Vector2f(0.0f, 1.0f);
            vertexArray[1].texCoords = sf::Vector2f(1.0f, 0.0f);
            vertexArray[2].texCoords = sf::Vector2f(0.0f, 0.0f);
            renderTexture.draw(vertexArray, states);
        }
    }


    LightCollider::LightCollider() : mShape(), mLightOverShape(false)
    {
        mShape.setFillColor(sf::Color::Black);
    }

    LightCollider::LightCollider(std::size_t numPoints) : mShape(), mLightOverShape(false)
    {
        mShape.setPointCount(numPoints);
        mShape.setFillColor(sf::Color::Black);
    }

    sf::FloatRect LightCollider::getBoundingBox() const
    {
        return mShape.getGlobalBounds();
    }

    void LightCollider::setPointCount(std::size_t numPoints)
    {
        mShape.setPointCount(numPoints);
    }

    std::size_t LightCollider::getPointCount() const
    {
        return mShape.getPointCount();
    }

    void LightCollider::setPoint(std::size_t index, const sf::Vector2f& point)
    {
        mShape.setPoint(index, point);
    }

    sf::Vector2f LightCollider::getPoint(std::size_t index) const
    {
        return mShape.getPoint(index);
    }

    const sf::Transform& LightCollider::getTransform() const
    {
        return mShape.getTransform();
    }

    bool LightCollider::getLightOverShape() const
    {
        return mLightOverShape;
    }

    void LightCollider::setLightOverShape(bool los)
    {
        mLightOverShape = los;
    }

    void LightCollider::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        target.draw(mShape, states);
    }

    void LightCollider::setColor(const sf::Color& color)
    {
        mShape.setFillColor(color);
    }

    const sf::ConvexShape& LightCollider::getShape() const
    {
        return mShape;
    }

    const std::string PointLight::DEFAULT_TEXTURE_PATH = "resource/pointLightTexture.png";
    constexpr float PointLight::DEFAULT_RADIUS;
    constexpr float PointLight::DEFAULT_SHADOW_EXTEND_MULTIPLIER;

    PointLight::PointLight(const std::string& texturePath) : mSprite(), mSourcePoint(0.0f, 0.0f), mRadius(DEFAULT_RADIUS), mShadowOverExtendMultiplier(DEFAULT_SHADOW_EXTEND_MULTIPLIER)
    {
        loadTexture(texturePath);
    }

    sf::FloatRect PointLight::getBoundingBox() const
    {
        return mSprite.getGlobalBounds();
    }

    void PointLight::render(const sf::View& view,
                sf::RenderTexture& lightTexture,
                sf::RenderTexture& emissionTexture,
                sf::RenderTexture& antumbraTexture,
				sf::RenderStates states,
                const std::vector< std::pair<LightCollider*, TransformComponent*> >& colliders,
                sf::Shader& unshadowShader,
                sf::Shader& lightOverShapeShader,
                const TransformComponent& transf) const
    {
        states.transform *= transf.getTransform();

        // Render on Emission Texture : used by lightOverShapeShader
        emissionTexture.clear();
        emissionTexture.setView(view);
        emissionTexture.draw(mSprite, states);
        emissionTexture.display();

        //Init
        float shadowExtension = mShadowOverExtendMultiplier * (getBoundingBox().width + getBoundingBox().height);

        struct OuterEdges
        {
            std::vector<int> outerBoundaryIndices;
            std::vector<sf::Vector2f> outerBoundaryVectors;
        };

        std::vector<OuterEdges> outerEdges(colliders.size());

        std::vector<int> innerBoundaryIndices;
        std::vector<sf::Vector2f> innerBoundaryVectors;
        std::vector<Penumbra> penumbras;

        //draw light emission
        lightTexture.clear(sf::Color::Black);
        lightTexture.setView(view);
        lightTexture.draw(mSprite, states);

        //render shapes
        // Mask off light shape (over-masking - mask too much, reveal penumbra/antumbra afterwards)
        for (std::size_t i = 0; i < colliders.size(); ++i)
        {
            LightCollider* lc = colliders[i].first;
            TransformComponent* colliderTransf = colliders[i].second;
			sf::Transform colliderFinalTransf = states.transform;
            colliderFinalTransf *= colliderTransf->getTransform();
            colliderFinalTransf *= lc->getTransform();
            if (lc->isActive())
            {
                // Get boundaries
                innerBoundaryIndices.clear();
                innerBoundaryVectors.clear();
                penumbras.clear();
                getPenumbrasPoint(penumbras, innerBoundaryIndices, innerBoundaryVectors, outerEdges[i].outerBoundaryIndices,
                                  outerEdges[i].outerBoundaryVectors, *lc, *colliderTransf, transf);

                if (innerBoundaryIndices.size() != 2 || outerEdges[i].outerBoundaryIndices.size() != 2)
                {
                    continue;
                }

                sf::Vector2f as = colliderFinalTransf.transformPoint(lc->getPoint(outerEdges[i].outerBoundaryIndices[0]));
                sf::Vector2f bs = colliderFinalTransf.transformPoint(lc->getPoint(outerEdges[i].outerBoundaryIndices[1]));
                sf::Vector2f ad = outerEdges[i].outerBoundaryVectors[0];
                sf::Vector2f bd = outerEdges[i].outerBoundaryVectors[1];

                sf::Vector2f intersectionOuter;

                // Handle antumbras as a seperate case
                if (rayIntersect(as, ad, bs, bd, intersectionOuter))
                {
                    sf::Vector2f asi = colliderFinalTransf.transformPoint(lc->getPoint(innerBoundaryIndices[0]));
                    sf::Vector2f bsi = colliderFinalTransf.transformPoint(lc->getPoint(innerBoundaryIndices[1]));
                    sf::Vector2f adi = innerBoundaryVectors[0];
                    sf::Vector2f bdi = innerBoundaryVectors[1];

                    antumbraTexture.clear(sf::Color::White);
                    antumbraTexture.setView(view);

                    sf::Vector2f intersectionInner;

                    if (rayIntersect(asi, adi, bsi, bdi, intersectionInner))
                    {
                        sf::ConvexShape maskShape;
                        maskShape.setPointCount(3);
                        maskShape.setPoint(0, asi);
                        maskShape.setPoint(1, bsi);
                        maskShape.setPoint(2, intersectionInner);
                        maskShape.setFillColor(sf::Color::Black);
                        antumbraTexture.draw(maskShape);
                    }
                    else
                    {
                        sf::ConvexShape maskShape;
                        maskShape.setPointCount(4);
                        maskShape.setPoint(0, asi);
                        maskShape.setPoint(1, bsi);
                        maskShape.setPoint(2, bsi + normalizeVector(bdi) * shadowExtension);
                        maskShape.setPoint(3, asi + normalizeVector(adi) * shadowExtension);
                        maskShape.setFillColor(sf::Color::Black);
                        antumbraTexture.draw(maskShape);
                    }

                    sf::RenderStates penumbrasStates;
                    penumbrasStates.blendMode = sf::BlendAdd;
                    unmaskWithPenumbras(antumbraTexture, penumbrasStates, unshadowShader, penumbras, shadowExtension);

                    antumbraTexture.display();

                    lightTexture.setView(lightTexture.getDefaultView());
                    lightTexture.draw(sf::Sprite(antumbraTexture.getTexture()), sf::BlendMultiply);
                    lightTexture.setView(view);
                }
                else
                {
                    sf::ConvexShape maskShape;
                    maskShape.setPointCount(4);
                    maskShape.setPoint(0, as);
                    maskShape.setPoint(1, bs);
                    maskShape.setPoint(2, bs + normalizeVector(bd) * shadowExtension);
                    maskShape.setPoint(3, as + normalizeVector(ad) * shadowExtension);
                    maskShape.setFillColor(sf::Color::Black);
                    lightTexture.draw(maskShape);

                    sf::RenderStates penumbrasStates;
                    penumbrasStates.blendMode = sf::BlendMultiply;
                    unmaskWithPenumbras(lightTexture, penumbrasStates, unshadowShader, penumbras, shadowExtension);
                }
            }
        }

        for (std::size_t i = 0; i < colliders.size(); ++i)
        {
            LightCollider* collider = colliders[i].first;
            TransformComponent* colliderTransf = colliders[i].second;
            sf::RenderStates colliderStates;
            colliderStates.transform = colliderTransf->getTransform();
            if (collider->getLightOverShape())
            {
                colliderStates.shader = &lightOverShapeShader;
                collider->setColor(sf::Color::White);
                collider->render(lightTexture, colliderStates);
            }
            else
            {
                collider->setColor(sf::Color::Black);
                collider->render(lightTexture, colliderStates);
            }
        }

        lightTexture.display();
    }

    void PointLight::loadTexture(const std::string& path)
    {
        mTexture.load(path);
        if (mTexture.isLoaded())
        {
            mTexture.get()->setSmooth(true);
            mSprite.setTexture(*mTexture.get(), true);
        }
        mSprite.setOrigin({ mSprite.getTextureRect().width*0.5f, mSprite.getTextureRect().height*0.5f });
    }

    sf::Color PointLight::getColor() const
    {
        return mSprite.getColor();
    }

    void PointLight::setColor(const sf::Color& color)
    {
        mSprite.setColor(color);
    }

    sf::Vector2f PointLight::getScale() const
    {
        return mSprite.getScale();
    }

    sf::Vector2f PointLight::getPosition() const
    {
        return mSprite.getPosition();
    }

    void PointLight::setSourcePoint(const sf::Vector2f& source)
    {
        mSourcePoint = source;
    }

    sf::Vector2f PointLight::getSourcePoint() const
    {
        return mSourcePoint;
    }

    sf::Vector2f PointLight::getCastCenter() const
    {
        sf::Transform t = mSprite.getTransform();
        t.translate(mSprite.getOrigin());
        return t.transformPoint(mSourcePoint);
    }

    const Image& PointLight::getImage() const
    {
        return mTexture;
    }

    float PointLight::getRadius() const
    {
        return mRadius;
    }

    void PointLight::setRadius(float radius)
    {
        mRadius = radius;
    }

    float PointLight::getShadowExtendMultiplier() const
    {
        return mShadowOverExtendMultiplier;
    }

    void PointLight::setShadowExtendMultiplier(float multiplier)
    {
        mShadowOverExtendMultiplier = multiplier;
    }

    void PointLight::getPenumbrasPoint(std::vector<Penumbra>& penumbras,
                                       std::vector<int>& innerBoundaryIndices,
                                       std::vector<sf::Vector2f>& innerBoundaryVectors,
                                       std::vector<int>& outerBoundaryIndices,
                                       std::vector<sf::Vector2f>& outerBoundaryVectors,
                                       const LightCollider& collider,
                                       const TransformComponent& colliderTransform,
                                       const TransformComponent& lightTransform) const
   {
        sf::Vector2f sourceCenter = lightTransform.getTransform().transformPoint(getCastCenter());
        std::size_t numPoints = collider.getPointCount();
        if (numPoints == 0) return;
        sf::Transform colliderLocalTranform = collider.getTransform();
        colliderLocalTranform *= colliderTransform.getTransform();

        std::vector<bool> bothEdgesBoundaryWindings;
        bothEdgesBoundaryWindings.reserve(2);

        std::vector<bool> oneEdgeBoundaryWindings;
        oneEdgeBoundaryWindings.reserve(2);

        // Calculate front and back facing sides
        std::vector<bool> facingFrontBothEdges;
        facingFrontBothEdges.reserve(numPoints);

        std::vector<bool> facingFrontOneEdge;
        facingFrontOneEdge.reserve(numPoints);

        for (std::size_t i = 0; i < numPoints; ++i)
        {
            sf::Vector2f point = colliderLocalTranform.transformPoint(collider.getPoint(i));
            sf::Vector2f nextPoint = colliderLocalTranform.transformPoint(collider.getPoint((i < numPoints - 1) ? i + 1 : 0));

            sf::Vector2f firstEdgeRay;
            sf::Vector2f secondEdgeRay;
            sf::Vector2f firstNextEdgeRay;
            sf::Vector2f secondNextEdgeRay;

            {
                sf::Vector2f sourceToPoint = point - sourceCenter;
                sf::Vector2f perpendicularOffset = { -sourceToPoint.y, sourceToPoint.x };
                normalize(perpendicularOffset);
                perpendicularOffset *= mRadius;
                firstEdgeRay = point - (sourceCenter - perpendicularOffset);
                secondEdgeRay = point - (sourceCenter + perpendicularOffset);
            }
            {
                sf::Vector2f sourceToPoint = nextPoint - sourceCenter;
                sf::Vector2f perpendicularOffset = { -sourceToPoint.y, sourceToPoint.x };
                normalize(perpendicularOffset);
                perpendicularOffset *= mRadius;
                firstNextEdgeRay = nextPoint - (sourceCenter - perpendicularOffset);
                secondNextEdgeRay = nextPoint - (sourceCenter + perpendicularOffset);
            }

            sf::Vector2f pointToNextPoint = nextPoint - point;
            sf::Vector2f normal = {-pointToNextPoint.y, pointToNextPoint.x};
            normalize(normal);

            float firstEdgeDot = dotProduct(firstEdgeRay, normal);
            float secondEdgeDot = dotProduct(secondEdgeRay, normal);
            float firstNextEdgeDot = dotProduct(firstNextEdgeRay, normal);
            float secondNextEdgeDot = dotProduct(secondNextEdgeRay, normal);

            // Front facing, mark it
            facingFrontBothEdges.push_back((firstEdgeDot > 0.0f && secondEdgeDot > 0.0f) || (firstNextEdgeDot > 0.0f && secondNextEdgeDot > 0.0f));
            facingFrontOneEdge.push_back(firstEdgeDot > 0.0f || secondEdgeDot > 0.0f || firstNextEdgeDot > 0.0f || secondNextEdgeDot > 0.0f);
        }

        // Go through front/back facing list. Where the facing direction switches, there is a boundary
        for (std::size_t i = 1; i < numPoints; ++i)
        {
            if (facingFrontBothEdges[i] != facingFrontBothEdges[i - 1])
            {
                innerBoundaryIndices.push_back(i);
                bothEdgesBoundaryWindings.push_back(facingFrontBothEdges[i]);
            }
        }

        // Check looping indices separately
        if (facingFrontBothEdges[0] != facingFrontBothEdges[numPoints - 1])
        {
            innerBoundaryIndices.push_back(0);
            bothEdgesBoundaryWindings.push_back(facingFrontBothEdges[0]);
        }

        // Go through front/back facing list. Where the facing direction switches, there is a boundary
        for (std::size_t i = 1; i < numPoints; ++i)
        {
            if (facingFrontOneEdge[i] != facingFrontOneEdge[i - 1])
            {
                outerBoundaryIndices.push_back(i);
                oneEdgeBoundaryWindings.push_back(facingFrontOneEdge[i]);
            }
        }

        // Check looping indices separately
        if (facingFrontOneEdge[0] != facingFrontOneEdge[numPoints - 1])
        {
            outerBoundaryIndices.push_back(0);
            oneEdgeBoundaryWindings.push_back(facingFrontOneEdge[0]);
        }

        // Compute outer boundary vectors
        for (std::size_t bi = 0; bi < outerBoundaryIndices.size(); ++bi)
        {
            int penumbraIndex = outerBoundaryIndices[bi];
            bool winding = oneEdgeBoundaryWindings[bi];

            sf::Vector2f point = colliderLocalTranform.transformPoint(collider.getPoint(penumbraIndex));
            sf::Vector2f sourceToPoint = point - sourceCenter;
            sf::Vector2f perpendicularOffset = {-sourceToPoint.y, sourceToPoint.x};
            normalize(perpendicularOffset);
            perpendicularOffset *= mRadius;

            // Add boundary vector
            outerBoundaryVectors.push_back(winding ? point - (sourceCenter + perpendicularOffset) : point - (sourceCenter - perpendicularOffset));
        }

        for (unsigned bi = 0; bi < innerBoundaryIndices.size(); bi++)
        {
            int penumbraIndex = innerBoundaryIndices[bi];
            bool winding = bothEdgesBoundaryWindings[bi];

            sf::Vector2f point = colliderLocalTranform.transformPoint(collider.getPoint(penumbraIndex));
            sf::Vector2f sourceToPoint = point - sourceCenter;
            sf::Vector2f perpendicularOffset = normalizeVector({ -sourceToPoint.y, sourceToPoint.x }) * mRadius;
            sf::Vector2f firstEdgeRay = point - (sourceCenter + perpendicularOffset);
            sf::Vector2f secondEdgeRay = point - (sourceCenter - perpendicularOffset);

            // Add boundary vector
            innerBoundaryVectors.push_back(winding ? secondEdgeRay : firstEdgeRay);
            sf::Vector2f outerBoundaryVector = winding ? firstEdgeRay : secondEdgeRay;

            if (innerBoundaryIndices.size() == 1)
            {
                innerBoundaryVectors.push_back(outerBoundaryVector);
            }

            // Add penumbras
            bool hasPrevPenumbra = false;

            sf::Vector2f prevPenumbraLightEdgeVector;

            float prevBrightness = 1.0f;

            while (penumbraIndex != -1)
            {
                int nextPointIndex = ((std::size_t)penumbraIndex < numPoints - 1) ? penumbraIndex + 1 : 0;
                sf::Vector2f nextPoint = colliderLocalTranform.transformPoint(collider.getPoint(nextPointIndex));
                sf::Vector2f pointToNextPoint = nextPoint - point;

                int prevPointIndex = (penumbraIndex > 0) ? penumbraIndex - 1 : numPoints - 1;
                sf::Vector2f prevPoint = colliderLocalTranform.transformPoint(collider.getPoint(prevPointIndex));
                sf::Vector2f pointToPrevPoint = prevPoint - point;

                Penumbra penumbra;
                penumbra.source = point;

                if (!winding)
                {
                    penumbra.lightEdge = (hasPrevPenumbra) ? prevPenumbraLightEdgeVector : innerBoundaryVectors.back();
                    penumbra.darkEdge = outerBoundaryVector;
                    penumbra.lightBrightness = prevBrightness;

                    sf::Vector2f normalLightEdge = penumbra.lightEdge;
                    sf::Vector2f normalDarkEdge = penumbra.darkEdge;
                    sf::Vector2f normalPointToNextPoint = pointToNextPoint;
                    normalize(normalLightEdge);
                    normalize(normalDarkEdge);
                    normalize(normalPointToNextPoint);

                    // Next point, check for intersection
                    float intersectionAngle = std::acos(dotProduct( normalLightEdge, normalPointToNextPoint ));
                    float penumbraAngle = std::acos(dotProduct( normalLightEdge, normalDarkEdge ));

                    if (intersectionAngle < penumbraAngle)
                    {
                        prevBrightness = penumbra.darkBrightness = intersectionAngle / penumbraAngle;

                        //assert(prevBrightness >= 0.0f && prevBrightness <= 1.0f);

                        penumbra.darkEdge = pointToNextPoint;
                        penumbraIndex = nextPointIndex;

                        if (hasPrevPenumbra)
                        {
                            std::swap(penumbra.darkBrightness, penumbras.back().darkBrightness);
                            std::swap(penumbra.lightBrightness, penumbras.back().lightBrightness);
                        }

                        hasPrevPenumbra = true;
                        prevPenumbraLightEdgeVector = penumbra.darkEdge;
                        point = colliderLocalTranform.transformPoint(collider.getPoint(penumbraIndex));
                        sourceToPoint = point - sourceCenter;
                        perpendicularOffset = { -sourceToPoint.y, sourceToPoint.x };
                        normalize(perpendicularOffset);
                        perpendicularOffset *= mRadius;
                        outerBoundaryVector = point - (sourceCenter - perpendicularOffset);

                        if (!outerBoundaryVectors.empty())
                        {
                            outerBoundaryVectors[0] = penumbra.darkEdge;
                            outerBoundaryIndices[0] = penumbraIndex;
                        }
                    }
                    else
                    {
                        penumbra.darkBrightness = 0.0f;

                        if (hasPrevPenumbra)
                        {
                            std::swap(penumbra.darkBrightness, penumbras.back().darkBrightness);
                            std::swap(penumbra.lightBrightness, penumbras.back().lightBrightness);
                        }

                        hasPrevPenumbra = false;

                        if (!outerBoundaryVectors.empty())
                        {
                            outerBoundaryVectors[0] = penumbra.darkEdge;
                            outerBoundaryIndices[0] = penumbraIndex;
                        }

                        penumbraIndex = -1;
                    }
                }
                else // Winding = true
                {
                    penumbra.lightEdge = (hasPrevPenumbra) ? prevPenumbraLightEdgeVector : innerBoundaryVectors.back();
                    penumbra.darkEdge = outerBoundaryVector;
                    penumbra.lightBrightness = prevBrightness;

                    sf::Vector2f normalLightEdge = penumbra.lightEdge;
                    sf::Vector2f normalDarkEdge = penumbra.darkEdge;
                    sf::Vector2f normalPointToPrevPoint = pointToPrevPoint;
                    normalize(normalLightEdge);
                    normalize(normalDarkEdge);
                    normalize(normalPointToPrevPoint);

                    // Next point, check for intersection
                    float intersectionAngle = std::acos(dotProduct( normalLightEdge, normalPointToPrevPoint ));
                    float penumbraAngle = std::acos(dotProduct( normalLightEdge, normalDarkEdge ));

                    if (intersectionAngle < penumbraAngle)
                    {
                        prevBrightness = penumbra.darkBrightness = intersectionAngle / penumbraAngle;

                        //assert(prevBrightness >= 0.0f && prevBrightness <= 1.0f);

                        penumbra.darkEdge = pointToPrevPoint;
                        penumbraIndex = prevPointIndex;

                        if (hasPrevPenumbra)
                        {
                            std::swap(penumbra.darkBrightness, penumbras.back().darkBrightness);
                            std::swap(penumbra.lightBrightness, penumbras.back().lightBrightness);
                        }

                        hasPrevPenumbra = true;
                        prevPenumbraLightEdgeVector = penumbra.darkEdge;
                        point = colliderLocalTranform.transformPoint(collider.getPoint(penumbraIndex));
                        sourceToPoint = point - sourceCenter;
                        perpendicularOffset = { -sourceToPoint.y, sourceToPoint.x };
                        normalize(perpendicularOffset);
                        perpendicularOffset *= mRadius;
                        outerBoundaryVector = point - (sourceCenter + perpendicularOffset);

                        if (!outerBoundaryVectors.empty())
                        {
                            outerBoundaryVectors[1] = penumbra.darkEdge;
                            outerBoundaryIndices[1] = penumbraIndex;
                        }
                    }
                    else
                    {
                        penumbra.darkBrightness = 0.0f;

                        if (hasPrevPenumbra)
                        {
                            std::swap(penumbra.darkBrightness, penumbras.back().darkBrightness);
                            std::swap(penumbra.lightBrightness, penumbras.back().lightBrightness);
                        }

                        hasPrevPenumbra = false;

                        if (!outerBoundaryVectors.empty())
                        {
                            outerBoundaryVectors[1] = penumbra.darkEdge;
                            outerBoundaryIndices[1] = penumbraIndex;
                        }

                        penumbraIndex = -1;
                    }
                }

                penumbras.push_back(penumbra);
            }
        }
   }


    LightAffectorComponent::LightAffectorComponent() : mCallback(nullptr), mEmitter(nullptr), mActive(true) {}

    void LightAffectorComponent::setActive(bool active)
    {
        mActive = active;
    }

    bool LightAffectorComponent::isActive() const
    {
        return mActive;
    }



    LightFlickering::LightFlickering(float period, float strength) : mDirection(false), mPeriod(period), mStrength(strength) {}


    void LightFlickering::operator() (float delta, LightEmitterComponent& light)
    {
        float scale = mStrength * delta / mPeriod;
        if (mDirection)
        {
            light.mLight.mSprite.setScale( light.mLight.mSprite.getScale().x + scale,
                                           light.mLight.mSprite.getScale().y + scale );
        }
        else
        {
            light.mLight.mSprite.setScale( light.mLight.mSprite.getScale().x - scale,
                                           light.mLight.mSprite.getScale().y - scale );
        }
        if (mTimer.getElapsedTime().asMilliseconds() > mPeriod)
        {
            mTimer.restart();
            mDirection = !mDirection;
        }
    }

    RandomizedFlickering::RandomizedFlickering(float basePeriod, float strength) : mDirection(false),
                                                                 mPeriod(NumberGenerator::getFloatRandBetw(0.5f,1.0f)*basePeriod),
                                                                 mBasePeriod(basePeriod),
                                                                 mStrength(strength),
                                                                 mSizeMemorizer(0.0f) {}


    void RandomizedFlickering::operator() (float delta, LightEmitterComponent& light)
    {
        float scale = mStrength * delta / mPeriod;
        if (mDirection)
        {
            light.mLight.mSprite.setScale( light.mLight.mSprite.getScale().x + scale,
                                           light.mLight.mSprite.getScale().y + scale );
            mSizeMemorizer += scale;
            if (mSizeMemorizer >= 0 || mTimer.getElapsedTime().asMilliseconds() > mPeriod)
            {
                mTimer.restart();
                mDirection = false;
                mPeriod = NumberGenerator::getFloatRandBetw(0.5f,1.0f)*mBasePeriod;
            }
        }
        else
        {
            light.mLight.mSprite.setScale( light.mLight.mSprite.getScale().x - scale,
                                           light.mLight.mSprite.getScale().y - scale );
            mSizeMemorizer -= scale;
            if (mSizeMemorizer <= -mStrength*mPeriod || mTimer.getElapsedTime().asMilliseconds() > mPeriod)
            {
                mTimer.restart();
                mDirection = true;
                mPeriod = NumberGenerator::getFloatRandBetw(0.5f,1.0f)*mBasePeriod;
            }
        }
    }


    LightSystem::LightSystem() : mQuadTree(nullptr), mAmbientColor(sf::Color::White), mColorShift(0,0,0) {}

    void LightSystem::init(Application& app, quad::QuadTree<Entity>* quadtree,
              const sf::Vector2u &imageSize,
              const std::string& unshadowVertex,
              const std::string& unshadowFragment,
              const std::string& lightOverShapeVertex,
              const std::string& lightOverShapeFragment,
              const std::string& penumbraTexture)
    {
        mAppSignalLink.disconnect(); //disconnect for safety (if init is called multiple times)
        mAppSignalLink = app.onTargetSizeChanged([this] (const sf::Vector2u& targetsize)
                                  {
                                      setImageSize(targetsize);
                                  });

        mQuadTree = quadtree;

        mUnshadowShader.loadFromFile(unshadowVertex, unshadowFragment);
        mLightOverShapeShader.loadFromFile(lightOverShapeVertex, lightOverShapeFragment);

        setImageSize(imageSize);

        mPenumbraTexture.load(penumbraTexture);

        if (mPenumbraTexture.isLoaded())
        {
            mPenumbraTexture.get()->setSmooth(true);
            mUnshadowShader.setUniform("penumbraTexture", *mPenumbraTexture.get());
        }
        else
        {
            ungod::Logger::warning("No valid penumbra texture loaded!");
            ungod::Logger::endl();
        }
    }

    void LightSystem::setImageSize(const sf::Vector2u &imageSize)
    {
        mLightTexture.create(imageSize.x, imageSize.y);
        mEmissionTexture.create(imageSize.x, imageSize.y);
        mAntumbraTexture.create(imageSize.x, imageSize.y);
        mCompositionTexture.create(imageSize.x, imageSize.y);
        mLightOverShapeShader.setUniform("targetSizeInv", sf::Glsl::Vec2(1.0f / imageSize.x, 1.0f / imageSize.y));
        mLightOverShapeShader.setUniform("emissionTexture", mEmissionTexture.getTexture());
    }

    void LightSystem::render(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states, bool drawShadows)
    {
        mCompositionTexture.clear(mAmbientColor);
        mCompositionTexture.setView(mCompositionTexture.getDefaultView());

        //iterator over the one-light-components
        dom::Utility<Entity>::iterate<TransformComponent, LightEmitterComponent>(pull.getList(),
          [this, &target, &states, drawShadows] (Entity e, TransformComponent& lightTransf, LightEmitterComponent& light)
          {
              renderLight(target, states, e, lightTransf, light, drawShadows);
          });

        //iterate over the multiple-light-components
        dom::Utility<Entity>::iterate<TransformComponent, MultiLightEmitter>(pull.getList(),
          [this, &target, &states, drawShadows] (Entity e, TransformComponent& lightTransf, MultiLightEmitter& light)
          {
              for (std::size_t i = 0; i < light.getComponentCount(); ++i)
              {
                renderLight(target, states, e, lightTransf, light.getComponent(i), drawShadows);
              }
          });

        mCompositionTexture.display();

        sf::RenderStates lightstates{};
		lightstates.blendMode = sf::BlendMultiply;

		mDisplaySprite.setTexture(mCompositionTexture.getTexture(), true);
		sf::View view = target.getView();
		target.setView(target.getDefaultView());
		target.draw(mDisplaySprite, lightstates);
		target.setView(view);
    }


    void LightSystem::renderLight(sf::RenderTarget& target, sf::RenderStates states, Entity e, TransformComponent& lightTransf, LightEmitterComponent& light, bool drawShadows)
    {
        //pull all entities near the light
        quad::PullResult<Entity> shadowsPull;
        sf::FloatRect bounds = light.mLight.getBoundingBox();
        sf::Vector2f transformedUpperBound = lightTransf.getTransform().transformPoint( {bounds.left, bounds.top} );
        mQuadTree->retrieve(shadowsPull, { transformedUpperBound.x, transformedUpperBound.y, bounds.width, bounds.height });

        std::vector< std::pair<LightCollider*, TransformComponent*> > colliders;

        if (drawShadows)
        {
            //find the entities with light-colliders that are on the screen
            dom::Utility<Entity>::iterate<TransformComponent, ShadowEmitterComponent>(shadowsPull.getList(),
            [this, &colliders, &light, &lightTransf] (Entity e, TransformComponent& colliderTransf, ShadowEmitterComponent& shadow)
            {
                sf::FloatRect colliderBounds = colliderTransf.getTransform().transformRect( shadow.mLightCollider.getBoundingBox() );
                sf::FloatRect lightBounds = lightTransf.getTransform().transformRect( light.mLight.getBoundingBox() );
                //test if the collider is "in range" of the light. Do not render penumbras otherwise
                if ( colliderBounds.intersects(lightBounds) )
                    colliders.emplace_back( &shadow.mLightCollider, &colliderTransf );
            });

            dom::Utility<Entity>::iterate<TransformComponent, MultiShadowEmitter>(shadowsPull.getList(),
            [this, &colliders, &light, &lightTransf] (Entity e, TransformComponent& colliderTransf, MultiShadowEmitter& shadow)
            {
                for (std::size_t i = 0; i < shadow.getComponentCount(); ++i)
                {
                    sf::FloatRect colliderBounds = colliderTransf.getTransform().transformRect( shadow.getComponent(i).mLightCollider.getBoundingBox() );
                    sf::FloatRect lightBounds = lightTransf.getTransform().transformRect( light.mLight.getBoundingBox() );
                    //test if the collider is "in range" of the light. Do not render penumbras otherwise
                    if ( colliderBounds.intersects(lightBounds) )
                        colliders.emplace_back( &shadow.getComponent(i).mLightCollider, &colliderTransf );
                }
            });
        }

        //render the light and the colliders, draw umbras, penumbras + antumbras
        light.mLight.render(target.getView(), mLightTexture, mEmissionTexture, mAntumbraTexture, states,
                            colliders, mUnshadowShader, mLightOverShapeShader, lightTransf);

        //draw the resulting texture in the application window
        sf::RenderStates compoRenderStates;
        compoRenderStates.blendMode = sf::BlendAdd;
        mDisplaySprite.setTexture(mLightTexture.getTexture(), true);
        mCompositionTexture.draw(mDisplaySprite, compoRenderStates);
    }

    void LightSystem::update(const std::list<Entity>& entities, float delta)
    {
        //iterate over LightAffectors
        dom::Utility<Entity>::iterate<LightAffectorComponent>(entities,
          [delta, this] (Entity e, LightAffectorComponent& affector)
          {
              if (affector.isActive() && affector.mCallback)
                affector.mCallback( delta, *affector.mEmitter );
          });
        //iterate over MultilightLightAffectors
        dom::Utility<Entity>::iterate<MultiLightAffector>(entities,
          [delta, this] (Entity e, MultiLightAffector& affector)
          {
              for (std::size_t i = 0; i < affector.getComponentCount(); ++i)
              {
                  if (affector.getComponent(i).isActive() && affector.getComponent(i).mCallback)
                    affector.getComponent(i).mCallback( delta, *affector.getComponent(i).mEmitter );
              }
          });
    }

    void LightSystem::setAmbientColor(const sf::Color& color)
    {
        mAmbientColor = color;
    }

    sf::Color LightSystem::getAmbientColor() const
    {
        return mAmbientColor;
    }

    void LightSystem::interpolateAmbientLight(const sf::Color& color, float strength)
    {
        //compute the new color through simple linear interpolation
        mColorShift.x += (float)(color.r - mAmbientColor.r)/strength;
        if(mColorShift.x > 1)
        {
            mColorShift.x --;
            mAmbientColor.r ++;
        }
        else if(mColorShift.x < -1)
        {
            mColorShift.x ++;
            mAmbientColor.r --;
        }
        mColorShift.y += (float)(color.g - mAmbientColor.g)/strength;
        if(mColorShift.y > 1)
        {
            mColorShift.y --;
            mAmbientColor.g ++;
        }
        else if(mColorShift.y < -1)
        {
            mColorShift.y ++;
            mAmbientColor.g --;
        }
        mColorShift.z += (float)(color.b - mAmbientColor.b)/strength;
        if(mColorShift.z > 1)
        {
            mColorShift.z --;
            mAmbientColor.b ++;
        }
        else if(mColorShift.z < -1)
        {
            mColorShift.z ++;
            mAmbientColor.b --;
        }
    }

    void LightSystem::setLocalLightPosition(LightEmitterComponent& le, Entity e, const sf::Vector2f& position)
    {
        le.mLight.mSprite.setPosition(position);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(le.mLight.getBoundingBox()));
    }

    void LightSystem::setLightScale(LightEmitterComponent& le, Entity e, const sf::Vector2f& scale)
    {
        le.mLight.mSprite.setScale(scale);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(le.mLight.getBoundingBox()));
    }

    void LightSystem::setLightOrigin(LightEmitterComponent& le, Entity e, const sf::Vector2f& origin)
    {
        le.mLight.setSourcePoint(origin);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(le.mLight.getBoundingBox()));
    }

     void LightSystem::setLightColor(LightEmitterComponent& le, const sf::Color& color)
    {
        le.mLight.mSprite.setColor(color);
    }

    void LightSystem::setPoint(ShadowEmitterComponent& se, Entity e, const sf::Vector2f& point, std::size_t i)
    {
        se.mLightCollider.setPoint(i, point);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(se.mLightCollider.getBoundingBox()));
    }

    void LightSystem::setPointCount(Entity e, ShadowEmitterComponent& se, std::size_t num)
    {
        se.mLightCollider.setPointCount(num);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(se.mLightCollider.getBoundingBox()));
    }

    void LightSystem::setPoints(Entity e, const std::vector<sf::Vector2f>& points)
    {
        ShadowEmitterComponent& shadow = e.modify<ShadowEmitterComponent>();
        shadow.mLightCollider.setPointCount(points.size());
        for (std::size_t i = 0; i < points.size(); ++i)
            shadow.mLightCollider.setPoint(i, points[i]);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(shadow.mLightCollider.getBoundingBox()));
    }

    void LightSystem::setPoints(Entity e, const std::vector<sf::Vector2f>& points, std::size_t colliderIndex)
    {
        MultiShadowEmitter& multi = e.modify<MultiShadowEmitter>();
        multi.getComponent(colliderIndex).mLightCollider.setPointCount(points.size());
        for (std::size_t i = 0; i < points.size(); ++i)
            multi.getComponent(colliderIndex).mLightCollider.setPoint(i, points[i]);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(multi.getComponent(colliderIndex).mLightCollider.getBoundingBox()));
    }

    void LightSystem::setLightOverShape(ShadowEmitterComponent& se, bool lightOverShape)
    {
        se.mLightCollider.setLightOverShape(lightOverShape);
    }

    void LightSystem::setLightRadius(LightEmitterComponent& le, float radius)
    {
        le.mLight.setRadius(radius);
    }

    void LightSystem::setShadowExtendMultiplier(LightEmitterComponent& le, float multiplier)
    {
        le.mLight.setShadowExtendMultiplier(multiplier);
    }

    void LightSystem::setAffectorCallback(Entity e, const std::function<void(float, LightEmitterComponent&)>& callback)
    {
        setAffectorCallback(callback, e.modify<LightAffectorComponent>(), e.modify<LightEmitterComponent>());
    }

    void LightSystem::setAffectorCallback(Entity e, std::size_t lightIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
    {
        setAffectorCallback(callback, e.modify<LightAffectorComponent>(), e.modify<MultiLightEmitter>().getComponent(lightIndex));
    }

    void LightSystem::setAffectorCallback(Entity e, std::size_t lightIndex, std::size_t affectorIndex, const std::function<void(float, LightEmitterComponent&)>& callback)
    {
        setAffectorCallback(callback, e.modify<MultiLightAffector>().getComponent(affectorIndex), e.modify<MultiLightEmitter>().getComponent(lightIndex));
    }

    void LightSystem::setAffectorCallback(const std::function<void(float, LightEmitterComponent&)>& callback, LightAffectorComponent& affector, LightEmitterComponent& emitter)
    {
        affector.mCallback = callback;
        affector.mEmitter = &emitter;
    }

    void LightSystem::loadLightTexture(LightEmitterComponent& le, const std::string& textureID)
    {
        le.mLight.loadTexture(textureID);
    }

    void LightSystem::setLightActive(LightEmitterComponent& le, bool active)
    {
        le.mLight.setActive(active);
    }

    void LightSystem::onContentsChanged(const std::function<void(Entity, const sf::IntRect&)>& callback)
    {
        mContentsChangedSignal.connect(callback);
    }

    sf::Vector2f LightSystem::getLowerBound(Entity e)
    {
        sf::Vector2f lowerBounds(0,0);
        if (e.has<LightEmitterComponent>())
        {
            const LightEmitterComponent& emitter = e.get<LightEmitterComponent>();
            sf::FloatRect bbox = emitter.getLight().getBoundingBox();
            lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
            lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
        }
        if (e.has<MultiLightEmitter>())
        {
            const MultiLightEmitter& emitter = e.get<MultiLightEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).getLight().getBoundingBox();
                lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
                lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
            }
        }
        if (e.has<ShadowEmitterComponent>())
        {
            const ShadowEmitterComponent& emitter = e.get<ShadowEmitterComponent>();
            sf::FloatRect bbox = emitter.mLightCollider.getBoundingBox();
            lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
            lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
        }
        if (e.has<MultiShadowEmitter>())
        {
            const MultiShadowEmitter& emitter = e.get<MultiShadowEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).mLightCollider.getBoundingBox();
                lowerBounds.x = std::max(bbox.left + bbox.width, lowerBounds.x);
                lowerBounds.y = std::max(bbox.top + bbox.height, lowerBounds.y);
            }
        }
        return lowerBounds;
    }

    sf::Vector2f LightSystem::getUpperBound(Entity e)
    {
        sf::Vector2f upperBound(0,0);
        if (e.has<LightEmitterComponent>())
        {
            const LightEmitterComponent& emitter = e.get<LightEmitterComponent>();
            sf::FloatRect bbox = emitter.getLight().getBoundingBox();
            upperBound.x = std::min(bbox.left, upperBound.x);
            upperBound.y = std::min(bbox.top, upperBound.y);
        }
        if (e.has<MultiLightEmitter>())
        {
            const MultiLightEmitter& emitter = e.get<MultiLightEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).getLight().getBoundingBox();
                upperBound.x = std::min(bbox.left, upperBound.x);
                upperBound.y = std::min(bbox.top, upperBound.y);
            }
        }
        if (e.has<ShadowEmitterComponent>())
        {
            const ShadowEmitterComponent& emitter = e.get<ShadowEmitterComponent>();
            sf::FloatRect bbox = emitter.mLightCollider.getBoundingBox();
            upperBound.x = std::min(bbox.left, upperBound.x);
            upperBound.y = std::min(bbox.top, upperBound.y);
        }
        if (e.has<MultiShadowEmitter>())
        {
            const MultiShadowEmitter& emitter = e.get<MultiShadowEmitter>();
            for (unsigned i = 0; i <emitter.getComponentCount(); ++i)
            {
                sf::FloatRect bbox = emitter.getComponent(i).mLightCollider.getBoundingBox();
                upperBound.x = std::min(bbox.left, upperBound.x);
                upperBound.y = std::min(bbox.top, upperBound.y);
            }
        }
        return upperBound;
    }

    void LightSystem::moveLights(Entity e, const sf::Vector2f& vec)
    {
        if (e.has<LightEmitterComponent>())
        {
            e.modify<LightEmitterComponent>().mLight.mSprite.move(vec);
        }
        if (e.has<MultiLightEmitter>())
        {
            MultiLightEmitter& light = e.modify<MultiLightEmitter>();
            for (std::size_t i = 0; i < light.getComponentCount(); ++i)
            {
                light.getComponent(i).mLight.mSprite.move(vec);
            }
        }
    }

    void LightSystem::moveLightColliders(Entity e, const sf::Vector2f& vec)
    {
        if (e.has<ShadowEmitterComponent>())
        {
            e.modify<ShadowEmitterComponent>().mLightCollider.mShape.move(vec);
        }
        if (e.has<MultiShadowEmitter>())
        {
            MultiShadowEmitter& shadow = e.modify<MultiShadowEmitter>();
            for (std::size_t i = 0; i < shadow.getComponentCount(); ++i)
            {
                shadow.getComponent(i).mLightCollider.mShape.move(vec);
            }
        }
    }

    LightSystem::~LightSystem()
    {
        mAppSignalLink.disconnect();
    }
}
