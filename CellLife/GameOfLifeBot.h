#pragma once
#include "CellBot.h"

class GameOfLifeBot :
	public CellBot
{
protected:
	bool newCell = true;

public:
	using CellBot::CellBot;

	virtual void nextMove(CellWorld *world);
};

