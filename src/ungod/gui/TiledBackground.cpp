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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ungod/gui/TiledBackground.h"
#include <TGUI/RendererDefines.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_TEXTURE(TiledBackgroundRenderer, Texture)

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TiledBackground::TiledBackground()
    {
        m_type = "TiledBackground";

        m_renderer = aurora::makeCopied<TiledBackgroundRenderer>();
        setRenderer(RendererData::create({}));

        m_vertices.setPrimitiveType(sf::Quads);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TiledBackground::TiledBackground(const Texture& texture) :
        TiledBackground{}
    {
        getRenderer()->setTexture(texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TiledBackground::Ptr TiledBackground::create(const Texture& texture)
    {
        return std::make_shared<TiledBackground>(texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TiledBackground::Ptr TiledBackground::copy(TiledBackground::ConstPtr backgr)
    {
        if (backgr)
            return std::static_pointer_cast<TiledBackground>(backgr->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::setPosition(const Layout2d& position)
    {
        sf::Vector2f offset;
        offset.x = position.x.getValue() - getPosition().x;
        offset.y = position.y.getValue() - getPosition().y;
        for (unsigned i = 0; i < m_vertices.getVertexCount(); ++i)
        {
            m_vertices[i].position += offset;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::setSize(const Layout2d& size)
    {
        const auto& texture = getRenderer()->getTexture();

        float texwidth = texture.getImageSize().x;
        float texheight = texture.getImageSize().y;

        //check for valid texture
        if (texwidth == 0 || texheight == 0)
            return;

        Widget::setSize(size);

        float horizontalTileCount = size.x.getValue() / texwidth;
        float verticalTileCount = size.y.getValue() / texheight;
        float cpyvert = verticalTileCount;
        unsigned ceilHoriz = (unsigned)std::ceil(horizontalTileCount);
        unsigned ceilVert = (unsigned)std::ceil(verticalTileCount);

        m_vertices.resize(4*(std::size_t)ceilHoriz*(std::size_t)ceilVert);

        for (unsigned i = 0; i < ceilHoriz; ++i)
        {
            verticalTileCount = cpyvert;
            for (unsigned j = 0; j < ceilVert; ++j)
            {
                sf::Vertex* quad = &m_vertices[4*((std::size_t)i+(std::size_t)j*(std::size_t)ceilHoriz)];

                //indicate "how much" of the texture the next tile covers
                float gapx;
                float gapy;

                if (verticalTileCount - 1.0f >= 0.0f)
                    gapy = 1.0f;
                else
                    gapy = verticalTileCount;

                if (horizontalTileCount - 1.0f >= 0.0f)
                    gapx = 1.0f;
                else
                    gapx = horizontalTileCount;

                quad[0].position = sf::Vector2f(getPosition().x + i*texwidth, getPosition().y + j*texheight);
                quad[1].position = sf::Vector2f(getPosition().x + (i + gapx)*texwidth, getPosition().y + j*texheight);
                quad[2].position = sf::Vector2f(getPosition().x + (i + gapx)*texwidth, getPosition().y + (j + gapy)*texheight);
                quad[3].position = sf::Vector2f(getPosition().x + i*texwidth, getPosition().y + (j + gapy)*texheight);

                quad[0].texCoords = sf::Vector2f(0, 0);
                quad[1].texCoords = sf::Vector2f(gapx*texwidth, 0);
                quad[2].texCoords = sf::Vector2f(gapx*texwidth, gapy*texheight);
                quad[3].texCoords = sf::Vector2f(0, gapy*texheight);

                verticalTileCount -= 1.0f;
            }
            horizontalTileCount -= 1.0f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::ignoreMouseEvents(bool ignore)
    {
        m_ignoringMouseEvents = ignore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TiledBackground::isIgnoringMouseEvents() const
    {
        return m_ignoringMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TiledBackground::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the picture
        if (!m_ignoringMouseEvents && (sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos)))
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        if (mouseDown)
        {
            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;
                onDoubleClick.emit(this, pos - getPosition());
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TiledBackground::getSignal(std::string signalName)
    {
        if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::rendererChanged(const std::string& property)
    {
        if (property == "texture")
        {
            if (getSize() == sf::Vector2f{0,0})
                setSize(getRenderer()->getTexture().getImageSize());
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);
            for (unsigned i = 0; i < m_vertices.getVertexCount(); ++i)
            {
                m_vertices[i].color.a = (sf::Uint8)(255*m_opacityCached);
            }
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // When double-clicking, the second click has to come within 500 milliseconds
        if (m_animationTimeElapsed >= sf::milliseconds(500))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TiledBackground::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        states.texture = &getRenderer()->getTexture().getData()->texture;
        target.draw(m_vertices, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
