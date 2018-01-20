#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>

/// Utility to draw a line takes two sf::Vector2f, sf::RenderWindow, and a colour (optional)
void DrawDebugLine(const sf::Vector2f& A,
                   const sf::Vector2f& B,
                   sf::RenderTarget& target,
                   const sf::Color& colour = sf::Color::Yellow);

/// Utility to draw a line takes four floats(x1,y1,x2,y2), sf::RenderWindow, and a colour (optional)
void DrawDebugLine(const float& ax,
                   const float& ay,
                   const float& bx,
                   const float& by,
                   sf::RenderTarget& target,
                   const sf::Color& colour = sf::Color::Yellow);

/// Utility to draw a line takes two sf::Vector2f (start position, direction), float norm, sf::RenderWindow, and a
/// colour (optional)
void DrawDebugLine(const sf::Vector2f& pos,
                   const sf::Vector2f& dir,
                   const float& norm,
                   sf::RenderTarget& target,
                   const sf::Color& colour = sf::Color::Yellow);

/// Draw debug circle
void DrawDebugCircle(const sf::Vector2f& pos,
                     const float& radius,
                     sf::RenderTarget& target,
                     const sf::Color& colour = sf::Color::Yellow);

/// Utility to calculate the norm of a sf::Vector2f;
const float Norm(const sf::Vector2f& v);

const float NormSquared(const sf::Vector2f& v);

/// Utility to normalize a sf::Vector2f; (unit vector)
const sf::Vector2f Normalize(const sf::Vector2f& vec);

const sf::Vector2f Midpoint(const sf::Vector2f& pos1, const sf::Vector2f& pos2);

const sf::Vector2f GetCenter(const sf::ConvexShape& shape);

////////////////////////////////////////////////////////////
/// Utility to calculate the distance between two sf::Vector2f (points)
const float Distance(const sf::Vector2f& v1, const sf::Vector2f& v2);

const float DotProduct(const sf::Vector2f& a, const sf::Vector2f& b);
const float DotProduct(const sf::Vector2f& a, const float s);

/// Returns float of the crossproduct of two sf::Vectors
const float CrossProduct(const sf::Vector2f& a, const sf::Vector2f& b);

/// Returns crossproduct of
const sf::Vector2f CrossProduct(const sf::Vector2f& a, const float s);
const sf::Vector2f CrossProduct(const float s, const sf::Vector2f& a);

const bool isLeft(const sf::Vector2f& a1, const sf::Vector2f& a2, const sf::Vector2f& testPt);

/// Utility to lerp between 2 values with a value from 0 to 1
/// Precise method which guarantees v = b when t = 1.
float Lerp(const float& a, const float& b, const float& t);

/// Utility to lerp between 2 values with a value from 0 to 1
/// Imprecise method which does not guarantee v = v1 when t = 1, due to floating-point arithmetic error.
float LerpFast(const float& a, const float& b, const float& t);

/// Utility to clamp a float into the bounds of 2 floats
float Clamp(const float& value, const float& min, const float& max);

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Other Useful Functions///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/// Utility to print a sf::Vector2 as a string
std::string VectorToStr(const sf::Vector2f& vec);
std::string VectorToStr(const sf::Vector2i& vec);
std::string VectorToStr(const sf::Vector2u& vec);

/// Centers the given sf::Shape at the origin
void CenterOrgin(sf::CircleShape& shape);
void CenterOrgin(sf::Shape& shape);

// Calculates the edge normal assuming clockwise order
const sf::Vector2f CalcEdgeNormal(const sf::Vector2f& a, const sf::Vector2f& b);

const sf::Vector2f CalcIntersectionPoint(const sf::Vector2f& a1,
                                         const sf::Vector2f& b1,
                                         const sf::Vector2f& a2,
                                         const sf::Vector2f& b2);

bool ptContatinedInPoly(sf::Vector2f testPt, const sf::ConvexShape& poly);
