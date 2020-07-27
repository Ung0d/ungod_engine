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

#ifndef UNGOD_CAMERA_H
#define UNGOD_CAMERA_H

#include "ungod/serialization/Serializable.h"
#include "ungod/base/Entity.h"
#include "owls/Signal.h"
#include "ungod/serialization/SerialCamera.h"

namespace ungod
{
    class CameraAffector;
    class RenderLayer;


    /** \brief A camera defines the area of a world that is visible on the screen. It can be locked to an entity with
    * transform component in order to follow it. */
    class Camera : public Serializable<Camera>
    {
    friend struct SerialBehavior<Camera>;
    public:
        /** \brief Default constructs a camera for later initialization. Note that using an unitilialized camera
        * is undefined bahavior. */
        Camera();

        /** \brief Constructs an initialized camera. */
        Camera(sf::RenderTarget& target);

        /** \brief Initializes a camera for two step construction. */
        void init(sf::RenderTarget& target);

        /** \brief Handles window resizing. */
        void handleEvent(const sf::Event& event);

        /** \brief Updates the internals of the camera and makes entity following ect possible. */
        void update(float delta);

        /** \brief Must be invoked when a rendering process, that shall make use of the camera, begins. */
        void renderBegin(float depth = 1);

        /** \brief Must be invoked when a rendering process, that shall make use of the camera, ends. */
        void renderEnd();

        /** \brief Returns the current center point of the camera. */
        const sf::Vector2f& getCenter() const { return mView.getCenter(); }

        /** \brief Locks the camera to the given entity. It must have a transform component. */
        void lockToEntity(Entity e);

        /** \brief The camera centers the given position. */
        void lookAt(const sf::Vector2f& position);

        /** \brief Sets the camera zoom. Default is 1.0f. */
        void setZoom(float zoom);

        /** \brief Gets the camera zoom. */
        float getZoom();

        /** \brief Sets the current view of the camera. */
        void setViewSize(const sf::Vector2f& viewsize);

        /** \brief Resets the cameras settings to the default. */
        void reset();

        /** \brief Sets the radius of the dead zone around the locked entity.
        * Indicates how fast the camera scrolles when following a locked entity. */
        void setDeadZoneRadius(float radius);

        /** \brief Sets the scroll speed of the camera. */
        void setScrollSpeed(float speed);

        /** \brief Registers a cameraffector. */
        void attachAffector(std::unique_ptr<CameraAffector>& affector);

        /** \brief Sets a noise value that is applied to the camera center. */
        void setNoise(const sf::Vector2f& noise);

        /** \brief Gets the current noise value. */
        const sf::Vector2f& getNoise();

        /** \brief Returns the current view of the camera. optionally calculating in the given render depth. */
        const sf::View& getView() const;
        sf::View getView(float depth) const;

        /** \brief Registers new callback for the view-size-changed-signal. */
        void onViewSizeChanged(const std::function<void(const sf::Vector2f&)>& callback);

        /** \brief Registers new callback for the view-position-changed-signal. */
        void onViewCenterChanged(const std::function<void(const sf::Vector2f&)>& callback);

        /** \brief Can be called if the relative point for the camera changes, for instance if a new world node becomes active. */
        void relativeTo(const sf::Vector2f& point);

    public:
        static std::unique_ptr<CameraAffector> makeScreenShake(float duration, float frequency, float amplitude);
        static std::unique_ptr<CameraAffector> makeSmoothZoom(float duration, float targetzoom);

    public:
        static constexpr float DEFAULT_DEAD_ZONE_RADIUS = 200.0f;
        static constexpr float DEFAULT_SCROLL_SPEED = 0.25f;
        static constexpr float SCROLL_LOCK = 10.0f;

    private:
        sf::RenderTarget* mTarget;
        sf::View mView;
        sf::View mStor;
        sf::View mDefault;
        Entity mLocked;
        float mCurZoom;
        owls::Signal<const sf::Vector2f&> mViewSizeChanged;
        owls::Signal<const sf::Vector2f&> mViewCenterChanged;
        float mDeadZoneRadius;  //< indicate how far from the view center the locked entity can move without manipulating the camera
        float mScrollSpeed; //< indicates how fast the camera scrolles when following a locked entity
        bool mMoving;
        sf::Vector2f mNoise;
        std::list<std::unique_ptr<CameraAffector>> mAffectors;  //< affector objects that manipulate the camera for a certain amount of time
        sf::Vector2f mRelative;
    };

    /** \brief An affector that manipulates the camera for a certain amount of time. */
    class CameraAffector
    {
    friend class Camera;
    protected:
        CameraAffector(float duration) : mDuration(duration), mTimer(0.0f) {}

        const float mDuration;
        float mTimer;

        virtual void init(Camera& camera) {}
        virtual void update(Camera& camera, float delta) {}
        virtual void stop(Camera& camera) {}
    };

    /** \brief Affector that realizes screen shaking. */
    class ScreenShake : public CameraAffector
    {
    friend class Camera;
    private:
        ScreenShake(float duration, float frequency, float amplitude) :
            CameraAffector(duration), mFrequency(frequency), mAmplitude(amplitude),
			mSampleCount(0) {}

        virtual void init(Camera& camera) override;
        virtual void update(Camera& camera, float delta) override;
        virtual void stop(Camera& camera) override;

        float mFrequency;
        float mAmplitude;
        std::size_t mSampleCount;
        std::vector<float> mSamplesX;
        std::vector<float> mSamplesY;
        sf::Vector2f mNoise;
    };

    /** \brief Affector that realizes smooth zooming. */
    class SmoothZoom : public CameraAffector
    {
    friend class Camera;
    private:
        SmoothZoom(float duration, float targetzoom) :
            CameraAffector(duration), mOriginalZoom(1.0f), mTargetZoom(targetzoom) {}

        virtual void init(Camera& camera) override;
        virtual void update(Camera& camera, float delta) override;
        virtual void stop(Camera& camera) override;

        float mOriginalZoom;
        float mTargetZoom;
    };
}

#endif // UNGOD_CAMERA_H
