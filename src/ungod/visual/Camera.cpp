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

#include "ungod/visual/Camera.h"
#include "ungod/base/Transform.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/Utility.h"
#include "ungod/base/World.h"
#include "ungod/visual/RenderLayer.h"

namespace ungod
{
    constexpr float Camera::DEFAULT_DEAD_ZONE_RADIUS;
    constexpr float Camera::DEFAULT_SCROLL_SPEED;
    constexpr float Camera::SCROLL_LOCK;

    Camera::Camera() : mTarget(nullptr), mCurZoom(1.0f), mDeadZoneRadius(DEFAULT_DEAD_ZONE_RADIUS), mScrollSpeed(DEFAULT_SCROLL_SPEED), mMoving(false)
    {
    }

    Camera::Camera(sf::RenderTarget& target) : mTarget(nullptr), mCurZoom(1.0f), mDeadZoneRadius(DEFAULT_DEAD_ZONE_RADIUS), mScrollSpeed(DEFAULT_SCROLL_SPEED), mMoving(false)
    {
        init(target);
    }

    void Camera::init(sf::RenderTarget& target)
    {
        mTarget = &target;
        mDefault = target.getView();
        reset();
    }

    void Camera::handleEvent(const sf::Event& event)
    {
        if (event.type == sf::Event::Resized)
        {
            setViewSize( sf::Vector2f{(float)event.size.width, (float)event.size.height} );
        }
    }

    void Camera::update(float delta)
    {
        //handle affectors
        auto it = std::begin(mAffectors);
        while (it != std::end(mAffectors))
        {
            (*it)->mTimer += delta;
            if ((*it)->mTimer > (*it)->mDuration)
            {
                (*it)->stop(*this);
                mAffectors.erase(it++);
            }
            else
            {
                (*it)->update(*this, delta);
                ++it;
            }
        }

        if (!mLocked)
            return;  //nothing to do anymore

        sf::Vector2f lockedCenter = mLocked.getWorld().getContainer()->mapToGlobalPosition(mLocked.get<TransformComponent>().getCenterPosition());

        float dist = distance(lockedCenter, mView.getCenter());

        if (!mMoving && dist > mDeadZoneRadius)
            mMoving = true;

        if (mMoving)
        {
            //move camera towards the player, move slightly faster the greater the distance
            float multiplier = delta * mScrollSpeed / mDeadZoneRadius;
            lookAt( mView.getCenter() + multiplier*(lockedCenter - mView.getCenter()) );
            if (dist < SCROLL_LOCK)
                mMoving = false;
        }
    }

    void Camera::renderBegin(const RenderLayer* layer)
    {
        mStor = mTarget->getView();
        sf::View finView;
        if (layer)
            finView = getView(layer);
        else
            finView = getView();
        finView.move(mNoise);
        finView.setCenter( std::round(finView.getCenter().x), std::round(finView.getCenter().y) );
        finView.setSize( std::round(finView.getSize().x), std::round(finView.getSize().y) );
        mTarget->setView(finView);
    }

    void Camera::renderEnd()
    {
        mTarget->setView(mStor);
    }

    void Camera::lockToEntity(Entity e)
    {
        mLocked = e;
    }

    void Camera::lookAt(const sf::Vector2f& position)
    {
        mView.setCenter(position);
        mViewCenterChanged(mView.getCenter());
    }

    void Camera::setZoom(float zoom)
    {
        mCurZoom = zoom;
        mView.setSize(zoom * mDefault.getSize());
        mViewSizeChanged(mView.getSize());
    }

    float Camera::getZoom()
    {
        return mCurZoom;
    }

    void Camera::setViewSize(const sf::Vector2f& viewsize)
    {
        mDefault = sf::View({0, 0, viewsize.x, viewsize.y});
        sf::Vector2f center = mView.getCenter();
        mView = mDefault;
        lookAt(center);
        setZoom(mCurZoom);
    }

    void Camera::reset()
    {
        mView = mDefault;
        mViewCenterChanged(mView.getCenter());
        mViewSizeChanged(mView.getSize());
    }

    void Camera::setDeadZoneRadius(float radius)
    {
        mDeadZoneRadius = radius;
    }

    void Camera::setScrollSpeed(float speed)
    {
        mScrollSpeed = speed;
    }

    void Camera::attachAffector(std::unique_ptr<CameraAffector>& affector)
    {
        affector->init(*this);
        mAffectors.emplace_back(std::move(affector));
    }

    void Camera::setNoise(const sf::Vector2f& noise)
    {
        mNoise = noise;
    }

    const sf::Vector2f& Camera::getNoise()
    {
        return mNoise;
    }

    const sf::View& Camera::getView() const
    {
        return mView;
    }

    sf::View Camera::getView(const RenderLayer* layer) const
    {
        sf::View finView = mView;
        sf::Vector2f scaledDiff = layer->getRenderDepth() * (mView.getCenter() - layer->getContainer()->getPosition());
        finView.setCenter(layer->getContainer()->getPosition() + scaledDiff);
        return finView;
    }

    void Camera::onViewSizeChanged(const std::function<void(const sf::Vector2f&)>& callback)
    {
        mViewSizeChanged.connect(callback);
    }

    void Camera::onViewCenterChanged(const std::function<void(const sf::Vector2f&)>& callback)
    {
        mViewCenterChanged.connect(callback);
    }

    std::unique_ptr<CameraAffector> Camera::makeScreenShake(float duration, float frequency, float amplitude)
    {
        return std::unique_ptr<CameraAffector>(new ScreenShake(duration, frequency, amplitude));
    }

    std::unique_ptr<CameraAffector> Camera::makeSmoothZoom(float duration, float targetzoom)
    {
        return std::unique_ptr<CameraAffector>(new SmoothZoom(duration, targetzoom));
    }


    void ScreenShake::init(Camera& camera)
    {
        mNoise = camera.getNoise();

        mSampleCount = (std::size_t)std::ceil((mDuration/1000) * mFrequency);
        mSamplesX.reserve(mSampleCount);
        mSamplesY.reserve(mSampleCount);
        for (unsigned i = 0; i < mSampleCount; ++i)
        {
            mSamplesX.emplace_back( NumberGenerator::getFloatRandBetw(-1,1) );
            mSamplesY.emplace_back( NumberGenerator::getFloatRandBetw(-1,1) );
        }
    }

    void ScreenShake::update(Camera& camera, float delta)
    {
        float s = (mTimer/1000.0f)*mFrequency;
        std::size_t s0 = (std::size_t)std::floor(s);
        std::size_t s1 = s0 +1;

        float ampXFinal = mAmplitude * (mSamplesX[s0] + (s - s0)*(mSamplesX[s1]- mSamplesX[s0])) * (mDuration - mTimer) / mDuration;
        float ampYFinal = mAmplitude * (mSamplesY[s0] + (s - s0)*(mSamplesY[s1]- mSamplesY[s0])) * (mDuration - mTimer) / mDuration;

        camera.setNoise( { ampXFinal, ampYFinal } );
    }

    void ScreenShake::stop(Camera& camera)
    {
        camera.setNoise(mNoise);
    }



    void SmoothZoom::init(Camera& camera)
    {
        mOriginalZoom = camera.getZoom();
    }

    void SmoothZoom::update(Camera& camera, float delta)
    {
        camera.setZoom( mOriginalZoom + (mTimer / mDuration) * (mTargetZoom - mOriginalZoom) );
    }

    void SmoothZoom::stop(Camera& camera)
    {
        camera.setZoom(mTargetZoom);
    }
}
