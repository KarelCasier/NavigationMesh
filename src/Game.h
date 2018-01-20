#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <memory>
#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "tinyxml2.h"
#include "Player.h"
#include "QuadTree.h"
#include "ShadowManager.h"
#include "NavMesh.h"
#include "GameObject.h"

class Game : private sf::NonCopyable
{
public:
    
    static Game* Instance()
    {
        return s_pInstance;
    }

	//< Hackish code to beable to send the path
	static Game* Instance(const std::string& path)
    {
        if (s_pInstance == nullptr)
        {
            s_pInstance = new Game(path);
        }
        return s_pInstance;
    }
    
    Game(const std::string& appPath);
    void					run();
    
    sf::RenderWindow*		getWindow() { return &mWindow; }
    sf::View*				getWorldView() { return &mWorldView; }
	
	void					loadMap();
    
    bool					getMouseButtonState(sf::Mouse::Button button) { return sf::Mouse::isButtonPressed(button); }
    sf::Vector2f			getMousePosition() { return mWindow.mapPixelToCoords((sf::Mouse::getPosition(mWindow))); }
    sf::Vector2f			getMousePositionRelativeToWorldView() { return mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow), mWorldView);}
	
	sf::ConvexShape*		createCircle(const int& numPts, const float& radius, const int& rotationDeg, const sf::Vector2f& location);
	sf::ConvexShape*		createRectangle(const float& width, const float& height, const int& rotationDeg, const sf::Vector2f& location);
	sf::ConvexShape*		createPolygon(const char* points, const int& rotationDeg, const sf::Vector2f& location);
	
	sf::Vector2f			getPlayerPosition() { return mPlayer.getPosition(); }
	
	NavMesh*				getNavMesh() { return &navMesh; }
	
private:
    void					processEvents();
    void					update(sf::Time dtTime);
    void					render();
	void					init();
    
    void					updateStatistics(sf::Time elapsedTime);
    
private:
    static Game*			s_pInstance;
    
    static const sf::Time	TimePerFrame;
    
    sf::RenderWindow		mWindow;
    
    sf::Font				mFont;
    sf::Text				mStatisticsText;
    sf::Time				mStatisticsUpdateTime;
    std::size_t				mStatisticsNumFrames;
    sf::RectangleShape		mTextBackground;
    
    sf::View				mUIView;
    sf::View				mWorldView;
    
    sf::Vector2f			camPos;
	sf::Vector2f			lastCamPos;
    
    TextureHolder           GameTextures;
    FontHolder              GameFonts;
	
	tinyxml2::XMLDocument	map;
	float 					tileSize;
	
    Player                  mPlayer;
	GameObject*				pObject;
    
    sf::RectangleShape      mBackdrop;
	
    std::vector<sf::ConvexShape*>	shapes;
	QuadTree						quadTree;
	ShadowManager					shadowManager;
	NavMesh							navMesh;
    sf::Vector2f                    targetPos;
	
	std::vector<sf::VertexArray*>	lines;
	
	
	
	struct Normal
	{
		sf::Vector2f pos;
		sf::Vector2f normal;
	};
	
	std::vector<Normal*> normals;
	
};

typedef Game TheGame;
