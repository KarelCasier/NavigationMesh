#include "ShadowManager.h"
#include "CollisionHelpers.h"
#include <math.h>
#include <algorithm>
#include "GlobalConstants.hpp"

ShadowManager::ShadowManager(sf::View* view)
: pView(view)
{
}

void ShadowManager::addShape(sf::ConvexShape* newShape)
{
    mShadowCasters.push_back(newShape);
    mShadows.push_back(new sf::ConvexShape);
}

void ShadowManager::updateShadows(const sf::Vector2f& newLightPos)
{
    lightPos = newLightPos;

    ///< Update screen corners for shadow drawing
    const sf::Vector2f& viewPos = pView->getCenter();
    const sf::Vector2f& viewDimentions = pView->getSize();
    mScreenCorners.at(0) =
        viewPos + sf::Vector2f(-0.5f * viewDimentions.x - 1, -0.5f * viewDimentions.y - 1);           // Top Left
    mScreenCorners.at(1) = viewPos + sf::Vector2f(0.5f * viewDimentions.x, -0.5f * viewDimentions.y); // Top Right
    mScreenCorners.at(2) =
        viewPos + sf::Vector2f(0.5f * viewDimentions.x + 1, 0.5f * viewDimentions.y + 1);             // Bottom Right
    mScreenCorners.at(3) = viewPos + sf::Vector2f(-0.5f * viewDimentions.x, 0.5f * viewDimentions.y); // Bottom Left

    // sf::Vector2f topEdgeVec = mScreenCorners.at(0) - mScreenCorners.at(1);
    // sf::Vector2f rightEdgeVec = mScreenCorners.at(1) - mScreenCorners.at(2);
    // sf::Vector2f botEdgeVec = mScreenCorners.at(2) - mScreenCorners.at(3);
    // sf::Vector2f leftEdgeVec = mScreenCorners.at(0) - mScreenCorners.at(3);

    ///< Create a ray from the light source through each vertex of each object to the endge of the screen
    std::vector<sf::ConvexShape*>::iterator shadow = mShadows.begin();
    for (const sf::ConvexShape* shape : mShadowCasters) {
        // const int rotationDeg = shape->getRotation();
        const sf::Vector2f& shapePos = shape->getPosition();
        if (Norm(viewPos - shapePos) > WINDOWCORNERTOCORNER) {
            ///< Not on screen
            (*shadow)->setPointCount(0);
            ++shadow;
            continue;
        }
        ///< Create a shape for the shadow
        (*shadow)->setFillColor(sf::Color::Black);
        std::vector<sf::Vector2f> shadowPoints;

        int numVerticies = (int)shape->getPointCount();

        for (int i = 0; i < numVerticies; i++) {
            sf::Vector2f v = shape->getPoint(i) + shapePos;

            ///< Get intersection on each face
            sf::Vector2f topIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[0], mScreenCorners[1]);
            sf::Vector2f rightIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[1], mScreenCorners[2]);
            sf::Vector2f botIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[2], mScreenCorners[3]);
            sf::Vector2f leftIntersect = GetIntersectionPoint(lightPos, v, mScreenCorners[3], mScreenCorners[0]);

            ///< Choose the minimal intersection point for all walls
            sf::Vector2f minimalIntersection;

            if (v.y <= lightPos.y && v.x >= lightPos.x) // Bottom left quad (vertex is the orgin)
            {
                if (NormSquared(topIntersect) < NormSquared(rightIntersect)) {
                    minimalIntersection = topIntersect;
                } else {
                    minimalIntersection = rightIntersect;
                }
            } else if (v.y <= lightPos.y && v.x <= lightPos.x) // Bottom right quad
            {
                if (NormSquared(topIntersect) < NormSquared(leftIntersect)) {
                    minimalIntersection = topIntersect;
                } else {
                    minimalIntersection = leftIntersect;
                }
            } else if (v.y >= lightPos.y && v.x >= lightPos.x) // Top left quad
            {
                if (NormSquared(botIntersect) < NormSquared(rightIntersect)) {
                    minimalIntersection = botIntersect;
                } else {
                    minimalIntersection = rightIntersect;
                }
            } else if (v.y >= lightPos.y && v.x <= lightPos.x) // Top right quad
            {
                if (NormSquared(botIntersect) < NormSquared(leftIntersect)) {
                    minimalIntersection = botIntersect;
                } else {
                    minimalIntersection = leftIntersect;
                }
            }

            ///< Avoid creating shadows that don't actually exist when objects are big (ie edges of screen when vertex
            ///< is not on the screen
            if (NormSquared(lightPos - minimalIntersection) > NormSquared(lightPos - v)) {
                shadowPoints.push_back(minimalIntersection);
                // mDebugPoints.push_back(shadowPoints.back());
                // mDebugLines.push_back(shadowPoints.back());
            }

            std::vector<sf::Vector2f> possiblePoints;

            ///< Find the points that make up the shadow

            ///< Get all intersections the ray makes with the edges of the shape
            for (int j = 0; j < numVerticies - 1; j++) {
                sf::Vector2f edgeIntersection = GetIntersectionPoint(
                    lightPos, minimalIntersection, shape->getPoint(j) + shapePos, shape->getPoint(j + 1) + shapePos);
                if (isPointOnLineSegment(edgeIntersection, shape->getPoint(j) + shapePos,
                                         shape->getPoint(j + 1) + shapePos)) {
                    sf::Vector2f offset = Normalize(v - lightPos) * 10.f;
                    possiblePoints.push_back(edgeIntersection + offset);
                }
            }
            sf::Vector2f edgeIntersection =
                GetIntersectionPoint(lightPos, minimalIntersection, shape->getPoint(numVerticies - 1) + shapePos,
                                     shape->getPoint(0) + shapePos);
            if (isPointOnLineSegment(edgeIntersection, shape->getPoint(numVerticies - 1) + shapePos,
                                     shape->getPoint(0) + shapePos)) {
                sf::Vector2f offset = Normalize(v - lightPos) * 10.f;
                possiblePoints.push_back(edgeIntersection + offset);
            }

            ///< Only use the closest intersection
            if (!possiblePoints.empty()) {
                shadowPoints.push_back(getMinimumDistfromPoint(lightPos, possiblePoints));
                // mDebugPoints.push_back(shadowPoints.back());
                // mDebugLines.push_back(shadowPoints.back());
            }
        }
        ///< Shoot rays at the corners and add them if they cross one of the shape edges
        for (int i = 0; i < numVerticies - 1; i++) {
            if (doIntersect(lightPos, mScreenCorners[0], shape->getPoint(i) + shapePos,
                            shape->getPoint(i + 1) + shapePos)) {
                shadowPoints.push_back(mScreenCorners[0]);
                // mDebugPoints.push_back(shadowPoints.back());
                // mDebugLines.push_back(shadowPoints.back());
            }
            if (doIntersect(lightPos, mScreenCorners[1], shape->getPoint(i) + shapePos,
                            shape->getPoint(i + 1) + shapePos)) {
                shadowPoints.push_back(mScreenCorners[1]);
                // mDebugPoints.push_back(shadowPoints.back());
                // mDebugLines.push_back(shadowPoints.back());
            }
            if (doIntersect(lightPos, mScreenCorners[2], shape->getPoint(i) + shapePos,
                            shape->getPoint(i + 1) + shapePos)) {
                shadowPoints.push_back(mScreenCorners[2]);
                // mDebugPoints.push_back(shadowPoints.back());
                // mDebugLines.push_back(shadowPoints.back());
            }
            if (doIntersect(lightPos, mScreenCorners[3], shape->getPoint(i) + shapePos,
                            shape->getPoint(i + 1) + shapePos)) {
                shadowPoints.push_back(mScreenCorners[3]);
                // mDebugPoints.push_back(shadowPoints.back());
                // mDebugLines.push_back(shadowPoints.back());
            }

            ///< Add points to make shadows look better for bigger objects
            //< DOESN"T WORK BECAUSE IT'S WONT BE A CONVEX POLYGON
            /*/
                        float norm = Norm(shape->getPoint(i+1) - shape->getPoint(i));
                        if (norm > 200)
                        {
                                sf::Vector2f unitVec = (shape->getPoint(i+1) - shape->getPoint(i)) / norm;

                                int numRays = (int)norm / 100;

                                float distanceBetweenRays = norm / numRays;


                                for (int j = 1; j < numRays; ++j)
                                {
                                        sf::Vector2f pos = shape->getPoint(i) + shapePos + unitVec *
             (distanceBetweenRays * (float)(j)); sf::Vector2f offset = Normalize(lightPos-pos) * -10.f;
                                        mDebugPoints.push_back(pos + offset);
                                        shadowPoints.push_back(pos + offset);
                                }
                        }
             //*/
        }
        ///< Add points to make shadows look better for bigger objects
        //< DOESN"T WORK BECAUSE IT'S WONT BE A CONVEX POLYGON
        /*/
                float norm = Norm(shape->getPoint(numVerticies-1) - shape->getPoint(0));
                if (norm > 200)
                {
                        sf::Vector2f unitVec = (shape->getPoint(numVerticies-1) - shape->getPoint(0)) / norm;

                        int numRays = (int)norm / 100;

                        float distanceBetweenRays = norm / numRays;


                        for (int j = 1; j < numRays; ++j)
                        {
                                //sf::Vector2f pos = shape->getPoint(0) + unitVec * (distanceBetweenRays * (float)(j));
                                //sf::Vector2f offset = Normalize(lightPos-pos) * -10.f;
                                //mDebugPoints.push_back(pos + offset);
                                //shadowPoints.push_back(pos + offset);
                        }
                }
         //*/

        ///< Sort points in clockwise order (for creating a convex shape)
        shadowPoints = convex_hull(shadowPoints);

        (*shadow)->setPointCount(shadowPoints.size());

        for (int k = 0; k < shadowPoints.size(); k++) {
            (*shadow)->setPoint(k, shadowPoints.at(k));
        }

        if (shadow != mShadows.end()) {
            shadow++;
        }
    }
}

void ShadowManager::drawShadows(sf::RenderTarget& target)
{
    for (const sf::ConvexShape* shadow : mShadows) {
        target.draw(*shadow);
    }

    for (const sf::Vector2f& vec : mDebugLines) {
        DrawDebugLine(lightPos, vec, target);
    }
    mDebugLines.clear();

    for (const sf::Vector2f& vec : mDebugPoints) {
        DrawDebugCircle(vec, 5.f, target);
    }
    mDebugPoints.clear();
}