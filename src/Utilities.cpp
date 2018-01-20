#include "Utilities.h"
#include <cmath>

void DrawDebugLine(const sf::Vector2f& A, const sf::Vector2f& B, sf::RenderTarget& target, const sf::Color& colour)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = A;
    line[1].position = B;
    line[0].color = colour;
    line[1].color = colour;
    target.draw(line);
}

void DrawDebugLine(const float& ax,
                   const float& ay,
                   const float& bx,
                   const float& by,
                   sf::RenderTarget& target,
                   const sf::Color& colour)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = sf::Vector2f(ax, ay);
    line[1].position = sf::Vector2f(bx, by);
    line[0].color = colour;
    line[1].color = colour;
    target.draw(line);
}

void DrawDebugLine(const sf::Vector2f& pos,
                   const sf::Vector2f& dir,
                   const float& norm,
                   sf::RenderTarget& target,
                   const sf::Color& colour)
{
    // sf::Vector2f normalizedDir = Normalize(dir);
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = pos;
    line[0].color = colour;
    line[1].position = pos + dir * norm;
    line[1].color = colour;
    target.draw(line);
}

void DrawDebugCircle(const sf::Vector2f& pos, const float& radius, sf::RenderTarget& target, const sf::Color& colour)
{
    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setOutlineColor(colour);
    circle.setOutlineThickness(2);
    circle.setFillColor(sf::Color::Transparent);
    circle.setPosition(pos);
    CenterOrgin(circle);
    target.draw(circle);
}

const sf::Vector2f GetCenter(const sf::ConvexShape& shape)
{
    sf::Vector2f centerPos(0, 0);
    for (int i = 0; i < shape.getPointCount(); i++) {
        centerPos += shape.getPoint(i);
    }
    return (centerPos / (float)shape.getPointCount()) + shape.getPosition();
}

const sf::Vector2f Midpoint(const sf::Vector2f& pos1, const sf::Vector2f& pos2)
{
    return (pos1 + pos2) / 2.f;
}

const float Norm(const sf::Vector2f& v)
{
    return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
}

const float NormSquared(const sf::Vector2f& v)
{
    return std::pow(v.x, 2) + std::pow(v.y, 2);
}

const sf::Vector2f Normalize(const sf::Vector2f& vec)
{
    return vec / Norm(vec);
}

const float Distance(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
    return std::sqrt(std::pow((v2.x - v1.x), 2) + std::pow((v2.y - v1.y), 2));
}

const float DotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

const float DotProduct(const sf::Vector2f& a, const float& s)
{
    return a.x * s + a.y * s;
}

const float CrossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.y - a.y * b.x;
}
const sf::Vector2f CrossProduct(const sf::Vector2f& a, const float s)
{
    return sf::Vector2f(s * a.y, -s * a.x);
}
const sf::Vector2f CrossProduct(const float s, const sf::Vector2f& a)
{
    return sf::Vector2f(-s * a.y, s * a.x);
}

const bool isLeft(const sf::Vector2f& a1, const sf::Vector2f& a2, const sf::Vector2f& testPt)
{
    return ((a2.x - a1.x) * (testPt.y - a1.y) - (a2.y - a1.y) * (testPt.x - a1.x)) > 0;
}

float Lerp(const float& a, const float& b, const float& t)
{
    return (1 - t) * a + t * b;
}

float LerpFast(const float& a, const float& b, const float& t)
{
    return a + t * (b - a);
}

float Clamp(const float& value, const float& min, const float& max)
{
    float result;
    if (value > max)
        result = max;
    else if (value < min)
        result = min;
    else
        result = value;
    return result;
}

std::string VectorToStr(const sf::Vector2f& vec)
{
    return std::string(" [ " + std::to_string(vec.x) + " , " + std::to_string(vec.y) + " ] ");
}
std::string VectorToStr(const sf::Vector2i& vec)
{
    return std::string(" [ " + std::to_string(vec.x) + " , " + std::to_string(vec.y) + " ] ");
}
std::string VectorToStr(const sf::Vector2u& vec)
{
    return std::string(" [ " + std::to_string(vec.x) + " , " + std::to_string(vec.y) + " ] ");
}

void CenterOrgin(sf::CircleShape& shape)
{
    shape.setOrigin(sf::Vector2f(shape.getRadius(), shape.getRadius()));
}

void CenterOrgin(sf::Shape& shape)
{
    shape.setOrigin(sf::Vector2f(shape.getLocalBounds().width, shape.getLocalBounds().height) / 2.f);
}

const sf::Vector2f CalcEdgeNormal(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return Normalize(sf::Vector2f((b.y - a.y), -(b.x - a.x)));
}

const sf::Vector2f CalcIntersectionPoint(const sf::Vector2f& x1,
                                         const sf::Vector2f& x2,
                                         const sf::Vector2f& y1,
                                         const sf::Vector2f& y2)
{
    ///< Get line in form: y = mx + b
    float m1;
    if (x1.x - x2.x == 0) {
        m1 = 999999999999;
    } else {
        m1 = (x1.y - x2.y) / (x1.x - x2.x);
    }
    float b1 = x2.y - m1 * x2.x;

    ///< Get line in form: y = mx + b
    float m2;

    if (y1.x - y2.x == 0) {
        m2 = 999999999999;
    } else {
        m2 = (y1.y - y2.y) / (y1.x - y2.x);
    }

    float b2 = y2.y - m2 * y2.x;

    if (m1 - m2 == 0) {
        // no intersection
        // return y1;
    }
    float xIntercect = (b2 - b1) / (m1 - m2);
    float yIntercect = m1 * xIntercect + b1;

    return sf::Vector2f(xIntercect, yIntercect);
}

//< Algorythm by: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
bool ptContatinedInPoly(sf::Vector2f testPt, const sf::ConvexShape& poly)
{
    testPt -= poly.getPosition();
    int i, j = 0;
    int nvert = (int)poly.getPointCount();
    bool isContatined = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((poly.getPoint(i).y > testPt.y) != (poly.getPoint(j).y > testPt.y)) &&
            (testPt.x < (poly.getPoint(j).x - poly.getPoint(i).x) * (testPt.y - poly.getPoint(i).y) /
                                (poly.getPoint(j).y - poly.getPoint(i).y) +
                            poly.getPoint(i).x))
            isContatined = !isContatined;
    }
    return isContatined;
}
