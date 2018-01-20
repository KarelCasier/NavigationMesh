#include "Game.h"
#include <algorithm>
#include <functional>
#include "CollisionHelpers.h"
#include <math.h>
#include "GlobalConstants.hpp"
#include "tinyxml2.h"
#include <stdlib.h>
#include "ResourcePath.hpp"

const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f); // = 0.6 seconds per frame, therefore 60fps

Game* Game::s_pInstance = nullptr;                                              ///< set game instance pointer to nullptr

Game::Game(const std::string& path)
: mWindow(sf::VideoMode(WINDOWWIDTH, WINDOWHEIGHT), "Untitled by Karel Casier")		///< Initialize window
, mFont()
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
, mWorldView(mWindow.getDefaultView())                                                            ///< View used to render world
, mUIView(sf::Vector2f(WINDOWWIDTH, WINDOWHEIGHT) / 2.f, sf::Vector2f(WINDOWWIDTH, WINDOWHEIGHT)) ///< View used to render UI
, camPos(0, 0)
, quadTree(0.f,0.f,10000.f,10000.f,0,4)
, shadowManager(&mWorldView)
, targetPos(sf::Vector2f(2550,2500))
{
    ResPath::path = path;
    int numDirBack = 1;
    for ( int i = 0; i < numDirBack; ++i ) {
        size_t lastSep = ResPath::path.rfind("/");
        ResPath::path = ResPath::path.substr(0, lastSep + 1);
    }
    
    mWindow.setKeyRepeatEnabled(true);
    mWindow.setVerticalSyncEnabled(true);

    ///<------------------------------------------------------->                ///< Statistics Initializing

    GameFonts.load(FontID::Default, "MYRIADPRO-BOLD.OTF");

    mFont = GameFonts.get(FontID::Default);
    
    mStatisticsText.setFont(mFont);
    mStatisticsText.setPosition(5.f, 5.f);
    mStatisticsText.setCharacterSize(15);
    
    mTextBackground.setSize(sf::Vector2f(160.f, 30.f));
    mTextBackground.setFillColor(sf::Color(110, 110, 110, 80));
    ///<------------------------------------------------------->
	
	GameTextures.load(TextureID::PlaceHolder, "Circle.png");
    
    mBackdrop.setSize(sf::Vector2f(10000.f, 10000.f));
    mBackdrop.setFillColor(sf::Color(100,100,100));
    
	std::cout << "Cannot Handle Rotation" << std::endl;
	
	mPlayer.setPosition(200, 60);
		
	loadMap();
	
}

void Game::init()
{
	pObject = new GameObject(GameTextures.get(TextureID::PlaceHolder), 20, 20);
}

