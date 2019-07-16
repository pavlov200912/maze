#include "pch.h"
#include "LevelParser.h"
#include <utility>


void LevelParser::ReadLevelFile(std::string file_name) // TODO: should throw IO exception
{
	std::ifstream in(file_name, std::ios::binary);
	/// Format of the LevelFile documentation 
	/// <bool isVertical, int x, int y, int count>
	while(true) // TODO: Change this, maybe add count in file 
	{
		WallInformation inf;
		if (in.eof()) break; // TODO: LAST ITERATION DUPLICATE FIX EMERGENCY!!!
		in.read(reinterpret_cast<char*>(&inf.isVertical), sizeof(bool));
		in.read(reinterpret_cast<char*>(&inf.x), sizeof(int));
		in.read(reinterpret_cast<char*>(&inf.y), sizeof(int));
		in.read(reinterpret_cast<char*>(&inf.count), sizeof(int));
		wallsToDraw.emplace_back(inf);
		if (in.eof()) break;
	}
	in.close();
}





void LevelParser::AddWallsToDraw()
{
	// TODO: Draw only necessary walls, check screen scrolling (somehow?)
	wallHandler->ClearWalls(); // TODO: REMOVE THIS CRUNCH
	for(const auto& inf: wallsToDraw)
	{
		float x = zeroPos.x + wallHandler->getVerticalHeight() * inf.x;
		float y = zeroPos.y - wallHandler->getVerticalWidth() / 2 - wallHandler->getVerticalHeight() * inf.y;
		wallHandler->addWall(x, y, inf.count, (inf.isVertical) ? WallsHandler::VERTICAL : WallsHandler::HORIZONTAL); 
	}
}

