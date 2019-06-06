#ifndef _ELLIPSESHAPE__
#define _ELLIPSESHAPE__

#include <SFML/Graphics.hpp>

namespace sf
{
    class EllipseShape : public sf::Shape
    {
    private:
        sf::Vector2f sRadius;
    public:
        explicit EllipseShape(const sf::Vector2f& cRadius = sf::Vector2f(1,1));
        const float getHorizontalRadius() const {return sRadius.x;}
        const float getVerticalRadius()   const {return sRadius.y;}
        const float getRadius(const sf::Vector2f& sDirection) const;
        void        setRadius(const sf::Vector2f& nRadius);
        virtual sf::Vector2f getPoint(std::size_t index) const override;
        virtual std::size_t getPointCount() const override {return 30;}
        const sf::Vector2f getCenterPosition() const;
    };


    EllipseShape::EllipseShape(const sf::Vector2f& cRadius) : sRadius(cRadius)
    {
        update();
    }

    /*________________________________________________________________________________
    Returns the radius of the ellipse along the given direction-vector. Calculates the
    angle between sDirection and the x-unit-vector.
    _________________________________________________________________________________*/
    const float EllipseShape::getRadius(const sf::Vector2f& sDirection) const
    {
        const float sqmagn = pow(sDirection.x,2) + pow(sDirection.y,2);
        float cosAlphaSq;
        if(sqmagn == 0)
            cosAlphaSq = 0;
        else
            cosAlphaSq = pow(sDirection.x,2) / sqmagn;
        const float sinAlphaSq = 1 - cosAlphaSq;
        return (sRadius.x*sRadius.y)/sqrt(pow(sRadius.x,2)*sinAlphaSq + pow(sRadius.y,2)*cosAlphaSq);
    }

    void EllipseShape::setRadius(const sf::Vector2f& nRadius)
    {
        sRadius = nRadius;
        update();
    }

    sf::Vector2f EllipseShape::getPoint(std::size_t index) const
    {
        static const float pi = 3.141592654f;

        float angle = index * 2 * pi / getPointCount() - pi / 2;
        float x = std::cos(angle) * sRadius.x;
        float y = std::sin(angle) * sRadius.y;

        return sf::Vector2f(sRadius.x + x, sRadius.y + y);
    }

    const sf::Vector2f EllipseShape::getCenterPosition() const
    {
        sf::Vector2f sCenter = getPosition();
        sCenter.x += sRadius.x;
        sCenter.y += sRadius.y;
        return sCenter;
    }
}

#endif // _ELLIPSESHAPE__
