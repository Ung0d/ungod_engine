#ifndef UEDIT_PARTICLE_EDIT_STATE_H
#define UEDIT_PARTICLE_EDIT_STATE_H

#include "ungod/base/Entity.h"
#include "EntityEditState.h"

namespace uedit
{
    /** \brief A base class for particle editors. */
    class ParticleEditor
    {
    public:
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) {}
        virtual void mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) {}
        virtual void mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) {}
        virtual void mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) {}
    } ;



    /** \brief A base class that for entity-preview behavior (state pattern impl) */
    class ParticleEditState : public EditState
    {
    public:
        ParticleEditState(EntityPreview&, ungod::Entity e);

        virtual void handleInput(EntityPreview& preview, const sf::Event& event);
        virtual void update(EntityPreview& preview, float delta);
        virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states);

        //refreshes the internal editors, if distributions, affectors, emitters ect have changed
        void refresh(EntityPreview& preview, ungod::Entity e);

    private:
        std::unique_ptr<ParticleEditor> mPositionDistEditor;
        bool mMouseDown;
    };


    namespace detail
    {
        /// Position distributions
        class FixedPositionEditor : public ParticleEditor
        {
        public:
            FixedPositionEditor() : mBound(false) {}

            virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;
            virtual void mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
        private:
            bool mBound;
        };


        class EllipseDistEditor : public ParticleEditor
        {
        public:
            EllipseDistEditor() : mBoundCenter(false), mBoundRadX(false), mBoundRadY(false) {}

            virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;
            virtual void mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
        private:
            bool mBoundCenter;
            bool mBoundRadX;
            bool mBoundRadY;
        };


        class RectangleDistEditor : public ParticleEditor
        {
        public:
            RectangleDistEditor() : mBoundTopleft(false), mBoundSizes(false) {}

            virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;
            virtual void mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
        private:
            bool mBoundTopleft;
            bool mBoundSizes;
        };


        class LineSegmentDistEditor : public ParticleEditor
        {
        public:
            LineSegmentDistEditor() : mBound1(false), mBound2(false) {}

            virtual void render(EntityPreview& preview, sf::RenderWindow& window, sf::RenderStates states) override;
            virtual void mouseClick(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseMove(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
            virtual void mouseUp(EntityPreview& preview, const sf::Vector2f& mouseRelativeToEntity) override;
        private:
            bool mBound1;
            bool mBound2;
        };
    }
}

#endif // UEDIT_PARTICLE_EDIT_STATE_H
