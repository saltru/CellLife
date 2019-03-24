#pragma once
#include "CellWorldPhysics.h"
#include "GameOfLifeBot.h"

class GameOfLifePhysics :
	public CellWorldPhysics
{
public:
	using CellWorldPhysics::CellWorldPhysics;

	virtual void tick(CellWorld * world);
};

typedef shared_ptr<class GameOfLifePhysics> sharedGameOfLifePhysics;
typedef weak_ptr<class GameOfLifePhysics> weakGameOfLifePhysics;