void Game::loadMap()
{
	std::string fileName = ResPath::path + "LevelTri.tmx";
	map.LoadFile(fileName.c_str());
	
	tileSize = 5.f;
	
	bool spawnLocSet = false;
	
	tinyxml2::XMLElement* pObjectGroup = map.FirstChildElement("map")->FirstChildElement("objectgroup");
	while (pObjectGroup)
	{
		if (((std::string)"walls").compare(pObjectGroup->Attribute("name")) == 0)
		{
			tinyxml2::XMLElement* pEle = pObjectGroup->FirstChildElement();
			while (pEle)
			{
				tinyxml2::XMLElement* subObject = pEle->FirstChildElement();
				if (subObject)
				{
					tinyxml2::XMLElement* typeEle = pEle->FirstChildElement("polygon");
					if (typeEle)
					{
						sf::ConvexShape* poly = createPolygon(typeEle->Attribute("points"), pEle->IntAttribute("rotation"), sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y")));
						shapes.push_back(poly);
					}
					typeEle = pEle->FirstChildElement("ellipse");
					if (typeEle)
					{
						sf::ConvexShape* circle = createCircle(64, pEle->FloatAttribute("width")/2.f, pEle->IntAttribute("rotation"), sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y")));
						shapes.push_back(circle);
					}
				}
				else ///< Regular rectangle
				{
					sf::ConvexShape* rect = createRectangle(pEle->FloatAttribute("width"), pEle->FloatAttribute("height"), pEle->IntAttribute("rotation"), sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y")));
					shapes.push_back(rect);
				}
				pEle = pEle->NextSiblingElement();
			}
		}
		else if (((std::string)"navmesh").compare(pObjectGroup->Attribute("name")) == 0)
		{
			tinyxml2::XMLElement* pEle = pObjectGroup->FirstChildElement();
			while (pEle)
			{
				tinyxml2::XMLElement* subObject = pEle->FirstChildElement();
				if (subObject)
				{
					tinyxml2::XMLElement* typeEle = pEle->FirstChildElement("polygon");
					if (typeEle)
					{
						sf::ConvexShape* poly = createPolygon(typeEle->Attribute("points"), pEle->IntAttribute("rotation"), sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y")));
						navMesh.AddPoly(poly);
					}
					typeEle = pEle->FirstChildElement("ellipse");
					if (typeEle)
					{
						sf::ConvexShape* circle = createCircle(64, pEle->FloatAttribute("width")/2.f, pEle->IntAttribute("rotation"), sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y")));
						navMesh.AddPoly(circle);
					}
				}
				else ///< Regular rectangle
				{
					sf::ConvexShape* rect = createRectangle(pEle->FloatAttribute("width"), pEle->FloatAttribute("height"), pEle->IntAttribute("rotation"), sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y")));
					navMesh.AddPoly(rect);
				}
				pEle = pEle->NextSiblingElement();
			}
		}
		else if (((std::string)"spawn").compare(pObjectGroup->Attribute("name")) == 0)
		{
			tinyxml2::XMLElement* pEle = pObjectGroup->FirstChildElement();
			mPlayer.setPosition(sf::Vector2f(pEle->FloatAttribute("x"),pEle->FloatAttribute("y"))*tileSize);
			spawnLocSet = true;
		}
		
		pObjectGroup = pObjectGroup->NextSiblingElement();
		
	}
	
	assert(spawnLocSet == true);
	
	for (sf::ConvexShape* shape : shapes)
	{
		///<Verify all verticies of each shape are ordered clockwise
		int ptCnt = (int)shape->getPointCount();
		///< Set up vector of verticies
		std::vector<sf::Vector2f> points;
		for (auto i = 0; i < ptCnt; ++i)
		{
			points.push_back(shape->getPoint(i));
		}
		///< Get ordered verticies
		std::vector<sf::Vector2f> newPoints = convex_hull(points);
		for (auto i = 0; i < ptCnt; ++i)
		{
			shape->setPoint(i, newPoints.at(i));
		}
		
		/*
		///< Calculate Normals
		for (auto i = 0; i < ptCnt-1; ++i)
		{
			Normal* newNormal = new Normal();
			newNormal->pos = shape->getPosition() + shape->getPoint(i) + (shape->getPoint(i+1) - shape->getPoint(i)) / 2.f;
			newNormal->normal = CalcEdgeNormal(shape->getPoint(i), shape->getPoint(i+1));
			normals.push_back(newNormal);
		}
		
		Normal* newNormal = new Normal();
		newNormal->pos = shape->getPosition() + shape->getPoint(ptCnt-1) + (shape->getPoint(0) - shape->getPoint(ptCnt-1)) / 2.f;
		newNormal->normal = CalcEdgeNormal(shape->getPoint(ptCnt-1), shape->getPoint(0));
		normals.push_back(newNormal);
		*/
		
		shape->setFillColor(sf::Color::Red);
		quadTree.AddObject(shape);
		shadowManager.addShape(shape);
	}
	
	navMesh.CreateGraph();
	
}

sf::ConvexShape* Game::createCircle(const int& numPts, const float& r, const int& rotationDeg, const sf::Vector2f& location)
{
	sf::ConvexShape* circle = new sf::ConvexShape;
	circle->setPointCount(numPts);
	float angleIncrement = 2 * M_PI / numPts;
	float currentAngle = 0;
	for (int i = 0; i < numPts; ++i)
	{
		circle->setPoint(i, sf::Vector2f(r * cosf(currentAngle), r * sinf(currentAngle)) * tileSize);
		currentAngle += angleIncrement;
	}
	
	circle->setPosition(location*tileSize);
	circle->move(r*5,r*5);
	
	return circle;
}

sf::ConvexShape* Game::createRectangle(const float& width,const float& height, const int& rotationDeg, const sf::Vector2f& location)
{
	//const float rotationRad = rotationDeg * M_PI / 180.f;
	sf::ConvexShape* rect = new sf::ConvexShape;
	rect->setPointCount(4);
	rect->setPoint(0, sf::Vector2f(0,0));
	rect->setPoint(1, sf::Vector2f(width,0)*tileSize);
	rect->setPoint(2, sf::Vector2f(width,height)*tileSize);
	rect->setPoint(3, sf::Vector2f(0,height)*tileSize);
	rect->setPosition(location*tileSize);
	rect->rotate(rotationDeg);
	return rect;
	
}

sf::ConvexShape* Game::createPolygon(const char *points, const int& rotationDeg, const sf::Vector2f &location)
{
	//EXAMPLE OBJECT FORMAT
	//<object id="36" x="609.605" y="458.976">
	//<polygon points="0,0 287.081,-46.0748 419.989,150.629 244.551,381.003 -42.5306,310.119 -77.9727,124.047"/>
	//</object>
	sf::ConvexShape* poly = new sf::ConvexShape;
	std::vector<sf::Vector2f> verticies;
	std::string pts(points);
	
	
	std::size_t comma = pts.find(",");
	while (comma != std::string::npos)
	{
		std::string xStr = pts.substr(0, comma);
		//std::cout << "x: " <<  xStr;
		float x = std::stof(xStr);
		
		std::size_t space = pts.find(" ");
		std::string yStr = pts.substr(comma + 1, space-1 - xStr.length());
		//std::cout << " y: " << yStr << std::endl;
		float y = std::stof(yStr);
		
		verticies.push_back(sf::Vector2f(x,y));
		//std::cout << VectorToStr(verticies.back()) << std::endl;
		if (space != std::string::npos)
		{
			pts = pts.substr(space+1);
			comma = pts.find(",");
		}
		else
		{
			break;
		}
	}
	
	int numPts = (int)verticies.size();
	poly->setPointCount(numPts);
	
	for (int i = 0; i < numPts; ++i)
	{
		poly->setPoint(i, verticies.at(i)*tileSize);
	}
	poly->setPosition(location*tileSize);
	
	return poly;
	
}

void Game::run()
{
	init();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        sf::Time elapsedTime = clock.restart();                                 ///< Note restart returns the time on the clock then restarts the clock
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame)                              ///< Ensures that the logic is caught up before rendering again
        {
            timeSinceLastUpdate -= TimePerFrame;
            
            processEvents();
            update(TimePerFrame);
        }
        
        updateStatistics(elapsedTime);
        render();
    }
    mWindow.close();
}

void Game::processEvents()
{
    
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Key::Escape)
            mWindow.close();
    
        mPlayer.handleInput(event);
    }
    
    mPlayer.handleRealtimeInput();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        mWorldView.zoom(1.01);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        mWorldView.zoom(0.99);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i pixel_pos = sf::Mouse::getPosition(mWindow);
        targetPos = mWindow.mapPixelToCoords(pixel_pos);
    }
}

void Game::update(sf::Time dTime)
{
	sf::Vector2f prevPos = mPlayer.getPosition();
	
    mPlayer.update(dTime);
	
	sf::Vector2f nextPos = mPlayer.getPosition();
	
//	for (auto shape : shapes)
//	{
//		shape->setFillColor(sf::Color::White);
//	}
//	
	///< Player Collisions
	std::vector<sf::ConvexShape*>	potentialCollisionShapes = quadTree.GetObjectsAt(mPlayer.getPosition().x, mPlayer.getPosition().y);
	
	for (auto shape : potentialCollisionShapes)
	{
		shape->setFillColor(sf::Color::Red);
		int numPts = (int)shape->getPointCount();
		for (int i = 0; i < numPts-1; ++i)
		{
			if (doIntersect(prevPos, nextPos, shape->getPosition() + shape->getPoint(i), shape->getPosition() + shape->getPoint(i+1)))
			{
				sf::Vector2f shapeNormal(CalcEdgeNormal(shape->getPoint(i), shape->getPoint(i+1)));
				sf::Vector2f dir(nextPos-prevPos);
				sf::Vector2f invNormal(-shapeNormal);
				invNormal *= Norm(dir);
				mPlayer.setPosition(prevPos + (dir - invNormal));
				sf::Vector2f u((DotProduct(mPlayer.getVelocity(), shapeNormal) / DotProduct(shapeNormal, shapeNormal)) * shapeNormal);
				mPlayer.setVelocity(mPlayer.getVelocity()-u);
			}
		}
		if (doIntersect(prevPos, nextPos, shape->getPosition() + shape->getPoint(numPts-1), shape->getPosition() + shape->getPoint(0)))
		{
			sf::Vector2f shapeNormal(CalcEdgeNormal(shape->getPoint(numPts-1), shape->getPoint(0)));
			
			sf::Vector2f dir(nextPos-prevPos);
			sf::Vector2f invNormal(-shapeNormal);
			invNormal *= Norm(dir);
			mPlayer.setPosition(prevPos + (dir - invNormal));
			sf::Vector2f u((DotProduct(mPlayer.getVelocity(), shapeNormal) / DotProduct(shapeNormal, shapeNormal)) * shapeNormal);
			mPlayer.setVelocity(mPlayer.getVelocity()-u);
		}
	}
	
//	shadowManager.updateShadows(nextPos);
	
	
	
	///< Center camera around the point between camera and player
	
//	sf::Vector2f playerToCameraDifference =getMousePositionRelativeToWorldView() - mPlayer.getPosition();
//	sf::Vector2f dir = Normalize(playerToCameraDifference);
//	float lengthSquared = Clamp(Norm(playerToCameraDifference), 0, 400);
//	dir *= lengthSquared;
//	mWorldView.setCenter(mPlayer.getPosition() + 0.5f * (dir));
	mWorldView.setCenter(mPlayer.getPosition());

	pObject->update(dTime);
	
}

void Game::updateStatistics(sf::Time elapsedTime)
{
    std::stringstream strStream;
    
    mStatisticsUpdateTime += elapsedTime;
    mStatisticsNumFrames += 1;
    
    if (mStatisticsUpdateTime >= sf::seconds(1.0f))
    {
        strStream << "Frames / Second = " << mStatisticsNumFrames << "\n";// << "Time / Update = " << mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames << "us" << std::endl;
        
        mStatisticsText.setString(strStream.str());
        
        mStatisticsUpdateTime -= sf::seconds(1.0f);
        mStatisticsNumFrames = 0;
    }
}

void Game::render()
{
    //mWindow.clear(sf::Color(50, 50, 50,1.0));
	mWindow.clear(sf::Color(0, 0, 0,1.0));
	
    mWindow.setView(mWorldView);
    mWindow.draw(mBackdrop);
	
	navMesh.Draw(mWindow);

	for (const sf::ConvexShape* shape : shapes)
	{
		mWindow.draw(*shape);
		/*
		sf::RectangleShape bounds;
		bounds.setPosition(shape->getGlobalBounds().left,shape->getGlobalBounds().top);
		bounds.setSize(sf::Vector2f(shape->getGlobalBounds().width, shape->getGlobalBounds().height));
		bounds.setFillColor(sf::Color::Transparent);
		bounds.setOutlineThickness(1.f);
		bounds.setOutlineColor(sf::Color::Blue);
		mWindow.draw(bounds);
		*/
	}
//	shadowManager.drawShadows(mWindow);
//	quadTree.Draw(mWindow);
    /*/
	for (auto line : lines)
	{
		mWindow.draw(*line);
	}
     //*/
	DrawDebugCircle(targetPos, 20.f, mWindow, sf::Color::Blue);
	//*//
	//auto path = navMesh.GetNavPath(mPlayer.getPosition(), targetPos);
	auto path = navMesh.GetNavPath(pObject->getPosition(), targetPos);
	for ( int i = 0; i < path.size(); ++i ) {
		if (i < path.size() - 1 ) {
			DrawDebugLine(path[i], path[i+1], mWindow, sf::Color::Blue);
		}
		DrawDebugCircle(path[i], 10.f, mWindow, sf::Color::Blue);
	}//*/
	mWindow.draw(*pObject);	
    mWindow.draw(mPlayer);
    mWindow.setView(mUIView);
    mWindow.draw(mTextBackground);
    mWindow.draw(mStatisticsText);
    mWindow.setView(mWorldView);
    mWindow.display();
}
