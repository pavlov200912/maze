#pragma once
/// Класс, который должен взаимодействовать с текстовым представлением уровня
/// Принимает на вход абсолютные координаты относительного нуля, указатель на объект Wall
#include "pch.h"
#include "Wall.h"


//TODO: Add Exceptions
class LevelParser
{
public:

	LevelParser(const DirectX::SimpleMath::Vector2& zero_pos, WallsHandler* wall_handler)
		: zeroPos(zero_pos),
		  wallHandler(wall_handler)
	{
	}

	void ReadLevelFile(std::string file_name);
	void AddWallsToDraw();
	void Update(const DirectX::SimpleMath::Vector2& zero_pos)
	{
		zeroPos = zero_pos;
	}

private:
	void my_log(std::string message) 
	{
#ifndef NDEBUG
		// print message
		util::ServiceLocator::getFileLogger()->print<util::SeverityType::info>(message);
#endif
	}
	
	struct WallInformation
	{
		bool	 isVertical;
		int		 x;
		int		 y;
		int		 count;
	};


	DirectX::SimpleMath::Vector2 zeroPos;
	WallsHandler*				 wallHandler; 
	std::vector<WallInformation> wallsToDraw;
};

