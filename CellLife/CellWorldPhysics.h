#pragma once
#include <memory>
#include "CellBot.h"

using namespace std;

class CellWorld;

class CellWorldPhysics
{
public:
	CellWorldPhysics() {}
	~CellWorldPhysics() {}

	virtual void tick(CellWorld * world) {}
};

typedef shared_ptr<class CellWorldPhysics> sharedCellWorldPhysics;
typedef weak_ptr<class CellWorldPhysics> weakCellWorldPhysics;