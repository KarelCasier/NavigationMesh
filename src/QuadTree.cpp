#include "QuadTree.h"

QuadTree::QuadTree(
    const float& x, const float& y, const float& width, const float& height, const int& _depth, const int& maxDepth)
: mX(x)
, mY(y)
, mWidth(width)
, mHeight(height)
, mDepth(_depth)
, maxDepth(maxDepth)
, NW(nullptr)
, NE(nullptr)
, SW(nullptr)
, SE(nullptr)
{
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(0, 0, 0, 0));
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color(64, 128, 255));

    if (mDepth != maxDepth) {
        ///< Divide into 4 chunks
        NW = new QuadTree(x, y, width / 2.f, height / 2.f, mDepth + 1, maxDepth);
        NE = new QuadTree(x + width / 2.f, y, width / 2.f, height / 2.f, mDepth + 1, maxDepth);
        SW = new QuadTree(x, y + height / 2.f, width / 2.f, height / 2.f, mDepth + 1, maxDepth);
        SE = new QuadTree(x + width / 2.f, y + height / 2.f, width / 2.f, height / 2.f, mDepth + 1, maxDepth);
    }
}

QuadTree::~QuadTree()
{
    if (NW) {
        delete NW;
    }
    if (NE) {
        delete NE;
    }
    if (SW) {
        delete SW;
    }
    if (SE) {
        delete SE;
    }
}

void QuadTree::AddObject(sf::ConvexShape* object)
{
    if (mDepth == maxDepth) {
        ///< Don't divide anymore, just add the object to current quad
        objects.push_back(object);
        return;
    }

    ///< Add to correct quad
    if (Contains(NW, object)) {
        NW->AddObject(object);
        return;
    } else if (Contains(NE, object)) {
        NE->AddObject(object);
        return;
    } else if (Contains(SW, object)) {
        SW->AddObject(object);
        return;
    } else if (Contains(SE, object)) {
        SE->AddObject(object);
        return;
    }

    if (Contains(this, object)) {
        objects.push_back(object);
    }
}

std::vector<sf::ConvexShape*> QuadTree::GetObjectsAt(const float& x, const float& y)
{
    if (mDepth == maxDepth) {
        return objects;
    }

    std::vector<sf::ConvexShape*> objectsToReturn, childObjectsToReturn;
    if (!objects.empty()) {
        objectsToReturn = objects;
    }

    if (x > mX + mWidth / 2.0f && x < mX + mWidth) {
        if (y > mY + mHeight / 2.0f && y < mY + mHeight) {
            childObjectsToReturn = SE->GetObjectsAt(x, y);
            objectsToReturn.insert(objectsToReturn.end(), childObjectsToReturn.begin(), childObjectsToReturn.end());
            return objectsToReturn;
        } else if (y > mY && y <= mY + mHeight / 2.0f) {
            childObjectsToReturn = NE->GetObjectsAt(x, y);
            objectsToReturn.insert(objectsToReturn.end(), childObjectsToReturn.begin(), childObjectsToReturn.end());
            return objectsToReturn;
        }
    } else if (x > mX && x <= mX + mWidth / 2.0f) {
        if (y > mY + mHeight / 2.0f && y < mY + mHeight) {
            childObjectsToReturn = SW->GetObjectsAt(x, y);
            objectsToReturn.insert(objectsToReturn.end(), childObjectsToReturn.begin(), childObjectsToReturn.end());
            return objectsToReturn;
        } else if (y > mY && y <= mY + mHeight / 2.0f) {
            childObjectsToReturn = NW->GetObjectsAt(x, y);
            objectsToReturn.insert(objectsToReturn.end(), childObjectsToReturn.begin(), childObjectsToReturn.end());
            return objectsToReturn;
        }
    }
    return objectsToReturn;
}

bool QuadTree::Contains(const QuadTree* child, const sf::ConvexShape* object)
{
    return !(object->getGlobalBounds().left < child->mX || object->getGlobalBounds().top < child->mY ||
             object->getGlobalBounds().left > child->mX + child->mWidth ||
             object->getGlobalBounds().top > child->mY + child->mHeight ||
             object->getGlobalBounds().left + object->getGlobalBounds().width < child->mX ||
             object->getGlobalBounds().top + object->getGlobalBounds().height < child->mY ||
             object->getGlobalBounds().left + object->getGlobalBounds().width > child->mX + child->mWidth ||
             object->getGlobalBounds().top + object->getGlobalBounds().height > child->mY + child->mHeight);
}

void QuadTree::Clear()
{
    if (mDepth == maxDepth) {
        objects.clear();
        return;
    } else {
        NW->Clear();
        NE->Clear();
        SW->Clear();
        SE->Clear();
    }

    if (!objects.empty()) {
        objects.clear();
    }
}

void QuadTree::Draw(sf::RenderTarget& target)
{
    target.draw(shape);

    if (NW) {
        NW->Draw(target);
    }
    if (NE) {
        NE->Draw(target);
    }
    if (SW) {
        SW->Draw(target);
    }
    if (SE) {
        SE->Draw(target);
    }
}