#include "config.h"
#include "Game.h"
#include <stdexcept>
#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		TheGame::Instance(std::string(argv[0]))->run();
	}
	catch (std::exception& e)
	{
		std::cout << std::endl << "EXCEPTION: " << e.what() << std::endl;
	}
}
