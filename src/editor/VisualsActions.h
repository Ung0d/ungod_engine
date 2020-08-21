#ifndef UEDIT_VISUALS_ACTIONS_H
#define UEDIT_VISUALS_ACTIONS_H

#include "ungod/base/Entity.h"
#include "ungod/visual/Visual.h"
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
        void setMultiSpriteTextureRect(ungod::Entity e, const std::string& key,unsigned i);
        void setSpritePosition(ungod::Entity e, const sf::Vector2f& pos);
        void setMultiSpritePosition(ungod::Entity e, const sf::Vector2f& pos,unsigned i);
        void setVertexarrayPosition(ungod::Entity e, const sf::Vector2f& pos,unsigned i);
        void setSpriteScale(ungod::Entity e, const sf::Vector2f& scale);
        void setSpriteRotation(ungod::Entity e, float rota);
        void setSpriteColor(ungod::Entity e, const sf::Color& color);
        void setMultiSpriteScale(ungod::Entity e, const sf::Vector2f& scale,unsigned i);
        void setMultiSpriteRotation(ungod::Entity e, float rota,unsigned i);
        void newVertexArrayRect(ungod::Entity e, const sf::FloatRect& rect);
        void setVertexArrayRect(ungod::Entity e,unsigned i, const std::string& key);
        void setVertexArrayRectColor(ungod::Entity e,unsigned i, const sf::Color& color);
        void flipVertexX(ungod::Entity e, unsigned i);
        void flipVertexY(ungod::Entity e, unsigned i);

    private:
        ActionManager& mActionManager;
    };
}

#endif