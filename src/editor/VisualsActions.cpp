#include "VisualsActions.h"
#include "ActionManager.h"

namespace uedit
{
    void VisualsActions::setEntityHideForCamera(ungod::Entity e, bool hideForCamera)
    {
        bool prev = e.get<ungod::VisualsComponent>().isHiddenForCamera();
        mActionManager.action(std::function([this, hideForCamera](ungod::Entity e)
            { e.getWorld().getVisualsHandler().setHideForCamera(e, hideForCamera); }),
            std::function([this, prev](ungod::Entity e)
                { e.getWorld().getVisualsHandler().setHideForCamera(e, prev); }),
            e);
    }


    void VisualsActions::loadTexture(ungod::Entity e, const std::string& file)
    {
        std::string oldFile = e.get<ungod::VisualsComponent>().getFilePath();
        mActionManager.action(std::function([this, file](ungod::Entity e) { e.getWorld().getVisualsHandler().loadTexture(e, file, ungod::LoadPolicy::SYNC); }),
            std::function([this, oldFile](ungod::Entity e) { if (oldFile != "") e.getWorld().getVisualsHandler().loadTexture(e, oldFile, ungod::LoadPolicy::SYNC); }),
            e);
    }

    void VisualsActions::loadMetadata(ungod::Entity e, const std::string& file)
    {
        std::string oldFile = e.get<ungod::SpriteMetadataComponent>().getFilePath();
        mActionManager.action(std::function([this, file](ungod::Entity e) { e.getWorld().getVisualsHandler().loadMetadata(e, file); }),
            std::function([this, oldFile](ungod::Entity e) { if (oldFile != "") e.getWorld().getVisualsHandler().loadMetadata(e, oldFile); }),
            e);
    }

    void VisualsActions::setSpriteTextureRect(ungod::Entity e, const std::string& key)
    {
        auto oldrect = e.get<ungod::SpriteComponent>().getSprite().getTextureRect();
        mActionManager.action(std::function([this, key](ungod::Entity e) { e.getWorld().getVisualsHandler().setSpriteTextureRect(e, key); }),
            std::function([this, oldrect](ungod::Entity e) { e.getWorld().getVisualsHandler().setSpriteTextureRect(e, oldrect); }),
            e);
    }

    void VisualsActions::setMultiSpriteTextureRect(ungod::Entity e, const std::string& key, std::size_t i)
    {
        auto oldrect = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getTextureRect();
        mActionManager.action(std::function([this, key](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setSpriteTextureRect(e, key, i); }),
            std::function([this, oldrect](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setSpriteTextureRect(e, oldrect, i); }),
            e, i);
    }

    void VisualsActions::setSpritePosition(ungod::Entity e, const sf::Vector2f& pos)
    {
        auto oldpos = e.get<ungod::SpriteComponent>().getSprite().getPosition();
        mActionManager.action(std::function([this, pos](ungod::Entity e) { e.getWorld().getVisualsHandler().setSpritePosition(e, pos); }),
            std::function([this, oldpos](ungod::Entity e) { e.getWorld().getVisualsHandler().setSpritePosition(e, oldpos); }),
            e);
    }

    void VisualsActions::setMultiSpritePosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i)
    {
        auto oldpos = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getPosition();
        mActionManager.action(std::function([this, pos](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setSpritePosition(e, pos, i); }),
            std::function([this, oldpos](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setSpritePosition(e, oldpos, i); }),
            e, i);
    }

    void VisualsActions::setVertexarrayPosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i)
    {
        auto oldpos = e.get<ungod::VertexArrayComponent>().getVertices().getPosition(i);
        mActionManager.action(std::function([this, pos](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setTextureRectPosition(e, pos, i); }),
            std::function([this, oldpos](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setTextureRectPosition(e, oldpos, i); }),
            e, i);
    }

    void VisualsActions::setSpriteScale(ungod::Entity e, const sf::Vector2f& scale)
    {
        auto oldscale = e.get<ungod::SpriteComponent>().getSprite().getScale();
        mActionManager.action(std::function([this, scale](ungod::Entity e) { e.getWorld().getVisualsHandler().setScale(e, scale); }),
            std::function([this, oldscale](ungod::Entity e) { e.getWorld().getVisualsHandler().setScale(e, oldscale); }),
            e);
    }

    void VisualsActions::setSpriteRotation(ungod::Entity e, float rota)
    {
        auto oldrota = e.get<ungod::SpriteComponent>().getSprite().getRotation();
        mActionManager.action(std::function([this, rota](ungod::Entity e) { e.getWorld().getVisualsHandler().setRotation(e, rota); }),
            std::function([this, oldrota](ungod::Entity e) { e.getWorld().getVisualsHandler().setRotation(e, oldrota); }),
            e);
    }

    void VisualsActions::setMultiSpriteScale(ungod::Entity e, const sf::Vector2f& scale, std::size_t i)
    {
        auto oldscale = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getScale();
        mActionManager.action(std::function([this, scale](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setScale(e, scale, i); }),
            std::function([this, oldscale](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setScale(e, oldscale, i); }),
            e, i);
    }

    void VisualsActions::setMultiSpriteRotation(ungod::Entity e, float rota, std::size_t i)
    {
        auto oldrota = e.get<ungod::MultiSpriteComponent>().getComponent(i).getSprite().getRotation();
        mActionManager.action(std::function([this, rota](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setRotation(e, rota, i); }),
            std::function([this, oldrota](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setRotation(e, oldrota, i); }),
            e, i);
    }

    void VisualsActions::newVertexArrayRect(ungod::Entity e, const sf::FloatRect& rect)
    {
        mActionManager.action(std::function([this, rect](ungod::Entity e) { e.getWorld().getVisualsHandler().newVertexTextureRect(e, rect); }),
            std::function([this](ungod::Entity e) { e.getWorld().getVisualsHandler().removeLastVertexTextureRect(e); }),
            e);
    }

    void VisualsActions::setVertexArrayRect(ungod::Entity e, std::size_t i, const std::string& key)
    {
        auto oldrect = e.get<ungod::VertexArrayComponent>().getVertices().getTextureRect(i);
        mActionManager.action(std::function([this, key](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setVertexTextureRect(e, i, key); }),
            std::function([this, oldrect](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setVertexTextureRect(e, i, oldrect); }),
            e, i);
    }

    void VisualsActions::setVertexArrayRectColor(ungod::Entity e, std::size_t i, const sf::Color& color)
    {
        auto oldcol = e.get<ungod::VertexArrayComponent>().getVertices().getRectColor(i);
        mActionManager.action(std::function([this, color](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setArrayRectColor(e, color, i); }),
            std::function([this, oldcol](ungod::Entity e, std::size_t i) { e.getWorld().getVisualsHandler().setArrayRectColor(e, oldcol, i); }),
            e, i);
    }

    void VisualsActions::flipVertexX(ungod::Entity e, unsigned i)
    {
        mActionManager.action(std::function([this](ungod::Entity e, unsigned i) { e.getWorld().getVisualsHandler().flipVertexX(e, i); }),
            std::function([this](ungod::Entity e, unsigned i) { e.getWorld().getVisualsHandler().flipVertexX(e, i); }),
            e, i);
    }

    void VisualsActions::flipVertexY(ungod::Entity e, unsigned i)
    {
        mActionManager.action(std::function([this](ungod::Entity e, unsigned i) { e.getWorld().getVisualsHandler().flipVertexY(e, i); }),
            std::function([this](ungod::Entity e, unsigned i) { e.getWorld().getVisualsHandler().flipVertexY(e, i); }),
            e, i);
    }
}