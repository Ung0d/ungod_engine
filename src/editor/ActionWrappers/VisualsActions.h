#ifndef UEDIT_VISUALS_ACTIONS_H
#define UEDIT_VISUALS_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/base/Visuals.h"
#include <unordered_map>

namespace uedit
{
    class ActionManager;

    class VisualsActions
    {
    public:
        VisualsActions(ActionManager& am) : mActionManager(am) {}

        void setEntityHideForCamera(ungod::Entity e, bool hideForCamera);
        void loadTexture(ungod::Entity e, const std::string& file);
        void loadMetadata(ungod::Entity e, const std::string& file);
        void setSpriteTextureRect(ungod::Entity e, const std::string& key);
        void setMultiSpriteTextureRect(ungod::Entity e, const std::string& key, std::size_t i);
        void setSpritePosition(ungod::Entity e, const sf::Vector2f& pos);
        void setMultiSpritePosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i);
        void setVertexarrayPosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i);
        void setSpriteScale(ungod::Entity e, const sf::Vector2f& scale);
        void setSpriteRotation(ungod::Entity e, float rota);
        void setMultiSpriteScale(ungod::Entity e, const sf::Vector2f& scale, std::size_t i);
        void setMultiSpriteRotation(ungod::Entity e, float rota, std::size_t i);
        void setVertexRectCount(ungod::Entity e, std::size_t numrect);
        void setVertexArrayRect(ungod::Entity e, std::size_t i, const std::string& key);
        void setVertexArrayRectColor(ungod::Entity e, std::size_t i, const sf::Color& color);
        void flipVertexX(ungod::Entity e, unsigned i);
        void flipVertexY(ungod::Entity e, unsigned i);

    private:
        ActionManager& mActionManager;
    }
}

#endif